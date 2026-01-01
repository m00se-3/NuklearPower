#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                          PAGE ELEMENT
   *
   * ===============================================================*/
  NK_LIB page_element*
  create_page_element(context* ctx) {
    page_element* elem;
    if (ctx->freelist) {
      /* unlink page element from free list */
      elem = ctx->freelist;
      ctx->freelist = elem->next;
    } else if (ctx->use_pool) {
      /* allocate page element from memory pool */
      elem = pool_alloc(&ctx->pool);
      NK_ASSERT(elem);
      if (!elem)
        return 0;
    } else {
      /* allocate new page element from back of fixed size memory buffer */
      NK_STORAGE const std::size_t size = sizeof(struct page_element);
      NK_STORAGE const std::size_t align = alignof(struct page_element);
      elem = (page_element*) buffer_alloc(&ctx->memory, buffer_allocation_type::BUFFER_BACK, size, align);
      NK_ASSERT(elem);
      if (!elem)
        return 0;
    }
    zero_struct(*elem);
    elem->next = 0;
    elem->prev = 0;
    return elem;
  }
  NK_LIB void
  lipage_element_into_freelist(context* ctx,
                               page_element* elem) {
    /* link table into freelist */
    if (!ctx->freelist) {
      ctx->freelist = elem;
    } else {
      elem->next = ctx->freelist;
      ctx->freelist = elem;
    }
  }
  NK_LIB void
  free_page_element(context* ctx, page_element* elem) {
    /* we have a pool so just add to free list */
    if (ctx->use_pool) {
      lipage_element_into_freelist(ctx, elem);
      return;
    }
    /* if possible remove last element from back of fixed memory buffer */
    {
      const void* elem_end = (void*) (elem + 1);
      const void* buffer_end = (std::uint8_t*) ctx->memory.memory.ptr + ctx->memory.size;
      if (elem_end == buffer_end)
        ctx->memory.size -= sizeof(page_element);
      else
        lipage_element_into_freelist(ctx, elem);
    }
  }
} // namespace nk
