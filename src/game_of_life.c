#include <game_of_life/game_of_life.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void makeFixedPosition(int *x, int *y) {
  *x = (*x + GRID_X_LIMIT) % GRID_X_LIMIT;
  *y = (*y + GRID_Y_LIMIT) % GRID_Y_LIMIT;
}

void createGameOfLifeInstace(GameOfLife *gameOfLife) {
  clearCellsGrid(gameOfLife);
}

uint8_t getAmountOfNeighboors(const GameOfLife *gameOfLife, int x, int y) {
  makeFixedPosition(&x, &y);
  uint8_t amount = 0;

  for (int ny = -1; ny <= 1; ny++) {
    for (int nx = -1; nx <= 1; nx++) {
      if (nx == 0 && ny == 0) {
        continue;
      }

      int xPosToCheck = nx + x;
      int yPosToCheck = ny + y;

      makeFixedPosition(&xPosToCheck, &yPosToCheck);
      if (gameOfLife->cellsGrid[xPosToCheck][yPosToCheck]) {
        amount++;
      }
    }
  }
  return amount;
}

void clearCellsGrid(GameOfLife *gameOfLife) {
  bool cell[GRID_X_LIMIT][GRID_Y_LIMIT] = {0};
  memcpy(gameOfLife->cellsGrid, cell,
         sizeof(bool) * GRID_X_LIMIT * GRID_Y_LIMIT);
}

bool getCellState(const GameOfLife *gameOfLife, int x, int y) {
  makeFixedPosition(&x, &y);
  return gameOfLife->cellsGrid[x][y];
}
void setCellState(GameOfLife *gameOfLife, int x, int y, bool state) {
  makeFixedPosition(&x, &y);
  gameOfLife->cellsGrid[x][y] = state;
}

void simulateGameOfLife(GameOfLife *gameOfLife) {
  bool temp_cellGrid[GRID_X_LIMIT][GRID_Y_LIMIT] = {0};

  for (int yPos = 0; yPos < GRID_X_LIMIT; yPos++) {
    for (int xPos = 0; xPos < GRID_Y_LIMIT; xPos++) {
      uint8_t neighboors = getAmountOfNeighboors(gameOfLife, xPos, yPos);

      bool wasAlive = gameOfLife->cellsGrid[xPos][yPos];
      temp_cellGrid[xPos][yPos] =
          (neighboors == 3) | ((wasAlive) & (neighboors == 2));
    }
  }
  memcpy(gameOfLife->cellsGrid, temp_cellGrid,
         sizeof(bool) * GRID_X_LIMIT * GRID_Y_LIMIT);
}
