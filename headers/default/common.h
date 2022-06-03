#ifndef COMMON_HEADER
#define COMMON_HEADER set

#define GETREADY_STATE 0
#define GAMEPLAY_STATE 1
#define GAMEOVER_STATE 2

void DrawNumberWithPalette(uint8_t x,uint8_t y, uint32_t number,uint8_t digits,uint8_t palette,uint8_t start);
void DrawTextWithPalette(uint8_t x, uint8_t y, unsigned char *text,uint8_t palette, uint8_t start );

extern uint8_t level,joypadCurrent,joypadPrevious;

#endif