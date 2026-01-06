#ifndef NK_POWER_IMAGE_HPP
#define NK_POWER_IMAGE_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {

  /* =============================================================================
   *
   *                                  IMAGE
   *
   * ============================================================================= */
  NK_API resource_handle handle_ptr(void*);
  NK_API resource_handle handle_id(int);
  NK_API struct image image_handle(resource_handle);
  NK_API struct image image_ptr(void*);
  NK_API struct image image_id(int);
  NK_API bool image_is_subimage(const struct image* img);
  NK_API struct image subimage_ptr(void*, unsigned short w, unsigned short h, rectf sub_region);
  NK_API struct image subimage_id(int, unsigned short w, unsigned short h, rectf sub_region);
  NK_API struct image subimage_handle(resource_handle, unsigned short w, unsigned short h, rectf sub_region);
  /* =============================================================================
   *
   *                                  9-SLICE
   *
   * ============================================================================= */
  NK_API nine_slice nine_slice_handle(resource_handle, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
  NK_API nine_slice nine_slice_ptr(void*, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
  NK_API nine_slice nine_slice_id(int, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
  NK_API int nine_slice_is_sub9slice(const nine_slice* img);
  NK_API nine_slice sub9slice_ptr(void*, unsigned short w, unsigned short h, rectf sub_region, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
  NK_API nine_slice sub9slice_id(int, unsigned short w, unsigned short h, rectf sub_region, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
  NK_API nine_slice sub9slice_handle(resource_handle, unsigned short w, unsigned short h, rectf sub_region, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
}

#endif