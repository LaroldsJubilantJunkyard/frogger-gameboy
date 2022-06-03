#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "graphics/GameOverBackground.h"
#include "graphics/Font.h"


void SetupGameOver(){

    set_bkg_data(0,GameOverBackground_TILE_COUNT,GameOverBackground_tiles);
    set_bkg_data(GameOverBackground_TILE_COUNT,Font_TILE_COUNT,Font_tiles);
    set_bkg_palette(0,GameOverBackground_PALETTE_COUNT,GameOverBackground_palettes);
    set_bkg_palette(GameOverBackground_PALETTE_COUNT,1,Font_palettes);
    set_bkg_palette_entry(0,0,RGB8(255, 255, 255));
    set_bkg_palette_entry(1,0,RGB8(255, 255, 255));



    VBK_REG=1;set_bkg_tiles(0,0,20,18,GameOverBackground_map_attributes);
    VBK_REG=0;set_bkg_tiles(0,0,20,18,GameOverBackground_map);
}

uint8_t UpdateGameOver(){

    level=0;
    
    if((joypadCurrent & J_A) && !(joypadPrevious & J_A))return GETREADY_STATE;
    if((joypadCurrent & J_START) && !(joypadPrevious & J_START))return GETREADY_STATE;
    
    return GAMEOVER_STATE;
}