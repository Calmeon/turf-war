CC = gcc
FLAGS = -g -Wall 
MEDIATOR = mediator
PLAYER = player
MAP = map
GAME = game
UNITS = units
STRATEGY = strategy

MAP_FILENAME = map.txt
STATUS_FILENAME = status.txt
ORDERS_FILENAME = orders.txt
TIME_LIMIT = 5
TEST_NUMBER = 1000

# Build mediator and player program with all dependencies
all: $(MEDIATOR).out $(PLAYER).out

$(MEDIATOR).out: $(MEDIATOR).o $(MAP).o $(GAME).o $(UNITS).o
	$(CC) $(FLAGS) $^ -o $@

$(MEDIATOR).o: $(MEDIATOR).c 
	$(CC) $(FLAGS) -c $^ -o $@

$(PLAYER).out: $(PLAYER).o $(MAP).o $(GAME).o $(UNITS).o $(STRATEGY).o
	$(CC) $(FLAGS) $^ -o $@

$(PLAYER).o: $(PLAYER).c
	$(CC) $(FLAGS) -c $^ -o $@

$(MAP).o: $(MAP).c
	$(CC) $(FLAGS) -c $^ -o $@

$(GAME).o: $(GAME).c
	$(CC) $(FLAGS) -c $^ -o $@

$(UNITS).o: $(UNITS).c
	$(CC) $(FLAGS) -c $^ -o $@

$(STRATEGY).o: $(STRATEGY).c
	$(CC) $(FLAGS) -c $^ -o $@

# Run game(mediator)
play: $(MEDIATOR).out $(PLAYER).out
	./$(MEDIATOR).out $(MAP_FILENAME) $(STATUS_FILENAME) $(ORDERS_FILENAME) $(TIME_LIMIT)

# Run player program
player: $(PLAYER).out
	./$(PLAYER).out $(MAP_FILENAME) $(STATUS_FILENAME) $(ORDERS_FILENAME) $(TIME_LIMIT)

# Run game TEST_NUMBER times
test: $(MEDIATOR).out $(PLAYER).out
	@for i in $$(seq 1 $(TEST_NUMBER)); do \
        ./$(MEDIATOR).out $(MAP_FILENAME) $(STATUS_FILENAME) $(ORDERS_FILENAME) $(TIME_LIMIT); \
    done

# Run game with valgrind
memcheck: $(MEDIATOR).out
	valgrind --leak-check=yes --track-origins=yes \
	./$(MEDIATOR).out $(MAP_FILENAME) $(STATUS_FILENAME) $(ORDERS_FILENAME) $(TIME_LIMIT)

# Delete files generated in process
clean:
	rm -f *.o  *.out $(STATUS_FILENAME) $(ORDERS_FILENAME)
