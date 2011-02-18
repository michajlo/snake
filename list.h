/**
 * @file list.h
 * Header for list implementation
 */



#ifndef __LIST_H__
#define __LIST_H__

struct list_node{
	void *data;
	struct list_node *next;
	struct list_node *prev;
};
typedef struct list_node NODE;

typedef struct {
	NODE *firstNode;
	NODE *lastNode;
} LIST;

void list_init(LIST *list);
void list_insert_after(LIST *list, NODE *node, void *datum);
void list_insert_before(LIST *list, NODE *node, void *datum);
void list_prepend(LIST *list, void *datum);
void list_append( LIST *list, void *datum );
void list_map( LIST *list, void (*f)(void *) );
void *list_front( LIST *list );
void *list_remove(LIST *list, NODE *node);
void list_delete( LIST *list );
int list_empty(LIST *list);


#endif

