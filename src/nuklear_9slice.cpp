#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                          9-SLICE
   *
   * ===============================================================*/
  NK_API struct nine_slice
  sub9slice_ptr(void* ptr, unsigned short w, unsigned short h, struct rectf sub_region, unsigned short l, unsigned short t, unsigned short r, unsigned short b) {
    struct nine_slice s;
    struct image* i = &s.img;
    zero(&s, sizeof(s));
    i->handle.ptr = ptr;
    i->w = w;
    i->h = h;
    i->region[0] = (unsigned short) sub_region.x;
    i->region[1] = (unsigned short) sub_region.y;
    i->region[2] = (unsigned short) sub_region.w;
    i->region[3] = (unsigned short) sub_region.h;
    s.l = l;
    s.t = t;
    s.r = r;
    s.b = b;
    return s;
  }
  NK_API struct nine_slice
  sub9slice_id(const int id, unsigned short w, unsigned short h, struct rectf sub_region, unsigned short l, unsigned short t, unsigned short r, unsigned short b) {
    struct nine_slice s;
    struct image* i = &s.img;
    zero(&s, sizeof(s));
    i->handle.id = id;
    i->w = w;
    i->h = h;
    i->region[0] = (unsigned short) sub_region.x;
    i->region[1] = (unsigned short) sub_region.y;
    i->region[2] = (unsigned short) sub_region.w;
    i->region[3] = (unsigned short) sub_region.h;
    s.l = l;
    s.t = t;
    s.r = r;
    s.b = b;
    return s;
  }
  NK_API struct nine_slice
  sub9slice_handle(resource_handle handle, unsigned short w, unsigned short h, struct rectf sub_region, unsigned short l, unsigned short t, unsigned short r, unsigned short b) {
    struct nine_slice s;
    struct image* i = &s.img;
    zero(&s, sizeof(s));
    i->handle = handle;
    i->w = w;
    i->h = h;
    i->region[0] = (unsigned short) sub_region.x;
    i->region[1] = (unsigned short) sub_region.y;
    i->region[2] = (unsigned short) sub_region.w;
    i->region[3] = (unsigned short) sub_region.h;
    s.l = l;
    s.t = t;
    s.r = r;
    s.b = b;
    return s;
  }
  NK_API struct nine_slice
  nine_slice_handle(const resource_handle handle, unsigned short l, unsigned short t, unsigned short r, unsigned short b) {
    struct nine_slice s;
    struct image* i = &s.img;
    zero(&s, sizeof(s));
    i->handle = handle;
    i->w = 0;
    i->h = 0;
    i->region[0] = 0;
    i->region[1] = 0;
    i->region[2] = 0;
    i->region[3] = 0;
    s.l = l;
    s.t = t;
    s.r = r;
    s.b = b;
    return s;
  }
  NK_API struct nine_slice
  nine_slice_ptr(void* ptr, unsigned short l, unsigned short t, unsigned short r, unsigned short b) {
    struct nine_slice s;
    struct image* i = &s.img;
    zero(&s, sizeof(s));
    NK_ASSERT(ptr);
    i->handle.ptr = ptr;
    i->w = 0;
    i->h = 0;
    i->region[0] = 0;
    i->region[1] = 0;
    i->region[2] = 0;
    i->region[3] = 0;
    s.l = l;
    s.t = t;
    s.r = r;
    s.b = b;
    return s;
  }
  NK_API struct nine_slice
  nine_slice_id(const int id, unsigned short l, unsigned short t, unsigned short r, unsigned short b) {
    struct nine_slice s;
    struct image* i = &s.img;
    zero(&s, sizeof(s));
    i->handle.id = id;
    i->w = 0;
    i->h = 0;
    i->region[0] = 0;
    i->region[1] = 0;
    i->region[2] = 0;
    i->region[3] = 0;
    s.l = l;
    s.t = t;
    s.r = r;
    s.b = b;
    return s;
  }
  NK_API int
  nine_slice_is_sub9slice(const struct nine_slice* img) {
    NK_ASSERT(img);
    return !(img->img.w == 0 && img->img.h == 0);
  }
} // namespace nk
