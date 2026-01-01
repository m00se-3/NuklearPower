#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                              TABLE
   *
   * ===============================================================*/
  NK_LIB table*
  create_table(context* ctx) {
    page_element* elem = create_page_element(ctx);
    if (!elem)
      return 0;
    zero_struct(*elem);
    return &elem->data.tbl;
  }
  NK_LIB void
  free_table(context* ctx, table* tbl) {
    page_data* pd = NK_CONTAINER_OF(tbl, union page_data, tbl);
    page_element* pe = NK_CONTAINER_OF(pd, struct page_element, data);
    free_page_element(ctx, pe);
  }
  NK_LIB void
  push_table(window* win, table* tbl) {
    if (!win->tables) {
      win->tables = tbl;
      tbl->next = 0;
      tbl->prev = 0;
      tbl->size = 0;
      win->table_count = 1;
      return;
    }
    win->tables->prev = tbl;
    tbl->next = win->tables;
    tbl->prev = 0;
    tbl->size = 0;
    win->tables = tbl;
    win->table_count++;
  }
  NK_LIB void
  remove_table(window* win, table* tbl) {
    if (win->tables == tbl)
      win->tables = tbl->next;
    if (tbl->next)
      tbl->next->prev = tbl->prev;
    if (tbl->prev)
      tbl->prev->next = tbl->next;
    tbl->next = 0;
    tbl->prev = 0;
  }
  NK_LIB unsigned int*
  add_value(context* ctx, window* win,
            const hash name, const unsigned int value) {
    NK_ASSERT(ctx);
    NK_ASSERT(win);
    if (!win || !ctx)
      return 0;
    if (!win->tables || win->tables->size >= NK_VALUE_PAGE_CAPACITY) {
      table* tbl = create_table(ctx);
      NK_ASSERT(tbl);
      if (!tbl)
        return 0;
      push_table(win, tbl);
    }
    win->tables->seq = win->seq;
    win->tables->keys[win->tables->size] = name;
    win->tables->values[win->tables->size] = value;
    return &win->tables->values[win->tables->size++];
  }
  NK_LIB unsigned int*
  find_value(const window* win, const hash name) {
    table* iter = win->tables;
    while (iter) {
      unsigned int i = 0;
      unsigned int size = iter->size;
      for (i = 0; i < size; ++i) {
        if (iter->keys[i] == name) {
          iter->seq = win->seq;
          return &iter->values[i];
        }
      }
      size = NK_VALUE_PAGE_CAPACITY;
      iter = iter->next;
    }
    return 0;
  }
} // namespace nk
