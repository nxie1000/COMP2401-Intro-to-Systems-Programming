#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> 
#include <semaphore.h> 

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_BASIC 2
#define LOG_LEVEL_MORE 3
#define LOG_LEVEL_VERBOSE 4

// Set this to the level of logging that you would like to use
#define LOG_LEVEL LOG_LEVEL_MORE

// Set this to 0 if you want to continue running after logging an error
#define EXIT_ON_LOG_ERROR 1

#define ERR_OK 0
#define ERR_NULL_POINTER -1
#define ERR_NOT_IMPLEMENTED -2
#define ERR_NO_VALID_CONNECTIONS -3
#define ERR_VERIFICATION_FAILED -4
#define ERR_HUNTER_NOT_FOUND -5
#define ERR_SCRAP_NOT_FOUND -6

#define CASH_QUOTA    50000
#define NUM_SEARCHES  2
#define DIFFICULTY    2
#define MAX_SCRAPS    2
#define MAX_STR       32
#define NUM_HUNTERS   100   // To simplify the assignment, we will assume we have exactly this many hunters every run
#define NUM_ROOMS     2   // To simplify the assignment, we will assume we have exactly this many rooms every run

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3
#define UP 4
#define DOWN 5
#define NUM_DIRECTIONS 6

// These macros are used in the provided code, but you do not need to use them yourself
#define OPPOSITE_DIR(dir) dir%2 ? dir-1 : dir+1

// Forward declare and typedef structures for clarity later
typedef struct Scrap Scrap;
typedef struct ScrapNode ScrapNode;
typedef struct ScrapList ScrapList;
typedef struct Room Room;
typedef struct Hunter Hunter;
typedef struct HunterNode HunterNode;
typedef struct HunterList HunterList;
typedef struct Team Team;

struct Scrap {
    int key;
    int value;
    char name[MAX_STR];
};

struct ScrapNode {
    Scrap *data;
    ScrapNode *next;
};

struct ScrapList {
    ScrapNode *head;
    int total_value;
    sem_t mutex;
};

struct HunterNode {
    Hunter *data;
    HunterNode *next;
};

struct HunterList {
    HunterNode *head;
    sem_t mutex;
};

struct Room {
    int id;
    char name[MAX_STR];
    ScrapList scraps;
    HunterList   hunters;
    Room* connections[NUM_DIRECTIONS]; // N, E, S, W, UP, DOWN, initialized to NULL
};

struct Hunter {
    Room* room;
    int     cash;
    char    name[MAX_STR];
    Team    *team;
};

struct Team {
    Hunter hunters[NUM_HUNTERS]; // In this simulation, there will always be NUM_HUNTERS, so we won't track size
    ScrapList scraps;
};

// Scrap-related functions
int scrap_create(Scrap** scrap, int key, int value, const char* name);
int scrap_destroy(Scrap** scrap);

int scrap_list_init(ScrapList* list);
int scrap_list_add(ScrapList* list, Scrap* scrap);
int scrap_list_pop(ScrapList* list, int key, Scrap** scrap);
int scrap_list_clean(ScrapList* list);

// Room-related functions
int room_init(Room* room, int id, const char* name);
int room_connect(Room* room_a, int direction, Room* room_b);
int room_clean(Room* room);

// Hunter-related functions
int hunter_init(Hunter* hunter, const char* name, Room* starting_room, Team* team);
int hunter_move(Hunter* hunter, Room* new_room);
int hunter_search(Hunter *hunter, Scrap **found);
int hunter_take_scrap(Hunter *hunter, Scrap *scrap);

void* hunter_thread(void* arg);

int hunter_list_init(HunterList* list);
int hunter_list_add(HunterList* list, Hunter* hunter);
int hunter_list_remove(HunterList* list, Hunter* hunter);
int hunter_list_clean(HunterList* list);

// Team-related functions
int team_init(Team* team, Room* starting_room);
int team_clean(Team* team);

// Utils to help the simulation get setup and analyzed
int util_generate_rooms(Room rooms[]);
int util_get_random_direction(const Room* from_room);
int util_verify_totals(Team* team);
int util_get_random_key();
void util_log(int level, const char* message, ...);
int util_error(int error, const char* message, ...);
int util_verify_rooms(Team *team, Room rooms[]);
