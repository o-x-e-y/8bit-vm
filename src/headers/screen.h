#ifndef __OXEY_CCE_SCREEN_H
#define __OXEY_CCE_SCREEN_H

#include <SDL3/SDL.h>

#include "cpu.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192  // 256 * 3 / 4, 4:3
#define PIXEL_SCALE 5

typedef struct {
    uint8_t* buffer;
    size_t len;
} screen_buffer;

void initScreen(CPU* cpu);

#endif