#include <cstring>
#include <nk/nuklear.hpp>

namespace nk {
  /* ==============================================================
   *
   *                          CONTEXT
   *
   * ===============================================================*/
  INTERN void
  setup(context* ctx, const user_font* font) {
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    zero_struct(*ctx);
    style_default(ctx);
    ctx->seq = 1;
    if (font)
      ctx->style.font = font;
#ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT
    draw_list_init(&ctx->draw_list);
#endif
  }
#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
  NK_API bool
  init_default(struct context* ctx, const struct user_font* font) {
    struct allocator alloc;
    alloc.userdata.ptr = 0;
    alloc.alloc = malloc;
    alloc.free = mfree;
    return init(ctx, &alloc, font);
  }
#endif
  NK_API bool
  init_fixed(context* ctx, void* memory, const std::size_t size,
             const user_font* font) {
    NK_ASSERT(memory);
    if (!memory)
      return 0;
    setup(ctx, font);
    buffer_init_fixed(&ctx->memory, memory, size);
    ctx->use_pool = false;
    return 1;
  }
  NK_API bool
  init_custom(context* ctx, memory_buffer* cmds,
              memory_buffer* pool, const user_font* font) {
    NK_ASSERT(cmds);
    NK_ASSERT(pool);
    if (!cmds || !pool)
      return 0;

    setup(ctx, font);
    ctx->memory = *cmds;
    if (pool->type == allocation_type::BUFFER_FIXED) {
      /* take memory from buffer and alloc fixed pool */
      pool_init_fixed(&ctx->pool, pool->memory.ptr, pool->memory.size);
    } else {
      /* create dynamic pool from buffer allocator */
      const allocator* alloc = &pool->pool;
      pool_init(&ctx->pool, alloc, NK_POOL_DEFAULT_CAPACITY);
    }
    ctx->use_pool = true;
    return 1;
  }
  NK_API bool
  init(context* ctx, const allocator* alloc,
       const user_font* font) {
    NK_ASSERT(alloc);
    if (!alloc)
      return 0;
    setup(ctx, font);
    buffer_init(&ctx->memory, alloc, NK_DEFAULT_COMMAND_BUFFER_SIZE);
    pool_init(&ctx->pool, alloc, NK_POOL_DEFAULT_CAPACITY);
    ctx->use_pool = true;
    return 1;
  }
#ifdef NK_INCLUDE_COMMAND_USERDATA
  NK_API void
  set_user_data(struct context* ctx, resource_handle handle) {
    if (!ctx)
      return;
    ctx->userdata = handle;
    if (ctx->current)
      ctx->current->buffer.userdata = handle;
  }
#endif
  NK_API void
  free(context* ctx) {
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    buffer_free(&ctx->memory);
    if (ctx->use_pool)
      pool_free(&ctx->pool);

    zero(&ctx->input, sizeof(ctx->input));
    zero(&ctx->style, sizeof(ctx->style));
    zero(&ctx->memory, sizeof(ctx->memory));

    ctx->seq = 0;
    ctx->build = 0;
    ctx->begin = 0;
    ctx->end = 0;
    ctx->active = 0;
    ctx->current = 0;
    ctx->freelist = 0;
    ctx->count = 0;
  }
  NK_API void
  clear(context* ctx) {
    NK_ASSERT(ctx);

    if (!ctx)
      return;
    if (ctx->use_pool)
      buffer_clear(&ctx->memory);
    else
      buffer_reset(&ctx->memory, buffer_allocation_type::BUFFER_FRONT);

    ctx->build = 0;
    ctx->memory.calls = 0;
    ctx->last_widget_state = 0;
    ctx->style.cursor_active = ctx->style.cursors[static_cast<unsigned>(style_cursor::CURSOR_ARROW)];
    std::memcpy(&ctx->overlay, 0, sizeof(ctx->overlay));

    /* garbage collector */
    window* iter = ctx->begin;
    while (iter) {
      /* make sure valid minimized windows do not get removed */
      if ((iter->flags & static_cast<decltype(iter->flags)>(window_flags::WINDOW_MINIMIZED)) &&
          !(iter->flags & static_cast<decltype(iter->flags)>(window_flags::WINDOW_CLOSED)) &&
          iter->seq == ctx->seq) {
        iter = iter->next;
        continue;
      }
      /* remove hotness from hidden or closed windows*/
      if (((iter->flags & static_cast<decltype(iter->flags)>(window_flags::WINDOW_HIDDEN)) ||
           (iter->flags & static_cast<decltype(iter->flags)>(window_flags::WINDOW_CLOSED))) &&
          iter == ctx->active) {
        ctx->active = iter->prev;
        ctx->end = iter->prev;
        if (!ctx->end)
          ctx->begin = 0;
        if (ctx->active)
          ctx->active->flags &= ~static_cast<decltype(iter->flags)>(window_flags::WINDOW_ROM);
      }
      /* free unused popup windows */
      if (iter->popup.win && iter->popup.win->seq != ctx->seq) {
        free_window(ctx, iter->popup.win);
        iter->popup.win = 0;
      }
      /* remove unused window state tables */
      {
        table* it = iter->tables;
        while (it) {
          table* n = it->next;
          if (it->seq != ctx->seq) {
            remove_table(iter, it);
            zero(it, sizeof(page_data));
            free_table(ctx, it);
            if (it == iter->tables)
              iter->tables = n;
          }
          it = n;
        }
      }
      /* window itself is not used anymore so free */
      if (iter->seq != ctx->seq || iter->flags & static_cast<decltype(iter->flags)>(window_flags::WINDOW_CLOSED)) {
        window* next = iter->next;
        remove_window(ctx, iter);
        free_window(ctx, iter);
        iter = next;
      } else
        iter = iter->next;
    }
    ctx->seq++;
  }
  NK_LIB void
  start_buffer(context* ctx, command_buffer* buffer) {
    NK_ASSERT(ctx);
    NK_ASSERT(buffer);
    if (!ctx || !buffer)
      return;
    buffer->begin = ctx->memory.allocated;
    buffer->end = buffer->begin;
    buffer->last = buffer->begin;
    buffer->clip = null_rect;
  }
  NK_LIB void
  start(context* ctx, window* win) {
    NK_ASSERT(ctx);
    NK_ASSERT(win);
    start_buffer(ctx, &win->buffer);
  }
  NK_LIB void
  start_popup(context* ctx, window* win) {
    NK_ASSERT(ctx);
    NK_ASSERT(win);
    if (!ctx || !win)
      return;

    /* save buffer fill state for popup */
    popup_buffer* buf = &win->popup.buf;
    buf->begin = win->buffer.end;
    buf->end = win->buffer.end;
    buf->parent = win->buffer.last;
    buf->last = buf->begin;
    buf->active = true;
  }
  NK_LIB void
  finish_popup(context* ctx, window* win) {
    NK_ASSERT(ctx);
    NK_ASSERT(win);
    if (!ctx || !win)
      return;

    popup_buffer* buf = &win->popup.buf;
    buf->last = win->buffer.last;
    buf->end = win->buffer.end;
  }
  NK_LIB void
  finish_buffer(context* ctx, command_buffer* buffer) {
    NK_ASSERT(ctx);
    NK_ASSERT(buffer);
    if (!ctx || !buffer)
      return;
    buffer->end = ctx->memory.allocated;
  }
  NK_LIB void
  finish(context* ctx, window* win) {

    NK_ASSERT(ctx);
    NK_ASSERT(win);
    if (!ctx || !win)
      return;
    finish_buffer(ctx, &win->buffer);
    if (!win->popup.buf.active)
      return;

    const popup_buffer* buf = &win->popup.buf;
    void* memory = ctx->memory.memory.ptr;
    command* parent_last = ptr_add(command, memory, buf->parent);
    parent_last->next = buf->end;
  }
  NK_LIB void
  build(context* ctx) {
    window* it = 0;
    command* cmd = 0;
    std::uint8_t* buffer = 0;

    /* draw cursor overlay */
    if (!ctx->style.cursor_active)
      ctx->style.cursor_active = ctx->style.cursors[static_cast<unsigned>(style_cursor::CURSOR_ARROW)];
    if (ctx->style.cursor_active && !ctx->input.mouse.grabbed && ctx->style.cursor_visible) {
      rectf mouse_bounds;
      const cursor* cursor = ctx->style.cursor_active;
      command_buffer_init(&ctx->overlay, &ctx->memory, NK_CLIPPING_OFF);
      start_buffer(ctx, &ctx->overlay);

      mouse_bounds.x = ctx->input.mouse.pos.x - cursor->offset.x;
      mouse_bounds.y = ctx->input.mouse.pos.y - cursor->offset.y;
      mouse_bounds.w = cursor->size.x;
      mouse_bounds.h = cursor->size.y;

      draw_image(&ctx->overlay, mouse_bounds, &cursor->img, white);
      finish_buffer(ctx, &ctx->overlay);
    }
    /* build one big draw command list out of all window buffers */
    it = ctx->begin;
    buffer = (std::uint8_t*) ctx->memory.memory.ptr;
    while (it != 0) {
      window* next = it->next;
      if (it->buffer.last == it->buffer.begin || (it->flags & static_cast<decltype(it->flags)>(window_flags::WINDOW_HIDDEN)) ||
          it->seq != ctx->seq)
        goto cont;

      cmd = ptr_add(command, buffer, it->buffer.last);
      while (next && ((next->buffer.last == next->buffer.begin) ||
                      (next->flags & static_cast<decltype(next->flags)>(window_flags::WINDOW_HIDDEN)) || next->seq != ctx->seq))
        next = next->next; /* skip empty command buffers */

      if (next)
        cmd->next = next->buffer.begin;
    cont:
      it = next;
    }
    /* append all popup draw commands into lists */
    it = ctx->begin;
    while (it != 0) {
      window* next = it->next;
      popup_buffer* buf;
      if (!it->popup.buf.active)
        goto skip;

      buf = &it->popup.buf;
      cmd->next = buf->begin;
      cmd = ptr_add(command, buffer, buf->last);
      buf->active = false;
    skip:
      it = next;
    }
    if (cmd) {
      /* append overlay commands */
      if (ctx->overlay.end != ctx->overlay.begin)
        cmd->next = ctx->overlay.begin;
      else
        cmd->next = ctx->memory.allocated;
    }
  }
  NK_API const command*
  _begin(context* ctx) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    if (!ctx->count)
      return 0;

    const std::uint8_t* buffer = (std::uint8_t*) ctx->memory.memory.ptr;
    if (!ctx->build) {
      build(ctx);
      ctx->build = true;
    }
    window* iter = ctx->begin;
    while (iter && ((iter->buffer.begin == iter->buffer.end) ||
                    (iter->flags & static_cast<decltype(iter->flags)>(window_flags::WINDOW_HIDDEN)) || iter->seq != ctx->seq))
      iter = iter->next;
    if (!iter)
      return 0;
    return ptr_add_const(command, buffer, iter->buffer.begin);
  }

  NK_API const command*
  _next(context* ctx, const command* cmd) {
    NK_ASSERT(ctx);
    if (!ctx || !cmd || !ctx->count)
      return 0;
    if (cmd->next >= ctx->memory.allocated)
      return 0;
    const std::uint8_t* buffer = (std::uint8_t*) ctx->memory.memory.ptr;
    const command* next = ptr_add_const(command, buffer, cmd->next);
    return next;
  }
} // namespace nk
