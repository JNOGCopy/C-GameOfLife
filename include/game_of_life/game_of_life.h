#pragma once

#include <stdint.h>
#define GRID_X_LIMIT 480
#define GRID_Y_LIMIT 270

#include <stdbool.h>

typedef struct GameOfLife {
  bool cellsGrid[GRID_X_LIMIT][GRID_Y_LIMIT];
} GameOfLife;

void makeFixedPosition(int *x, int *y);
void createGameOfLifeInstace(GameOfLife *gameOfLife);
uint8_t getAmountOfNeighboors(const GameOfLife *gameOfLife, int x, int y);

void clearCellsGrid(GameOfLife *gameOfLife);

bool getCellState(const GameOfLife *gameOfLife, int x, int y);
void setCellState(GameOfLife *gameOfLife, int x, int y, bool state);

void simulateGameOfLife(GameOfLife *gameOfLife);
