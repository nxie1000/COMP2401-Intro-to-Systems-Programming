#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "defs.h"

// Making this function static means no other file can link to it
/*
 * Generates a scrap with random characteristics and assigns it to the provided pointer.
 * (out) **scrap - pointer to the scrap to be generated
 * 
 * Returns: ERR_OK if successful, error code otherwise
*/
static int _generate_scrap(Scrap **scrap);

/*
 * Generates rooms and sets up their connections, including populating scrap.
 * (in/out) rooms[] - array of rooms to be initialized with values. Must be minimum sized 10 for this example data.
 * 
 * Returns: ERR_OK if successful, error code otherwise
*/
int util_generate_rooms(Room rooms[]) {
    int i, j, n_scraps, err, curr_id = 1000;
    Scrap *scrap;

    err = room_init(&rooms[0], curr_id++, "Living Room");
    err = room_init(&rooms[1], curr_id++, "Dining Room");
    err = room_init(&rooms[2], curr_id++, "Kitchen");
    err = room_init(&rooms[3], curr_id++, "Foyer");
    err = room_init(&rooms[4], curr_id++, "Utility Room");
    err = room_init(&rooms[5], curr_id++, "Garage");
    err = room_init(&rooms[6], curr_id++, "Master Bedroom");
    err = room_init(&rooms[7], curr_id++, "Bathroom");
    err = room_init(&rooms[8], curr_id++, "Nursery");
    err = room_init(&rooms[9], curr_id++, "Basement");

    if (err != ERR_OK) {
        return err;
    }

    int connections[][3] = {
        {0, EAST, 2},
        {0, WEST, 6},
        {0, SOUTH, 3},
        {0, DOWN, 9},

        {2, NORTH, 1},
        {2, SOUTH, 4},

        {4, SOUTH, 5},

        {6, SOUTH, 8},

        {3, SOUTH, 7}
    };

    for (i = 0; (unsigned long long)i < sizeof(connections) / sizeof(connections[0]); ++i) {
        err = room_connect(&rooms[connections[i][0]], connections[i][1], &rooms[connections[i][2]]);
        if (err != ERR_OK) {
            return err;
        }
    }

    // Populate each room with some random scraps
    int total_value = 0;
    while (total_value < CASH_QUOTA) {
        for (i = 0; i < NUM_ROOMS; ++i) {
            n_scraps = rand() % MAX_SCRAPS;

            for (j = 0; j < n_scraps; ++j) {
                err = _generate_scrap(&scrap);
                if (err != ERR_OK) {
                    return err;
                }

                total_value += scrap->value;

                err = scrap_list_add(&rooms[i].scraps, scrap);
                if (err != ERR_OK) {
                    free(scrap);
                    return err;
                }
            }
        }
    }

    util_log(LOG_LEVEL_MORE, "Total value of scraps in generated map: %d\n", total_value);
    return ERR_OK;
}

/*
 * Selects a random valid direction from the given room.
 * (in) *room - the room from which to choose a direction
 * 
 * Returns: a valid direction index, or -1 if no connections are available
*/
int util_get_random_direction(const Room *room) {
    int i, j, direction = -1;
    int options[NUM_DIRECTIONS];

    // Array of all directions
    for (i = 0; i < NUM_DIRECTIONS; ++i) {
        options[i] = i;
    }
    // Random swap them a bit
    for (i = 0; i < NUM_DIRECTIONS - 1; ++i) {
        j = rand() % NUM_DIRECTIONS;
        direction = options[i];
        options[i] = options[j];
        options[j] = direction;
    }
    // Check each to find a valid direction
    for (i = 0; i < NUM_DIRECTIONS; ++i) {
        direction = options[i];
        if (room->connections[direction] != NULL) {
            break;
        }
    }
    
    return direction;
}

/*
 * Verifies that the sum of the hunters' cash matches the sum of scrap values.
 * (in) *team - the team to verify
 * 
 * Returns: ERR_OK if sums match, ERR_VERIFICATION_FAILED otherwise
*/
int util_verify_totals(Team* team) {
    int team_sum = 0, scrap_sum = 0, err = ERR_OK;
    ScrapNode *curr = team->scraps.head;

    for (int i = 0; i < NUM_HUNTERS; ++i) {
        team_sum += team->hunters[i].cash;
    }

    while (curr != NULL) {
        scrap_sum += curr->data->value;
        curr = curr->next;
    }

    // Print if there is a mismatch between the hunter total, the calculated scrap total, or the scrap list's total
    if (team_sum != scrap_sum) {
        util_error(ERR_VERIFICATION_FAILED, "[X] Verification failed: Team cash (%d) does not match calculated scrap value (%d)\n", team_sum, scrap_sum);
        err = ERR_VERIFICATION_FAILED;
    }
    if (scrap_sum != team->scraps.total_value) {
        util_error(ERR_VERIFICATION_FAILED, "[X] Verification failed: Calculated scrap total (%d) does not match scrap list's total (%d)\n", scrap_sum, team->scraps.total_value);
        err = ERR_VERIFICATION_FAILED;
    }
    return err;
}

/*
 * Generates a random key value for scrap identification.
 * 
 * Returns: a random key value between 0 and DIFFICULTY
*/
int util_get_random_key() {
    return rand() % DIFFICULTY;
}

/*
 * Generates a scrap with random characteristics and assigns it to the provided pointer.
 * (out) **scrap - pointer to the scrap to be generated
 * 
 * Returns: ERR_OK if successful, error code otherwise
*/
static int _generate_scrap(Scrap **scrap) {
    if (scrap == NULL) return -1;
    
    struct Modifier {
        const char* name;
        int value;
    };

    struct Item {
        const char* name;
        int base_value;
    };

    static const struct Modifier modifiers[] = {
        { "Large", 5 },
        { "Normal", 0}, 
        { "Shiny", 3 },
        { "Rusty", 2 },
        { "Ancient", 10 },
        { "Modern", 7 }
    };

    static const struct Item items[] = {
        { "Air Horn", 10 },
        { "Bottles", 3 },
        { "Bolt", 4 },
        { "Egg Beater", 6 },
        { "Hair Brush", 8 },
        { "Plastic Cup", 2 },
        { "Whoopie-Cushion", 15 }
    };

    int key = util_get_random_key();

    // Choose a random modifier and item
    int modifier_index = rand() % (sizeof(modifiers) / sizeof(modifiers[0]));
    int item_index = rand() % (sizeof(items) / sizeof(items[0]));

    char scrap_name[MAX_STR];
    snprintf(scrap_name, MAX_STR, "%s %s", modifiers[modifier_index].name, items[item_index].name);

    int value = modifiers[modifier_index].value + items[item_index].base_value;

    return scrap_create(scrap, key, value, scrap_name);
}

/*
 * Logs a message with the given logging level and format.
 * (in) level - the logging level for the message
 * (in) *format - the format string (similar to printf)
 * 
 * Uses variadic arguments to allow passing a variable number of arguments.
*/
void util_log(int level, const char* format, ...) {  
    
    if (LOG_LEVEL < level) return;

    // This is how we can use "Variadic Arguments" in C
    // It allows us to pass a variable number of arguments to a function, like how printf() works.
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

/*
 * Logs an error message with the given error code and format.
 * (in) error - the error code to log
 * (in) *format - the format string (similar to printf)
 * 
 * Returns: the error code provided. Exits the program if EXIT_ON_LOG_ERROR is set and error is not ERR_OK.
*/
int util_error(int error, const char* format, ...) {
    if (LOG_LEVEL < LOG_LEVEL_ERROR || error == ERR_OK) return error;
    
    // Only print an error if there's an error
    printf("[X] [");

    switch (error) {
        case ERR_NULL_POINTER:
            printf("NULL POINTER");
            break;
        case ERR_NOT_IMPLEMENTED:
            printf("NOT IMPLEMENTED");
            break;
        case ERR_NO_VALID_CONNECTIONS:
            printf("NO VALID CONNECTIONS");
            break;
        case ERR_VERIFICATION_FAILED:
            printf("VERIFICATION FAILED");
            break;
        case ERR_HUNTER_NOT_FOUND:
            printf("HUNTER NOT FOUND");
            break;
        case ERR_SCRAP_NOT_FOUND:
            printf("SCRAP NOT FOUND");
            break;
        default:
            printf("UNKNOWN ERROR");
            break;
    }

    printf("] ");
    
    // This is how we can use "Variadic Arguments" in C
    // It allows us to pass a variable number of arguments to a function, like how printf() works.
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");

    if (EXIT_ON_LOG_ERROR && error != ERR_OK) {
        exit(1);
    }

    return error;
}

