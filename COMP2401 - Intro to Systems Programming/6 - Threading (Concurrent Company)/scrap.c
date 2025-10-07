#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
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
    if (scrap == NULL) {
        return ERR_NULL_POINTER;//null pointer error
    }

    (*scrap) = (Scrap*)malloc(sizeof(Scrap));

    (*scrap)->key = key;
    (*scrap)->value = value;
    strncpy((*scrap)->name, name, MAX_STR - 1);
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

    free(*scrap);
    *scrap = NULL;
    return ERR_OK;
}
/*
 * Initializes a ScrapList, setting the head to NULL and total_value to 0.
 * (out) *list - the ScrapList to initialize
 * 
 * Returns: ERR_NULL_POINTER if list is NULL, ERR_OK otherwise
*/
int scrap_list_init(ScrapList* list) {
    if (list == NULL) {
        return ERR_NULL_POINTER;//null pointer error
    }

    list->head = NULL;
    list->total_value = 0;
    sem_init(&list->mutex,0,1);//initialize the semaphore
    return ERR_OK;
}
/*
 * Adds a scrap to the head of a ScrapList.
 * (in/out) *list - the ScrapList to add the scrap to
 * (in) *scrap - the scrap to add
 * 
 * Returns: ERR_NULL_POINTER if list or scrap is NULL, ERR_OK otherwise
*/
int scrap_list_add(ScrapList* list, Scrap* scrap) {
    if (list == NULL || scrap == NULL) {
        return ERR_NULL_POINTER;
    }


    ScrapNode *new_node = malloc(sizeof(ScrapNode));
    new_node->data = scrap;
    new_node->next = list->head;
    list->head = new_node;

    list->total_value += scrap->value;

    return ERR_OK;
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
    if (list == NULL || scrap == NULL) {
        return ERR_NULL_POINTER;
    }


    ScrapNode *current = list->head;
    ScrapNode *prev = NULL;

    *scrap = NULL;

    while (current != NULL && current->data->key != key) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        return ERR_SCRAP_NOT_FOUND;
    }


    *scrap = current->data;


    if (prev == NULL) {
        list->head = current->next;
    }
    else {

        prev->next = current->next;
    }

    list->total_value -= (*scrap)->value;

    free(current);
    return ERR_OK;
}

/*
 * Cleans up a ScrapList, freeing all nodes and their associated scraps.
 * (in/out) *list - the ScrapList to clean
 * 
 * Returns: ERR_NULL_POINTER if list is NULL, ERR_OK otherwise
*/
int scrap_list_clean(ScrapList* list) {
    if (list == NULL) {
        return ERR_NULL_POINTER;
    }

    ScrapNode* current = list->head;
    ScrapNode* next = NULL;

    while (current != NULL) {
        next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
    sem_destroy(&list->mutex);//destroy semaphore
    return ERR_OK;
}
