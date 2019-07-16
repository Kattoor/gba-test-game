#include <stdlib.h>
#include <math.h>

#define MEM_IO 0x04000000
#define REG_DISPCNT *((volatile u32*)(MEM_IO+0x0000))
#define DCNT_BG2 0x0400
#define DCNT_MODE3 0x0003
#define MEM_VRAM 0x06000000
#define vid_mem ((COLOR*)MEM_VRAM)
#define WIDTH 240
#define HEIGHT 160
#define M3_SIZE M3_WIDTH * M3_HEIGHT
#define REG_VCOUNT *(volatile u16*)0x04000006

#define CLR_BLACK 0x0000
#define CLR_RED 0x001F
#define CLR_LIME 0x03E0
#define CLR_YELLOW 0x03FF
#define CLR_BLUE 0x7C00
#define CLR_MAG 0x7C1F
#define CLR_CYAN 0x7FE0
#define CLR_WHITE 0x7FFF

#define REG_KEYINPUT *(volatile u16*)0x04000130
#define KEY_DOWN_NOW(key) (~(REG_KEYINPUT) & key)

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int uint;
typedef u16 COLOR;

COLOR RGB15(u32 red, u32 green, u32 blue) {
    return red | (green << 5) | (blue << 10);
}

void vsync() {
    while (REG_VCOUNT >= 160);
    while (REG_VCOUNT < 160);
}

void checkMovement(u32* x, u32* y) {
    if (KEY_DOWN_NOW(0x0010)) {
        (*x)++;
    }
    if (KEY_DOWN_NOW(0x0020)) {
        (*x)--;
    }
    if (KEY_DOWN_NOW(0x0040)) {
        (*y)--;
    }
    if (KEY_DOWN_NOW(0x0080)) {
        (*y)++;
    }
}

void drawPlayer(u32 x, u32 y, COLOR c) {
    vid_mem[y * WIDTH + x] = c;
    vid_mem[y * WIDTH + x + 1] = c;
    vid_mem[y * WIDTH + x + 2] = c;
    vid_mem[y * WIDTH + x + WIDTH] = c;
    vid_mem[y * WIDTH + x + WIDTH + 1] = c;
    vid_mem[y * WIDTH + x + WIDTH + 2] = c;
    vid_mem[y * WIDTH + x + WIDTH + WIDTH] = c;
    vid_mem[y * WIDTH + x + WIDTH + WIDTH + 1] = c;
    vid_mem[y * WIDTH + x + WIDTH + WIDTH + 2] = c;
}

int main() {
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

    u32 x = 0;
    u32 y = 0;

    while (1) {
        vsync();

        drawPlayer(x, y, CLR_BLACK);
        checkMovement(&x, &y);
        drawPlayer(x, y, CLR_RED);
    }

    return 0;
}
