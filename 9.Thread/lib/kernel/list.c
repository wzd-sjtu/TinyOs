#include "list.h"
#include "interrupt.h"

#ifndef NULL
#define NULL 0
#endif

void list_init (struct list* list) {
    list->head.prev = NULL;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = NULL;
}

void list_insert_before(struct list_elem* before, struct list_elem* elem) {
    // need atom behavour, close the interrupt
    enum intr_status old_status = intr_disable();

    before->prev->next = elem;
    elem->next = before;
    elem->prev = before->prev;
    before->prev = elem;

    intr_set_status(old_status);
}
void list_push(struct list* plist, struct list_elem* elem) {

    // push to head, inner is with a stop of interrupt
    list_insert_before(plist->head.next, elem);

}
// this function is used for other thing!
void list_iterate(struct list* plist);
void list_append(struct list* plist, struct list_elem* elem) {
    list_insert_before(&plist->tail, elem);
}
void list_remove(struct list_elem* pelem) {
    enum intr_status old_status = intr_disable();

    pelem->prev->next = pelem->next;
    pelem->next->prev = pelem->prev;

    intr_set_status(old_status);
}
struct list_elem* list_pop(struct list* plist) {
    // pop element
    struct list_elem* elem = plist->head.next;
    list_remove(elem);
    return elem;
}
int list_empty(struct list* plist) {
    return (plist->head.next == &plist->tail? 1:0); // whether it is empty?
}
uint32_t list_len(struct list* plist) {
    struct list_elem* elem = plist->head.next;
    uint32_t length = 0;
    while(elem != &plist->tail) {
        length++;
        elem = elem->next;
    }
    return length;
}
struct list_elem* list_traversal(struct list* plist, function func, int arg) {
    struct list_elem* elem = plist->head.next;
    if(list_empty(plist)) return NULL;
    // 回调函数
    while(elem != &plist->tail) {
        if(func(elem, arg)) {
            return elem;
        }
        elem = elem->next;
    }
    return NULL;
}
int elem_find(struct list* plist, struct list_elem* obj_elem) {
    struct list_elem* elem = plist->head.next;
    while(elem != &plist->tail) {
        if(elem == obj_elem) {
            return 1;
        }
        elem = elem->next;
    }
    return 0;
}