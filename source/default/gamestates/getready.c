#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "graphics/GetReadyBackground.h"
#include "graphics/Font.h"

void SetupGetReady(){

    set_bkg_data(0,GetReadyBackground_TILE_COUNT,GetReadyBackground_tiles);
    set_bkg_data(GetReadyBackground_TILE_COUNT,Font_TILE_COUNT,Font_tiles);
    set_bkg_palette(0,GetReadyBackground_PALETTE_COUNT,GetReadyBackground_palettes);
    set_bkg_palette(GetReadyBackground_PALETTE_COUNT,1,Font_palettes);
    set_bkg_palette_entry(0,0,RGB8(255, 255, 255));


    VBK_REG=1;set_bkg_tiles(0,0,20,18,GetReadyBackground_map_attributes);
    VBK_REG=0;set_bkg_tiles(0,0,20,18,GetReadyBackground_map);

}

uint8_t UpdateGetReady(){

    if((joypadCurrent & J_A) && !(joypadPrevious & J_A))return GAMEPLAY_STATE;
    if((joypadCurrent & J_START) && !(joypadPrevious & J_START))return GAMEPLAY_STATE;
    
    return GETREADY_STATE;
    
}