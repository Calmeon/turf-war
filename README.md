# What is Turf war ?

Turf war is text turn based strategy game with multiple rules. Game is self-playing. 
Main program serves as the mediator between CPU players. Communication is limited to text files.
    
Part where program can be developed further are strategies used by CPUs.

# Using Turf war

Directory containing full source of an application is src.

In src directory we can use mediator program directly using:

    ./mediator.out map.txt status.txt orders.txt [time_limit]

Where:
- map.txt - file with map which is the only input required (example map is provided in src)
- status.txt - filename of file where status communication would be done
- orders.txt - filename of file where orders communication would be done
- time_limit - time limit superimposed on CPU players

## Makefile

Simpler way to use program is to use prepared makefile commands.

To test one time game:

    make play

To run multiple instances of game:

    make test

To clean up build files and unnecessary files:

    make clean

Every variable used in Makefile are on top of the Makefile.

# Strategies

Here are described approaches that CPU take in decision-making.

## Build

- Program considering how much gold it has and depending on that, creates list with affordable units.
- Then program draw one unit from that list and builds it.
- Exception is if player is in bad economic position, and can repair it by mining gold. Then workers are prioritized.

## Attack

- Units check if they have enemy in range and attack it.
- If in range of unit is enemy base it is being prioritized.

## Move

- Units move in general direction of their target.
    - Workers target mines.
    - Other units target enemy base.
- If there are units in range of player unit stay in this place until enemy unit is defeated.
- Considering that unit can make couple moves in turn, if unit have opportunity to move and attack it will do this.
