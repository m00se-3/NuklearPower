#include <nk/nuklear.hpp>

namespace nk {
  /* ===============================================================
   *
   *                              POOL
   *
   * ===============================================================*/
  NK_LIB void
  pool_init(pool* pool, const allocator* alloc,
            const unsigned int capacity) {
    NK_ASSERT(capacity >= 1);
    zero(pool, sizeof(*pool));
    pool->alloc = *alloc;
    pool->capacity = capacity;
    pool->type = allocation_type::BUFFER_DYNAMIC;
    pool->pages = 0;
  }
  NK_LIB void
  pool_free(pool* pool) {
    if (!pool)
      return;
    page* iter = pool->pages;
    if (pool->type == allocation_type::BUFFER_FIXED)
      return;
    while (iter) {
      page* next = iter->next;
      pool->alloc.free(pool->alloc.userdata, iter);
      iter = next;
    }
  }
  NK_LIB void
  pool_init_fixed(pool* pool, void* memory, const std::size_t size) {
    zero(pool, sizeof(*pool));
    NK_ASSERT(size >= sizeof(page));
    if (size < sizeof(page))
      return;
    /* first page_element is embedded in page, additional elements follow in adjacent space */
    pool->capacity = (unsigned) (1 + (size - sizeof(page)) / sizeof(page_element));
    pool->pages = (page*) memory;
    pool->type = allocation_type::BUFFER_FIXED;
    pool->size = size;
  }
  NK_LIB page_element*
  pool_alloc(pool* pool) {
    if (!pool->pages || pool->pages->size >= pool->capacity) {
      /* allocate new page */
      if (pool->type == allocation_type::BUFFER_FIXED) {
        NK_ASSERT(pool->pages);
        if (!pool->pages)
          return 0;
        NK_ASSERT(pool->pages->size < pool->capacity);
        return 0;
      } else {
        std::size_t size = sizeof(struct page);
        size += (pool->capacity - 1) * sizeof(page_element);
        page* page = (struct page*) pool->alloc.alloc(pool->alloc.userdata, 0, size);
        page->next = pool->pages;
        pool->pages = page;
        page->size = 0;
      }
    }
    return &pool->pages->win[pool->pages->size++];
  }
} // namespace nk
