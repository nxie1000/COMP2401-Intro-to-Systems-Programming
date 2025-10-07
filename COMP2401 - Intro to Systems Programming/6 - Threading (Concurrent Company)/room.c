#include <string.h>
#include <stdlib.h>
#include "defs.h"

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


int room_connect(Room* room_a, int direction, Room* room_b) {
    room_a->connections[direction] = room_b;
    room_b->connections[OPPOSITE_DIR(direction)] = room_a;
    return ERR_OK;
}

int room_clean(Room* room) {
    hunter_list_clean(&room->hunters);
    scrap_list_clean(&room->scraps);
    return ERR_OK;
}

