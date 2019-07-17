#include <stdlib.h>
#include <math.h>

#define MEM_IO 0x04000000
#define REG_DISPCNT *((volatile u32*)(MEM_IO))

#define DCNT_BG2 0x0400
#define DCNT_MODE3 0x0003

#define MEM_VRAM 0x06000000
#define vid_mem ((COLOR*)MEM_VRAM)

#define WIDTH 240
#define HEIGHT 160
#define PLAYER_SIZE 9

#define REG_VCOUNT *(volatile u16*)0x04000006

#define CLR_RED 0x001F
#define CLR_BLUE 0x7C00
#define CLR_WHITE 0x7FFF

#define REG_KEYINPUT *(volatile u16*)0x04000130
#define KEY_DOWN_NOW(key) (~(REG_KEYINPUT) & key)

#define KEY_A 0x0001
#define KEY_RIGHT 0x0010
#define KEY_LEFT 0x0020
#define KEY_MASK 0x03FF

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int uint;
typedef u16 COLOR;

u16 __key_curr = 0;
u16 __key_prev = 0;

struct Bar {
    u32 x;
    u32 y;
    u32 width;
    u32 height;
};

struct Bar bars[3] = {
        {20,  120, 50, 5},
        {100, 90,  10, 10},
        {140, 55,  10, 10}
};

COLOR RGB15(u32 red, u32 green, u32 blue) {
    return red | (green << 5) | (blue << 10);
}

u32 key_held(u32 key) {
    return (__key_curr & __key_prev) & key;
}

void key_poll() {
    __key_prev = __key_curr;
    __key_curr = ~REG_KEYINPUT & KEY_MASK;
}

u32 key_hit(u32 key) {
    return (__key_curr & ~__key_prev) & key;
}

u32 key_released(u32 key) {
    return (~__key_curr & __key_prev) & key;
}

void v_sync() {
    while (REG_VCOUNT >= HEIGHT);
    while (REG_VCOUNT < HEIGHT);
}

void check_movement(u32 *x, u32 *y, int *velocityX, int *velocityY) {
    if (key_held(KEY_RIGHT))
        *velocityX = *x + PLAYER_SIZE < WIDTH - 1
                    ? 2
                    : *x + PLAYER_SIZE < WIDTH
                      ? 1
                      : 0;
    else if (key_held(KEY_LEFT))
        *velocityX = *x > 1
                    ? -2
                    : *x > 0
                      ? -1
                      : 0;
    else
        *velocityX = 0;

    if (key_hit(KEY_A))
        *velocityY = -10;
}

void move_player(u32 *x, u32 *y, int *velocityX, int *velocityY) {
    (*x) += *velocityX;
    (*y) += *velocityY;
}

void gravity(u32 *x, u32 *y, int *velocityX, int *velocityY) {
    if (*y >= HEIGHT - PLAYER_SIZE - 4 && *y < HEIGHT * 5) {
        *velocityY = 0;
        *y = HEIGHT - PLAYER_SIZE - 4;
    } else if (*y >= HEIGHT - PLAYER_SIZE && *y > HEIGHT * 5) {
        *velocityY *= -1;
        *y = 0;
    } else {
        (*velocityY)++;
    }

    for (int i = 0; i < sizeof(bars) / sizeof(bars[0]); i++) {
        if (*y + PLAYER_SIZE >= bars[i].y &&
            *y <= bars[i].y + bars[i].height &&
            *x + PLAYER_SIZE >= bars[i].x &&
            *x <= bars[i].x + bars[i].width) {
            if (*velocityY > 0) {
                *velocityY = 0;
                *y = bars[i].y - PLAYER_SIZE;
            } else if (*velocityY < 0) {
                *velocityY *= -1;
                *y = bars[i].y + bars[i].height;
            }
        }
    }
}

void draw_ground() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++)
            vid_mem[y * WIDTH + x] = CLR_WHITE;
    }

    for (int i = 0; i < WIDTH; i++) {
        vid_mem[(HEIGHT - 4) * WIDTH + i] = CLR_BLUE;
        vid_mem[(HEIGHT - 3) * WIDTH + i] = CLR_BLUE;
        vid_mem[(HEIGHT - 2) * WIDTH + i] = CLR_BLUE;
        vid_mem[(HEIGHT - 1) * WIDTH + i] = CLR_BLUE;
    }
}

void draw_bars() {
    for (int i = 0; i < sizeof(bars) / sizeof(bars[0]); i++) {
        for (int y = 0; y < bars[i].height; y++) {
            for (int x = 0; x < bars[i].width; x++)
                vid_mem[(bars[i].y + y) * WIDTH + bars[i].x + x] = CLR_BLUE;
        }
    }
}

void draw_player(u32 x, u32 y, COLOR c) {
    for (int i = 0; i < PLAYER_SIZE; i++) {
        for (int j = 0; j < PLAYER_SIZE; j++)
            vid_mem[y * WIDTH + x + i + (WIDTH * j)] = c;
    }
}

int main() {
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

    u32 x = 40;
    u32 y = 40;

    int velocityY = 0;
    int velocityX = 0;

    draw_ground();
    draw_bars();

    while (1) {
        v_sync();
        draw_player(x, y, CLR_WHITE);
        key_poll();
        check_movement(&x, &y, &velocityX, &velocityY);
        move_player(&x, &y, &velocityX, &velocityY);
        gravity(&x, &y, &velocityX, &velocityY);
        draw_player(x, y, CLR_RED);
    }

    return 0;
}
