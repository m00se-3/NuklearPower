#ifndef NK_POWER_MEMORY_HPP
#define NK_POWER_MEMORY_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {

  /** ==============================================================
   *
   *                          MEMORY BUFFER
   *
   * ===============================================================*/
  /**
   * \page Memory Buffer
   * A basic (double)-buffer with linear allocation and resetting as only
   * freeing policy. The buffer's main purpose is to control all memory management
   * inside the GUI toolkit and still leave memory control as much as possible in
   * the hand of the user while also making sure the library is easy to use if
   * not as much control is needed.
   * In general all memory inside this library can be provided from the user in
   * three different ways.
   *
   * The first way and the one providing most control is by just passing a fixed
   * size memory block. In this case all control lies in the hand of the user
   * since he can exactly control where the memory comes from and how much memory
   * the library should consume. Of course using the fixed size API removes the
   * ability to automatically resize a buffer if not enough memory is provided so
   * you have to take over the resizing. While being a fixed sized buffer sounds
   * quite limiting, it is very effective in this library since the actual memory
   * consumption is quite stable and has a fixed upper bound for a lot of cases.
   *
   * If you don't want to think about how much memory the library should allocate
   * at all time or have a very dynamic UI with unpredictable memory consumption
   * habits but still want control over memory allocation you can use the dynamic
   * allocator based API. The allocator consists of two callbacks for allocating
   * and freeing memory and optional userdata so you can plugin your own allocator.
   *
   * The final and easiest way can be used by defining
   * NK_INCLUDE_DEFAULT_ALLOCATOR which uses the standard library memory
   * allocation functions malloc and free and takes over complete control over
   * memory in this library.
   */
#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
  void buffer_init_default(memory_buffer*);
#endif
  void buffer_init(memory_buffer*, const allocator*, std::size_t initial_size);
  void buffer_init_fixed(memory_buffer*, void* memory, std::size_t size);
  void buffer_info(memory_status*, const memory_buffer*);
  void buffer_push(memory_buffer*, buffer_allocation_type type, const void* memory, std::size_t size, std::size_t align);
  void buffer_mark(memory_buffer*, buffer_allocation_type type);
  void buffer_reset(memory_buffer*, buffer_allocation_type type);
  void buffer_clear(memory_buffer*);
  void buffer_free(memory_buffer*);
  void* buffer_memory(const memory_buffer*);
  const void* buffer_memory_const(const memory_buffer*);
  std::size_t buffer_total(const memory_buffer*);

#ifndef NK_MEMCPY
  void* memcopy(void* dst, const void* src, std::size_t n);
#endif
#ifndef NK_MEMSET
  void memset(void* ptr, int c0, std::size_t size);
#endif

  /* pool */
  void pool_init(pool* pool, const allocator* alloc, unsigned int capacity);
  void pool_free(pool* pool);
  void pool_init_fixed(pool* pool, void* memory, std::size_t size);
  page_element* pool_alloc(pool* pool);


}

#endif