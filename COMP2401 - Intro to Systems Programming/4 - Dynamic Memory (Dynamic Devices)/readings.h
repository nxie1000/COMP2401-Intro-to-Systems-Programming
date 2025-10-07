// Device codes
#define DEVICE_EMF 1
#define DEVICE_THERMAL 2
#define DEVICE_SOUND 3


// Error codes
#define ERR_OK 0
#define ERR_NULL_POINTER -1
#define ERR_MEMORY_ALLOCATION -2
#define ERR_DUPLICATE_ID -3
#define ERR_ID_NOT_FOUND -4
#define ERR_NOT_IMPLEMENTED -5
#define ERR_INVALID_DEVICE -6


// Capacity threshold for shrinking array
#define SHRINK_THRESHOLD 0.75
#define MAX_STR 64      // Fixed size for room name


// UI menu options
#define MENU_ADD 1
#define MENU_REMOVE 2
#define MENU_PRINT 3
#define MENU_EXIT 0


// Structure for a reading
typedef struct {
    // TODO 02: Correctly add the fields of the Reading here.
    int id;
    int device;
    float value;
    char room[MAX_STR];
} Reading;


// Dynamic array to store readings
typedef struct {
    // TODO 01: Correctly add the fields of a ReadingCollection here.
    Reading* readings;
    int size;
    int capacity;
} ReadingCollection;


// Readings prototypes
int init_readings(ReadingCollection *collection);
int init_reading(Reading *reading, int id, int device, float value, const char *room);
int add_reading(ReadingCollection *collection, const Reading* reading);
int remove_reading(ReadingCollection *collection, int id);
int print_reading(const Reading *reading);
int print_readings(const ReadingCollection *collection);
int free_readings(ReadingCollection *collection);
int find_reading(ReadingCollection *collection, int id);


// UI Prototypes
int add_reading_input(int *id, int *device, float *value, char room[]);
void print_menu(int *choice);
int print_error(int error, const char* note);




