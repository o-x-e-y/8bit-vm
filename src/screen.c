#include "headers/screen.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_thread.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;

static const uint32_t colors[16] = {
    0x282828FF,  // GRUVBOX_BLACK
    0x665C51FF,  // GRUVBOX_DARK_GREY
    0x928374FF,  // GRUVBOX_GREY
    // 0xA89984FF,  // GRUVBOX_LIGHT_GREY
    0xF2E5BCFF,  // GRUVBOX_WHITE
    0xCC241DFF,  // GRUVBOX_DARK_RED
    0xFB4934FF,  // GRUVBOX_RED
    0xD65D0EFF,  // GRUVBOX_DARK_ORANGE
    0xFE8019FF,  // GRUVBOX_ORANGE
    0xFABD2FFF,  // GRUVBOX_YELLOW
    0xD79921FF,  // GRUVBOX_DARK_YELLOW
    0x98971AFF,  // GRUVBOX_DARK_GREEN (olive green)
    0xB8BB26FF,  // GRUVBOX_GREEN
    0x8EC07CFF,  // GRUVBOX_CYAN
    0x83A598FF,  // GRUVBOX_BLUE
    0x458588FF,  // GRUVBOX_DARK_BLUE
    0xD3869BFF,  // GRUVBOX_MAGENTA
};

static SDL_AppResult initialize_sdl() {
    SDL_SetAppMetadata("8-bit Custom VM", "0.0.1", "8-bit-vm");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to init SDL, error: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("8-bit Custom VM", SCREEN_WIDTH * PIXEL_SCALE,
                                     SCREEN_HEIGHT * PIXEL_SCALE, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderVSync(renderer, 1);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                SCREEN_WIDTH, SCREEN_HEIGHT);
    if (texture == NULL) {
        SDL_Log("Texture could not be created, error: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderScale(renderer, PIXEL_SCALE, PIXEL_SCALE);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    return SDL_APP_CONTINUE;
}

static void cleanup_sdl() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

static void update_and_render(screen_buffer frame_buffer) {
    uint32_t rgba_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

    for (size_t i = 0; i < frame_buffer.len; ++i) {
        rgba_buffer[i * 2] = colors[frame_buffer.buffer[i] >> 4];
        rgba_buffer[i * 2 + 1] = colors[frame_buffer.buffer[i] & 0x0F];
    }

    SDL_UpdateTexture(texture, NULL, rgba_buffer, SCREEN_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);

    SDL_RenderPresent(renderer);
}

static int render_thread(void* cpuData) {
    CPU* cpu = (CPU*)cpuData;

    screen_buffer buffer =
        (screen_buffer){.buffer = &cpu->memory[MEMORY_SIZE - 1 - SCREEN_HEIGHT * SCREEN_WIDTH / 2],
                        .len = SCREEN_HEIGHT * SCREEN_WIDTH / 2};

    SDL_AppResult init = initialize_sdl();
    if (init != SDL_APP_CONTINUE) {
        return init;
    }

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        update_and_render(buffer);

        // SDL_Delay(16);  // ~60 fps
    }

    cleanup_sdl();

    return SDL_APP_SUCCESS;
}

void initScreen(CPU* cpu) {
    SDL_Thread* screen_thread_handle = SDL_CreateThread(render_thread, "SDL VM Screen Thread", cpu);

    SDL_DetachThread(screen_thread_handle);
}
