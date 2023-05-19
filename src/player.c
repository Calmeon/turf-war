/*
Player program representing game player
*/

#include "map.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int time_limit;
    char *map_filename, *status_filename, *commands_filename;
    Map board;

    // Get passed arguments
    if (argc < 4)
    {
        printf("Usage: ./<program_name> map.txt status.txt commands.txt [time_limit]\n");
        exit(EXIT_FAILURE);
    }
    map_filename = argv[1];
    status_filename = argv[2];
    commands_filename = argv[3];
    time_limit = (argc == 5) ? atoi(argv[4]) : 5;

    printf("Map file: %s   Status file: %s   Commands file: %s   Time limit: %d\n\n",
           map_filename, status_filename, commands_filename, time_limit);

    loadMap(&board, map_filename);
    printMap(&board);

    freeMap(&board);
    return 0;
}
