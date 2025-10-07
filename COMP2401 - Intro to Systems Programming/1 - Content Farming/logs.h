// Read `main.c` for information about these forward declarations.
// Do NOT define the functionality here, but instead, define them below.
#define MAX_ENTRIES 16
#define ERR_OK 0
#define ERR_INVALID_DURATION  -2
#define ERR_INVALID_ID -3
// Do *not* place your function implementations right here; these are
// forward declarations, and are intended to tell the rest of the code
// that these functions will be found. It means we do not have to worry
// about the order that our functions are written in.
// You can use these "prototypes" or "signatures", and copy-paste them to
// write your functions below, similar to the broken "sort_entries" function.
int input_logs(int ids[], float durations[], int views[]);
int print_logs(int ids[], float durations[], int views[], int size);
int remove_duplicates(int ids[], float durations[], int views[], int size);
int invalid_id(int id);
int invalid_duration(float reading);


