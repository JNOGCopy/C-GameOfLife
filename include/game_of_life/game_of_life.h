#pragma once

#include <stdbool.h>

typedef struct GameOfLife {
  bool cellsGrid[GRID_MAX_X][GRID_MAX_Y];
} GameOfLife;

void createGameOfLifeInstace(GameOfLife *gameOfLife) {}
