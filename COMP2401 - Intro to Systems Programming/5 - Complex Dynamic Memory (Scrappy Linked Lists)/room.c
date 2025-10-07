#include <string.h>
#include <stdlib.h>
#include "defs.h"

/*
 * Initializes a room with the given id and name.
 * (out) *room - the room to initialize
 * (in) id - the unique identifier for the room
 * (in) *name - the name of the room
 *
 * Returns: ERR_OK
 */
int room_init(Room* room, int id, const char* name) {
    room->id = id;
    strncpy(room->name, name, MAX_STR);
    hunter_list_init(&room->hunters);
    scrap_list_init(&room->scraps);

    for (int i = 0; i < NUM_DIRECTIONS; ++i) {
        room->connections[i] = NULL;
    }

    return ERR_OK;
}

/*
 * Connects two rooms in the specified direction.
 * (in/out) *room_a - the first room to connect
 * (in) direction - the direction from room_a to room_b
 * (in/out) *room_b - the second room to connect
 *
 * Returns: ERR_OK
*/
int room_connect(Room* room_a, int direction, Room* room_b) {
    room_a->connections[direction] = room_b;
    room_b->connections[OPPOSITE_DIR(direction)] = room_a;
    return ERR_OK;
}

/*
 * Cleans up a room by freeing all hunters and scraps.
 * (in/out) *room - the room to clean
 *
 * Returns: ERR_OK
 */
int room_clean(Room* room) {
    if (room==NULL){
       return ERR_NULL_POINTER;//null pointer error
    } 
    int err;

    err = hunter_list_clean(&room->hunters);
    if (err!=ERR_OK){//if error occurs while cleaning hunters
        return err;
    }

    err = scrap_list_clean(&room->scraps);
    if (err!=ERR_OK){//if error occurs while cleaning scraps
        return err;
    }
    return ERR_OK;
}
