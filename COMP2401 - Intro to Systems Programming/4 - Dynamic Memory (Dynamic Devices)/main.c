#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "readings.h"


// Main control flow
int main() {
    ReadingCollection reading_collection;
    int choice = -1;


    // Temporary variables to store results of menu inputs
    int id, device;
    float value;
    char room[MAX_STR];
    //Reading reading; //this is put elsewhere now


    int error = print_error(init_readings(&reading_collection), "init_readings()");


    if (error) {
        return 0;
    }


    // Print menu and get user choice
    do {
        print_menu(&choice);


        switch(choice) {
            case MENU_ADD:
                error = add_reading_input(&id, &device, &value, room);
                // TODO 03: Correctly initialize a reading with these values and add it to the reading_collection
                if (error==ERR_OK){//if reading input successful
                    Reading reading;
                    error = init_reading(&reading,id,device,value,room);//initialize reading with values
                    if (error==ERR_OK){
                        error = add_reading(&reading_collection,&reading);//add to collection
                        print_error(error,"add_reading()");
                    } else{
                        print_error(error,"init_reading()");
                    }
                } else{
                    print_error(error,"add_reading_input()");
                }
                break;
            case MENU_REMOVE:
                printf("Enter reading ID to remove: ");
                scanf("%d",&id);
                // TODO 04: Correctly remove a reading from the reading_collection
                error = remove_reading(&reading_collection, id); //call remove_reading with the id
                print_error(error,"remove_reading()");//print error if any
                break;
            case MENU_PRINT:
                error = print_readings(&reading_collection);
                break;
            case MENU_EXIT:
                printf("Exiting...\n");
                break;
        }


    } while (choice != MENU_EXIT);


    // Free dynamic memory
    free_readings(&reading_collection);


    return 0;
}






