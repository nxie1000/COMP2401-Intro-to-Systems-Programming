#include <stdio.h>
#include <stdlib.h>
#include "scrap.h"

int handle_not_implemented(int err, const char* fname);

int main(void) {
    struct ScrapType airhorn, bigbolt, bottles;
    struct Scrap scrap;
    struct ScrapCollection collection;
    int err;

    // Setup Scrap Types
    printf("1. Initializing scrap types\n");
    err = init_scrap_type(&airhorn, "Air horn", 120);
    handle_not_implemented(err, "init_scrap_type()");

    init_scrap_type(&bigbolt, "Big Bolt", 20);
    init_scrap_type(&bottles, "Bottles", 60);

    // Setup and test a single Scrap
    printf("2. Initializing a single scrap\n");
    err = init_scrap(&scrap, &airhorn, CONDITION_PRISTINE);
    handle_not_implemented(err, "init_scrap()");

    printf("3. Printing single scrap\n");
    print_scrap(&scrap);
    handle_not_implemented(err, "print_scrap()");

    // Initialize collection
    printf("4. Initializing scrap collection\n");
    err = init_scrap_collection(&collection);
    handle_not_implemented(err, "init_scrap_collection()");

    printf("5. Adding several scraps in varied order\n");
    // Add several scraps in varied order
    err = add_scrap(&collection, scrap);
    handle_not_implemented(err, "add_scrap()");

    init_scrap(&scrap, &bigbolt, CONDITION_POOR); 
    add_scrap(&collection, scrap);
    init_scrap(&scrap, &bottles, CONDITION_TYPICAL);
    add_scrap(&collection, scrap);
    init_scrap(&scrap, &bigbolt, CONDITION_PRISTINE);
    add_scrap(&collection, scrap);
    init_scrap(&scrap, &airhorn, CONDITION_USELESS); 
    add_scrap(&collection, scrap);
    init_scrap(&scrap, &bottles, CONDITION_POOR);
    add_scrap(&collection, scrap);

    printf("6. Printing the scrap collection\n");
    print_scrap_collection(&collection);
    handle_not_implemented(err, "print_scrap_collection()");

    printf("7. Testing what happens when trying to hit capacity\n");
    // Fill to max
    int amount_to_fill = MAX_SCRAP - collection.size;
    for (int i = 0; i < amount_to_fill; ++i) {
        init_scrap(&scrap, &airhorn, CONDITION_POOR);
        add_scrap(&collection, scrap);
    }

    // Final collection output
    printf("8. Final scrap collection output after adding until capacity\n");
    print_scrap_collection(&collection);

    printf("9. Testing what happens when trying to exceed capacity\n");
    err = add_scrap(&collection, scrap); // Should exceed capacity
    if (err != ERR_MAX_CAPACITY) {
        printf("[X] add_scrap did not fail at max capacity!\n");
    }
    printf("[✓] add_scrap failed as expected at max capacity.\n");
    printf("10. Exiting tester. Consider adding additional tests.\n");

}

int handle_not_implemented(int err, const char* fname) {
    if (err == ERR_NOT_IMPLEMENTED) {
        printf("Function: [%s] is not implemented, exited tester.\n", fname);
        exit(0);
    }
    return err;
}

