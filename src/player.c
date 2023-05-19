// Player program representing game player

#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "map.h"

int main(int argc, char* argv[]) {
    int time_limit;
    char *map_filename, *status_filename, *commands_filename;
    Map board;

    // Get passed arguments
    if (argc < 4) {
        printf("Usage: ./<program_name> map.txt status.txt commands.txt [time_limit]\n");
        exit(EXIT_FAILURE);
    }
    map_filename = argv[1];
    status_filename = argv[2];
    commands_filename = argv[3];
    time_limit = (argc == 5) ? atoi(argv[4]) : 5;

    printf("Map file: %s   Status file: %s   Commands file: %s   Time limit: %d\n\n",
           map_filename, status_filename, commands_filename, time_limit);

    load_map(&board, map_filename);
    print_map(&board);

    free_map(&board);
    return 0;
}
