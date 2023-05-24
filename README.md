# Turf war
Turn based strategy game for 2 players

## Repository content

src -> Directory containing full source of an apllication

## What has been done

- Project setup
- Map loading
- Setting up players data
- Base of mediator
- Basic status preparation
- Unit constructors and adding system
- Player status loading into memory
- Orders file communication
- Handle orders
    - Build
    - Move
    - Attack
- Handle game ending
    - By exceeding time limit
    - By base destruction
    - By exceeding turn limit
- Processing of turn idle actions
    - Building units
    - Clearing destroyed units
    - Reseting units
    - Adding gold (workers in mines)
- Full moves validation (bad move results in win for another player)
- Strategies:
    - Build
    - Attack
    - Move (Target base or mines)
