#include "nuklear.h"
#include "nuklear_internal.h"
#include <cstring>

namespace nk {
  /* ==============================================================
   *
   *                          BUFFER
   *
   * ===============================================================*/
#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
  NK_LIB void*
  malloc(handle unused, void *old,std::size_t size)
  {
    NK_UNUSED(unused);
    NK_UNUSED(old);
    return malloc(size);
  }
  NK_LIB void
  mfree(handle unused, void *ptr)
  {
    NK_UNUSED(unused);
    free(ptr);
  }
  NK_API void
  buffer_init_default(struct buffer *buffer)
  {
    struct allocator alloc;
    alloc.userdata.ptr = 0;
    alloc.alloc = malloc;
    alloc.free = mfree;
    buffer_init(buffer, &alloc, NK_BUFFER_DEFAULT_INITIAL_SIZE);
  }
#endif

  NK_API void
  buffer_init(struct memory_buffer *b, const struct allocator *a,
      std::size_t initial_size)
  {
    NK_ASSERT(b);
    NK_ASSERT(a);
    NK_ASSERT(initial_size);
    if (!b || !a || !initial_size) return;

    zero(b, sizeof(*b));
    b->type = allocation_type::BUFFER_DYNAMIC;
    b->memory.ptr = a->alloc(a->userdata,0, initial_size);
    b->memory.size = initial_size;
    b->size = initial_size;
    b->grow_factor = 2.0f;
    b->pool = *a;
  }
  NK_API void
  buffer_init_fixed(struct memory_buffer *b, void *m, std::size_t size)
  {
    NK_ASSERT(b);
    NK_ASSERT(m);
    NK_ASSERT(size);
    if (!b || !m || !size) return;

    zero(b, sizeof(*b));
    b->type = allocation_type::BUFFER_FIXED;
    b->memory.ptr = m;
    b->memory.size = size;
    b->size = size;
  }
  NK_LIB void*
  buffer_align(void *unaligned,
      std::size_t align, std::size_t *alignment,
      enum buffer_allocation_type type)
  {
    void *memory = 0;
    switch (type) {
      default:
      case buffer_allocation_type::BUFFER_MAX:
      case buffer_allocation_type::BUFFER_FRONT:
        if (align) {
          memory = NK_ALIGN_PTR(unaligned, align);
          *alignment = (std::size_t)((std::uint8_t*)memory - (std::uint8_t*)unaligned);
        } else {
          memory = unaligned;
          *alignment = 0;
        }
        break;
      case buffer_allocation_type::BUFFER_BACK:
        if (align) {
          memory = NK_ALIGN_PTR_BACK(unaligned, align);
          *alignment = (std::size_t)((std::uint8_t*)unaligned - (std::uint8_t*)memory);
        } else {
          memory = unaligned;
          *alignment = 0;
        }
        break;
    }
    return memory;
  }
  NK_LIB void*
  buffer_realloc(struct memory_buffer *b, std::size_t capacity, std::size_t *size)
  {
    void *temp;
    std::size_t buffer_size;

    NK_ASSERT(b);
    NK_ASSERT(size);
    if (!b || !size || !b->pool.alloc || !b->pool.free)
      return 0;

    buffer_size = b->memory.size;
    temp = b->pool.alloc(b->pool.userdata, b->memory.ptr, capacity);
    NK_ASSERT(temp);
    if (!temp) return 0;

    *size = capacity;
    if (temp != b->memory.ptr) {
      std::memcpy(temp, b->memory.ptr, buffer_size);
      b->pool.free(b->pool.userdata, b->memory.ptr);
    }

    if (b->size == buffer_size) {
      /* no back buffer so just set correct size */
      b->size = capacity;
      return temp;
    } else {
      /* copy back buffer to the end of the new buffer */
      void *dst, *src;
      std::size_t back_size;
      back_size = buffer_size - b->size;
      dst = ptr_add(void, temp, capacity - back_size);
      src = ptr_add(void, temp, b->size);
      std::memcpy(dst, src, back_size);
      b->size = capacity - back_size;
    }
    return temp;
  }
  NK_LIB void*
  buffer_alloc(struct memory_buffer *b, enum buffer_allocation_type type,
      std::size_t size, std::size_t align)
  {
    int full;
    std::size_t alignment;
    void *unaligned;
    void *memory;

    NK_ASSERT(b);
    NK_ASSERT(size);
    if (!b || !size) return 0;
    b->needed += size;

    /* calculate total size with needed alignment + size */
    if (type == buffer_allocation_type::BUFFER_FRONT)
      unaligned = ptr_add(void, b->memory.ptr, b->allocated);
    else unaligned = ptr_add(void, b->memory.ptr, b->size - size);
    memory = buffer_align(unaligned, align, &alignment, type);

    /* check if buffer has enough memory*/
    if (type == buffer_allocation_type::BUFFER_FRONT)
      full = ((b->allocated + size + alignment) > b->size);
    else full = ((b->size - NK_MIN(b->size,(size + alignment))) <= b->allocated);

    if (full) {
      std::size_t capacity;
      if (b->type != allocation_type::BUFFER_DYNAMIC)
        return 0;
      NK_ASSERT(b->pool.alloc && b->pool.free);
      if (b->type != allocation_type::BUFFER_DYNAMIC || !b->pool.alloc || !b->pool.free)
        return 0;

      /* buffer is full so allocate bigger buffer if dynamic */
      capacity = (std::size_t)((float)b->memory.size * b->grow_factor);
      capacity = NK_MAX(capacity, round_up_pow2((unsigned int)(b->allocated + size)));
      b->memory.ptr = buffer_realloc(b, capacity, &b->memory.size);
      if (!b->memory.ptr) return 0;

      /* align newly allocated pointer */
      if (type == buffer_allocation_type::BUFFER_FRONT)
        unaligned = ptr_add(void, b->memory.ptr, b->allocated);
      else unaligned = ptr_add(void, b->memory.ptr, b->size - size);
      memory = buffer_align(unaligned, align, &alignment, type);
    }
    if (type == buffer_allocation_type::BUFFER_FRONT)
      b->allocated += size + alignment;
    else b->size -= (size + alignment);
    b->needed += alignment;
    b->calls++;
    return memory;
  }
  NK_API void
  buffer_push(struct memory_buffer *b, enum buffer_allocation_type type,
      const void *memory, std::size_t size, std::size_t align)
  {
    void *mem = buffer_alloc(b, type, size, align);
    if (!mem) return;
    std::memcpy(mem, memory, size);
  }
  NK_API void
  buffer_mark(memory_buffer *buffer, buffer_allocation_type type)
  {
    NK_ASSERT(buffer);
    if (!buffer) return;
    buffer->marker[static_cast<unsigned>(type)].active = true;
    if (type == buffer_allocation_type::BUFFER_BACK)
      buffer->marker[static_cast<unsigned>(type)].offset = buffer->size;
    else buffer->marker[static_cast<unsigned>(type)].offset = buffer->allocated;
  }
  NK_API void
  buffer_reset(struct memory_buffer *buffer, enum buffer_allocation_type type)
  {
    NK_ASSERT(buffer);
    if (!buffer) return;
    if (type == buffer_allocation_type::BUFFER_BACK) {
      /* reset back buffer either back to marker or empty */
      buffer->needed -= (buffer->memory.size - buffer->marker[static_cast<unsigned>(type)].offset);
      if (buffer->marker[static_cast<unsigned>(type)].active)
        buffer->size = buffer->marker[static_cast<unsigned>(type)].offset;
      else buffer->size = buffer->memory.size;
      buffer->marker[static_cast<unsigned>(type)].active = false;
    } else {
      /* reset front buffer either back to back marker or empty */
      buffer->needed -= (buffer->allocated - buffer->marker[static_cast<unsigned>(type)].offset);
      if (buffer->marker[static_cast<unsigned>(type)].active)
        buffer->allocated = buffer->marker[static_cast<unsigned>(type)].offset;
      else buffer->allocated = 0;
      buffer->marker[static_cast<unsigned>(type)].active = false;
    }
  }
  NK_API void
  buffer_clear(struct memory_buffer *b)
  {
    NK_ASSERT(b);
    if (!b) return;
    b->allocated = 0;
    b->size = b->memory.size;
    b->calls = 0;
    b->needed = 0;
  }
  NK_API void
  buffer_free(struct memory_buffer *b)
  {
    NK_ASSERT(b);
    if (!b || !b->memory.ptr) return;
    if (b->type == allocation_type::BUFFER_FIXED) return;
    if (!b->pool.free) return;
    NK_ASSERT(b->pool.free);
    b->pool.free(b->pool.userdata, b->memory.ptr);
  }
  NK_API void
  buffer_info(struct memory_status *s, const struct memory_buffer *b)
  {
    NK_ASSERT(b);
    NK_ASSERT(s);
    if (!s || !b) return;
    s->allocated = b->allocated;
    s->size =  b->memory.size;
    s->needed = b->needed;
    s->memory = b->memory.ptr;
    s->calls = b->calls;
  }
  NK_API void*
  buffer_memory(struct memory_buffer *buffer)
  {
    NK_ASSERT(buffer);
    if (!buffer) return 0;
    return buffer->memory.ptr;
  }
  NK_API const void*
  buffer_memory_const(const struct memory_buffer *buffer)
  {
    NK_ASSERT(buffer);
    if (!buffer) return 0;
    return buffer->memory.ptr;
  }
  NK_API std::size_t
  buffer_total(const struct memory_buffer *buffer)
  {
    NK_ASSERT(buffer);
    if (!buffer) return 0;
    return buffer->memory.size;
  }
}
