CC = gcc
FLAGS = -g -Wall
MEDIATOR = mediator
PLAYER = player
MAP = map

MAP_FILENAME = map.txt
STATUS_FILENAME = status.txt
COMMANDS_FILENAME = commands.txt
TIME_LIMIT = 60

build: $(MEDIATOR).out $(PLAYER).out

$(MEDIATOR).out: $(MEDIATOR).o $(MAP).o
	$(CC) $(FLAGS) $^ -o $@

$(MEDIATOR).o: $(MEDIATOR).c 
	$(CC) $(FLAGS) -c $^ -o $@

$(PLAYER).out: $(PLAYER).o $(MAP).o
	$(CC) $(FLAGS) $^ -o $@

$(PLAYER).o: $(PLAYER).c
	$(CC) $(FLAGS) -c $^ -o $@

$(MAP).o: $(MAP).c
	$(CC) $(FLAGS) -c $^ -o $@

test: $(MEDIATOR).out
	./$(MEDIATOR).out $(MAP_FILENAME) $(STATUS_FILENAME) $(COMMANDS_FILENAME) $(TIME_LIMIT)

memcheck: $(MEDIATOR).out
	valgrind --leak-check=yes --track-origins=yes \
	./$(MEDIATOR).out $(MAP_FILENAME) $(STATUS_FILENAME) $(COMMANDS_FILENAME) $(TIME_LIMIT)

clean:
	rm -f *.o  *.out
