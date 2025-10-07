#include <stdlib.h>
#include <string.h>
#include "defs.h"

/*
 * Creates a scrap with the given key, value, and name.
 * (out) **scrap - pointer to the scrap to be created
 * (in) key - the key of the scrap
 * (in) value - the value of the scrap
 * (in) *name - the name of the scrap
 *
 * Returns: ERR_NULL_POINTER if scrap is NULL, ERR_OK otherwise
 */
int scrap_create(Scrap** scrap, int key, int value, const char* name) {
    if (scrap==NULL){
        return ERR_NULL_POINTER;//null pointer error
    }
    *scrap = (Scrap*)malloc(sizeof(Scrap));//allocate memory for scrap
    if (*scrap==NULL){
       return ERR_NULL_POINTER;//failed memory allocation
    }

    (*scrap)->key =key; //assign values to key, value, copy name, null terminator
    (*scrap)->value =value;
    strncpy((*scrap)->name, name, MAX_STR);
    (*scrap)->name[MAX_STR - 1] = '\0';

    return ERR_OK;
}

/*
 * Destroys a scrap, freeing the allocated memory.
 * (in/out) **scrap - pointer to the scrap to be destroyed
 *
 * Returns: ERR_NULL_POINTER if scrap or *scrap is NULL, ERR_OK otherwise
 */
int scrap_destroy(Scrap** scrap) {
    if (scrap == NULL || *scrap == NULL) {
        return ERR_NULL_POINTER;
    }
    // TODO: Your code here
    free(*scrap);//free the scrap memory
    *scrap = NULL;//assigns pointer to that memory of NULL
    return ERR_OK;//replaced with correct return value
}

/*
 * Initializes a ScrapList, setting the head to NULL and total_value to 0.
 * (out) *list - the ScrapList to initialize
 *
 * Returns: ERR_NULL_POINTER if list is NULL, ERR_OK otherwise
 */
int scrap_list_init(ScrapList* list) {
    if (list==NULL){
        return ERR_NULL_POINTER;//null pointer error
    }
    list->head = NULL;//set head to NULL,total value is also 0
    list->total_value = 0;
    return ERR_OK;//replaced with correct return value
}

/*
 * Adds a scrap to the head of a ScrapList.
 * (in/out) *list - the ScrapList to add the scrap to
 * (in) *scrap - the scrap to add
 *
 * Returns: ERR_NULL_POINTER if list or scrap is NULL, ERR_OK otherwise
 */
int scrap_list_add(ScrapList* list, Scrap* scrap) {
    if (list==NULL || scrap==NULL){
        return ERR_NULL_POINTER;//null pointer error
    } 

    ScrapNode* newNode = (ScrapNode*)malloc(sizeof(ScrapNode));//create memory for new ScrapNode
    if (newNode==NULL){
        return ERR_NULL_POINTER;//failure to allocate memory
    }
    newNode->data = scrap;//initialize data, next
    newNode->next = list->head;
    list->head = newNode;//list head is now the new node
    list->total_value += scrap->value; //add scrap's value to the total value
    return ERR_OK;//replaced with correct return value
}

/*
 * Pops a scrap with the given key from a ScrapList.
 * (in/out) *list - the ScrapList to pop the scrap from
 * (in) key - the key of the scrap to pop
 * (out) **scrap - pointer to the popped scrap
 *
 * Returns: ERR_NULL_POINTER if list or scrap is NULL, ERR_SCRAP_NOT_FOUND if the scrap is not in the list, ERR_OK otherwise
 */
int scrap_list_pop(ScrapList* list, int key, Scrap** scrap) {
    if (list==NULL || scrap==NULL){
        *scrap = NULL;//if input is invalid, *scrap should be NULL
        return ERR_NULL_POINTER;//null pointer error
    }

    *scrap = NULL;//*scrap should be NULL if scrap is not found

    ScrapNode* curr = list->head;//start from head of list
    ScrapNode* prev = NULL;

    while (curr!=NULL && curr->data->key !=key){
        prev = curr;//previous node
        curr = curr->next;//next node
    }
    if (curr==NULL){
        return ERR_SCRAP_NOT_FOUND;//scrap isn't found
    }
    *scrap = curr->data;//scrap is found
    list->total_value -= curr->data->value;//subtract scrap's value from total value

    if (prev==NULL){//scrap is at head
        list->head =curr->next;
    }else{//scrap not at head
        prev->next= curr->next;
    }
    free(curr);//free node, not data
    return ERR_OK;
}

/*
 * Cleans up a ScrapList, freeing all nodes and their associated scraps.
 * (in/out) *list - the ScrapList to clean
 *
 * Returns: ERR_NULL_POINTER if list is NULL, ERR_OK otherwise
 */
int scrap_list_clean(ScrapList* list) {
    if (list==NULL){
       return ERR_NULL_POINTER; //null pointer error
    } 
    ScrapNode* curr = list->head;
    ScrapNode* next;
    int err;
    while (curr!=NULL) {//while curr isn't NULL
        next = curr->next;
        err = scrap_destroy(&curr->data); //free scrap data
        if (err!=ERR_OK){
            return err;//return error is scrap_destroy fails
        }
        free(curr);//free node
        curr = next;
    }
    list->head = NULL;//head is now NULL
    list->total_value = 0;
    return ERR_OK;
}
