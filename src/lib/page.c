#include "page.h"

void page_set_flag(Page* page, PageFlag flag) {
    page->flags |= (1 << flag);
}
void page_clear_flag(Page* page, PageFlag flag) {
    page->flags &= ~(1 << flag);
}
bool page_check_flag(Page* page, PageFlag flag) {
    return (page->flags >> flag) & 1;
}

