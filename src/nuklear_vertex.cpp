#include <cmath>
#include <cstring>
#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                              VERTEX
   *
   * ===============================================================*/
#ifndef NK_INV_SQRT
#define NK_INV_SQRT inv_sqrt
  NK_LIB float
  inv_sqrt(float n) {
    const float threehalfs = 1.5f;
    union {
      unsigned int i;
      float f;
    } conv = {0};
    conv.f = n;
    float x2 = n * 0.5f;
    conv.i = 0x5f375A84 - (conv.i >> 1);
    conv.f = conv.f * (threehalfs - (x2 * conv.f * conv.f));
    return conv.f;
  }
#endif
#ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT
  NK_API void
  draw_list_init(struct draw_list* list) {
    std::size_t i = 0;
    NK_ASSERT(list);
    if (!list)
      return;
    zero(list, sizeof(*list));
    for (i = 0; i < NK_LEN(list->circle_vtx); ++i) {
      const float a = ((float) i / (float) NK_LEN(list->circle_vtx)) * 2 * NK_PI;
      list->circle_vtx[i].x = std::cosf(a);
      list->circle_vtx[i].y = std::sinf(a);
    }
  }
  NK_API void
  draw_list_setup(struct draw_list* canvas, const struct convert_config* config,
                  struct buffer* cmds, struct buffer* vertices, struct buffer* elements,
                  enum anti_aliasing line_aa, enum anti_aliasing shape_aa) {
    NK_ASSERT(canvas);
    NK_ASSERT(config);
    NK_ASSERT(cmds);
    NK_ASSERT(vertices);
    NK_ASSERT(elements);
    if (!canvas || !config || !cmds || !vertices || !elements)
      return;

    canvas->buffer = cmds;
    canvas->config = *config;
    canvas->elements = elements;
    canvas->vertices = vertices;
    canvas->line_AA = line_aa;
    canvas->shape_AA = shape_aa;
    canvas->clip_rect = null_rect;

    canvas->cmd_offset = 0;
    canvas->element_count = 0;
    canvas->vertex_count = 0;
    canvas->cmd_offset = 0;
    canvas->cmd_count = 0;
    canvas->path_count = 0;
  }
  NK_API const struct draw_command*
  _draw_list_begin(const struct draw_list* canvas, const struct buffer* buffer) {
    std::byte* memory;
    std::size_t offset;
    const struct draw_command* cmd;

    NK_ASSERT(buffer);
    if (!buffer || !buffer->size || !canvas->cmd_count)
      return 0;

    memory = (std::byte*) buffer->memory.ptr;
    offset = buffer->memory.size - canvas->cmd_offset;
    cmd = std::uintptr_t_add(const struct draw_command, memory, offset);
    return cmd;
  }
  NK_API const struct draw_command*
  _draw_list_end(const struct draw_list* canvas, const struct buffer* buffer) {
    std::size_t size;
    std::size_t offset;
    std::byte* memory;
    const struct draw_command* end;

    NK_ASSERT(buffer);
    NK_ASSERT(canvas);
    if (!buffer || !canvas)
      return 0;

    memory = (std::byte*) buffer->memory.ptr;
    size = buffer->memory.size;
    offset = size - canvas->cmd_offset;
    end = std::uintptr_t_add(const struct draw_command, memory, offset);
    end -= (canvas->cmd_count - 1);
    return end;
  }
  NK_API const struct draw_command*
  _draw_list_next(const struct draw_command* cmd,
                  const struct buffer* buffer, const struct draw_list* canvas) {
    const struct draw_command* end;
    NK_ASSERT(buffer);
    NK_ASSERT(canvas);
    if (!cmd || !buffer || !canvas)
      return 0;

    end = _draw_list_end(canvas, buffer);
    if (cmd <= end)
      return 0;
    return (cmd - 1);
  }
  INTERN vec2f*
  draw_list_alloc_path(struct draw_list* list, int count) {
    vec2f* points;
    NK_STORAGE const std::size_t point_align = alignof(vec2f);
    NK_STORAGE const std::size_t point_size = sizeof(vec2f);
    points = (vec2f*)
        buffer_alloc(list->buffer, NK_BUFFER_FRONT,
                     point_size * (std::size_t) count, point_align);

    if (!points)
      return 0;
    if (!list->path_offset) {
      void* memory = buffer_memory(list->buffer);
      list->path_offset = (unsigned int) ((std::byte*) points - (std::byte*) memory);
    }
    list->path_count += (unsigned int) count;
    return points;
  }
  INTERN vec2f
  draw_list_path_last(struct draw_list* list) {
    void* memory;
    vec2f* point;
    NK_ASSERT(list->path_count);
    memory = buffer_memory(list->buffer);
    point = std::uintptr_t_add(vec2f, memory, list->path_offset);
    point += (list->path_count - 1);
    return *point;
  }
  INTERN struct draw_command*
  draw_list_push_command(struct draw_list* list, rectf clip,
                         handle texture) {
    NK_STORAGE const std::size_t cmd_align = alignof(struct draw_command);
    NK_STORAGE const std::size_t cmd_size = sizeof(struct draw_command);
    struct draw_command* cmd;

    NK_ASSERT(list);
    cmd = (struct draw_command*)
        buffer_alloc(list->buffer, NK_BUFFER_BACK, cmd_size, cmd_align);

    if (!cmd)
      return 0;
    if (!list->cmd_count) {
      std::byte* memory = (std::byte*) buffer_memory(list->buffer);
      std::size_t total = buffer_total(list->buffer);
      memory = std::uintptr_t_add(std::byte, memory, total);
      list->cmd_offset = (std::size_t) (memory - (std::byte*) cmd);
    }

    cmd->elem_count = 0;
    cmd->clip_rect = clip;
    cmd->texture = texture;
#ifdef NK_INCLUDE_COMMAND_USERDATA
    cmd->userdata = list->userdata;
#endif

    list->cmd_count++;
    list->clip_rect = clip;
    return cmd;
  }
  INTERN struct draw_command*
  draw_list_command_last(struct draw_list* list) {
    void* memory;
    std::size_t size;
    struct draw_command* cmd;
    NK_ASSERT(list->cmd_count);

    memory = buffer_memory(list->buffer);
    size = buffer_total(list->buffer);
    cmd = std::uintptr_t_add(struct draw_command, memory, size - list->cmd_offset);
    return (cmd - (list->cmd_count - 1));
  }
  INTERN void
  draw_list_add_clip(struct draw_list* list, rectf rect) {
    NK_ASSERT(list);
    if (!list)
      return;
    if (!list->cmd_count) {
      draw_list_push_command(list, rect, list->config.tex_null.texture);
    } else {
      struct draw_command* prev = draw_list_command_last(list);
      if (prev->elem_count == 0)
        prev->clip_rect = rect;
      draw_list_push_command(list, rect, prev->texture);
    }
  }
  INTERN void
  draw_list_push_image(struct draw_list* list, handle texture) {
    NK_ASSERT(list);
    if (!list)
      return;
    if (!list->cmd_count) {
      draw_list_push_command(list, null_rect, texture);
    } else {
      struct draw_command* prev = draw_list_command_last(list);
      if (prev->elem_count == 0) {
        prev->texture = texture;
#ifdef NK_INCLUDE_COMMAND_USERDATA
        prev->userdata = list->userdata;
#endif
      } else if (prev->texture.id != texture.id
#ifdef NK_INCLUDE_COMMAND_USERDATA
                 || prev->userdata.id != list->userdata.id
#endif
      ) {
        draw_list_push_command(list, prev->clip_rect, texture);
      }
    }
  }
#ifdef NK_INCLUDE_COMMAND_USERDATA
  NK_API void
  draw_list_push_userdata(struct draw_list* list, handle userdata) {
    list->userdata = userdata;
  }
#endif
  INTERN void*
  draw_list_alloc_vertices(struct draw_list* list, std::size_t count) {
    void* vtx;
    NK_ASSERT(list);
    if (!list)
      return 0;
    vtx = buffer_alloc(list->vertices, NK_BUFFER_FRONT,
                       list->config.vertex_size * count, list->config.vertex_alignment);
    if (!vtx)
      return 0;
    list->vertex_count += (unsigned int) count;

    /* This assert triggers because your are drawing a lot of stuff and nuklear
     * defined `draw_index` as `unsigned short` to safe space be default.
     *
     * So you reached the maximum number of indices or rather vertexes.
     * To solve this issue please change typedef `draw_index` to `std::uint32_t`
     * and don't forget to specify the new element size in your drawing
     * backend (OpenGL, DirectX, ...). For example in OpenGL for `glDrawElements`
     * instead of specifying `GL_UNSIGNED_SHORT` you have to define `GL_UNSIGNED_INT`.
     * Sorry for the inconvenience. */
    if (sizeof(draw_index) == 2)
      NK_ASSERT((list->vertex_count < unsigned short_MAX &&
                 "To many vertices for 16-bit vertex indices. Please read comment above on how to solve this problem"));
    return vtx;
  }
  INTERN draw_index*
  draw_list_alloc_elements(struct draw_list* list, std::size_t count) {
    draw_index* ids;
    struct draw_command* cmd;
    NK_STORAGE const std::size_t elem_align = alignof(draw_index);
    NK_STORAGE const std::size_t elem_size = sizeof(draw_index);
    NK_ASSERT(list);
    if (!list)
      return 0;

    ids = (draw_index*)
        buffer_alloc(list->elements, NK_BUFFER_FRONT, elem_size * count, elem_align);
    if (!ids)
      return 0;
    cmd = draw_list_command_last(list);
    list->element_count += (unsigned int) count;
    cmd->elem_count += (unsigned int) count;
    return ids;
  }
  INTERN int
  draw_vertex_layout_element_is_end_of_layout(
      const struct draw_vertex_layout_element* element) {
    return (element->attribute == NK_VERTEX_ATTRIBUTE_COUNT ||
            element->format == NK_FORMAT_COUNT);
  }
  INTERN void
  draw_vertex_color(void* attr, const float* vals,
                    enum draw_vertex_layout_format format) {
    /* if this triggers you tried to provide a value format for a color */
    float val[4];
    NK_ASSERT(format >= NK_FORMAT_COLOR_BEGIN);
    NK_ASSERT(format <= NK_FORMAT_COLOR_END);
    if (format < NK_FORMAT_COLOR_BEGIN || format > NK_FORMAT_COLOR_END)
      return;

    val[0] = NK_SATURATE(vals[0]);
    val[1] = NK_SATURATE(vals[1]);
    val[2] = NK_SATURATE(vals[2]);
    val[3] = NK_SATURATE(vals[3]);

    switch (format) {
      default:
        NK_ASSERT(0 && "Invalid vertex layout color format");
        break;
      case NK_FORMAT_R8G8B8A8:
      case NK_FORMAT_R8G8B8: {
        struct color col = rgba_fv(val);
        std::memcpy(attr, &col.r, sizeof(col));
      } break;
      case NK_FORMAT_B8G8R8A8: {
        struct color col = rgba_fv(val);
        struct color bgra = rgba(col.b, col.g, col.r, col.a);
        std::memcpy(attr, &bgra, sizeof(bgra));
      } break;
      case NK_FORMAT_R16G15B16: {
        unsigned short col[3];
        col[0] = (unsigned short) (val[0] * (float) unsigned short_MAX);
        col[1] = (unsigned short) (val[1] * (float) unsigned short_MAX);
        col[2] = (unsigned short) (val[2] * (float) unsigned short_MAX);
        std::memcpy(attr, col, sizeof(col));
      } break;
      case NK_FORMAT_R16G15B16A16: {
        unsigned short col[4];
        col[0] = (unsigned short) (val[0] * (float) unsigned short_MAX);
        col[1] = (unsigned short) (val[1] * (float) unsigned short_MAX);
        col[2] = (unsigned short) (val[2] * (float) unsigned short_MAX);
        col[3] = (unsigned short) (val[3] * (float) unsigned short_MAX);
        std::memcpy(attr, col, sizeof(col));
      } break;
      case NK_FORMAT_R32G32B32: {
        std::uint32_t col[3];
        col[0] = (std::uint32_t) (val[0] * (float) unsigned int_MAX);
        col[1] = (std::uint32_t) (val[1] * (float) unsigned int_MAX);
        col[2] = (std::uint32_t) (val[2] * (float) unsigned int_MAX);
        std::memcpy(attr, col, sizeof(col));
      } break;
      case NK_FORMAT_R32G32B32A32: {
        std::uint32_t col[4];
        col[0] = (std::uint32_t) (val[0] * (float) unsigned int_MAX);
        col[1] = (std::uint32_t) (val[1] * (float) unsigned int_MAX);
        col[2] = (std::uint32_t) (val[2] * (float) unsigned int_MAX);
        col[3] = (std::uint32_t) (val[3] * (float) unsigned int_MAX);
        std::memcpy(attr, col, sizeof(col));
      } break;
      case NK_FORMAT_R32G32B32A32_FLOAT:
        std::memcpy(attr, val, sizeof(float) * 4);
        break;
      case NK_FORMAT_R32G32B32A32_DOUBLE: {
        double col[4];
        col[0] = (double) val[0];
        col[1] = (double) val[1];
        col[2] = (double) val[2];
        col[3] = (double) val[3];
        std::memcpy(attr, col, sizeof(col));
      } break;
      case NK_FORMAT_RGB32:
      case NK_FORMAT_RGBA32: {
        struct color col = rgba_fv(val);
        std::uint32_t color = color_u32(col);
        std::memcpy(attr, &color, sizeof(color));
      } break;
    }
  }
  INTERN void
  draw_vertex_element(void* dst, const float* values, int value_count,
                      enum draw_vertex_layout_format format) {
    int value_index;
    void* attribute = dst;
    /* if this triggers you tried to provide a color format for a value */
    NK_ASSERT(format < NK_FORMAT_COLOR_BEGIN);
    if (format >= NK_FORMAT_COLOR_BEGIN && format <= NK_FORMAT_COLOR_END)
      return;
    for (value_index = 0; value_index < value_count; ++value_index) {
      switch (format) {
        default:
          NK_ASSERT(0 && "invalid vertex layout format");
          break;
        case NK_FORMAT_SCHAR: {
          char value = (char) NK_CLAMP((float) NK_SCHAR_MIN, values[value_index], (float) NK_SCHAR_MAX);
          std::memcpy(attribute, &value, sizeof(value));
          attribute = (void*) ((char*) attribute + sizeof(char));
        } break;
        case NK_FORMAT_SSHORT: {
          short value = (short) NK_CLAMP((float) NK_SSHORT_MIN, values[value_index], (float) NK_SSHORT_MAX);
          std::memcpy(attribute, &value, sizeof(value));
          attribute = (void*) ((char*) attribute + sizeof(value));
        } break;
        case NK_FORMAT_SINT: {
          int value = (int) NK_CLAMP((float) NK_SINT_MIN, values[value_index], (float) NK_SINT_MAX);
          std::memcpy(attribute, &value, sizeof(value));
          attribute = (void*) ((char*) attribute + sizeof(int));
        } break;
        case NK_FORMAT_UCHAR: {
          unsigned char value = (unsigned char) NK_CLAMP((float) NK_UCHAR_MIN, values[value_index], (float) NK_UCHAR_MAX);
          std::memcpy(attribute, &value, sizeof(value));
          attribute = (void*) ((char*) attribute + sizeof(unsigned char));
        } break;
        case NK_FORMAT_USHORT: {
          unsigned short value = (unsigned short) NK_CLAMP((float) unsigned short_MIN, values[value_index], (float) unsigned short_MAX);
          std::memcpy(attribute, &value, sizeof(value));
          attribute = (void*) ((char*) attribute + sizeof(value));
        } break;
        case NK_FORMAT_UINT: {
          std::uint32_t value = (std::uint32_t) NK_CLAMP((float) unsigned int_MIN, values[value_index], (float) unsigned int_MAX);
          std::memcpy(attribute, &value, sizeof(value));
          attribute = (void*) ((char*) attribute + sizeof(std::uint32_t));
        } break;
        case NK_FORMAT_FLOAT:
          std::memcpy(attribute, &values[value_index], sizeof(values[value_index]));
          attribute = (void*) ((char*) attribute + sizeof(float));
          break;
        case NK_FORMAT_DOUBLE: {
          double value = (double) values[value_index];
          std::memcpy(attribute, &value, sizeof(value));
          attribute = (void*) ((char*) attribute + sizeof(double));
        } break;
      }
    }
  }
  INTERN void*
  draw_vertex(void* dst, const struct convert_config* config,
              vec2f pos, vec2f uv, struct colorf color) {
    void* result = (void*) ((char*) dst + config->vertex_size);
    const struct draw_vertex_layout_element* elem_iter = config->vertex_layout;
    while (!draw_vertex_layout_element_is_end_of_layout(elem_iter)) {
      void* address = (void*) ((char*) dst + elem_iter->offset);
      switch (elem_iter->attribute) {
        case NK_VERTEX_ATTRIBUTE_COUNT:
        default:
          NK_ASSERT(0 && "wrong element attribute");
          break;
        case NK_VERTEX_POSITION:
          draw_vertex_element(address, &pos.x, 2, elem_iter->format);
          break;
        case NK_VERTEX_TEXCOORD:
          draw_vertex_element(address, &uv.x, 2, elem_iter->format);
          break;
        case NK_VERTEX_COLOR:
          draw_vertex_color(address, &color.r, elem_iter->format);
          break;
      }
      elem_iter++;
    }
    return result;
  }
  NK_API void
  draw_list_stroke_poly_line(struct draw_list* list, const vec2f* points,
                             const unsigned int points_count, struct color color, enum draw_list_stroke closed,
                             float thickness, enum anti_aliasing aliasing) {
    std::size_t count;
    int thick_line;
    struct colorf col;
    struct colorf col_trans;
    NK_ASSERT(list);
    if (!list || points_count < 2)
      return;

    color.a = (std::uint8_t) ((float) color.a * list->config.global_alpha);
    count = points_count;
    if (!closed)
      count = points_count - 1;
    thick_line = thickness > 1.0f;

#ifdef NK_INCLUDE_COMMAND_USERDATA
    draw_list_push_userdata(list, list->userdata);
#endif

    color.a = (std::uint8_t) ((float) color.a * list->config.global_alpha);
    color_fv(&col.r, color);
    col_trans = col;
    col_trans.a = 0;

    if (aliasing == NK_ANTI_ALIASING_ON) {
      /* ANTI-ALIASED STROKE */
      const float AA_SIZE = 1.0f;
      NK_STORAGE const std::size_t pnt_align = alignof(vec2f);
      NK_STORAGE const std::size_t pnt_size = sizeof(vec2f);

      /* allocate vertices and elements  */
      std::size_t i1 = 0;
      std::size_t vertex_offset;
      std::size_t index = list->vertex_count;

      const std::size_t idx_count = (thick_line) ? (count * 18) : (count * 12);
      const std::size_t vtx_count = (thick_line) ? (points_count * 4) : (points_count * 3);

      void* vtx = draw_list_alloc_vertices(list, vtx_count);
      draw_index* ids = draw_list_alloc_elements(list, idx_count);

      std::size_t size;
      vec2f *normals, *temp;
      if (!vtx || !ids)
        return;

      /* temporary allocate normals + points */
      vertex_offset = (std::size_t) ((std::byte*) vtx - (std::byte*) list->vertices->memory.ptr);
      buffer_mark(list->vertices, NK_BUFFER_FRONT);
      size = pnt_size * ((thick_line) ? 5 : 3) * points_count;
      normals = (vec2f*) buffer_alloc(list->vertices, NK_BUFFER_FRONT, size, pnt_align);
      if (!normals)
        return;
      temp = normals + points_count;

      /* make sure vertex pointer is still correct */
      vtx = (void*) ((std::byte*) list->vertices->memory.ptr + vertex_offset);

      /* calculate normals */
      for (i1 = 0; i1 < count; ++i1) {
        const std::size_t i2 = ((i1 + 1) == points_count) ? 0 : (i1 + 1);
        vec2f diff = vec2_sub(points[i2], points[i1]);
        float len;

        /* vec2 inverted length  */
        len = vec2_len_sqr(diff);
        if (len != 0.0f)
          len = NK_INV_SQRT(len);
        else
          len = 1.0f;

        diff = vec2_muls(diff, len);
        normals[i1].x = diff.y;
        normals[i1].y = -diff.x;
      }

      if (!closed)
        normals[points_count - 1] = normals[points_count - 2];

      if (!thick_line) {
        std::size_t idx1, i;
        if (!closed) {
          vec2f d;
          temp[0] = vec2_add(points[0], vec2_muls(normals[0], AA_SIZE));
          temp[1] = vec2_sub(points[0], vec2_muls(normals[0], AA_SIZE));
          d = vec2_muls(normals[points_count - 1], AA_SIZE);
          temp[(points_count - 1) * 2 + 0] = vec2_add(points[points_count - 1], d);
          temp[(points_count - 1) * 2 + 1] = vec2_sub(points[points_count - 1], d);
        }

        /* fill elements */
        idx1 = index;
        for (i1 = 0; i1 < count; i1++) {
          vec2f dm;
          float dmr2;
          std::size_t i2 = ((i1 + 1) == points_count) ? 0 : (i1 + 1);
          std::size_t idx2 = ((i1 + 1) == points_count) ? index : (idx1 + 3);

          /* average normals */
          dm = vec2_muls(vec2_add(normals[i1], normals[i2]), 0.5f);
          dmr2 = dm.x * dm.x + dm.y * dm.y;
          if (dmr2 > 0.000001f) {
            float scale = 1.0f / dmr2;
            scale = NK_MIN(100.0f, scale);
            dm = vec2_muls(dm, scale);
          }

          dm = vec2_muls(dm, AA_SIZE);
          temp[i2 * 2 + 0] = vec2_add(points[i2], dm);
          temp[i2 * 2 + 1] = vec2_sub(points[i2], dm);

          ids[0] = (draw_index) (idx2 + 0);
          ids[1] = (draw_index) (idx1 + 0);
          ids[2] = (draw_index) (idx1 + 2);
          ids[3] = (draw_index) (idx1 + 2);
          ids[4] = (draw_index) (idx2 + 2);
          ids[5] = (draw_index) (idx2 + 0);
          ids[6] = (draw_index) (idx2 + 1);
          ids[7] = (draw_index) (idx1 + 1);
          ids[8] = (draw_index) (idx1 + 0);
          ids[9] = (draw_index) (idx1 + 0);
          ids[10] = (draw_index) (idx2 + 0);
          ids[11] = (draw_index) (idx2 + 1);
          ids += 12;
          idx1 = idx2;
        }

        /* fill vertices */
        for (i = 0; i < points_count; ++i) {
          const vec2f uv = list->config.tex_null.uv;
          vtx = draw_vertex(vtx, &list->config, points[i], uv, col);
          vtx = draw_vertex(vtx, &list->config, temp[i * 2 + 0], uv, col_trans);
          vtx = draw_vertex(vtx, &list->config, temp[i * 2 + 1], uv, col_trans);
        }
      } else {
        std::size_t idx1, i;
        const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
        if (!closed) {
          vec2f d1 = vec2_muls(normals[0], half_inner_thickness + AA_SIZE);
          vec2f d2 = vec2_muls(normals[0], half_inner_thickness);

          temp[0] = vec2_add(points[0], d1);
          temp[1] = vec2_add(points[0], d2);
          temp[2] = vec2_sub(points[0], d2);
          temp[3] = vec2_sub(points[0], d1);

          d1 = vec2_muls(normals[points_count - 1], half_inner_thickness + AA_SIZE);
          d2 = vec2_muls(normals[points_count - 1], half_inner_thickness);

          temp[(points_count - 1) * 4 + 0] = vec2_add(points[points_count - 1], d1);
          temp[(points_count - 1) * 4 + 1] = vec2_add(points[points_count - 1], d2);
          temp[(points_count - 1) * 4 + 2] = vec2_sub(points[points_count - 1], d2);
          temp[(points_count - 1) * 4 + 3] = vec2_sub(points[points_count - 1], d1);
        }

        /* add all elements */
        idx1 = index;
        for (i1 = 0; i1 < count; ++i1) {
          vec2f dm_out, dm_in;
          const std::size_t i2 = ((i1 + 1) == points_count) ? 0 : (i1 + 1);
          std::size_t idx2 = ((i1 + 1) == points_count) ? index : (idx1 + 4);

          /* average normals */
          vec2f dm = vec2_muls(vec2_add(normals[i1], normals[i2]), 0.5f);
          float dmr2 = dm.x * dm.x + dm.y * dm.y;
          if (dmr2 > 0.000001f) {
            float scale = 1.0f / dmr2;
            scale = NK_MIN(100.0f, scale);
            dm = vec2_muls(dm, scale);
          }

          dm_out = vec2_muls(dm, ((half_inner_thickness) + AA_SIZE));
          dm_in = vec2_muls(dm, half_inner_thickness);
          temp[i2 * 4 + 0] = vec2_add(points[i2], dm_out);
          temp[i2 * 4 + 1] = vec2_add(points[i2], dm_in);
          temp[i2 * 4 + 2] = vec2_sub(points[i2], dm_in);
          temp[i2 * 4 + 3] = vec2_sub(points[i2], dm_out);

          /* add indexes */
          ids[0] = (draw_index) (idx2 + 1);
          ids[1] = (draw_index) (idx1 + 1);
          ids[2] = (draw_index) (idx1 + 2);
          ids[3] = (draw_index) (idx1 + 2);
          ids[4] = (draw_index) (idx2 + 2);
          ids[5] = (draw_index) (idx2 + 1);
          ids[6] = (draw_index) (idx2 + 1);
          ids[7] = (draw_index) (idx1 + 1);
          ids[8] = (draw_index) (idx1 + 0);
          ids[9] = (draw_index) (idx1 + 0);
          ids[10] = (draw_index) (idx2 + 0);
          ids[11] = (draw_index) (idx2 + 1);
          ids[12] = (draw_index) (idx2 + 2);
          ids[13] = (draw_index) (idx1 + 2);
          ids[14] = (draw_index) (idx1 + 3);
          ids[15] = (draw_index) (idx1 + 3);
          ids[16] = (draw_index) (idx2 + 3);
          ids[17] = (draw_index) (idx2 + 2);
          ids += 18;
          idx1 = idx2;
        }

        /* add vertices */
        for (i = 0; i < points_count; ++i) {
          const vec2f uv = list->config.tex_null.uv;
          vtx = draw_vertex(vtx, &list->config, temp[i * 4 + 0], uv, col_trans);
          vtx = draw_vertex(vtx, &list->config, temp[i * 4 + 1], uv, col);
          vtx = draw_vertex(vtx, &list->config, temp[i * 4 + 2], uv, col);
          vtx = draw_vertex(vtx, &list->config, temp[i * 4 + 3], uv, col_trans);
        }
      }
      /* free temporary normals + points */
      buffer_reset(list->vertices, NK_BUFFER_FRONT);
    } else {
      /* NON ANTI-ALIASED STROKE */
      std::size_t i1 = 0;
      std::size_t idx = list->vertex_count;
      const std::size_t idx_count = count * 6;
      const std::size_t vtx_count = count * 4;
      void* vtx = draw_list_alloc_vertices(list, vtx_count);
      draw_index* ids = draw_list_alloc_elements(list, idx_count);
      if (!vtx || !ids)
        return;

      for (i1 = 0; i1 < count; ++i1) {
        float dx, dy;
        const vec2f uv = list->config.tex_null.uv;
        const std::size_t i2 = ((i1 + 1) == points_count) ? 0 : i1 + 1;
        const vec2f p1 = points[i1];
        const vec2f p2 = points[i2];
        vec2f diff = vec2_sub(p2, p1);
        float len;

        /* vec2 inverted length  */
        len = vec2_len_sqr(diff);
        if (len != 0.0f)
          len = NK_INV_SQRT(len);
        else
          len = 1.0f;
        diff = vec2_muls(diff, len);

        /* add vertices */
        dx = diff.x * (thickness * 0.5f);
        dy = diff.y * (thickness * 0.5f);

        vtx = draw_vertex(vtx, &list->config, vec2(p1.x + dy, p1.y - dx), uv, col);
        vtx = draw_vertex(vtx, &list->config, vec2(p2.x + dy, p2.y - dx), uv, col);
        vtx = draw_vertex(vtx, &list->config, vec2(p2.x - dy, p2.y + dx), uv, col);
        vtx = draw_vertex(vtx, &list->config, vec2(p1.x - dy, p1.y + dx), uv, col);

        ids[0] = (draw_index) (idx + 0);
        ids[1] = (draw_index) (idx + 1);
        ids[2] = (draw_index) (idx + 2);
        ids[3] = (draw_index) (idx + 0);
        ids[4] = (draw_index) (idx + 2);
        ids[5] = (draw_index) (idx + 3);

        ids += 6;
        idx += 4;
      }
    }
  }
  NK_API void
  draw_list_fill_poly_convex(struct draw_list* list,
                             const vec2f* points, const unsigned int points_count,
                             struct color color, enum anti_aliasing aliasing) {
    struct colorf col;
    struct colorf col_trans;

    NK_STORAGE const std::size_t pnt_align = alignof(vec2f);
    NK_STORAGE const std::size_t pnt_size = sizeof(vec2f);
    NK_ASSERT(list);
    if (!list || points_count < 3)
      return;

#ifdef NK_INCLUDE_COMMAND_USERDATA
    draw_list_push_userdata(list, list->userdata);
#endif

    color.a = (std::uint8_t) ((float) color.a * list->config.global_alpha);
    color_fv(&col.r, color);
    col_trans = col;
    col_trans.a = 0;

    if (aliasing == NK_ANTI_ALIASING_ON) {
      std::size_t i = 0;
      std::size_t i0 = 0;
      std::size_t i1 = 0;

      const float AA_SIZE = 1.0f;
      std::size_t vertex_offset = 0;
      std::size_t index = list->vertex_count;

      const std::size_t idx_count = (points_count - 2) * 3 + points_count * 6;
      const std::size_t vtx_count = (points_count * 2);

      void* vtx = draw_list_alloc_vertices(list, vtx_count);
      draw_index* ids = draw_list_alloc_elements(list, idx_count);

      std::size_t size = 0;
      vec2f* normals = 0;
      unsigned int vtx_inner_idx = (unsigned int) (index + 0);
      unsigned int vtx_outer_idx = (unsigned int) (index + 1);
      if (!vtx || !ids)
        return;

      /* temporary allocate normals */
      vertex_offset = (std::size_t) ((std::byte*) vtx - (std::byte*) list->vertices->memory.ptr);
      buffer_mark(list->vertices, NK_BUFFER_FRONT);
      size = pnt_size * points_count;
      normals = (vec2f*) buffer_alloc(list->vertices, NK_BUFFER_FRONT, size, pnt_align);
      if (!normals)
        return;
      vtx = (void*) ((std::byte*) list->vertices->memory.ptr + vertex_offset);

      /* add elements */
      for (i = 2; i < points_count; i++) {
        ids[0] = (draw_index) (vtx_inner_idx);
        ids[1] = (draw_index) (vtx_inner_idx + ((i - 1) << 1));
        ids[2] = (draw_index) (vtx_inner_idx + (i << 1));
        ids += 3;
      }

      /* compute normals */
      for (i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++) {
        vec2f p0 = points[i0];
        vec2f p1 = points[i1];
        vec2f diff = vec2_sub(p1, p0);

        /* vec2 inverted length  */
        float len = vec2_len_sqr(diff);
        if (len != 0.0f)
          len = NK_INV_SQRT(len);
        else
          len = 1.0f;
        diff = vec2_muls(diff, len);

        normals[i0].x = diff.y;
        normals[i0].y = -diff.x;
      }

      /* add vertices + indexes */
      for (i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++) {
        const vec2f uv = list->config.tex_null.uv;
        vec2f n0 = normals[i0];
        vec2f n1 = normals[i1];
        vec2f dm = vec2_muls(vec2_add(n0, n1), 0.5f);
        float dmr2 = dm.x * dm.x + dm.y * dm.y;
        if (dmr2 > 0.000001f) {
          float scale = 1.0f / dmr2;
          scale = NK_MIN(scale, 100.0f);
          dm = vec2_muls(dm, scale);
        }
        dm = vec2_muls(dm, AA_SIZE * 0.5f);

        /* add vertices */
        vtx = draw_vertex(vtx, &list->config, vec2_sub(points[i1], dm), uv, col);
        vtx = draw_vertex(vtx, &list->config, vec2_add(points[i1], dm), uv, col_trans);

        /* add indexes */
        ids[0] = (draw_index) (vtx_inner_idx + (i1 << 1));
        ids[1] = (draw_index) (vtx_inner_idx + (i0 << 1));
        ids[2] = (draw_index) (vtx_outer_idx + (i0 << 1));
        ids[3] = (draw_index) (vtx_outer_idx + (i0 << 1));
        ids[4] = (draw_index) (vtx_outer_idx + (i1 << 1));
        ids[5] = (draw_index) (vtx_inner_idx + (i1 << 1));
        ids += 6;
      }
      /* free temporary normals + points */
      buffer_reset(list->vertices, NK_BUFFER_FRONT);
    } else {
      std::size_t i = 0;
      std::size_t index = list->vertex_count;
      const std::size_t idx_count = (points_count - 2) * 3;
      const std::size_t vtx_count = points_count;
      void* vtx = draw_list_alloc_vertices(list, vtx_count);
      draw_index* ids = draw_list_alloc_elements(list, idx_count);

      if (!vtx || !ids)
        return;
      for (i = 0; i < vtx_count; ++i)
        vtx = draw_vertex(vtx, &list->config, points[i], list->config.tex_null.uv, col);
      for (i = 2; i < points_count; ++i) {
        ids[0] = (draw_index) index;
        ids[1] = (draw_index) (index + i - 1);
        ids[2] = (draw_index) (index + i);
        ids += 3;
      }
    }
  }
  NK_API void
  draw_list_path_clear(struct draw_list* list) {
    NK_ASSERT(list);
    if (!list)
      return;
    buffer_reset(list->buffer, NK_BUFFER_FRONT);
    list->path_count = 0;
    list->path_offset = 0;
  }
  NK_API void
  draw_list_path_line_to(struct draw_list* list, vec2f pos) {
    vec2f* points = 0;
    struct draw_command* cmd = 0;
    NK_ASSERT(list);
    if (!list)
      return;
    if (!list->cmd_count)
      draw_list_add_clip(list, null_rect);

    cmd = draw_list_command_last(list);
    if (cmd && cmd->texture.ptr != list->config.tex_null.texture.ptr)
      draw_list_push_image(list, list->config.tex_null.texture);

    points = draw_list_alloc_path(list, 1);
    if (!points)
      return;
    points[0] = pos;
  }
  NK_API void
  draw_list_path_arc_to_fast(struct draw_list* list, vec2f center,
                             float radius, int a_min, int a_max) {
    int a = 0;
    NK_ASSERT(list);
    if (!list)
      return;
    if (a_min <= a_max) {
      for (a = a_min; a <= a_max; a++) {
        const vec2f c = list->circle_vtx[(std::size_t) a % NK_LEN(list->circle_vtx)];
        const float x = center.x + c.x * radius;
        const float y = center.y + c.y * radius;
        draw_list_path_line_to(list, vec2(x, y));
      }
    }
  }
  NK_API void
  draw_list_path_arc_to(struct draw_list* list, vec2f center,
                        float radius, float a_min, float a_max, unsigned int segments) {
    unsigned int i = 0;
    NK_ASSERT(list);
    if (!list)
      return;
    if (radius == 0.0f)
      return;

    /*  This algorithm for arc drawing relies on these two trigonometric identities[1]:
            sin(a + b) = sin(a) * cos(b) + cos(a) * sin(b)
            cos(a + b) = cos(a) * cos(b) - sin(a) * sin(b)

        Two coordinates (x, y) of a point on a circle centered on
        the origin can be written in polar form as:
            x = r * cos(a)
            y = r * sin(a)
        where r is the radius of the circle,
            a is the angle between (x, y) and the origin.

        This allows us to rotate the coordinates around the
        origin by an angle b using the following transformation:
            x' = r * cos(a + b) = x * cos(b) - y * sin(b)
            y' = r * sin(a + b) = y * cos(b) + x * sin(b)

        [1] https://en.wikipedia.org/wiki/List_of_trigonometric_identities#Angle_sum_and_difference_identities
    */
    {
      const float d_angle = (a_max - a_min) / (float) segments;
      const float sin_d = std::sinf(d_angle);
      const float cos_d = std::cosf(d_angle);

      float cx = std::cosf(a_min) * radius;
      float cy = std::sinf(a_min) * radius;
      for (i = 0; i <= segments; ++i) {
        float new_cx, new_cy;
        const float x = center.x + cx;
        const float y = center.y + cy;
        draw_list_path_line_to(list, vec2(x, y));

        new_cx = cx * cos_d - cy * sin_d;
        new_cy = cy * cos_d + cx * sin_d;
        cx = new_cx;
        cy = new_cy;
      }
    }
  }
  NK_API void
  draw_list_path_rect_to(struct draw_list* list, vec2f a,
                         vec2f b, float rounding) {
    float r;
    NK_ASSERT(list);
    if (!list)
      return;
    r = rounding;
    r = NK_MIN(r, ((b.x - a.x) < 0) ? -(b.x - a.x) : (b.x - a.x));
    r = NK_MIN(r, ((b.y - a.y) < 0) ? -(b.y - a.y) : (b.y - a.y));

    if (r == 0.0f) {
      draw_list_path_line_to(list, a);
      draw_list_path_line_to(list, vec2(b.x, a.y));
      draw_list_path_line_to(list, b);
      draw_list_path_line_to(list, vec2(a.x, b.y));
    } else {
      draw_list_path_arc_to_fast(list, vec2(a.x + r, a.y + r), r, 6, 9);
      draw_list_path_arc_to_fast(list, vec2(b.x - r, a.y + r), r, 9, 12);
      draw_list_path_arc_to_fast(list, vec2(b.x - r, b.y - r), r, 0, 3);
      draw_list_path_arc_to_fast(list, vec2(a.x + r, b.y - r), r, 3, 6);
    }
  }
  NK_API void
  draw_list_path_curve_to(struct draw_list* list, vec2f p2,
                          vec2f p3, vec2f p4, unsigned int num_segments) {
    float t_step;
    unsigned int i_step;
    vec2f p1;

    NK_ASSERT(list);
    NK_ASSERT(list->path_count);
    if (!list || !list->path_count)
      return;
    num_segments = NK_MAX(num_segments, 1);

    p1 = draw_list_path_last(list);
    t_step = 1.0f / (float) num_segments;
    for (i_step = 1; i_step <= num_segments; ++i_step) {
      float t = t_step * (float) i_step;
      float u = 1.0f - t;
      float w1 = u * u * u;
      float w2 = 3 * u * u * t;
      float w3 = 3 * u * t * t;
      float w4 = t * t * t;
      float x = w1 * p1.x + w2 * p2.x + w3 * p3.x + w4 * p4.x;
      float y = w1 * p1.y + w2 * p2.y + w3 * p3.y + w4 * p4.y;
      draw_list_path_line_to(list, vec2(x, y));
    }
  }
  NK_API void
  draw_list_path_fill(struct draw_list* list, struct color color) {
    vec2f* points;
    NK_ASSERT(list);
    if (!list)
      return;
    points = (vec2f*) buffer_memory(list->buffer);
    draw_list_fill_poly_convex(list, points, list->path_count, color, list->config.shape_AA);
    draw_list_path_clear(list);
  }
  NK_API void
  draw_list_path_stroke(struct draw_list* list, struct color color,
                        enum draw_list_stroke closed, float thickness) {
    vec2f* points;
    NK_ASSERT(list);
    if (!list)
      return;
    points = (vec2f*) buffer_memory(list->buffer);
    draw_list_stroke_poly_line(list, points, list->path_count, color,
                               closed, thickness, list->config.line_AA);
    draw_list_path_clear(list);
  }
  NK_API void
  draw_list_stroke_line(struct draw_list* list, vec2f a,
                        vec2f b, struct color col, float thickness) {
    NK_ASSERT(list);
    if (!list || !col.a)
      return;
    if (list->line_AA == NK_ANTI_ALIASING_ON) {
      draw_list_path_line_to(list, a);
      draw_list_path_line_to(list, b);
    } else {
      draw_list_path_line_to(list, vec2_sub(a, vec2(0.5f, 0.5f)));
      draw_list_path_line_to(list, vec2_sub(b, vec2(0.5f, 0.5f)));
    }
    draw_list_path_stroke(list, col, NK_STROKE_OPEN, thickness);
  }
  NK_API void
  draw_list_fill_rect(struct draw_list* list, rectf rect,
                      struct color col, float rounding) {
    NK_ASSERT(list);
    if (!list || !col.a)
      return;

    if (list->line_AA == NK_ANTI_ALIASING_ON) {
      draw_list_path_rect_to(list, vec2(rect.x, rect.y),
                             vec2(rect.x + rect.w, rect.y + rect.h), rounding);
    } else {
      draw_list_path_rect_to(list, vec2(rect.x - 0.5f, rect.y - 0.5f),
                             vec2(rect.x + rect.w, rect.y + rect.h), rounding);
    }
    draw_list_path_fill(list, col);
  }
  NK_API void
  draw_list_stroke_rect(struct draw_list* list, rectf rect,
                        struct color col, float rounding, float thickness) {
    NK_ASSERT(list);
    if (!list || !col.a)
      return;
    if (list->line_AA == NK_ANTI_ALIASING_ON) {
      draw_list_path_rect_to(list, vec2(rect.x, rect.y),
                             vec2(rect.x + rect.w, rect.y + rect.h), rounding);
    } else {
      draw_list_path_rect_to(list, vec2(rect.x - 0.5f, rect.y - 0.5f),
                             vec2(rect.x + rect.w, rect.y + rect.h), rounding);
    }
    draw_list_path_stroke(list, col, NK_STROKE_CLOSED, thickness);
  }
  NK_API void
  draw_list_fill_rect_multi_color(struct draw_list* list, rectf rect,
                                  struct color left, struct color top, struct color right,
                                  struct color bottom) {
    void* vtx;
    struct colorf col_left, col_top;
    struct colorf col_right, col_bottom;
    draw_index* idx;
    draw_index index;

    color_fv(&col_left.r, left);
    color_fv(&col_right.r, right);
    color_fv(&col_top.r, top);
    color_fv(&col_bottom.r, bottom);

    NK_ASSERT(list);
    if (!list)
      return;

    draw_list_push_image(list, list->config.tex_null.texture);
    index = (draw_index) list->vertex_count;
    vtx = draw_list_alloc_vertices(list, 4);
    idx = draw_list_alloc_elements(list, 6);
    if (!vtx || !idx)
      return;

    idx[0] = (draw_index) (index + 0);
    idx[1] = (draw_index) (index + 1);
    idx[2] = (draw_index) (index + 2);
    idx[3] = (draw_index) (index + 0);
    idx[4] = (draw_index) (index + 2);
    idx[5] = (draw_index) (index + 3);

    vtx = draw_vertex(vtx, &list->config, vec2(rect.x, rect.y), list->config.tex_null.uv, col_left);
    vtx = draw_vertex(vtx, &list->config, vec2(rect.x + rect.w, rect.y), list->config.tex_null.uv, col_top);
    vtx = draw_vertex(vtx, &list->config, vec2(rect.x + rect.w, rect.y + rect.h), list->config.tex_null.uv, col_right);
    vtx = draw_vertex(vtx, &list->config, vec2(rect.x, rect.y + rect.h), list->config.tex_null.uv, col_bottom);
  }
  NK_API void
  draw_list_fill_triangle(struct draw_list* list, vec2f a,
                          vec2f b, vec2f c, struct color col) {
    NK_ASSERT(list);
    if (!list || !col.a)
      return;
    draw_list_path_line_to(list, a);
    draw_list_path_line_to(list, b);
    draw_list_path_line_to(list, c);
    draw_list_path_fill(list, col);
  }
  NK_API void
  draw_list_stroke_triangle(struct draw_list* list, vec2f a,
                            vec2f b, vec2f c, struct color col, float thickness) {
    NK_ASSERT(list);
    if (!list || !col.a)
      return;
    draw_list_path_line_to(list, a);
    draw_list_path_line_to(list, b);
    draw_list_path_line_to(list, c);
    draw_list_path_stroke(list, col, NK_STROKE_CLOSED, thickness);
  }
  NK_API void
  draw_list_fill_circle(struct draw_list* list, vec2f center,
                        float radius, struct color col, unsigned int segs) {
    float a_max;
    NK_ASSERT(list);
    if (!list || !col.a)
      return;
    a_max = NK_PI * 2.0f * ((float) segs - 1.0f) / (float) segs;
    draw_list_path_arc_to(list, center, radius, 0.0f, a_max, segs);
    draw_list_path_fill(list, col);
  }
  NK_API void
  draw_list_stroke_circle(struct draw_list* list, vec2f center,
                          float radius, struct color col, unsigned int segs, float thickness) {
    float a_max;
    NK_ASSERT(list);
    if (!list || !col.a)
      return;
    a_max = NK_PI * 2.0f * ((float) segs - 1.0f) / (float) segs;
    draw_list_path_arc_to(list, center, radius, 0.0f, a_max, segs);
    draw_list_path_stroke(list, col, NK_STROKE_CLOSED, thickness);
  }
  NK_API void
  draw_list_stroke_curve(struct draw_list* list, vec2f p0,
                         vec2f cp0, vec2f cp1, vec2f p1,
                         struct color col, unsigned int segments, float thickness) {
    NK_ASSERT(list);
    if (!list || !col.a)
      return;
    draw_list_path_line_to(list, p0);
    draw_list_path_curve_to(list, cp0, cp1, p1, segments);
    draw_list_path_stroke(list, col, NK_STROKE_OPEN, thickness);
  }
  INTERN void
  draw_list_push_rect_uv(struct draw_list* list, vec2f a,
                         vec2f c, vec2f uva, vec2f uvc,
                         struct color color) {
    void* vtx;
    vec2f uvb;
    vec2f uvd;
    vec2f b;
    vec2f d;

    struct colorf col;
    draw_index* idx;
    draw_index index;
    NK_ASSERT(list);
    if (!list)
      return;

    color_fv(&col.r, color);
    uvb = vec2(uvc.x, uva.y);
    uvd = vec2(uva.x, uvc.y);
    b = vec2(c.x, a.y);
    d = vec2(a.x, c.y);

    index = (draw_index) list->vertex_count;
    vtx = draw_list_alloc_vertices(list, 4);
    idx = draw_list_alloc_elements(list, 6);
    if (!vtx || !idx)
      return;

    idx[0] = (draw_index) (index + 0);
    idx[1] = (draw_index) (index + 1);
    idx[2] = (draw_index) (index + 2);
    idx[3] = (draw_index) (index + 0);
    idx[4] = (draw_index) (index + 2);
    idx[5] = (draw_index) (index + 3);

    vtx = draw_vertex(vtx, &list->config, a, uva, col);
    vtx = draw_vertex(vtx, &list->config, b, uvb, col);
    vtx = draw_vertex(vtx, &list->config, c, uvc, col);
    vtx = draw_vertex(vtx, &list->config, d, uvd, col);
  }
  NK_API void
  draw_list_add_image(struct draw_list* list, struct image texture,
                      rectf rect, struct color color) {
    NK_ASSERT(list);
    if (!list)
      return;
    /* push new command with given texture */
    draw_list_push_image(list, texture.handle);
    if (image_is_subimage(&texture)) {
      /* add region inside of the texture  */
      vec2f uv[2];
      uv[0].x = (float) texture.region[0] / (float) texture.w;
      uv[0].y = (float) texture.region[1] / (float) texture.h;
      uv[1].x = (float) (texture.region[0] + texture.region[2]) / (float) texture.w;
      uv[1].y = (float) (texture.region[1] + texture.region[3]) / (float) texture.h;
      draw_list_push_rect_uv(list, vec2(rect.x, rect.y),
                             vec2(rect.x + rect.w, rect.y + rect.h), uv[0], uv[1], color);
    } else
      draw_list_push_rect_uv(list, vec2(rect.x, rect.y),
                             vec2(rect.x + rect.w, rect.y + rect.h),
                             vec2(0.0f, 0.0f), vec2(1.0f, 1.0f), color);
  }
  NK_API void
  draw_list_add_text(struct draw_list* list, const struct user_font* font,
                     rectf rect, const char* text, int len, float font_height,
                     struct color fg) {
    float x = 0;
    int text_len = 0;
    rune unicode = 0;
    rune next = 0;
    int glyph_len = 0;
    int next_glyph_len = 0;
    struct user_font_glyph g;

    NK_ASSERT(list);
    if (!list || !len || !text)
      return;
    if (!intERSECT(rect.x, rect.y, rect.w, rect.h,
                   list->clip_rect.x, list->clip_rect.y, list->clip_rect.w, list->clip_rect.h))
      return;

    draw_list_push_image(list, font->texture);
    x = rect.x;
    glyph_len = utf_decode(text, &unicode, len);
    if (!glyph_len)
      return;

    /* draw every glyph image */
    fg.a = (std::uint8_t) ((float) fg.a * list->config.global_alpha);
    while (text_len < len && glyph_len) {
      float gx, gy, gh, gw;
      float char_width = 0;
      if (unicode == NK_UTF_INVALID)
        break;

      /* query currently drawn glyph information */
      next_glyph_len = utf_decode(text + text_len + glyph_len, &next, (int) len - text_len);
      font->query(font->userdata, font_height, &g, unicode,
                  (next == NK_UTF_INVALID) ? '\0' : next);

      /* calculate and draw glyph drawing rectangle and image */
      gx = x + g.offset.x;
      gy = rect.y + g.offset.y;
      gw = g.width;
      gh = g.height;
      char_width = g.xadvance;
      draw_list_push_rect_uv(list, vec2(gx, gy), vec2(gx + gw, gy + gh),
                             g.uv[0], g.uv[1], fg);

      /* offset next glyph */
      text_len += glyph_len;
      x += char_width;
      glyph_len = next_glyph_len;
      unicode = next;
    }
  }
  NK_API flag
  convert(struct context* ctx, struct buffer* cmds,
          struct buffer* vertices, struct buffer* elements,
          const struct convert_config* config) {
    flag res = NK_CONVERT_SUCCESS;
    const struct command* cmd;
    NK_ASSERT(ctx);
    NK_ASSERT(cmds);
    NK_ASSERT(vertices);
    NK_ASSERT(elements);
    NK_ASSERT(config);
    NK_ASSERT(config->vertex_layout);
    NK_ASSERT(config->vertex_size);
    if (!ctx || !cmds || !vertices || !elements || !config || !config->vertex_layout)
      return NK_CONVERT_INVALID_PARAM;

    draw_list_setup(&ctx->draw_list, config, cmds, vertices, elements,
                    config->line_AA, config->shape_AA);
    foreach (cmd, ctx) {
#ifdef NK_INCLUDE_COMMAND_USERDATA
      ctx->draw_list.userdata = cmd->userdata;
#endif
      switch (cmd->type) {
        case NK_COMMAND_NOP:
          break;
        case NK_COMMAND_SCISSOR: {
          const struct command_scissor* s = (const struct command_scissor*) cmd;
          draw_list_add_clip(&ctx->draw_list, rect(s->x, s->y, s->w, s->h));
        } break;
        case NK_COMMAND_LINE: {
          const struct command_line* l = (const struct command_line*) cmd;
          draw_list_stroke_line(&ctx->draw_list, vec2(l->begin.x, l->begin.y),
                                vec2(l->end.x, l->end.y), l->color, l->line_thickness);
        } break;
        case NK_COMMAND_CURVE: {
          const struct command_curve* q = (const struct command_curve*) cmd;
          draw_list_stroke_curve(&ctx->draw_list, vec2(q->begin.x, q->begin.y),
                                 vec2(q->ctrl[0].x, q->ctrl[0].y), vec2(q->ctrl[1].x, q->ctrl[1].y), vec2(q->end.x, q->end.y), q->color,
                                 config->curve_segment_count, q->line_thickness);
        } break;
        case NK_COMMAND_RECT: {
          const struct command_rect* r = (const struct command_rect*) cmd;
          draw_list_stroke_rect(&ctx->draw_list, rect(r->x, r->y, r->w, r->h),
                                r->color, (float) r->rounding, r->line_thickness);
        } break;
        case NK_COMMAND_RECT_FILLED: {
          const struct command_rect_filled* r = (const struct command_rect_filled*) cmd;
          draw_list_fill_rect(&ctx->draw_list, rect(r->x, r->y, r->w, r->h),
                              r->color, (float) r->rounding);
        } break;
        case NK_COMMAND_RECT_MULTI_COLOR: {
          const struct command_rect_multi_color* r = (const struct command_rect_multi_color*) cmd;
          draw_list_fill_rect_multi_color(&ctx->draw_list, rect(r->x, r->y, r->w, r->h),
                                          r->left, r->top, r->right, r->bottom);
        } break;
        case NK_COMMAND_CIRCLE: {
          const struct command_circle* c = (const struct command_circle*) cmd;
          draw_list_stroke_circle(&ctx->draw_list, vec2((float) c->x + (float) c->w / 2, (float) c->y + (float) c->h / 2), (float) c->w / 2, c->color,
                                  config->circle_segment_count, c->line_thickness);
        } break;
        case NK_COMMAND_CIRCLE_FILLED: {
          const struct command_circle_filled* c = (const struct command_circle_filled*) cmd;
          draw_list_fill_circle(&ctx->draw_list, vec2((float) c->x + (float) c->w / 2, (float) c->y + (float) c->h / 2), (float) c->w / 2, c->color,
                                config->circle_segment_count);
        } break;
        case NK_COMMAND_ARC: {
          const struct command_arc* c = (const struct command_arc*) cmd;
          draw_list_path_line_to(&ctx->draw_list, vec2(c->cx, c->cy));
          draw_list_path_arc_to(&ctx->draw_list, vec2(c->cx, c->cy), c->r,
                                c->a[0], c->a[1], config->arc_segment_count);
          draw_list_path_stroke(&ctx->draw_list, c->color, NK_STROKE_CLOSED, c->line_thickness);
        } break;
        case NK_COMMAND_ARC_FILLED: {
          const struct command_arc_filled* c = (const struct command_arc_filled*) cmd;
          draw_list_path_line_to(&ctx->draw_list, vec2(c->cx, c->cy));
          draw_list_path_arc_to(&ctx->draw_list, vec2(c->cx, c->cy), c->r,
                                c->a[0], c->a[1], config->arc_segment_count);
          draw_list_path_fill(&ctx->draw_list, c->color);
        } break;
        case NK_COMMAND_TRIANGLE: {
          const struct command_triangle* t = (const struct command_triangle*) cmd;
          draw_list_stroke_triangle(&ctx->draw_list, vec2(t->a.x, t->a.y),
                                    vec2(t->b.x, t->b.y), vec2(t->c.x, t->c.y), t->color,
                                    t->line_thickness);
        } break;
        case NK_COMMAND_TRIANGLE_FILLED: {
          const struct command_triangle_filled* t = (const struct command_triangle_filled*) cmd;
          draw_list_fill_triangle(&ctx->draw_list, vec2(t->a.x, t->a.y),
                                  vec2(t->b.x, t->b.y), vec2(t->c.x, t->c.y), t->color);
        } break;
        case NK_COMMAND_POLYGON: {
          int i;
          const struct command_polygon* p = (const struct command_polygon*) cmd;
          for (i = 0; i < p->point_count; ++i) {
            vec2f pnt = vec2((float) p->points[i].x, (float) p->points[i].y);
            draw_list_path_line_to(&ctx->draw_list, pnt);
          }
          draw_list_path_stroke(&ctx->draw_list, p->color, NK_STROKE_CLOSED, p->line_thickness);
        } break;
        case NK_COMMAND_POLYGON_FILLED: {
          int i;
          const struct command_polygon_filled* p = (const struct command_polygon_filled*) cmd;
          for (i = 0; i < p->point_count; ++i) {
            vec2f pnt = vec2((float) p->points[i].x, (float) p->points[i].y);
            draw_list_path_line_to(&ctx->draw_list, pnt);
          }
          draw_list_path_fill(&ctx->draw_list, p->color);
        } break;
        case NK_COMMAND_POLYLINE: {
          int i;
          const struct command_polyline* p = (const struct command_polyline*) cmd;
          for (i = 0; i < p->point_count; ++i) {
            vec2f pnt = vec2((float) p->points[i].x, (float) p->points[i].y);
            draw_list_path_line_to(&ctx->draw_list, pnt);
          }
          draw_list_path_stroke(&ctx->draw_list, p->color, NK_STROKE_OPEN, p->line_thickness);
        } break;
        case NK_COMMAND_TEXT: {
          const struct command_text* t = (const struct command_text*) cmd;
          draw_list_add_text(&ctx->draw_list, t->font, rect(t->x, t->y, t->w, t->h),
                             t->string, t->length, t->height, t->foreground);
        } break;
        case NK_COMMAND_IMAGE: {
          const struct command_image* i = (const struct command_image*) cmd;
          draw_list_add_image(&ctx->draw_list, i->img, rect(i->x, i->y, i->w, i->h), i->col);
        } break;
        case NK_COMMAND_CUSTOM: {
          const struct command_custom* c = (const struct command_custom*) cmd;
          c->callback(&ctx->draw_list, c->x, c->y, c->w, c->h, c->callback_data);
        } break;
        default:
          break;
      }
    }
    res |= (cmds->needed > cmds->allocated + (cmds->memory.size - cmds->size)) ? NK_CONVERT_COMMAND_BUFFER_FULL : 0;
    res |= (vertices->needed > vertices->allocated) ? NK_CONVERT_VERTEX_BUFFER_FULL : 0;
    res |= (elements->needed > elements->allocated) ? NK_CONVERT_ELEMENT_BUFFER_FULL : 0;
    return res;
  }
  NK_API const struct draw_command*
  _draw_begin(const struct context* ctx,
              const struct buffer* buffer) {
    return _draw_list_begin(&ctx->draw_list, buffer);
  }
  NK_API const struct draw_command*
  _draw_end(const struct context* ctx, const struct buffer* buffer) {
    return _draw_list_end(&ctx->draw_list, buffer);
  }
  NK_API const struct draw_command*
  _draw_next(const struct draw_command* cmd,
             const struct buffer* buffer, const struct context* ctx) {
    return _draw_list_next(cmd, buffer, &ctx->draw_list);
  }
#endif
} // namespace nk
