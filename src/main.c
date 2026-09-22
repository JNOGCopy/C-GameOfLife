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
#include <sys/types.h>
#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include <game_of_life/game_of_life.h>

#define CELL_SIZE 8

static SDL_Window *appWindow;
static SDL_Renderer *appRenderer;

GameOfLife gameOfLife;

int xSelectedPos = 0;
int ySelectedPos = 0;

double lastTime, currentTime;
uint8_t currentGeneration;

bool isAutomatic = false;

void update(double deltaTime);

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  SDL_CreateWindowAndRenderer("GameOfLifea", 800, 800, SDL_WINDOW_RESIZABLE,
                              &appWindow, &appRenderer);
  createGameOfLifeInstace(&gameOfLife);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_KEY_DOWN:
    switch (event->key.key) {
    case SDLK_SPACE:
      simulateGameOfLife(&gameOfLife);
      break;
    case SDLK_A:
      isAutomatic ^= 1;
      break;
    case SDLK_R:
      clearCellsGrid(&gameOfLife);
      break;
    }
    break;
  case SDL_EVENT_MOUSE_MOTION:
    int windowSizeX = 0;
    int windowSizeY = 0;

    SDL_GetWindowSize(appWindow, &windowSizeX, &windowSizeY);
    xSelectedPos = event->motion.x / ((float)windowSizeX / GRID_X_LIMIT);
    ySelectedPos = event->motion.y / ((float)windowSizeY / GRID_Y_LIMIT);
    break;

  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    setCellState(&gameOfLife, xSelectedPos, ySelectedPos,
                 !getCellState(&gameOfLife, xSelectedPos, ySelectedPos));
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

void drawCell(const GameOfLife *gameOfLife, int x, int y) {
  makeFixedPosition(&x, &y);

  if (!getCellState(gameOfLife, x, y)) {
    return;
  }

  SDL_SetRenderDrawColor(appRenderer, 0, 255, 0, 255);
  SDL_FRect rect;

  int windowSizeX, windowSizeY;

  SDL_GetWindowSize(appWindow, &windowSizeX, &windowSizeY);

  float sizeX = windowSizeX / (float)(GRID_X_LIMIT);
  float sizeY = windowSizeY / (float)(GRID_Y_LIMIT);
  rect.x = (x * sizeX);
  rect.y = (y * sizeY);

  rect.h = sizeY;
  rect.w = sizeX;

  SDL_RenderFillRect(appRenderer, &rect);
}

void drawCellGrid(const GameOfLife *gameOfLife) {
  for (int y = 0; y < GRID_Y_LIMIT; y++) {
    for (int x = 0; x < GRID_X_LIMIT; x++) {
      drawCell(gameOfLife, x, y);
    }
  }
}

double timer = 0.0;
void update(double deltaTime) {
  if (isAutomatic) {
    timer += deltaTime;
  }

  SDL_SetRenderDrawColor(appRenderer, 100, 100, 100, 255);
  SDL_FRect rect;

  int windowSizeX;
  int windowSizeY;

  SDL_GetWindowSize(appWindow, &windowSizeX, &windowSizeY);

  float sizeX = windowSizeX / (float)(GRID_X_LIMIT);
  float sizeY = windowSizeY / (float)(GRID_Y_LIMIT);

  rect.x = (xSelectedPos)*sizeX;
  rect.y = (ySelectedPos)*sizeY;

  rect.h = sizeY;
  rect.w = sizeX;

  while (isAutomatic && timer > 0.015f) {
    simulateGameOfLife(&gameOfLife);
    timer -= 0.015f;
  }
  drawCellGrid(&gameOfLife);

  SDL_RenderFillRect(appRenderer, &rect);
}
