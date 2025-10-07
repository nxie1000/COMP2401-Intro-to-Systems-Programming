#include <stdio.h>
#include <string.h>
#include "readings.h"


int print_error(int error_code, const char* note) {
  if (note == NULL) {
    note = "";
  }
 
  switch(error_code) {
    case ERR_OK:
      break;
    case ERR_ID_NOT_FOUND:
      printf("Error: No reading found with ID in the reading collection [%s]\n", note);
      break;
    case ERR_DUPLICATE_ID:
      printf("Error: A reading with the provided ID already exists in the reading collection [%s]\n", note);
      break;
    case ERR_MEMORY_ALLOCATION:
      printf("Error: Memory allocation error; malloc/calloc failed to allocate memory [%s]\n", note);
      break;
    case ERR_NULL_POINTER:
      printf("Error: Null pointer passed as an argument; likely the pointer passed was not initialized properly [%s]\n", note);
      break;
    case ERR_NOT_IMPLEMENTED:
      printf("Error: Function not implemented [%s]\n", note);
      break;
    default:
      printf("Error: Unknown error [%s]\n", note);
      break;
  }


  return error_code;
}


// Helper function to get a reading input from the user
int add_reading_input(int *id, int *device, float *value, char *room) {
    printf("Enter reading ID: ");
    scanf("%d", id);


    // fgets() allows us to get a room name with spaces in it,
    // and it will read up to MAX_STR-1 characters then add the null terminator.
    // First, though, we'll use getc() to get rid of any new line characters that might
    // still be in our print buffer.
    printf("Enter room name: ");
    while ((getchar()) != '\n');
    fgets(room, MAX_STR, stdin);


    printf("Enter device type (%d = EMF, %d = Thermal, %d = Sound): ", DEVICE_EMF, DEVICE_THERMAL, DEVICE_SOUND);
    scanf("%d", device);


    if (*device != DEVICE_EMF && *device != DEVICE_THERMAL && *device != DEVICE_SOUND) {
      return ERR_INVALID_DEVICE;
    }


    printf("Enter value: ");
    scanf("%f", value);


    // However, we need to remove the newline character from fgets()'s input.
    room[strlen(room) - 1] = '\0';


    return ERR_OK;
}


/*
  Function: Print Menu
  Purpose:  Print the main menu and get the user's choice via user input
  Params:  
    Output: int* choice - the choice typed by the user
*/
void print_menu(int* choice)
{
  // NOTE: Do not make _any_ modifications to this function.
  int c = -1;


  printf("\nMAIN MENU\n");
  printf("  (%d) Add reading\n", MENU_ADD);
  printf("  (%d) Remove reading\n", MENU_REMOVE);
  printf("  (%d) Print reading\n", MENU_PRINT);
  printf("  (%d) Exit\n\n", MENU_EXIT);


  while (c < 0 || c > 3) {
    printf("Please enter your selection: ");
    scanf("%d", &c);
  }
  *choice = c;
  
  return;
}
