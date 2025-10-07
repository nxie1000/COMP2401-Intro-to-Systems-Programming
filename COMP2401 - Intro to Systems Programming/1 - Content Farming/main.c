#include <stdio.h>
#include "logs.h"
/*
#define MAX_ENTRIES 16
#define ERR_OK 0
#define ERR_INVALID_DURATION  -2
#define ERR_INVALID_ID -3
*/
/*
int input_logs(int ids[], float durations[], int views[]);
int print_logs(int ids[], float durations[], int views[], int size);
int remove_duplicates(int ids[], float durations[], int views[], int size);
int invalid_id(int id);
int invalid_duration(float reading);*/


int main(void) {
    int ids[MAX_ENTRIES];
    float durations[MAX_ENTRIES];
    int views[MAX_ENTRIES];
    int size = 0;
    //call input_logs
    size = input_logs(ids, durations, views);
    printf("\n--Original entries--\n");
    print_logs(ids, durations, views, size);


    size = remove_duplicates(ids, durations, views, size);
    printf("\n--After removing duplicate--\n");
    print_logs(ids, durations, views, size);


    return 0;
}




