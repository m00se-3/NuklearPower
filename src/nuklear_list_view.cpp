#include <nk/nuklear.hpp>

namespace nk {
  /* ===============================================================
   *
   *                          LIST VIEW
   *
   * ===============================================================*/
  NK_API bool
  list_view_begin(context* ctx, list_view* view,
                  const char* title, const flag flags, int row_height, const int row_count) {
    unsigned int* y_offset;

    NK_ASSERT(ctx);
    NK_ASSERT(view);
    NK_ASSERT(title);
    if (!ctx || !view || !title)
      return 0;

    window* win = ctx->current;
    const style* style = &ctx->style;
    const vec2f item_spacing = style->window.spacing;
    row_height += std::max(0, (int) item_spacing.y);

    /* find persistent list view scrollbar offset */
    const int title_len = (int) strlen(title);
    const hash title_hash = murmur_hash(title, (int) title_len, static_cast<hash>(panel_type::PANEL_GROUP));
    unsigned int* x_offset = find_value(win, title_hash);
    if (!x_offset) {
      x_offset = add_value(ctx, win, title_hash, 0);
      y_offset = add_value(ctx, win, title_hash + 1, 0);

      NK_ASSERT(x_offset);
      NK_ASSERT(y_offset);
      if (!x_offset || !y_offset)
        return 0;
      *x_offset = *y_offset = 0;
    } else
      y_offset = find_value(win, title_hash + 1);
    view->scroll_value = *y_offset;
    view->scroll_pointer = y_offset;

    *y_offset = 0;
    const int result = group_scrolled_offset_begin(ctx, x_offset, y_offset, title, flags);
    win = ctx->current;
    const panel* layout = win->layout;

    view->total_height = row_height * std::max(row_count, 1);
    view->begin = (int) std::max(((float) view->scroll_value / (float) row_height), 0.0f);
    view->count = (int) std::max(iceilf((layout->clip.h) / (float) row_height), 0);
    view->count = std::min(view->count, row_count - view->begin);
    view->end = view->begin + view->count;
    view->ctx = ctx;
    return result;
  }
  NK_API void
  list_view_end(list_view* view) {

    NK_ASSERT(view);
    NK_ASSERT(view->ctx);
    NK_ASSERT(view->scroll_pointer);
    if (!view || !view->ctx)
      return;

    const context* ctx = view->ctx;
    const window* win = ctx->current;
    panel* layout = win->layout;
    layout->at_y = layout->bounds.y + (float) view->total_height;
    *view->scroll_pointer = *view->scroll_pointer + view->scroll_value;
    group_end(view->ctx);
  }
} // namespace nk
