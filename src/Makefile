CC = gcc
FLAGS = -g -Wall
MEDIATOR = mediator
PLAYER = player
MAP = map.txt
STATUS = status.txt
COMMANDS = commands.txt
TIME_LIMIT = 60

all: $(MEDIATOR).out $(PLAYER).out

$(MEDIATOR).out: $(MEDIATOR).o
	$(CC) $(FLAGS) $^ -o $@

$(MEDIATOR).o: $(MEDIATOR).c
	$(CC) $(FLAGS) -c $^ -o $@

$(PLAYER).out: $(PLAYER).o
	$(CC) $(FLAGS) $^ -o $@

$(PLAYER).o: $(PLAYER).c
	$(CC) $(FLAGS) -c $^ -o $@


test:
	./$(MEDIATOR).out $(MAP) $(STATUS) $(COMMANDS) $(TIME_LIMIT)

clean:
	rm -f $(MEDIATOR).o  $(MEDIATOR).out $(PLAYER).o $(PLAYER).out
