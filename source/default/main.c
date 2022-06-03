#include <gb/gb.h>
#include "common.h"

uint8_t joypadCurrent= 0;
uint8_t joypadPrevious =0;


typedef struct Obstacle{
    uint8_t palette;
    uint8_t tileStart;
    uint8_t length;
    uint8_t repeat;

} Obstacle;


uint8_t UpdateGameplayState();
void SetupGameplay(void);
void EndGameplay();
uint8_t UpdateGetReady();
void SetupGetReady();
uint8_t UpdateGameOver();
void SetupGameOver();


void main(void){

    DISPLAY_ON;
    SHOW_BKG;
    SHOW_SPRITES;
    SPRITES_8x16;


    level=0;

    uint8_t currentState=255;
    uint8_t nextState=GETREADY_STATE;
   
    while(1){

        joypadPrevious =joypadCurrent;
        joypadCurrent= joypad();

        if(currentState!=nextState){

            switch(currentState){
                case GAMEPLAY_STATE:EndGameplay(); break;
            }
            

            currentState=nextState;

            switch(currentState){
                case GETREADY_STATE: SetupGetReady();break;
                case GAMEPLAY_STATE: SetupGameplay();break;
                case GAMEOVER_STATE: SetupGameOver(); break;
            }
            
        }

        switch(currentState){
            case GETREADY_STATE: nextState=UpdateGetReady(); break;
            case GAMEPLAY_STATE: nextState=UpdateGameplay(); break;
            case GAMEOVER_STATE: nextState=UpdateGameOver(); break;
        }


        wait_vbl_done();
    }
}