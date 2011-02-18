/**
 * @file list.c
 * List implementation
 * Code based on wikipedia doubly linked list implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/**
 * Initializes a list
 * @return the newly allocated list
 */
void list_init(LIST *list){
	list->firstNode=NULL;
	list->lastNode=NULL;
}

/**
 * Insert new node after given node
 * @param list LIST to perform insert on
 * @param node NODE to insert after
 * @param datum Data to insert
 */
void list_insert_after( LIST *list, NODE *node, void *datum){
	NODE* newNode=(NODE *)malloc(sizeof(NODE));
	if(newNode == NULL){
		fprintf(stderr,"Memory error\n");
		exit(1);
	}
	newNode->data=datum;
	newNode->prev=node;
	newNode->next=node->next;
	if(node->next == NULL){
		list->lastNode=newNode;
	}else{
		node->next->prev=newNode;
	}
	node->next=newNode;
}


/**
 * Insert new node before given node
 * @param list LIST to perform insert on
 * @param node NODE to insert before
 * @param datum Data to insert
 */
void list_insert_before( LIST *list, NODE *node, void *datum){
	NODE *newNode=(NODE *)malloc(sizeof(NODE));
	if(newNode == NULL){
		fprintf(stderr,"Memory error\n");
		exit(1);
	}
	newNode->data=datum;
	newNode->prev=node->prev;
	newNode->next=node;
	if(newNode->prev == NULL){
		list->firstNode=newNode;
	}else{
		node->prev->next=newNode;
	}
	node->prev=newNode;
}

/**
 * Appends an entry to the beginning of a list
 * @param list the pre-allocated list
 * @param datum a pointer to whatever should be added to the list
 */
void list_prepend( LIST *list, void *datum ){
	if(list->firstNode == NULL){
		NODE *newNode=(NODE *)malloc(sizeof(NODE));
		if(newNode == NULL){
			fprintf(stderr,"Memory error\n");
			exit(1);
		}
		newNode->data=datum;
		newNode->next=NULL;
		newNode->prev=NULL;
		list->firstNode=newNode;
		list->lastNode=newNode;
	}else{
		list_insert_after(list,list->firstNode,datum);
	}
}


/**
 * Appends an entry to the end of a list
 * @param list the pre-allocated list
 * @param datum a pointer to whatever should be added to the list
 */
void list_append( LIST *list, void *datum ){
	if(list->firstNode == NULL){
		list_prepend(list,datum);
	}else{
		list_insert_after(list,list->lastNode,datum);
	}
}



/**
 * Applies a function to each element of the list
 * @param list a list
 * @param f a function to apply to each element of the list
 */
void list_map( LIST *list, void (*f)(void *) ){
	NODE* listNode=list->firstNode;
	while(listNode != NULL){
		f(listNode->data);
		listNode=listNode->next;
	}
}



/**
 * Returns the element at the front of a list, and removes it from
 * the list.
 *
 * @param list the list
 * @return the entry at the front of the list, or NULL if no such entry exists
 */
void *list_front( LIST *list ){
	if(list->firstNode != NULL){
		return list_remove(list, list->firstNode);
	}else{
		return NULL;
	}
}

/**
 * Removes node from list, does not check that the node came from the same list,
 * this is up to the programmer.  Only frees the NODE struct 
 * @return void* which is the responsibility of the callee to free
 */
void *list_remove(LIST *list, NODE *node){
	void *ret=node->data;
	if(node->prev == NULL){ 		/*if first*/
		list->firstNode=node->next;
	}else{
		node->prev->next=node->next;
	}
	if(node->next == NULL){			/*if there's a next */
		list->lastNode=node->prev;
	}else{
		node->next->prev=node->prev;
	}
	free(node);
	return ret;
}

/**
 * Deletes (and frees) all entries in the list
 * @param list the list to delete
 */
void list_delete( LIST *list ){
	NODE *nextNode=list->firstNode,*currentNode;
	while(nextNode != NULL){
		currentNode=nextNode;
		nextNode=currentNode->next;
		if(currentNode->data != NULL){
			free(currentNode->data);
		}
		free(currentNode);
	}
}

/**
 * Check if the list is empty
 * @return 1 if empty, 0 if not
 */
int list_empty(LIST *list){
	return list->firstNode == NULL;
}

