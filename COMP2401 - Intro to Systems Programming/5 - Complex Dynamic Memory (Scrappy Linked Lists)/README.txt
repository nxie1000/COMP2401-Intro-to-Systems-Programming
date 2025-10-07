Name: Nickolas Xie
Student number: 101337215

Description of program:
Working with linked lists and double pointers, ensuring that all memory is freed.
Implementing functions in hunter.c, scrap.c, room.c files.
Hunters moving around rooms attempting to find scraps. 
If matching scrap is found, remove scrap, add it to inventory
If room has no matching scrap, repeat a few times
Once team's inventory has enough scrap found, the simulation terminates.
Checking with Valgrind to ensure no leaks.


Instructions:
1) Navigate to correct folder
2) gcc hunter.c scrap.c room.c main.c defs.h utils.c -o [name]
3) valgrind ./[name]
