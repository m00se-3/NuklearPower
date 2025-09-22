#include "nuklear.h"
#include "nuklear_internal.h"

/* ===============================================================
 *
 *                          9-SLICE
 *
 * ===============================================================*/
NK_API struct nk_nine_slice
nk_sub9slice_ptr(void *ptr, unsigned short w, unsigned short h, struct nk_rect rgn, unsigned short l, unsigned short t, unsigned short r, unsigned short b)
{
    struct nk_nine_slice s;
    struct nk_image *i = &s.img;
    nk_zero(&s, sizeof(s));
    i->handle.ptr = ptr;
    i->w = w; i->h = h;
    i->region[0] = (unsigned short)rgn.x;
    i->region[1] = (unsigned short)rgn.y;
    i->region[2] = (unsigned short)rgn.w;
    i->region[3] = (unsigned short)rgn.h;
    s.l = l; s.t = t; s.r = r; s.b = b;
    return s;
}
NK_API struct nk_nine_slice
nk_sub9slice_id(int id, unsigned short w, unsigned short h, struct nk_rect rgn, unsigned short l, unsigned short t, unsigned short r, unsigned short b)
{
    struct nk_nine_slice s;
    struct nk_image *i = &s.img;
    nk_zero(&s, sizeof(s));
    i->handle.id = id;
    i->w = w; i->h = h;
    i->region[0] = (unsigned short)rgn.x;
    i->region[1] = (unsigned short)rgn.y;
    i->region[2] = (unsigned short)rgn.w;
    i->region[3] = (unsigned short)rgn.h;
    s.l = l; s.t = t; s.r = r; s.b = b;
    return s;
}
NK_API struct nk_nine_slice
nk_sub9slice_handle(nk_handle handle, unsigned short w, unsigned short h, struct nk_rect rgn, unsigned short l, unsigned short t, unsigned short r, unsigned short b)
{
    struct nk_nine_slice s;
    struct nk_image *i = &s.img;
    nk_zero(&s, sizeof(s));
    i->handle = handle;
    i->w = w; i->h = h;
    i->region[0] = (unsigned short)rgn.x;
    i->region[1] = (unsigned short)rgn.y;
    i->region[2] = (unsigned short)rgn.w;
    i->region[3] = (unsigned short)rgn.h;
    s.l = l; s.t = t; s.r = r; s.b = b;
    return s;
}
NK_API struct nk_nine_slice
nk_nine_slice_handle(nk_handle handle, unsigned short l, unsigned short t, unsigned short r, unsigned short b)
{
    struct nk_nine_slice s;
    struct nk_image *i = &s.img;
    nk_zero(&s, sizeof(s));
    i->handle = handle;
    i->w = 0; i->h = 0;
    i->region[0] = 0;
    i->region[1] = 0;
    i->region[2] = 0;
    i->region[3] = 0;
    s.l = l; s.t = t; s.r = r; s.b = b;
    return s;
}
NK_API struct nk_nine_slice
nk_nine_slice_ptr(void *ptr, unsigned short l, unsigned short t, unsigned short r, unsigned short b)
{
    struct nk_nine_slice s;
    struct nk_image *i = &s.img;
    nk_zero(&s, sizeof(s));
    NK_ASSERT(ptr);
    i->handle.ptr = ptr;
    i->w = 0; i->h = 0;
    i->region[0] = 0;
    i->region[1] = 0;
    i->region[2] = 0;
    i->region[3] = 0;
    s.l = l; s.t = t; s.r = r; s.b = b;
    return s;
}
NK_API struct nk_nine_slice
nk_nine_slice_id(int id, unsigned short l, unsigned short t, unsigned short r, unsigned short b)
{
    struct nk_nine_slice s;
    struct nk_image *i = &s.img;
    nk_zero(&s, sizeof(s));
    i->handle.id = id;
    i->w = 0; i->h = 0;
    i->region[0] = 0;
    i->region[1] = 0;
    i->region[2] = 0;
    i->region[3] = 0;
    s.l = l; s.t = t; s.r = r; s.b = b;
    return s;
}
NK_API int
nk_nine_slice_is_sub9slice(const struct nk_nine_slice* slice)
{
    NK_ASSERT(slice);
    return !(slice->img.w == 0 && slice->img.h == 0);
}

