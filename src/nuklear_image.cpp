#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                          IMAGE
   *
   * ===============================================================*/
  NK_API resource_handle
  handle_ptr(void* ptr) {
    resource_handle handle = {0};
    handle.ptr = ptr;
    return handle;
  }
  NK_API resource_handle
  handle_id(int id) {
    resource_handle handle;
    zero_struct(handle);
    handle.id = id;
    return handle;
  }
  NK_API struct image
  subimage_ptr(void* ptr, unsigned short w, unsigned short h, struct rectf r) {
    struct image s;
    zero(&s, sizeof(s));
    s.handle.ptr = ptr;
    s.w = w;
    s.h = h;
    s.region[0] = (unsigned short) r.x;
    s.region[1] = (unsigned short) r.y;
    s.region[2] = (unsigned short) r.w;
    s.region[3] = (unsigned short) r.h;
    return s;
  }
  NK_API struct image
  subimage_id(int id, unsigned short w, unsigned short h, struct rectf r) {
    struct image s;
    zero(&s, sizeof(s));
    s.handle.id = id;
    s.w = w;
    s.h = h;
    s.region[0] = (unsigned short) r.x;
    s.region[1] = (unsigned short) r.y;
    s.region[2] = (unsigned short) r.w;
    s.region[3] = (unsigned short) r.h;
    return s;
  }
  NK_API struct image
  subimage_handle(resource_handle handle, unsigned short w, unsigned short h, struct rectf r) {
    struct image s;
    zero(&s, sizeof(s));
    s.handle = handle;
    s.w = w;
    s.h = h;
    s.region[0] = (unsigned short) r.x;
    s.region[1] = (unsigned short) r.y;
    s.region[2] = (unsigned short) r.w;
    s.region[3] = (unsigned short) r.h;
    return s;
  }
  NK_API struct image
  image_handle(resource_handle handle) {
    struct image s;
    zero(&s, sizeof(s));
    s.handle = handle;
    s.w = 0;
    s.h = 0;
    s.region[0] = 0;
    s.region[1] = 0;
    s.region[2] = 0;
    s.region[3] = 0;
    return s;
  }
  NK_API struct image
  image_ptr(void* ptr) {
    struct image s;
    zero(&s, sizeof(s));
    NK_ASSERT(ptr);
    s.handle.ptr = ptr;
    s.w = 0;
    s.h = 0;
    s.region[0] = 0;
    s.region[1] = 0;
    s.region[2] = 0;
    s.region[3] = 0;
    return s;
  }
  NK_API struct image
  image_id(int id) {
    struct image s;
    zero(&s, sizeof(s));
    s.handle.id = id;
    s.w = 0;
    s.h = 0;
    s.region[0] = 0;
    s.region[1] = 0;
    s.region[2] = 0;
    s.region[3] = 0;
    return s;
  }
  NK_API bool
  image_is_subimage(const struct image* img) {
    NK_ASSERT(img);
    return !(img->w == 0 && img->h == 0);
  }
  NK_API void
  image(struct context* ctx, struct image img) {
    struct window* win;
    struct rectf bounds;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    if (!widget(&bounds, ctx))
      return;
    draw_image(&win->buffer, bounds, &img, white);
  }
  NK_API void
  image_color(struct context* ctx, struct image img, struct color col) {
    struct window* win;
    struct rectf bounds;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    if (!widget(&bounds, ctx))
      return;
    draw_image(&win->buffer, bounds, &img, col);
  }
} // namespace nk
