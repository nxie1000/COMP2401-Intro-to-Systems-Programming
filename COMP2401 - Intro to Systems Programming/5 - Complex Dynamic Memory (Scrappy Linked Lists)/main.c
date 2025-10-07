#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "defs.h"

int main() {
    // Seed a random number generator
    srand(time(NULL));

    Room rooms[NUM_ROOMS];
    Team team;
    int i;

    // Create our initial rooms and a generated team
    util_error(util_generate_rooms(rooms), "Couldn't generate rooms");
    util_error(team_init(&team, rooms), "Couldn't initialize team");

    // While scrap list value is less than quota, perform simulation actions
    while (team.scraps.total_value < CASH_QUOTA) {
        // Each hunter will take their action
        for (i = 0; i < NUM_HUNTERS; ++i) {
            util_error(hunter_action(team.hunters + i), "Couldn't perform hunter action for hunter %d", i);
        }
    }

    util_log(LOG_LEVEL_BASIC, "Total value of collected scraps: $%d", team.scraps.total_value);

    util_error(util_verify_totals(&team), "Totals do not add up correctly.");
    util_error(team_clean(&team), "Couldn't clean team");

    for (i = 0; i < NUM_ROOMS; ++i) {
        util_error(room_clean(rooms + i), "Couldn't clean room %d", i);
    }
}
