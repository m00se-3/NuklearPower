#ifndef NK_POWER_TREE_HPP
#define NK_POWER_TREE_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {

  /** =============================================================================
   *
   *                                  TREE
   *
   * =============================================================================*/
  /**
   * \page Tree
   * Trees represent two different concept. First the concept of a collapsible
   * UI section that can be either in a hidden or visible state. They allow the UI
   * user to selectively minimize the current set of visible UI to comprehend.
   * The second concept are tree widgets for visual UI representation of trees.<br /><br />
   *
   * Trees thereby can be nested for tree representations and multiple nested
   * collapsible UI sections. All trees are started by calling of the
   * `tree_xxx_push_tree` functions and ended by calling one of the
   * `tree_xxx_pop_xxx()` functions. Each starting functions takes a title label
   * and optionally an image to be displayed and the initial collapse state from
   * the collapse_states section.<br /><br />
   *
   * The runtime state of the tree is either stored outside the library by the caller
   * or inside which requires a unique ID. The unique ID can either be generated
   * automatically from `__FILE__` and `__LINE__` with function `tree_push`,
   * by `__FILE__` and a user provided ID generated for example by loop index with
   * function `tree_push_id` or completely provided from outside by user with
   * function `tree_push_hashed`.
   *
   * # Usage
   * To create a tree you have to call one of the seven `tree_xxx_push_xxx`
   * functions to start a collapsible UI section and `tree_xxx_pop` to mark the
   * end.
   * Each starting function will either return `false(0)` if the tree is collapsed
   * or hidden and therefore does not need to be filled with content or `true(1)`
   * if visible and required to be filled.
   *
   * !!! Note
   *     The tree header does not require and layouting function and instead
   *     calculates a auto height based on the currently used font size
   *
   * The tree ending functions only need to be called if the tree content is
   * actually visible. So make sure the tree push function is guarded by `if`
   * and the pop call is only taken if the tree is visible.
   *
   * ```c
   * if (tree_push(ctx, NK_TREE_TAB, "Tree", NK_MINIMIZED)) {
   *     layout_row_dynamic(...);
   *     widget(...);
   *     tree_pop(ctx);
   * }
   * ```
   *
   * # Reference
   * Function                    | Description
   * ----------------------------|-------------------------------------------
   * tree_push                | Start a collapsible UI section with internal state management
   * tree_push_id             | Start a collapsible UI section with internal state management callable in a look
   * tree_push_hashed         | Start a collapsible UI section with internal state management with full control over internal unique ID use to store state
   * tree_image_push          | Start a collapsible UI section with image and label header
   * tree_image_push_id       | Start a collapsible UI section with image and label header and internal state management callable in a look
   * tree_image_push_hashed   | Start a collapsible UI section with image and label header and internal state management with full control over internal unique ID use to store state
   * tree_pop                 | Ends a collapsible UI section
   * tree_state_push          | Start a collapsible UI section with external state management
   * tree_state_image_push    | Start a collapsible UI section with image and label header and external state management
   * tree_state_pop           | Ends a collapsabale UI section
   *
   * # tree_type
   * Flag            | Description
   * ----------------|----------------------------------------
   * NK_TREE_NODE    | Highlighted tree header to mark a collapsible UI section
   * NK_TREE_TAB     | Non-highlighted tree header closer to tree representations
   */

  /**
   * # # tree_push
   * Starts a collapsible UI section with internal state management
   * !!! \warning
   *     To keep track of the runtime tree collapsible state this function uses
   *     defines `__FILE__` and `__LINE__` to generate a unique ID. If you want
   *     to call this function in a loop please use `tree_push_id` or
   *     `tree_push_hashed` instead.
   *
   * ```c
   * #define tree_push(ctx, type, title, state)
   * ```
   *
   * Parameter   | Description
   * ------------|-----------------------------------------------------------
   * \param[in] ctx     | Must point to an previously initialized `context` struct
   * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
   * \param[in] title   | Label printed in the tree header
   * \param[in] state   | Initial tree state value out of collapse_states
   *
   * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
   */
#define tree_push(ctx, type, title, state) tree_push_hashed(ctx, type, title, state, NK_FILE_LINE, strlen(NK_FILE_LINE), __LINE__)

  /**
   * # # tree_push_id
   * Starts a collapsible UI section with internal state management callable in a look
   * ```c
   * #define tree_push_id(ctx, type, title, state, id)
   * ```
   *
   * Parameter   | Description
   * ------------|-----------------------------------------------------------
   * \param[in] ctx     | Must point to an previously initialized `context` struct
   * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
   * \param[in] title   | Label printed in the tree header
   * \param[in] state   | Initial tree state value out of collapse_states
   * \param[in] id      | Loop counter index if this function is called in a loop
   *
   * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
   */
#define tree_push_id(ctx, type, title, state, id) tree_push_hashed(ctx, type, title, state, NK_FILE_LINE, strlen(NK_FILE_LINE), id)

  /**
   * # # tree_push_hashed
   * Start a collapsible UI section with internal state management with full
   * control over internal unique ID used to store state
   * ```c
   * bool tree_push_hashed(struct context*, enum tree_type, const char *title, enum collapse_states initial_state, const char *hash, int len,int seed);
   * ```
   *
   * Parameter   | Description
   * ------------|-----------------------------------------------------------
   * \param[in] ctx     | Must point to an previously initialized `context` struct
   * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
   * \param[in] title   | Label printed in the tree header
   * \param[in] state   | Initial tree state value out of collapse_states
   * \param[in] hash    | Memory block or string to generate the ID from
   * \param[in] len     | Size of passed memory block or string in __hash__
   * \param[in] seed    | Seeding value if this function is called in a loop or default to `0`
   *
   * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
   */
  NK_API bool tree_push_hashed(context*, nk::tree_type, const char* title, nk::collapse_states initial_state, const char* hash, int len, int seed);

  /**
   * # # tree_image_push
   * Start a collapsible UI section with image and label header
   * !!! \warning
   *     To keep track of the runtime tree collapsible state this function uses
   *     defines `__FILE__` and `__LINE__` to generate a unique ID. If you want
   *     to call this function in a loop please use `tree_image_push_id` or
   *     `tree_image_push_hashed` instead.
   *
   * ```c
   * #define tree_image_push(ctx, type, img, title, state)
   * ```
   * Parameter   | Description
   * ------------|-----------------------------------------------------------
   * \param[in] ctx     | Must point to an previously initialized `context` struct
   * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
   * \param[in] img     | Image to display inside the header on the left of the label
   * \param[in] title   | Label printed in the tree header
   * \param[in] state   | Initial tree state value out of collapse_states
   *
   * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
   */
#define tree_image_push(ctx, type, img, title, state) tree_image_push_hashed(ctx, type, img, title, state, NK_FILE_LINE, strlen(NK_FILE_LINE), __LINE__)

  /**
   * # # tree_image_push_id
   * Start a collapsible UI section with image and label header and internal state
   * management callable in a look
   *
   * ```c
   * #define tree_image_push_id(ctx, type, img, title, state, id)
   * ```
   *
   * Parameter   | Description
   * ------------|-----------------------------------------------------------
   * \param[in] ctx     | Must point to an previously initialized `context` struct
   * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
   * \param[in] img     | Image to display inside the header on the left of the label
   * \param[in] title   | Label printed in the tree header
   * \param[in] state   | Initial tree state value out of collapse_states
   * \param[in] id      | Loop counter index if this function is called in a loop
   *
   * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
   */
#define tree_image_push_id(ctx, type, img, title, state, id) tree_image_push_hashed(ctx, type, img, title, state, NK_FILE_LINE, strlen(NK_FILE_LINE), id)

  /**
   * # # tree_image_push_hashed
   * Start a collapsible UI section with internal state management with full
   * control over internal unique ID used to store state
   * ```c
   * bool tree_image_push_hashed(struct context*, enum tree_type, struct image, const char *title, enum collapse_states initial_state, const char *hash, int len,int seed);
   * ```
   *
   * Parameter   | Description
   * ------------|-----------------------------------------------------------
   * \param[in] ctx     | Must point to an previously initialized `context` struct
   * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
   * \param[in] img     | Image to display inside the header on the left of the label
   * \param[in] title   | Label printed in the tree header
   * \param[in] state   | Initial tree state value out of collapse_states
   * \param[in] hash    | Memory block or string to generate the ID from
   * \param[in] len     | Size of passed memory block or string in __hash__
   * \param[in] seed    | Seeding value if this function is called in a loop or default to `0`
   *
   * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
   */
  NK_API bool tree_image_push_hashed(context*, nk::tree_type, image, const char* title, nk::collapse_states initial_state, const char* hash, int len, int seed);

  /**
   * # # tree_pop
   * Ends a collapsabale UI section
   * ```c
   * void tree_pop(struct context*);
   * ```
   *
   * Parameter   | Description
   * ------------|-----------------------------------------------------------
   * \param[in] ctx     | Must point to an previously initialized `context` struct after calling `tree_xxx_push_xxx`
   */
  NK_API void tree_pop(context*);

  /**
   * # # tree_state_push
   * Start a collapsible UI section with external state management
   * ```c
   * bool tree_state_push(struct context*, enum tree_type, const char *title, enum collapse_states *state);
   * ```
   *
   * Parameter   | Description
   * ------------|-----------------------------------------------------------
   * \param[in] ctx     | Must point to an previously initialized `context` struct after calling `tree_xxx_push_xxx`
   * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
   * \param[in] title   | Label printed in the tree header
   * \param[in] state   | Persistent state to update
   *
   * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
   */
  NK_API bool tree_state_push(context*, nk::tree_type, const char* title, nk::collapse_states* state);

  /**
   * # # tree_state_image_push
   * Start a collapsible UI section with image and label header and external state management
   * ```c
   * bool tree_state_image_push(struct context*, enum tree_type, struct image, const char *title, enum collapse_states *state);
   * ```
   *
   * Parameter   | Description
   * ------------|-----------------------------------------------------------
   * \param[in] ctx     | Must point to an previously initialized `context` struct after calling `tree_xxx_push_xxx`
   * \param[in] img     | Image to display inside the header on the left of the label
   * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
   * \param[in] title   | Label printed in the tree header
   * \param[in] state   | Persistent state to update
   *
   * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
   */
  NK_API bool tree_state_image_push(context*, nk::tree_type, image, const char* title, nk::collapse_states* state);

  /**
   * # # tree_state_pop
   * Ends a collapsabale UI section
   * ```c
   * void tree_state_pop(struct context*);
   * ```
   *
   * Parameter   | Description
   * ------------|-----------------------------------------------------------
   * \param[in] ctx     | Must point to an previously initialized `context` struct after calling `tree_xxx_push_xxx`
   */
  NK_API void tree_state_pop(context*);

#define tree_element_push(ctx, type, title, state, sel) tree_element_push_hashed(ctx, type, title, state, sel, NK_FILE_LINE, strlen(NK_FILE_LINE), __LINE__)
#define tree_element_push_id(ctx, type, title, state, sel, id) tree_element_push_hashed(ctx, type, title, state, sel, NK_FILE_LINE, strlen(NK_FILE_LINE), id)
  NK_API bool tree_element_push_hashed(context*, nk::tree_type, const char* title, nk::collapse_states initial_state, bool* selected, const char* hash, int len, int seed);
  NK_API bool tree_element_image_push_hashed(context*, nk::tree_type, image, const char* title, nk::collapse_states initial_state, bool* selected, const char* hash, int len, int seed);
  NK_API void tree_element_pop(context*);

}

#endif