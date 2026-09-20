#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#define GRID_X_LIMIT 200
#define GRID_Y_LIMIT 200

#define CELL_SIZE 8

static SDL_Window *appWindow;
static SDL_Renderer *appRenderer;

bool cellGrid[GRID_X_LIMIT][GRID_Y_LIMIT] = {0};
bool last_cellGrid[GRID_X_LIMIT][GRID_Y_LIMIT] = {0};

typedef struct position {
  int x;
  int y;
} Position;

double lastTime, currentTime;
Position currentSelectedPosition = {0};

uint8_t currentGeneration;

void drawCell(const Position *position);
uint8_t getAmountOfNeighboors(const Position *position,
                              bool grid[GRID_X_LIMIT][GRID_Y_LIMIT]);
void handleSimulator();
void drawCellGrid();
void update(double deltaTime);

void setCellState(const Position *position, bool state);
bool getCellState(const Position *position);
Position createPosition(int x, int y);

bool automatic = false;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  SDL_CreateWindowAndRenderer("GameOfLifea", 800, 800, SDL_WINDOW_RESIZABLE,
                              &appWindow, &appRenderer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.key == SDLK_SPACE) {
      handleSimulator();
      printf("Presses Space");
    }
    if (event->key.key == SDLK_A) {
      automatic = !automatic;
    }
    if (event->key.key == SDLK_R) {
      bool clearedGrid[GRID_X_LIMIT][GRID_Y_LIMIT] = {0};
      memcpy(cellGrid, clearedGrid, sizeof(cellGrid));
    }
    if (event->key.key == SDLK_S) {
    }
  }
  if (event->type == SDL_EVENT_MOUSE_MOTION) {
    int windowSizeX;
    int windowSizeY;

    SDL_GetWindowSize(appWindow, &windowSizeX, &windowSizeY);

    float sizeX = windowSizeX / (float)(GRID_X_LIMIT);
    float sizeY = windowSizeY / (float)(GRID_Y_LIMIT);

    float x = event->motion.x / sizeX;
    float y = event->motion.y / sizeY;

    printf("X: %f\n Y: %f\n", x, y);

    currentSelectedPosition = createPosition(x, y);
  }
  if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    setCellState(&currentSelectedPosition,
                 !getCellState(&currentSelectedPosition));
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  SDL_SetRenderDrawColor(appRenderer, 0, 0, 0, 255);
  SDL_RenderClear(appRenderer);

  lastTime = currentTime;
  currentTime = SDL_GetTicks();
  double deltaTime = (currentTime - lastTime) / 1000;

  update(deltaTime);

  SDL_RenderPresent(appRenderer);

  return SDL_APP_CONTINUE;
}
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  SDL_DestroyRenderer(appRenderer);
  SDL_DestroyWindow(appWindow);
}

Position sanitazePosition(const Position *position) {
  Position newPos;
  newPos.x = (position->x + GRID_X_LIMIT) % GRID_X_LIMIT;
  newPos.y = (position->y + GRID_Y_LIMIT) % GRID_Y_LIMIT;

  return newPos;
}

void drawCell(const Position *position) {
  SDL_SetRenderDrawColor(appRenderer, 255, 255, 255, 255);
  SDL_FRect rect;

  Position sanitazedPos = sanitazePosition(position);

  int windowSizeX;
  int windowSizeY;

  SDL_GetWindowSize(appWindow, &windowSizeX, &windowSizeY);

  float sizeX = windowSizeX / (float)(GRID_X_LIMIT);
  float sizeY = windowSizeY / (float)(GRID_Y_LIMIT);
  rect.x = (sanitazedPos.x * sizeX) - 1;
  rect.y = (sanitazedPos.y * sizeY) - 1;

  rect.h = sizeY - 1;
  rect.w = sizeX - 1;

  SDL_RenderFillRect(appRenderer, &rect);
}

uint8_t getAmountOfNeighboors(const Position *position,
                              bool grid[GRID_X_LIMIT][GRID_Y_LIMIT]) {
  Position sanitazedPos = sanitazePosition(position);
  uint8_t amount = 0;

  for (int ny = -1; ny <= 1; ny++) {
    for (int nx = -1; nx <= 1; nx++) {
      if (nx == 0 && ny == 0) {
        continue;
      }

      Position posToCheck;
      posToCheck.x = sanitazedPos.x + nx;
      posToCheck.y = sanitazedPos.y + ny;
      posToCheck = sanitazePosition(&posToCheck);

      if (grid[posToCheck.x][posToCheck.y]) {
        amount++;
      }
    }
  }
  return amount;
}

void handleSimulator() {
  printf("-- START SIMULATOR -- \n");
  bool temp_cellGrid[GRID_X_LIMIT][GRID_Y_LIMIT] = {0};
  memcpy(last_cellGrid, cellGrid, sizeof(cellGrid));
  for (int yPos = 0; yPos < GRID_X_LIMIT; yPos++) {
    for (int xPos = 0; xPos < GRID_Y_LIMIT; xPos++) {
      Position position;
      position.x = xPos;
      position.y = yPos;
      position = sanitazePosition(&position);

      uint8_t neighboors = getAmountOfNeighboors(&position, last_cellGrid);

      bool wasAlive = last_cellGrid[position.x][position.y];
      bool isActive = (neighboors == 3) | ((wasAlive) & (neighboors == 2));

      temp_cellGrid[position.x][position.y] = isActive;

      /*printf("XPos = %d\n yPos = %d\n", position.x, position.y);
      printf("Neighboors = %d\n", neighboors);
      printf("wasAlive = %d\n", wasAlive);
      printf("isActive = %d\n", isActive);
      printf("\n");*/
    }
  }
  memcpy(cellGrid, temp_cellGrid, sizeof(cellGrid));
}

void drawCellGrid() {
  for (int yPos = 0; yPos < GRID_X_LIMIT; yPos++) {
    for (int xPos = 0; xPos < GRID_Y_LIMIT; xPos++) {
      Position position;
      position.x = xPos;
      position.y = yPos;
      position = sanitazePosition(&position);

      if (cellGrid[position.x][position.y]) {
        drawCell(&position);
        // printf("o");

      } else {
        // printf("-");
      }
    }
    // printf("\n");
  }
  // printf("\n");
}

double timer = 0.0;
void update(double deltaTime) {
  timer += deltaTime;

  SDL_SetRenderDrawColor(appRenderer, 100, 100, 100, 255);
  Position sanitazedPos = sanitazePosition(&currentSelectedPosition);

  if (automatic) {
    handleSimulator();
  }

  SDL_FRect rect;

  int windowSizeX;
  int windowSizeY;

  SDL_GetWindowSize(appWindow, &windowSizeX, &windowSizeY);

  float sizeX = windowSizeX / (float)(GRID_X_LIMIT);
  float sizeY = windowSizeY / (float)(GRID_Y_LIMIT);

  rect.x = (sanitazedPos.x * sizeX);
  rect.y = (sanitazedPos.y * sizeY);

  rect.h = sizeY;
  rect.w = sizeX;

  SDL_RenderFillRect(appRenderer, &rect);

  SDL_Surface *surface;
  SDL_Color textColor = {255, 255, 255, 0};
}

void setCellState(const Position *position, bool state) {
  Position sanitazedPos = sanitazePosition(position);
  cellGrid[sanitazedPos.x][sanitazedPos.y] = state;
}
bool getCellState(const Position *position) {
  Position sanitazedPos = sanitazePosition(position);
  return cellGrid[sanitazedPos.x][sanitazedPos.y];
}

Position createPosition(int x, int y) {
  Position position;
  position.x = x;
  position.y = y;
  return position;
}
