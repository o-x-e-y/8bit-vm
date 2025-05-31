#include "headers/screen.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_thread.h"
#include "headers/instructions.h"

#define WRITE_IDX 0x9fed
#define READ_IDX 0x9fee
#define RINGBUF 0x9fef
#define BUF_SIZE 0x0010

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;
static uint32_t rgba_buffer[SCREEN_WIDTH * SCREEN_HEIGHT] = {0};

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
    for (size_t i = 0; i < frame_buffer.len; ++i) {
        rgba_buffer[i * 2] = colors[frame_buffer.buffer[i] >> 4];
        rgba_buffer[i * 2 + 1] = colors[frame_buffer.buffer[i] & 0x0F];
    }

    SDL_UpdateTexture(texture, NULL, rgba_buffer, SCREEN_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);

    SDL_RenderPresent(renderer);
}

static char convert_input(SDL_Event* e) {
    switch (e->key.key) {
        case SDLK_A:
            return 'a';
        case SDLK_B:
            return 'b';
        case SDLK_C:
            return 'c';
        case SDLK_D:
            return 'd';
        case SDLK_E:
            return 'e';
        case SDLK_F:
            return 'f';
        case SDLK_G:
            return 'g';
        case SDLK_H:
            return 'h';
        case SDLK_I:
            return 'i';
        case SDLK_J:
            return 'j';
        case SDLK_K:
            return 'k';
        case SDLK_L:
            return 'l';
        case SDLK_M:
            return 'm';
        case SDLK_N:
            return 'n';
        case SDLK_O:
            return 'o';
        case SDLK_P:
            return 'p';
        case SDLK_Q:
            return 'q';
        case SDLK_R:
            return 'r';
        case SDLK_S:
            return 's';
        case SDLK_T:
            return 't';
        case SDLK_U:
            return 'u';
        case SDLK_V:
            return 'v';
        case SDLK_W:
            return 'w';
        case SDLK_X:
            return 'x';
        case SDLK_Y:
            return 'y';
        case SDLK_Z:
            return 'z';
        case SDLK_1:
            return '1';
        case SDLK_2:
            return '2';
        case SDLK_3:
            return '3';
        case SDLK_4:
            return '4';
        case SDLK_5:
            return '5';
        case SDLK_6:
            return '6';
        case SDLK_7:
            return '7';
        case SDLK_8:
            return '8';
        case SDLK_9:
            return '9';
        case SDLK_0:
            return '0';
        case SDLK_SPACE:
            return ' ';
        case SDLK_PERIOD:
            return '.';
        case SDLK_COMMA:
            return ',';
        case SDLK_APOSTROPHE:
            return '\'';
        case SDLK_BACKSPACE:
            return '\b';
        case SDLK_ESCAPE:
            return 1;
        default:
            return 0;
    }
}

static void write_input(CPU* cpu, char input) {
    // work directly in the CPU's memory to create and manage the ringbuffer that
    // the keyboard will use. The CPU's interrupts will also use this ringbuffer
    // internally to process keyboard input.
    if ((MEMORY(WRITE_IDX) + 1) % BUF_SIZE == MEMORY(READ_IDX)) {
        printf("buffer full?");
        return;
    }
    MEMORY(RINGBUF + MEMORY(WRITE_IDX)) = input;
    MEMORY(WRITE_IDX) = (MEMORY(WRITE_IDX) + 1) % BUF_SIZE;
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
    SDL_Event e;
    char input;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_KEY_DOWN) {
                input = convert_input(&e);

                // input was ESC
                if (input == 1) {
                    quit = true;
                    memset(cpu->memory, OP_HALT, MEMORY_SIZE);
                } else if (input) {
                    write_input(cpu, input);
                }
            } else if (e.type == SDL_EVENT_QUIT) {
                quit = true;
                memset(cpu->memory, OP_HALT, MEMORY_SIZE);
            }
        }

        update_and_render(buffer);

        // SDL_Delay(16);  // ~60 fps
    }

    cleanup_sdl();

    return SDL_APP_SUCCESS;
}

int runCpuSdl(void* cpuData) {
    CPU* cpu = (CPU*)cpuData;

    return runCpu(cpu);
}

void initScreen(CPU* cpu) {
    SDL_Thread* screen_thread_handle = SDL_CreateThread(runCpuSdl, "SDL VM CPU Thread", cpu);

    SDL_DetachThread(screen_thread_handle);

    render_thread(cpu);
}
