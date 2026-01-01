#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                          9-SLICE
   *
   * ===============================================================*/
  NK_API nine_slice
  sub9slice_ptr(void* ptr, const unsigned short w, const unsigned short h, const rectf sub_region, const unsigned short l, const unsigned short t, const unsigned short r, const unsigned short b) {
    nine_slice s;
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
  NK_API nine_slice
  sub9slice_id(const int id, const unsigned short w, const unsigned short h, const rectf sub_region, const unsigned short l, const unsigned short t, const unsigned short r, const unsigned short b) {
    nine_slice s;
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
  NK_API nine_slice
  sub9slice_handle(const resource_handle handle, const unsigned short w, const unsigned short h, const rectf sub_region, const unsigned short l, const unsigned short t, const unsigned short r, const unsigned short b) {
    nine_slice s;
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
  NK_API nine_slice
  nine_slice_handle(const resource_handle handle, const unsigned short l, const unsigned short t, const unsigned short r, const unsigned short b) {
    nine_slice s;
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
  NK_API nine_slice
  nine_slice_ptr(void* ptr, const unsigned short l, const unsigned short t, const unsigned short r, const unsigned short b) {
    nine_slice s;
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
  NK_API nine_slice
  nine_slice_id(const int id, const unsigned short l, const unsigned short t, const unsigned short r, const unsigned short b) {
    nine_slice s;
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
  nine_slice_is_sub9slice(const nine_slice* img) {
    NK_ASSERT(img);
    return !(img->img.w == 0 && img->img.h == 0);
  }
} // namespace nk
