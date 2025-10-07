#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <semaphore.h> 
#include "defs.h"

void* hunter_thread(void* arg) {
    Hunter* hunter = (Hunter*)arg;//cast back to Hunter* 

    Scrap *found = NULL;
    int err, direction;
    Room *new_room = NULL;

    while (hunter->team->scraps.total_value <CASH_QUOTA){//loop until quota is met
        direction = util_get_random_direction(hunter->room);//select new room
        new_room = hunter->room->connections[direction];

        err = hunter_move(hunter, new_room);//move to new room
        if (err != ERR_OK) {
            util_log(LOG_LEVEL_ERROR, "[%s] experienced an error while moving.\n", hunter->name);
            break;//exit when error occurs
        };

        err = hunter_search(hunter, &found);//search room 
        if (err != ERR_OK) {
            util_log(LOG_LEVEL_ERROR, "[%s] experienced an error while searching.\n", hunter->name);
            break; 
        }


        if (found != NULL) {//if scrap is found, add to team's list and add value
            err = hunter_take_scrap(hunter, found);
            if (err != ERR_OK) {
                util_log(LOG_LEVEL_ERROR, "[%s] experienced an error while taking a scrap.\n", hunter->name);
                break;
            }
        }
    }
    pthread_exit(NULL);//exit thread
}

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
    Room *old_room = hunter->room;

    util_log(LOG_LEVEL_MORE, "[%s] moving from [%s] into [%s]", hunter->name, old_room->name, new_room->name);

    if (old_room->id < new_room->id){
        sem_wait(&old_room->hunters.mutex);//ensure locks are acquired in some order to avoid deadlocks
        sem_wait(&new_room->hunters.mutex);//lock rooms based on ID for order
    } else{
        sem_wait(&new_room->hunters.mutex);//else wait on new room first
        sem_wait(&old_room->hunters.mutex);
    }

    err = hunter_list_remove(&old_room->hunters, hunter);
    if (err != ERR_OK){//if removing failed, unlock and return error
        sem_post(&old_room->hunters.mutex);
        sem_post(&new_room->hunters.mutex);
        return err;
    }

    err = hunter_list_add(&new_room->hunters, hunter);
    if (err != ERR_OK) {

        hunter_list_add(&old_room->hunters, hunter); // Try to put hunter back
        sem_post(&old_room->hunters.mutex);
        sem_post(&new_room->hunters.mutex);
        return err;
    }

    hunter->room = new_room;

    sem_post(&new_room->hunters.mutex);//unlock new room's hunters list
    sem_post(&old_room->hunters.mutex);

    return err;
}
/*
 * Searches for a scrap in the room the hunter is in.
 * (in) *hunter - the hunter who is searching for a scrap
 * (out) **found - where to store the resulting scrap, or NULL if no scrap was found
 * 
 * Returns: ERR_NULL_POINTER if hunter or found is NULL, or an error code from scrap_list_pop() (not including ERR_SCRAP_NOT_FOUND), or ERR_OK otherwise
*/
int hunter_search(Hunter *hunter, Scrap **found) {
    if (hunter == NULL || found == NULL) return ERR_NULL_POINTER;
    int key, err, searches_remaining = NUM_SEARCHES;

    util_log(LOG_LEVEL_VERBOSE, "[%s] searching for a scrap.", hunter->name);

    *found = NULL;

    sem_wait(&hunter->room->scraps.mutex); // Lock the scrap list of the current room
    while (*found == NULL && searches_remaining > 0) {
        key = util_get_random_key();
        err = scrap_list_pop(&hunter->room->scraps, key, found);


        if (err != ERR_OK && err != ERR_SCRAP_NOT_FOUND) {
            sem_post(&hunter->room->scraps.mutex); // Unlock before breaking
            break;
        }
        searches_remaining--;
    }
    sem_post(&hunter->room->scraps.mutex); // Unlock the scrap list

    util_log(LOG_LEVEL_VERBOSE, "[%s] DONE searching for a scrap.", hunter->name);

    if (err == ERR_SCRAP_NOT_FOUND) {
        err = ERR_OK;
    }

    return err;
}

/*
 * Stores the incoming scrap into the team and update's the hunter's personal value.
 * (in/out) *hunter - the hunter who is receiving the scrap
 * (in) *scrap - the scrap to be added to the team and value added to the hunter
 * 
 * Returns: ERR_NULL_POINTER if hunter or scrap is NULL, or an error code from scrap_list_add(), or ERR_OK otherwise
*/
int hunter_take_scrap(Hunter *hunter, Scrap *scrap) {
    if (hunter == NULL || scrap == NULL) return ERR_NULL_POINTER;
    int err;

    util_log(LOG_LEVEL_VERBOSE, "[%s] TAKING a scrap.", hunter->name);

    sem_wait(&hunter->team->scraps.mutex); //lock team's scrap list
    err = scrap_list_add(&hunter->team->scraps,scrap);//add scrap to team's list
    sem_post(&hunter->team->scraps.mutex); //unlock team's scrap list


    util_log(LOG_LEVEL_VERBOSE, "[%s] ADDED scrap to team.", hunter->name);
    hunter->cash += scrap->value;

    util_log(LOG_LEVEL_MORE, "[%s] found a %s worth $%d!", hunter->name, scrap->name, scrap->value);

    return err;
}
/*
 * Initializes a HunterList, setting the head to NULL.
 * (out) *list - the HunterList to initialize
 * 
 * Returns: ERR_NULL_POINTER if list is NULL, ERR_OK otherwise
*/
int hunter_list_init(HunterList* list) {
    if (list == NULL) return ERR_NULL_POINTER;
    list->head = NULL;
    sem_init(&list->mutex, 0, 1); // Initialize semaphore
    return ERR_OK;
}

int hunter_list_add(HunterList* list, Hunter* hunter) {

    if (list == NULL || hunter == NULL) return ERR_NULL_POINTER;


    HunterNode* new_node = malloc(sizeof(HunterNode));
    new_node->data = hunter;
    new_node->next = list->head;
    list->head = new_node;
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
    if (list == NULL || hunter == NULL) return ERR_NULL_POINTER;


    HunterNode* curr = list->head;
    HunterNode* prev = NULL;


    if (curr == NULL) return ERR_HUNTER_NOT_FOUND;


    if (curr->data == hunter) {
        list->head = curr->next;
        free(curr);
        return ERR_OK;
    }


    while (curr != NULL && curr->data != hunter) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) return ERR_HUNTER_NOT_FOUND;

    prev->next = curr->next;
    free(curr);

    return ERR_OK;
}
/*
 * Cleans up a HunterList, freeing all nodes.
 * (in/out) *list - the HunterList to clean
 * 
 * Returns: ERR_NULL_POINTER if list is NULL, ERR_OK otherwise
*/
int hunter_list_clean(HunterList* list) {
    if (list == NULL) return ERR_NULL_POINTER;

    HunterNode* curr = list->head;
    HunterNode* next;
    while (curr != NULL) {
        next = curr->next;
        free(curr);
        curr = next;
    }
    sem_destroy(&list->mutex);//destroy semaphore
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

    if (team == NULL || starting_room == NULL) return ERR_NULL_POINTER;

    
    for (i = 0; i < NUM_HUNTERS; ++i) {
        sprintf(name, "Hunter %02d", i + 1);
        err = hunter_init(&team->hunters[i], name, starting_room, team);
        if (err != ERR_OK) return err;
        err = hunter_list_add(&starting_room->hunters, &team->hunters[i]);
        if (err != ERR_OK) return err;
    }

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
    if (team == NULL) return ERR_NULL_POINTER;

    return scrap_list_clean(&team->scraps);
} 
