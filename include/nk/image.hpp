#ifndef NK_POWER_IMAGE_HPP
#define NK_POWER_IMAGE_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {

  /* =============================================================================
   *
   *                                  IMAGE
   *
   * ============================================================================= */
  resource_handle handle_ptr(void*);
  resource_handle handle_id(int);
  image image_handle(resource_handle);
  image image_ptr(void*);
  image image_id(int);
  bool image_is_subimage(const image* img);
  image subimage_ptr(void*, unsigned short w, unsigned short h, rectf sub_region);
  image subimage_id(int, unsigned short w, unsigned short h, rectf sub_region);
  image subimage_handle(resource_handle, unsigned short w, unsigned short h, rectf sub_region);
  /* =============================================================================
   *
   *                                  9-SLICE
   *
   * ============================================================================= */
  nine_slice nine_slice_handle(resource_handle, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
  nine_slice nine_slice_ptr(void*, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
  nine_slice nine_slice_id(int, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
  int nine_slice_is_sub9slice(const nine_slice* img);
  nine_slice sub9slice_ptr(void*, unsigned short w, unsigned short h, rectf sub_region, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
  nine_slice sub9slice_id(int, unsigned short w, unsigned short h, rectf sub_region, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
  nine_slice sub9slice_handle(resource_handle, unsigned short w, unsigned short h, rectf sub_region, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
}

#endif