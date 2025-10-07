#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "defs.h"

int main() {
    srand(time(NULL));

    Room rooms[NUM_ROOMS];
    Team team;
    int i;
    pthread_t hunter_threads[NUM_HUNTERS];//array for hunter threads

    util_error(util_generate_rooms(rooms), "Couldn't generate rooms");
    util_error(team_init(&team, rooms), "Couldn't initialize team");


    for (i = 0; i < NUM_HUNTERS; ++i) {
        if (pthread_create(&hunter_threads[i], NULL, hunter_thread, &team.hunters[i]) != 0) {
            util_error(ERR_NOT_IMPLEMENTED, "Failed to create thread for hunter %d", i);

            break;
        }
    }


    for (i = 0; i < NUM_HUNTERS; ++i) {
        if (pthread_join(hunter_threads[i], NULL) != 0) {
            util_error(ERR_NOT_IMPLEMENTED, "Failed to join thread for hunter %d", i);
        }
    }

 
    util_log(LOG_LEVEL_BASIC, "Total value of collected scraps: $%d", team.scraps.total_value);

    util_verify_totals(&team);
    util_verify_rooms(&team, rooms);

    util_error(team_clean(&team), "Couldn't clean team");

    for (i = 0; i < NUM_ROOMS; ++i) {
        util_error(room_clean(rooms + i), "Couldn't clean room %d", i);
    }

    //return 0; 
}
