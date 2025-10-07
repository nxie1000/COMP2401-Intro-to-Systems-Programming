OPT = -Wall -g 
OBJ = scrap.o room.o main.o hunter.o utils.o

a6: $(OBJ)
	gcc $(OPT) -lpthread -o a6 $(OBJ)

main.o: main.c
	gcc $(OPT) -pthread -c main.c -o main.o

utils.o: utils.c
	gcc $(OPT) -pthread -c utils.c -o utils.o

hunter.o: hunter.c
	gcc $(OPT) -pthread -c hunter.c -o hunter.o

scrap.o: scrap.c
	gcc $(OPT) -pthread -c scrap.c -o scrap.o

room.o: room.c
	gcc $(OPT) -pthread -c room.c -o room.o

clean:
	rm -f $(OBJ) a6
