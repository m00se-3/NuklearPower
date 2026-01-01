#include <cstring>
#include <memory>
#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ==============================================================
   *
   *                          DRAW
   *
   * ===============================================================*/
  NK_LIB void
  command_buffer_init(command_buffer* cb,
                      memory_buffer* b, const command_clipping clip) {
    NK_ASSERT(cb);
    NK_ASSERT(b);
    if (!cb || !b)
      return;
    cb->base = b;
    cb->use_clipping = (int) clip;
    cb->begin = b->allocated;
    cb->end = b->allocated;
    cb->last = b->allocated;
  }
  NK_LIB void
  command_buffer_reset(command_buffer* b) {
    NK_ASSERT(b);
    if (!b)
      return;
    b->begin = 0;
    b->end = 0;
    b->last = 0;
    b->clip = null_rect;
#ifdef NK_INCLUDE_COMMAND_USERDATA
    b->userdata.ptr = 0;
#endif
  }
  NK_LIB void*
  command_buffer_push(command_buffer* b,
                      const command_type t, const std::size_t size) {
    NK_STORAGE const std::size_t align = alignof(command);

    NK_ASSERT(b);
    NK_ASSERT(b->base);
    if (!b)
      return 0;
    command* cmd = (command*) buffer_alloc(b->base, buffer_allocation_type::BUFFER_FRONT, size, align);
    if (!cmd)
      return 0;

    /* make sure the offset to the next command is aligned */
    b->last = (std::size_t) ((std::uint8_t*) cmd - (std::uint8_t*) b->base->memory.ptr);
    void* unaligned = (std::uint8_t*) cmd + size;
    void* memory = NK_ALIGN_PTR(unaligned, align);
    const std::size_t alignment = (std::size_t) ((std::uint8_t*) memory - (std::uint8_t*) unaligned);
#ifdef NK_ZERO_COMMAND_MEMORY
    NK_MEMSET(cmd, 0, size + alignment);
#endif

    cmd->type = t;
    cmd->next = b->base->allocated + alignment;
#ifdef NK_INCLUDE_COMMAND_USERDATA
    cmd->userdata = b->userdata;
#endif
    b->end = cmd->next;
    return cmd;
  }
  NK_API void
  push_scissor(command_buffer* b, const rectf r) {
    NK_ASSERT(b);
    if (!b)
      return;

    b->clip.x = r.x;
    b->clip.y = r.y;
    b->clip.w = r.w;
    b->clip.h = r.h;
    command_scissor* cmd = (command_scissor*)
        command_buffer_push(b, command_type::COMMAND_SCISSOR, sizeof(*cmd));

    if (!cmd)
      return;
    cmd->x = (short) r.x;
    cmd->y = (short) r.y;
    cmd->w = (unsigned short) NK_MAX(0, r.w);
    cmd->h = (unsigned short) NK_MAX(0, r.h);
  }
  NK_API void
  stroke_line(command_buffer* b, float x0, float y0,
              float x1, float y1, float line_thickness, const color c) {
    NK_ASSERT(b);
    if (!b || line_thickness <= 0)
      return;
    command_line* cmd = (command_line*)
        command_buffer_push(b, command_type::COMMAND_LINE, sizeof(*cmd));
    if (!cmd)
      return;
    cmd->line_thickness = (unsigned short) line_thickness;
    cmd->begin.x = (short) x0;
    cmd->begin.y = (short) y0;
    cmd->end.x = (short) x1;
    cmd->end.y = (short) y1;
    cmd->color = c;
  }
  NK_API void
  stroke_curve(command_buffer* b, float ax, float ay,
               float ctrl0x, float ctrl0y, float ctrl1x, float ctrl1y,
               float bx, float by, float line_thickness, const color col) {
    NK_ASSERT(b);
    if (!b || col.a == 0 || line_thickness <= 0)
      return;

    command_curve* cmd = (command_curve*)
        command_buffer_push(b, command_type::COMMAND_CURVE, sizeof(*cmd));
    if (!cmd)
      return;
    cmd->line_thickness = (unsigned short) line_thickness;
    cmd->begin.x = (short) ax;
    cmd->begin.y = (short) ay;
    cmd->ctrl[0].x = (short) ctrl0x;
    cmd->ctrl[0].y = (short) ctrl0y;
    cmd->ctrl[1].x = (short) ctrl1x;
    cmd->ctrl[1].y = (short) ctrl1y;
    cmd->end.x = (short) bx;
    cmd->end.y = (short) by;
    cmd->color = col;
  }
  NK_API void
  stroke_rect(command_buffer* b, const rectf rect,
              float rounding, float line_thickness, const color c) {
    NK_ASSERT(b);
    if (!b || c.a == 0 || rect.w == 0 || rect.h == 0 || line_thickness <= 0)
      return;
    if (b->use_clipping) {
      const rectf* clip = &b->clip;
      if (!intERSECT(rect.x, rect.y, rect.w, rect.h,
                     clip->x, clip->y, clip->w, clip->h))
        return;
    }
    command_rect* cmd = (command_rect*)
        command_buffer_push(b, command_type::COMMAND_RECT, sizeof(*cmd));
    if (!cmd)
      return;
    cmd->rounding = (unsigned short) rounding;
    cmd->line_thickness = (unsigned short) line_thickness;
    cmd->x = (short) rect.x;
    cmd->y = (short) rect.y;
    cmd->w = (unsigned short) NK_MAX(0, rect.w);
    cmd->h = (unsigned short) NK_MAX(0, rect.h);
    cmd->color = c;
  }
  NK_API void
  fill_rect(command_buffer* b, const rectf rect,
            float rounding, const color c) {
    NK_ASSERT(b);
    if (!b || c.a == 0 || rect.w == 0 || rect.h == 0)
      return;
    if (b->use_clipping) {
      const rectf* clip = &b->clip;
      if (!intERSECT(rect.x, rect.y, rect.w, rect.h,
                     clip->x, clip->y, clip->w, clip->h))
        return;
    }

    command_rect_filled* cmd = (command_rect_filled*)
        command_buffer_push(b, command_type::COMMAND_RECT_FILLED, sizeof(*cmd));
    if (!cmd)
      return;
    cmd->rounding = (unsigned short) rounding;
    cmd->x = (short) rect.x;
    cmd->y = (short) rect.y;
    cmd->w = (unsigned short) NK_MAX(0, rect.w);
    cmd->h = (unsigned short) NK_MAX(0, rect.h);
    cmd->color = c;
  }
  NK_API void
  fill_rect_multi_color(command_buffer* b, const rectf rect,
                        const color left, const color top, const color right,
                        const color bottom) {
    NK_ASSERT(b);
    if (!b || rect.w == 0 || rect.h == 0)
      return;
    if (b->use_clipping) {
      const rectf* clip = &b->clip;
      if (!intERSECT(rect.x, rect.y, rect.w, rect.h,
                     clip->x, clip->y, clip->w, clip->h))
        return;
    }

    command_rect_multi_color* cmd = (command_rect_multi_color*)
        command_buffer_push(b, command_type::COMMAND_RECT_MULTI_COLOR, sizeof(*cmd));
    if (!cmd)
      return;
    cmd->x = (short) rect.x;
    cmd->y = (short) rect.y;
    cmd->w = (unsigned short) NK_MAX(0, rect.w);
    cmd->h = (unsigned short) NK_MAX(0, rect.h);
    cmd->left = left;
    cmd->top = top;
    cmd->right = right;
    cmd->bottom = bottom;
  }
  NK_API void
  stroke_circle(command_buffer* b, const rectf r,
                float line_thickness, const color c) {
    if (!b || r.w == 0 || r.h == 0 || line_thickness <= 0)
      return;
    if (b->use_clipping) {
      const rectf* clip = &b->clip;
      if (!intERSECT(r.x, r.y, r.w, r.h, clip->x, clip->y, clip->w, clip->h))
        return;
    }

    command_circle* cmd = (command_circle*)
        command_buffer_push(b, command_type::COMMAND_CIRCLE, sizeof(*cmd));
    if (!cmd)
      return;
    cmd->line_thickness = (unsigned short) line_thickness;
    cmd->x = (short) r.x;
    cmd->y = (short) r.y;
    cmd->w = (unsigned short) NK_MAX(r.w, 0);
    cmd->h = (unsigned short) NK_MAX(r.h, 0);
    cmd->color = c;
  }
  NK_API void
  fill_circle(command_buffer* b, const rectf r, const color c) {
    NK_ASSERT(b);
    if (!b || c.a == 0 || r.w == 0 || r.h == 0)
      return;
    if (b->use_clipping) {
      const rectf* clip = &b->clip;
      if (!intERSECT(r.x, r.y, r.w, r.h, clip->x, clip->y, clip->w, clip->h))
        return;
    }

    command_circle_filled* cmd = (command_circle_filled*)
        command_buffer_push(b, command_type::COMMAND_CIRCLE_FILLED, sizeof(*cmd));
    if (!cmd)
      return;
    cmd->x = (short) r.x;
    cmd->y = (short) r.y;
    cmd->w = (unsigned short) NK_MAX(r.w, 0);
    cmd->h = (unsigned short) NK_MAX(r.h, 0);
    cmd->color = c;
  }
  NK_API void
  stroke_arc(command_buffer* b, float cx, float cy, float radius,
             float a_min, float a_max, float line_thickness, const color c) {
    if (!b || c.a == 0 || line_thickness <= 0)
      return;
    command_arc* cmd = (command_arc*)
        command_buffer_push(b, command_type::COMMAND_ARC, sizeof(*cmd));
    if (!cmd)
      return;
    cmd->line_thickness = (unsigned short) line_thickness;
    cmd->cx = (short) cx;
    cmd->cy = (short) cy;
    cmd->r = (unsigned short) radius;
    cmd->a[0] = a_min;
    cmd->a[1] = a_max;
    cmd->color = c;
  }
  NK_API void
  fill_arc(command_buffer* b, float cx, float cy, float radius,
           float a_min, float a_max, const color c) {
    NK_ASSERT(b);
    if (!b || c.a == 0)
      return;
    command_arc_filled* cmd = (command_arc_filled*)
        command_buffer_push(b, command_type::COMMAND_ARC_FILLED, sizeof(*cmd));
    if (!cmd)
      return;
    cmd->cx = (short) cx;
    cmd->cy = (short) cy;
    cmd->r = (unsigned short) radius;
    cmd->a[0] = a_min;
    cmd->a[1] = a_max;
    cmd->color = c;
  }
  NK_API void
  stroke_triangle(command_buffer* b, float x0, float y0, float x1,
                  float y1, float x2, float y2, float line_thickness, const color c) {
    NK_ASSERT(b);
    if (!b || c.a == 0 || line_thickness <= 0)
      return;
    if (b->use_clipping) {
      const rectf* clip = &b->clip;
      if (!NK_INBOX(x0, y0, clip->x, clip->y, clip->w, clip->h) &&
          !NK_INBOX(x1, y1, clip->x, clip->y, clip->w, clip->h) &&
          !NK_INBOX(x2, y2, clip->x, clip->y, clip->w, clip->h))
        return;
    }

    command_triangle* cmd = (command_triangle*)
        command_buffer_push(b, command_type::COMMAND_TRIANGLE, sizeof(*cmd));
    if (!cmd)
      return;
    cmd->line_thickness = (unsigned short) line_thickness;
    cmd->a.x = (short) x0;
    cmd->a.y = (short) y0;
    cmd->b.x = (short) x1;
    cmd->b.y = (short) y1;
    cmd->c.x = (short) x2;
    cmd->c.y = (short) y2;
    cmd->color = c;
  }
  NK_API void
  fill_triangle(command_buffer* b, float x0, float y0, float x1,
                float y1, float x2, float y2, const color c) {
    NK_ASSERT(b);
    if (!b || c.a == 0)
      return;
    if (!b)
      return;
    if (b->use_clipping) {
      const rectf* clip = &b->clip;
      if (!NK_INBOX(x0, y0, clip->x, clip->y, clip->w, clip->h) &&
          !NK_INBOX(x1, y1, clip->x, clip->y, clip->w, clip->h) &&
          !NK_INBOX(x2, y2, clip->x, clip->y, clip->w, clip->h))
        return;
    }

    command_triangle_filled* cmd = (command_triangle_filled*)
        command_buffer_push(b, command_type::COMMAND_TRIANGLE_FILLED, sizeof(*cmd));
    if (!cmd)
      return;
    cmd->a.x = (short) x0;
    cmd->a.y = (short) y0;
    cmd->b.x = (short) x1;
    cmd->b.y = (short) y1;
    cmd->c.x = (short) x2;
    cmd->c.y = (short) y2;
    cmd->color = c;
  }
  NK_API void
  stroke_polygon(command_buffer* b, const float* points, const int point_count,
                 float line_thickness, const color col) {
    std::size_t size = 0;
    command_polygon* cmd;

    NK_ASSERT(b);
    if (!b || col.a == 0 || line_thickness <= 0)
      return;
    size = sizeof(*cmd) + sizeof(short) * 2 * (std::size_t) point_count;
    cmd = (command_polygon*) command_buffer_push(b, command_type::COMMAND_POLYGON, size);
    if (!cmd)
      return;
    cmd->color = col;
    cmd->line_thickness = (unsigned short) line_thickness;
    cmd->point_count = (unsigned short) point_count;
    for (int i = 0; i < point_count; ++i) {
      cmd->points[i].x = (short) points[i * 2];
      cmd->points[i].y = (short) points[i * 2 + 1];
    }
  }
  NK_API void
  fill_polygon(command_buffer* b, const float* points, const int point_count,
               const color col) {
    std::size_t size = 0;
    command_polygon_filled* cmd;

    NK_ASSERT(b);
    if (!b || col.a == 0)
      return;
    size = sizeof(*cmd) + sizeof(short) * 2 * (std::size_t) point_count;
    cmd = (command_polygon_filled*)
        command_buffer_push(b, command_type::COMMAND_POLYGON_FILLED, size);
    if (!cmd)
      return;
    cmd->color = col;
    cmd->point_count = (unsigned short) point_count;
    for (int i = 0; i < point_count; ++i) {
      cmd->points[i].x = (short) points[i * 2 + 0];
      cmd->points[i].y = (short) points[i * 2 + 1];
    }
  }
  NK_API void
  stroke_polyline(command_buffer* b, const float* points, const int point_count,
                  float line_thickness, const color col) {
    std::size_t size = 0;
    command_polyline* cmd;

    NK_ASSERT(b);
    if (!b || col.a == 0 || line_thickness <= 0)
      return;
    size = sizeof(*cmd) + sizeof(short) * 2 * (std::size_t) point_count;
    cmd = (command_polyline*) command_buffer_push(b, command_type::COMMAND_POLYLINE, size);
    if (!cmd)
      return;
    cmd->color = col;
    cmd->point_count = (unsigned short) point_count;
    cmd->line_thickness = (unsigned short) line_thickness;
    for (int i = 0; i < point_count; ++i) {
      cmd->points[i].x = (short) points[i * 2];
      cmd->points[i].y = (short) points[i * 2 + 1];
    }
  }
  NK_API void
  draw_image(command_buffer* b, const rectf r,
             const struct image* img, const color col) {
    NK_ASSERT(b);
    if (!b)
      return;
    if (b->use_clipping) {
      const rectf* c = &b->clip;
      if (c->w == 0 || c->h == 0 || !intERSECT(r.x, r.y, r.w, r.h, c->x, c->y, c->w, c->h))
        return;
    }

    command_image* cmd = (command_image*)
        command_buffer_push(b, command_type::COMMAND_IMAGE, sizeof(*cmd));
    if (!cmd)
      return;
    cmd->x = (short) r.x;
    cmd->y = (short) r.y;
    cmd->w = (unsigned short) NK_MAX(0, r.w);
    cmd->h = (unsigned short) NK_MAX(0, r.h);
    cmd->img = *img;
    cmd->col = col;
  }
  NK_API void
  draw_nine_slice(command_buffer* b, const rectf r,
                  const nine_slice* slc, const color col) {
    struct image img;
    const struct image* slcimg = (const struct image*) slc;
    const unsigned short rgnX = slcimg->region[0];
    const unsigned short rgnY = slcimg->region[1];
    const unsigned short rgnW = slcimg->region[2];
    const unsigned short rgnH = slcimg->region[3];

    /* top-left */
    img.handle = slcimg->handle;
    img.w = slcimg->w;
    img.h = slcimg->h;
    img.region[0] = rgnX;
    img.region[1] = rgnY;
    img.region[2] = slc->l;
    img.region[3] = slc->t;

    draw_image(b,
               rect(r.x, r.y, (float) slc->l, (float) slc->t),
               &img, col);

#define IMG_RGN(x, y, w, h)             \
  img.region[0] = (unsigned short) (x); \
  img.region[1] = (unsigned short) (y); \
  img.region[2] = (unsigned short) (w); \
  img.region[3] = (unsigned short) (h);

    /* top-center */
    IMG_RGN(rgnX + slc->l, rgnY, rgnW - slc->l - slc->r, slc->t);
    draw_image(b,
               rect(r.x + (float) slc->l, r.y, (float) (r.w - slc->l - slc->r), (float) slc->t),
               &img, col);

    /* top-right */
    IMG_RGN(rgnX + rgnW - slc->r, rgnY, slc->r, slc->t);
    draw_image(b,
               rect(r.x + r.w - (float) slc->r, r.y, (float) slc->r, (float) slc->t),
               &img, col);

    /* center-left */
    IMG_RGN(rgnX, rgnY + slc->t, slc->l, rgnH - slc->t - slc->b);
    draw_image(b,
               rect(r.x, r.y + (float) slc->t, (float) slc->l, (float) (r.h - slc->t - slc->b)),
               &img, col);

    /* center */
    IMG_RGN(rgnX + slc->l, rgnY + slc->t, rgnW - slc->l - slc->r, rgnH - slc->t - slc->b);
    draw_image(b,
               rect(r.x + (float) slc->l, r.y + (float) slc->t, (float) (r.w - slc->l - slc->r), (float) (r.h - slc->t - slc->b)),
               &img, col);

    /* center-right */
    IMG_RGN(rgnX + rgnW - slc->r, rgnY + slc->t, slc->r, rgnH - slc->t - slc->b);
    draw_image(b,
               rect(r.x + r.w - (float) slc->r, r.y + (float) slc->t, (float) slc->r, (float) (r.h - slc->t - slc->b)),
               &img, col);

    /* bottom-left */
    IMG_RGN(rgnX, rgnY + rgnH - slc->b, slc->l, slc->b);
    draw_image(b,
               rect(r.x, r.y + r.h - (float) slc->b, (float) slc->l, (float) slc->b),
               &img, col);

    /* bottom-center */
    IMG_RGN(rgnX + slc->l, rgnY + rgnH - slc->b, rgnW - slc->l - slc->r, slc->b);
    draw_image(b,
               rect(r.x + (float) slc->l, r.y + r.h - (float) slc->b, (float) (r.w - slc->l - slc->r), (float) slc->b),
               &img, col);

    /* bottom-right */
    IMG_RGN(rgnX + rgnW - slc->r, rgnY + rgnH - slc->b, slc->r, slc->b);
    draw_image(b,
               rect(r.x + r.w - (float) slc->r, r.y + r.h - (float) slc->b, (float) slc->r, (float) slc->b),
               &img, col);

#undef IMG_RGN
  }
  NK_API void
  push_custom(command_buffer* b, const rectf r,
              const command_custom_callback cb, const resource_handle usr) {
    NK_ASSERT(b);
    if (!b)
      return;
    if (b->use_clipping) {
      const rectf* c = &b->clip;
      if (c->w == 0 || c->h == 0 || !intERSECT(r.x, r.y, r.w, r.h, c->x, c->y, c->w, c->h))
        return;
    }

    command_custom* cmd = (command_custom*)
        command_buffer_push(b, command_type::COMMAND_CUSTOM, sizeof(*cmd));
    if (!cmd)
      return;
    cmd->x = (short) r.x;
    cmd->y = (short) r.y;
    cmd->w = (unsigned short) NK_MAX(0, r.w);
    cmd->h = (unsigned short) NK_MAX(0, r.h);
    cmd->callback_data = usr;
    cmd->callback = cb;
  }
  NK_API void
  draw_text(command_buffer* b, const rectf r,
            const char* string, int length, const user_font* font,
            const color bg, const color fg) {
    float text_width = 0;

    NK_ASSERT(b);
    NK_ASSERT(font);
    if (!b || !string || !length || (bg.a == 0 && fg.a == 0))
      return;
    if (b->use_clipping) {
      const rectf* c = &b->clip;
      if (c->w == 0 || c->h == 0 || !intERSECT(r.x, r.y, r.w, r.h, c->x, c->y, c->w, c->h))
        return;
    }

    /* make sure text fits inside bounds */
    text_width = font->width(font->userdata, font->height, string, length);
    if (text_width > r.w) {
      int glyphs = 0;
      float txt_width = (float) text_width;
      length = text_clamp(font, string, length, r.w, &glyphs, &txt_width, 0, 0);
    }

    if (!length)
      return;
    command_text* cmd = (command_text*)
        command_buffer_push(b, command_type::COMMAND_TEXT, sizeof(*cmd) + (std::size_t) (length + 1));
    if (!cmd)
      return;
    cmd->x = (short) r.x;
    cmd->y = (short) r.y;
    cmd->w = (unsigned short) r.w;
    cmd->h = (unsigned short) r.h;
    cmd->background = bg;
    cmd->foreground = fg;
    cmd->font = font;
    cmd->length = length;
    cmd->height = font->height;
    std::memcpy(cmd->string, string, (std::size_t) length);
    cmd->string[length] = '\0';
  }
} // namespace nk
