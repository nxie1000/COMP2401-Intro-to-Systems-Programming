#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "defs.h"

/*
 * Initializes a hunter with a starting cash of 0.
 * (out) *hunter - the hunter to initialize
 * (in) *name - the hunter's name
 * (in) *starting_room - the room where the hunter starts
 * (in) *team - the team the hunter is a part of
 *
 * Returns: ERR_NULL_POINTER if name or hunter is NULL, ERR_OK otherwise
 */
int hunter_init(Hunter* hunter, const char* name, Room* starting_room, Team* team) {
    if (hunter == NULL || name == NULL) return ERR_NULL_POINTER;

    strncpy(hunter->name, name, MAX_STR);
    hunter->name[MAX_STR - 1] = '\0';
    hunter->room = starting_room;
    hunter->cash = 0;
    hunter->team = team;

    return ERR_OK;
}

/*
 * Moves a hunter from their current room to a new room.
 * (in) *hunter - the hunter to move
 * (in) *new_room - the room to move the hunter to
 *
 * Returns: ERR_NULL_POINTER if hunter or new_room is NULL, or an error code from hunter_list_remove or hunter_list_add if they fail, ERR_OK otherwise
 */
int hunter_move(Hunter* hunter, Room* new_room) {
    if (hunter == NULL || new_room == NULL) return ERR_NULL_POINTER;

    int err = ERR_OK;
    // Remove from the old room
    util_log(LOG_LEVEL_VERBOSE, "[%s] leaving room %s...", hunter->name, hunter->room->name);
    err = hunter_list_remove(&hunter->room->hunters, hunter);
    if (err != ERR_OK) return err;

    // Add to the new room
    util_log(LOG_LEVEL_VERBOSE, "[%s] entering room %s...", hunter->name, new_room->name);
    err = hunter_list_add(&new_room->hunters, hunter);
    if (err != ERR_OK) return err;

    hunter->room = new_room;

    return err;
}

/*
 * Executes an action for a hunter, including moving to a new room and searching for scrap.
 * (in) *hunter - the hunter performing the action
 *
 * Returns: ERR_NULL_POINTER if hunter is NULL, or an error code from hunter_move, scrap_list_pop, or scrap_list_add if they fail, ERR_OK otherwise
 */
int hunter_action(Hunter* hunter) {
    int key, err, searches_remaining = NUM_SEARCHES;
    Scrap *found = NULL;

    // 1. Pick a random adjacent room
    int direction = util_get_random_direction(hunter->room);
    Room *new_room = hunter->room->connections[direction];

    // 2. Move to the new room
    err = hunter_move(hunter, new_room);
    if (err != ERR_OK) return err;

    // 3. Search the room by generating random keys and looking through the list
    while (found == NULL && searches_remaining > 0) {
        key = util_get_random_key();
        err = scrap_list_pop(&hunter->room->scraps, key, &found);
        // We can handle scrap not found, this is expected behaviour
        if (err != ERR_OK && err != ERR_SCRAP_NOT_FOUND) return err;
        searches_remaining--;
    }

    // 4. If a scrap was found, add it to the team's list and add the value to the hunter's cash
    if (found != NULL) {
        err = scrap_list_add(&hunter->team->scraps, found);
        if (err != ERR_OK) return err;

        hunter->cash += found->value;
        util_log(LOG_LEVEL_MORE, "[%s] found a %s worth $%d!", hunter->name, found->name, found->value);
    }

    return ERR_OK;
}

/*
 * Initializes a HunterList, setting the head to NULL.
 * (out) *list - the HunterList to initialize
 *
 * Returns: ERR_NULL_POINTER if list is NULL, ERR_OK otherwise
 */
int hunter_list_init(HunterList* list) {
    if (list==NULL){
        return ERR_NULL_POINTER; //null pointer error
    } 
    list->head = NULL;
    return ERR_OK;//replace with the correct value
}

/*
 * Adds a hunter to the head of a HunterList.
 * (in/out) *list - the HunterList to add the hunter to
 * (in) *hunter - the hunter to add
 *
 * Returns: ERR_NULL_POINTER if list or hunter is NULL, ERR_OK otherwise
 */
int hunter_list_add(HunterList* list, Hunter* hunter) {
    if (list==NULL || hunter==NULL){
       return ERR_NULL_POINTER; //null pointer error
    } 

    HunterNode* newNode = (HunterNode*)malloc(sizeof(HunterNode));//add hunter to head of a HunterList
    if (newNode==NULL){
        return ERR_NULL_POINTER;//failure to allocate the memory
    } 
    newNode->data = hunter;//newNode->data is now hunter
    newNode->next = list->head;//next is now head
    list->head = newNode;//head is now newNode
    return ERR_OK;
}

/*
 * Removes a hunter from a HunterList.
 * (in/out) *list - the HunterList to remove the hunter from
 * (in) *hunter - the hunter to remove
 *
 * Returns: ERR_NULL_POINTER if list or hunter is NULL, ERR_HUNTER_NOT_FOUND if the hunter is not in the list, ERR_OK otherwise
 */
int hunter_list_remove(HunterList* list, Hunter* hunter) {
    if (list==NULL || hunter==NULL){
        return ERR_NULL_POINTER;//null pointer error
    } 
    HunterNode* curr = list->head;//start from head of the list
    HunterNode* prev = NULL;//previous node is NULL

    while (curr!=NULL && curr->data !=hunter){
        prev = curr;//previous node is now current node
        curr = curr->next;//move to next node
    }
    if (curr==NULL){//hunter is not found
        return ERR_HUNTER_NOT_FOUND;
    }
    if (prev==NULL) {//hunter is at head
        list->head = curr->next;
    }else{
        prev->next = curr->next;//previous node's next is now current node's next
    }

    free(curr);//free 
    return ERR_OK;
}

/*
 * Cleans up a HunterList, freeing all nodes.
 * (in/out) *list - the HunterList to clean
 *
 * Returns: ERR_NULL_POINTER if list is NULL, ERR_OK otherwise
 */
int hunter_list_clean(HunterList* list) {
    if (list==NULL){
        return ERR_NULL_POINTER;//null pointer error
    }
    HunterNode* curr = list->head;//current node starts at head
    HunterNode* next;

    while (curr!=NULL){
        next = curr->next;//next is now current node's next
        free(curr);//free current node
        curr = next;//move to next node
    }
    list->head = NULL;//head is now NULL
    return ERR_OK;
}

/*
 * Initializes a team of hunters and assigns them to a starting room.
 * (out) *team - the team to initialize
 * (in) *starting_room - the room where the team starts
 *
 * Returns: ERR_NULL_POINTER if team or starting_room is NULL, or an error code from hunter_init, hunter_list_add, or scrap_list_init if they fail, ERR_OK otherwise
 */
int team_init(Team* team, Room* starting_room) {
    int i, err;
    char name[MAX_STR];
    if (team==NULL || starting_room==NULL){
        return ERR_NULL_POINTER;//null pointer error
    } 

    // Give all of the hunters basic starting names
    for (i = 0; i < NUM_HUNTERS; ++i) {
        sprintf(name, "Hunter %02d", i + 1);
        err = hunter_init(&team->hunters[i], name, starting_room, team);
        if (err != ERR_OK) return err;
        err = hunter_list_add(&starting_room->hunters, &team->hunters[i]);
        if (err != ERR_OK) return err;
    }

    // Initialize the list of scrap
    err = scrap_list_init(&team->scraps);
    if (err != ERR_OK) return err;

    return ERR_OK;
}

/*
 * Cleans up a team, freeing all scraps in their list.
 * (in/out) *team - the team to clean
 *
 * Returns: ERR_NULL_POINTER if team is NULL, or an error code from scrap_list_clean if it fails, ERR_OK otherwise
 */
int team_clean(Team* team) {
    if (team==NULL){
        return ERR_NULL_POINTER;//null pointer error
    }
    return scrap_list_clean(&team->scraps);//return the result of cleaning the scrap list
}
