#include <gb/gb.h>
#include <gb/metasprites.h>
#include <gb/cgb.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "graphics/GameplayBackground.h"
#include "graphics/Obstacles.h"
#include "graphics/Frog.h"
#include "graphics/Font.h"
#include "graphics/FrogCompleted.h"

#define FONT_TILES_START GameplayBackground_TILE_COUNT+Obstacles_TILE_COUNT+FrogCompleted_TILE_COUNT

#define YELLOW_BUS 0
#define RED_CAR 1
#define RED_TRUCK 2
#define BLUE_CAR 3
#define PURPLE_CAR 4
#define GREEN_CAR 5
#define WATER_LOG 6
#define WATER_GATOR 7
#define WATER_FROGS 8

#define WALKABLE 0
#define MOVING_WALKABLE 3
#define OBSTACLE 1
#define SOLID 2

#define NOFLIP 0
#define FLIP_VERTICAL 1
#define FLIP_HORIZONTAL 2

int16_t rowSlides[18];
int16_t rowSpeeds[18]={

    // End
   0,
    0,
    0,

    // Water
   8,
    -8,
    8,
    -8,
    -8,

    0,

    // Street
    8,
    -8,
    8,
    -8,
    -8,

    // UI & Start
    0,
    0,
   0,
    0
};

/*

#define	J_UP         0x04U
#define	J_DOWN       0x08U
#define	J_LEFT       0x02U
#define	J_RIGHT      0x01U
*/
const int16_t directions[9][2]={{0,0},{1,0},{-1,0},{0,0},{0,-1},{0,0},{0,0},{0,0},{0,1}};
const uint8_t frogMetaspriteInfo[9][2]={{0,0},{3,NOFLIP},{3,FLIP_HORIZONTAL},{0,0},{0,NOFLIP},{0,0},{0,0},{0,0},{0,FLIP_VERTICAL}};

uint8_t row=0;
uint16_t playerDeathAnimation=0;
int16_t frogX=0;
int16_t frogY=0;
uint8_t frogDirection=0;
uint8_t frogJump=0;
uint16_t frogJumpX=0;
uint16_t frogJumpY=0;
uint8_t lives=5;
uint32_t score=0;
uint8_t hit = FALSE;

uint8_t completionSpots[5]={FALSE,FALSE,FALSE,FALSE,FALSE};


typedef struct Obstacle{
    uint8_t palette;
    uint8_t tileStart;
    uint8_t length;
    uint8_t repeat;

} Obstacle;

const Obstacle allObstacles[]={
    // Street
    {.palette=3,.tileStart=1,.length=3,.repeat=FALSE},
    {.palette=7,.tileStart=4,.length=2,.repeat=FALSE},
    {.palette=7,.tileStart=6,.length=3,.repeat=FALSE},
    {.palette=1,.tileStart=9,.length=2,.repeat=FALSE},
    {.palette=4,.tileStart=11,.length=2,.repeat=FALSE},
    {.palette=5,.tileStart=13,.length=2,.repeat=FALSE},
    // Water
    {.palette=2,.tileStart=15,.length=4,.repeat=FALSE},
    {.palette=6,.tileStart=20,.length=5,.repeat=FALSE},
    {.palette=6,.tileStart=25,.length=3,.repeat=TRUE},
};

void ScrollBackgroundRows(){

    

    switch(LYC_REG){
        case 0:move_bkg(0,0);LYC_REG=23;break;
        case 23:move_bkg(-(rowSlides[3]>>4),0);LYC_REG+=8;break;
        case 31:move_bkg(-(rowSlides[4]>>4),0);LYC_REG+=8;break;
        case 39:move_bkg(-(rowSlides[5]>>4),0);LYC_REG+=8;break;
        case 47:move_bkg(-(rowSlides[6]>>4),0);LYC_REG+=8;break;
        case 55:move_bkg(-(rowSlides[7]>>4),0);LYC_REG+=8;break;
        case 63:move_bkg(0,0);LYC_REG+=8;break;
        case 71:move_bkg(-(rowSlides[9]>>4),0);LYC_REG+=8;break;
        case 79:move_bkg(-(rowSlides[10]>>4),0);LYC_REG+=8;break;
        case 87:move_bkg(-(rowSlides[11]>>4),0);LYC_REG+=8;break;
        case 95:move_bkg(-(rowSlides[12]>>4),0);LYC_REG+=8;break;
        case 103:move_bkg(-(rowSlides[13]>>4),0);LYC_REG+=8;break;
        default:move_bkg(0,0);LYC_REG=0;break;

    }


}

uint8_t GetPositionTile(int16_t x, int16_t y){

    int16_t crow= y/8;

    int16_t rowSlide = rowSlides[crow];

    int16_t x2=x-(rowSlide>>4)-4;
    

    int16_t column =x2/8;
    if(column<0)column+=32;
    else if (column>=32)column-=32;

    uint8_t tile= get_bkg_tile_xy(column,crow);


    uint8_t tileState = WALKABLE;

    if(tile>=1&&tile<=0x0E)tileState=OBSTACLE;
    else if(tile==0x1E||tile==0x21||tile==0x22||tile==0x24)tileState=SOLID;

    if(tile>=0x0f&&tile<=0x19)tileState=MOVING_WALKABLE;


    return tileState;
}

void DrawOnRow(uint8_t row, uint8_t startOffset, uint8_t spacing, Obstacle *obstacle){

    uint8_t length = obstacle->length;
    uint8_t start = obstacle->tileStart;
    uint8_t palette = obstacle->palette;

    // (spacing * (n-1))+(length*n)<=31
    // (spacing+length)*n-spacing<=31
    // (spacing + length)*n<=31+spacing
    // n <= (31+spacing)/(spacing+length)

    uint8_t count=(31+spacing)/(spacing+length);

    rowSlides[row]=startOffset;


    for(uint8_t i=0;i<count;i++){
       
       for(uint8_t j=0;j<length;j++){

           uint8_t j2= obstacle->repeat ? 0 : j;

            VBK_REG=1; set_bkg_tile_xy(i*(length+spacing)+j,row,palette);
            VBK_REG=0; set_bkg_tile_xy(i*(length+spacing)+j,row,start+j2);

       }
    }

}

void ResetFrog(){
    
    frogX=80<<4;
    frogY=124<<4;

    frogJumpX=frogX;
    frogJumpY=frogY;

    frogDirection=J_UP;
    frogJump=128;
}



void UpdateLives(){

    for(uint8_t i=1;i<=5;i++){
        if(i<=lives)set_bkg_tile_xy(i-1,17,FONT_TILES_START+39);
        else set_bkg_tile_xy(i-1,17,FONT_TILES_START+40);
    }
}

void UpdateScore(){

    DrawNumberWithPalette(0,16,score,5,0,FONT_TILES_START);
}

uint8_t FinishInSpot(uint8_t spot){

    ResetFrog();

    if(completionSpots[spot]==TRUE)return;

    completionSpots[spot]=TRUE;

    score+=200;

    uint8_t anyEmpty=FALSE;

    for(uint8_t i=0;i<5;i++){

        if(completionSpots[i]==FALSE)anyEmpty=TRUE;

        VBK_REG=1;set_bkg_tiles(1+4*i,1,2,2,FrogCompleted_map_attributes);
        VBK_REG=0;set_bkg_tiles(1+4*i,1,2,2,FrogCompleted_map);
    }


    return !anyEmpty;
}

void EndGameplay(){
    
    disable_interrupts();
    remove_LCD(ScrollBackgroundRows);
    set_interrupts(VBL_IFLAG);
    enable_interrupts();
}

void SetupGameplay(void){

    uint8_t i;

    for(i=0;i<18;i++){
        rowSlides[i]=0;

    }

    STAT_REG|=0x40;//enable LYC=LY interrupt
    LYC_REG=0;

    disable_interrupts();
    add_LCD(ScrollBackgroundRows);
    set_interrupts(LCD_IFLAG|VBL_IFLAG);
    enable_interrupts();

    set_bkg_data(0,Obstacles_TILE_COUNT,Obstacles_tiles);
    set_bkg_data(Obstacles_TILE_COUNT,GameplayBackground_TILE_COUNT,GameplayBackground_tiles);
    set_bkg_data(GameplayBackground_TILE_COUNT+Obstacles_TILE_COUNT,FrogCompleted_TILE_COUNT,FrogCompleted_tiles);
    set_bkg_data(FONT_TILES_START,Font_TILE_COUNT,Font_tiles);

    set_sprite_data(0,Frog_TILE_COUNT,Frog_tiles);

    set_bkg_palette(0,Obstacles_PALETTE_COUNT,Obstacles_palettes);
    set_sprite_palette(0,Frog_PALETTE_COUNT,Frog_palettes);


    // Fill all the background with street
    VBK_REG=1;fill_bkg_rect(0,0,32,32,1);
    VBK_REG=0;fill_bkg_rect(0,0,32,32,0);

    // Fill all the background with water
    VBK_REG=1;fill_bkg_rect(0,3,32,5,2);
    VBK_REG=0;fill_bkg_rect(0,3,32,5,0);

    // Draw the gameplay map
    VBK_REG=1;set_bkg_tiles(0,0,20,18,GameplayBackground_map_attributes);
    VBK_REG=0;set_bkg_based_tiles(0,0,20,18,GameplayBackground_map,Obstacles_TILE_COUNT);


    // Water

    DrawOnRow(3,0,3,&allObstacles[WATER_LOG]);
    DrawOnRow(4,0,3,&allObstacles[WATER_LOG]);
    DrawOnRow(5,0,3,&allObstacles[WATER_LOG]);
    DrawOnRow(6,0,3,&allObstacles[WATER_GATOR]);
    DrawOnRow(7,0,3,&allObstacles[WATER_FROGS]);


    // Street
    DrawOnRow(9,0,4,&allObstacles[YELLOW_BUS]);
    DrawOnRow(10,0,3,&allObstacles[GREEN_CAR]);
    DrawOnRow(11,0,4,&allObstacles[RED_TRUCK]);
    DrawOnRow(12,0,4,&allObstacles[BLUE_CAR]);
    DrawOnRow(13,0,4,&allObstacles[PURPLE_CAR]);

    // Reset our score and lives
    score=0;
    lives=5;

    // Reset our completion spots
    completionSpots[0]=FALSE;
    completionSpots[1]=FALSE;
    completionSpots[2]=FALSE;
    completionSpots[3]=FALSE;
    completionSpots[4]=FALSE;

    ResetFrog();
    UpdateScore();
    UpdateLives();

}

uint8_t UpdateGameplay(){

    uint8_t i=0;
    
    uint8_t frogFrame = frogMetaspriteInfo[frogDirection][0];
    uint8_t frogFlip = frogMetaspriteInfo[frogDirection][1];


    if(hit){

        frogFrame = ((playerDeathAnimation>>4)>=9?8:(playerDeathAnimation>>4))+6;

        playerDeathAnimation+=5;

        if((playerDeathAnimation>>4)>=10){
            hit=FALSE;
            if(lives>0)lives--;
            else return GAMEOVER_STATE;
            UpdateLives();
            ResetFrog();
        }



    }else{

        playerDeathAnimation=0;
    

        if(frogJump>=128){

            uint8_t nextDirection = 0;

            if((joypadCurrent & J_LEFT) && !(joypadPrevious  & J_LEFT)){
                nextDirection=J_LEFT;
            }else if((joypadCurrent & J_RIGHT) && !(joypadPrevious  & J_RIGHT)){
                nextDirection=J_RIGHT;
            }else if((joypadCurrent & J_DOWN) && !(joypadPrevious  & J_DOWN)){
                nextDirection=J_DOWN;
            }else if((joypadCurrent & J_UP) && !(joypadPrevious  & J_UP)){
                nextDirection=J_UP;
            }

            if(nextDirection!=0){

                frogJump=0;
                frogJumpX=frogX;
                frogJumpY=frogY;
                frogDirection=nextDirection;

                if(nextDirection==J_UP){
                    score+=10;
                    UpdateScore();
                }

            }else if((frogY>>4)/8<3){

                if(FinishInSpot(((frogX>>4)/8)/3)){

                    return GETREADY_STATE;
                }

            } else{

                uint8_t restingTile = GetPositionTile((frogX>>4)+3,(frogY>>4)+3);
                
                if(restingTile==MOVING_WALKABLE)frogX+=rowSpeeds[(frogY>>4)/8];
            }
        }else{

            uint8_t factor = 2;

            const uint8_t spd = 55;
            frogJump+=spd;
            if(frogJump>128)frogJump=128;

            uint8_t f = (128*frogJump)/128;

            if(frogJump>=128)f=128;

            frogX=frogJumpX+directions[frogDirection][0]*f;
            frogY=frogJumpY+directions[frogDirection][1]*f;


            frogFrame+=(frogJump/43);

        }


        // Check the frog's four corners
        if(GetPositionTile((frogX>>4)+3,(frogY>>4)+3)==OBSTACLE)hit=TRUE;
        else if(GetPositionTile((frogX>>4)-3,(frogY>>4)+3)==OBSTACLE)hit=TRUE;
        else if(GetPositionTile((frogX>>4)-3,(frogY>>4)-3)==OBSTACLE)hit=TRUE;
        else if(GetPositionTile((frogX>>4)+3,(frogY>>4)-3)==OBSTACLE)hit=TRUE;

    }

    if(frogFlip==NOFLIP)move_metasprite(Frog_metasprites[frogFrame],0,0,(frogX>>4),(frogY>>4)+8);
    else if(frogFlip==FLIP_HORIZONTAL)move_metasprite_vflip(Frog_metasprites[frogFrame],0,0,(frogX>>4)+16,(frogY>>4)+8);
    else if(frogFlip==FLIP_VERTICAL)move_metasprite_hflip(Frog_metasprites[frogFrame],0,0,(frogX>>4),(frogY>>4)+24);


    // For each row
    for(i=0;i<18;i++){

        // Slide the row based on it's speed
        rowSlides[i]+=rowSpeeds[i];

        // Make sure we keep the non-shifted value between 0 and 255 inclusive
        if(rowSlides[i]>(255<<4)){
            rowSlides[i]-=(255<<4);
        }else if(rowSlides[i]<0){
            rowSlides[i]+=(255<<4);
        }

    }

    return GAMEPLAY_STATE;

}