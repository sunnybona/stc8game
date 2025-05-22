/* 2014
 * Breakout game by Ilya Titov. Find building instructions on http://webboggles.com/
 * The code that does not fall under the licenses of sources listed below can be used non commercially with attribution.
 *
 * If you have problems uploading this sketch, this is probably due to sketch size - you need to update ld.exe in arduino\hardware\tools\avr\avr\bin
 * https://github.com/TCWORLD/ATTinyCore/tree/master/PCREL%20Patch%20for%20GCC
 *
 * This sketch is using the screen control and font functions written by Neven Boyanov for the http://tinusaur.wordpress.com/ project
 * Source code and font files available at: https://bitbucket.org/tinusaur/ssd1306xled
 * 
 * Sleep code is based on this blog post by Matthew Little:
 * http://www.re-innovation.co.uk/web12/index.php/en/blog-75/306-sleep-modes-on-attiny85
*/

#include "font6x8.h"


#include "globals.h"
#include "ssd1306xled.h"
#include "math.h"
#define SPEED 7
// ----------------------------------------------------------------------------

void setup();
void loop();
void resetAliens(void);
void drawPlatform(void);
void sendBlock(int);
void playSpaceAttack(void);
void beep(int,int);
void levelUp(int);
void drawFire(int x, int y);
void doDrawLS(long, byte);
void doDrawRS(long, byte);
void clearAlienArea(int row, int lastActive);
void doNumber (int x, int y, int value);
void resetGame();
void collision();




void sendBlock(int);






// ----------------------------------------------------------------------------


// Routines to set and clear bits (used in the sleep code)


long firecounter = 0; // Timer for managing fire
long aliencounter = 0; // Timer for alien movement
int mothercounter = 0; // Timer for movement of the mothership
int level = 1; // Game level - incremented every time you clear the screen
int mothershipX = 0; // position of mothership
int mothership = 0; // is mothership active?
int mothershipWidth = 8; // mothership width in pixels
int fireXidx, fireYidx; // mapping of player fire locaiton onto array of aliens
int leftLimit; // furtherst left in x-axis pixels that the aliens are currently
int positionNow = 0; // current position of the alien array (as steps from the left)
boolean alienDirection = 1; // current direction of travel for alien swarm - 1 is right 0 is left
int alienRow = 0; // which alien row are we considering
int alienFire[5][3]; // max 5 lots of alien fire  - indices are active, xpos, ypos
int playerFire[3]; // one lot of player fire - indices are active, xpos, ypos
boolean row[4][10]; // on-off array of aliens
int firstAlien = 0; // index of first live alien in the array
int lastAlien = 8; // index of last live alien in the array
int newFirst = 0; // as above when it changes
int newLast = 8; // ...
int aliensDead = 0; // how many aliens have been killed in total on this level?
int lastActiveRow = 2; // what's the lowest row in which we will find live aliens?
int deadOn1 = 0; // how many aliens are dead on row one (middle row)
int deadOn2 = 0; // how many aliens are dead on row two (bottom row)

boolean fire = 0;
int topScoreB = 0;
int player; //0 to 128-platformWidth  - this is the position of the player
int platformWidth = 16; 
boolean stopAnimate = 0; // this is set to 1 when a collision is detected
boolean mute = 0;
boolean newHigh = 0;
int score = 0; // score - this affects the difficulty of the game
int top = 0;


void main()
{
	setup();
	while(1)
	{
		loop();
	}
	
}

// Arduino stuff - setup
void setup() {
  ssd1306_init();
}

// Arduino stuff - loop
void loop() { 
   long startT = millis();
  long nowT =0;
  boolean sChange = 0;
	player = 96;
  ssd1306_fillscreen(0x00);
  
  // The lower case character set is seriously compromised because I've had to truncate the ASCII table
  // to release space for executable code - hence lower case y and w are remapped to h and / respectively.
  // There is no z in the table (or h!) as these aren't used anywhere in the text here and most of the 
  // symbols are also missing for the same reason (see my hacked version of font6x8.h - font6x8AJ.h for more detail)
  ssd1306_char_f6x8(0, 1, "S P A C E");
  ssd1306_char_f6x8(4, 2, "A T T A C K");
  ssd1306_char_f6x8(0, 7, "Stc8 Acrade"); // see comments above !

  ssd1306_setpos(85,1);
  ssd1306_send_data_start();
  sendBlock(1);
  sendBlock(0);  
  sendBlock(1);
  ssd1306_send_data_stop();
  ssd1306_setpos(85,2);
  ssd1306_send_data_start();
  sendBlock(0);
  sendBlock(2);
  sendBlock(0);
  ssd1306_send_data_stop();
  ssd1306_setpos(85,3);
  ssd1306_send_data_start();
  sendBlock(0);
  sendBlock(0);  
  sendBlock(1);
  sendBlock(0);  
  sendBlock(1);
  ssd1306_send_data_stop();
  
  drawPlatform();

 
//  while(ACT == HIGH) {
//    nowT = millis();
//    if (nowT - startT > 2000) {
//      sChange = 1;     
//      if (RIGHT == HIGH) {

//        ssd1306_char_f6x8(8, 0, "-HIGH SCORE RESET-");  
//      } else if (mute == 0) { mute = 1; ssd1306_char_f6x8(32, 0, "-- MUTE --"); } else { mute = 0; ssd1306_char_f6x8(23, 0, "-- SOUND ON --");  }    
//      break;
//    }
//    if (sChange == 1) break;
//  }  
  waitStart();


  if (sChange == 0) {
	  byte mm;
	  int i;
    for (mm = 112;mm>=1;mm--) {
      if ( (mm>=32) && (mm<56) ) drawFire(104,mm);
      if (mm == 32) {
        ssd1306_setpos(100,3);      
        ssd1306_send_data_start();
        sendBlock(0);
        ssd1306_send_data_stop();
        ssd1306_setpos(100,4);      
        ssd1306_send_data_start();
        sendBlock(0);
        ssd1306_send_data_stop();
        beep(30,100);
      }
      ssd1306_setpos(mm,0);      
      ssd1306_send_data_start();
      sendBlock(3);
      sendBlock(0);
      ssd1306_send_data_stop();
      drawPlatform();
      delay(20);
    }
    ssd1306_setpos(0,0);      
    ssd1306_send_data_start();
    sendBlock(0);
    sendBlock(0);
    ssd1306_send_data_stop();
    ssd1306_char_f6x8(0, 7, "Tiny Arcade");  // see comments above !
    delay(1500);
    ssd1306_init();
    ssd1306_fillscreen(0x00);
    stopAnimate = 0;
    score = 0;
    
    playSpaceAttack(); 
    top=topScoreB;
    
    ssd1306_fillscreen(0x00);
    ssd1306_char_f6x8(11, 1, "----------------");
    ssd1306_char_f6x8(11, 2, "G A M E  O V E R");
    ssd1306_char_f6x8(11, 3, "----------------");
    ssd1306_char_f6x8(37, 5, "SCORE:");
    doNumber(75, 5, score);
    if (!newHigh) {
      ssd1306_char_f6x8(21, 7, "HIGH SCORE:");
      doNumber(88, 7, top);
    }
    for (i = 0; i<1000; i = i+ 50){
      beep(50,i);
    }
    delay(2000);
    if (newHigh) {
      ssd1306_fillscreen(0x00);
      ssd1306_char_f6x8(10, 1, "----------------");
      ssd1306_char_f6x8(10, 3, " NEW HIGH SCORE ");
      ssd1306_char_f6x8(10, 7, "----------------");
      doNumber(50,5,top);
      for (i = 700; i>200; i = i - 50){
      beep(30,i);
      }
      newHigh = 0;
      delay(2700);    
    } 
  }
  system_sleep();
}

void doNumber (int x, int y, int value) {
    char temp[10] = {0,0,0,0,0,0,0,0,0,0};
    itoa(value,temp,10);
    ssd1306_char_f6x8(x, y, temp);
}




/* ------------------------
 *  SpaceAttack Code
 */
void playSpaceAttack() {
  byte i;
  
  firecounter = 0; // Timer for managing fire
  level = 1; // Game level - incremented every time you clear the screen
  topScoreB = 0; // highscore
  score = 0; // obvious
 
  // Initialisations
  for (i = 0; i<5;i++) {
    alienFire[i][0] = 0;
  }
  resetAliens();

  levelUp(1); // This also does various essential initialisations
  
  
  
  while (stopAnimate == 0) {
	 unsigned long lastFrame;
    while(1) {
     byte		afIndex;
	int inc,burnLimit,burn;
    aliencounter++;
    firecounter++;
    mothercounter++;
    lastFrame=millis();
    // deal with inputs
    if(random(0,1000) < 940) fire =1; // this reads the reset pin (pin 1) of the Attiny85 - to use it, see comments section above
    if (ACT==0) fire = 1;
	else fire=0;
    if(RIGHT==0) {
      
      if(player < 127-platformWidth) {
        player++;
        } 
    }
    if (LEFT==0){
      
      if (player >1){
        player--;
        } 
     }

    if ( (mothership == 0) && (random(0,1000) > 998) && (alienRow>0) ) {
      mothership = 1;
      mothershipX= 127-16;
    }

    // draw aliens
    for (inc = 0; inc <= lastActiveRow; inc ++) {
	  int bl;
      ssd1306_setpos((positionNow*8),inc+alienRow);
      ssd1306_send_data_start();
      for (bl = firstAlien; bl <=lastAlien; bl++){
        if(row[inc][bl]==1){
          if (inc == lastActiveRow) {
            if(random(0,1000) > (999-level)) { // this alien is going to fire!
              byte afIndex = 0;
              while (alienFire[afIndex][0] == 1) {
                afIndex++; // search for empty alien fire option
                if (afIndex == 5) break;
              } 
              if (afIndex < 5) { // we've found a slot
                alienFire[afIndex][0] = 1; // activate fire on this slot
                alienFire[afIndex][1] = ( (positionNow*8) + ((bl-firstAlien) * 8) + 4); // x position
                alienFire[afIndex][2] = (inc+alienRow+1)*8; // Where the fire starts
              }
            } // end of this alien firing
          } // only if we are on the lowest row of live aliens
          if ( (inc == 0) || (inc == 2) ) {
            sendBlock(1);
          } else {
            sendBlock(2);
          }
        }else {
          sendBlock(0);
        }
       }   
      ssd1306_send_data_stop();
    }


    // Display the score
    doNumber(0,6,score);
    
    // Burn clock cycles to keep game at constant (ish) speed when there are low numbers of live aliens 
    burnLimit = (8-(lastAlien-firstAlien));
    for (burn = 0; burn < burnLimit; burn+=2) {                  
        drawPlatform();    
        }

    // Display the mothership
    if (mothercounter >= 3) {
      mothercounter = 0;    
      // draw mothership
      if (mothership) {
        ssd1306_setpos(mothershipX,0);
        ssd1306_send_data_start();
        sendBlock(3);
        sendBlock(0);
        ssd1306_send_data_stop();
        mothershipX --;
        if (mothershipX == 0) {
          mothership = 0;                      
          ssd1306_setpos(mothershipX,0);
          ssd1306_send_data_start();
          sendBlock(0);
          sendBlock(0);
          ssd1306_send_data_stop();
        }
      }
    }

    // Move the aliens
    if (aliencounter >= (92-((level-1)*5)) ) {
        aliencounter = 0;
      if(alienDirection) { // Moving right
        // move down a row
        if (positionNow >= 6+(8-(lastAlien-firstAlien))) {
          alienDirection = 0;
          clearAlienArea(alienRow,lastActiveRow);                                   
          alienRow++;
        } else {
          positionNow++;
        }
      } else { // Moving left
        // move down a row
        if (positionNow <= 0) {
          alienDirection = 1;
          clearAlienArea(alienRow,lastActiveRow);                                   
          alienRow++;
        } else {
          positionNow --;
        }
      } 
      clearAlienArea(alienRow,lastActiveRow);                                   
    }

    // Fire !
    if ((fire == 1) && (playerFire[0] == 0)) { // fire has been pressed and we're not currently firing - initiate fire!!
      playerFire[0] = 1;
      playerFire[1] = player+platformWidth/2; // xposition of new fire!
      playerFire[2] = 56;
    }

    // Handle all firing-related stuff (in both directions!)
    if (firecounter >= 2) {                  
      firecounter = 0;
      // --- Deal with player Firing --- 
      if (playerFire[0] == 1) {                      
          drawFire(playerFire[1], playerFire[2]);
          if (playerFire[2] == 0) {
            ssd1306_setpos(playerFire[1],0);
            
            ssd1306_send_data_start();
            ssd1306_send_byte(B00000000);  
            ssd1306_send_data_stop();                                                          
            playerFire[0] = 0;
            fire = 0;
          } else {
            playerFire[2] = playerFire[2] - 1;                          
          }
      }
      
      // aliens are at positionNow * 8 + 8* their index
      
      leftLimit = positionNow*8;
      if ((fire == 1)) {
        fireXidx = firstAlien + floor((playerFire[1]-positionNow*8)/8);                    
        fireYidx=  floor(floor(playerFire[2]/8) - alienRow);
        
        if((mothership == 1) && (playerFire[1] >= mothershipX) && (playerFire[1] <= mothershipX +8) && playerFire[2] <=8) {
          long scm = random(1,100);
          if (scm<30) {
            score +=50;
          } else if (scm<60) {
            score += 100;
          } else if (scm<90) {
            score += 150;
          } else {
            score += 300;
          }
            
          beep(30,400);
          beep(30,300);          
          beep(30,200);
          beep(30,100);
          mothership = 0;
          ssd1306_setpos(mothershipX,0);
          ssd1306_send_data_start();
          sendBlock(0);
          sendBlock(0);
          ssd1306_send_data_stop();
        }

        // Alien has been hit
        if ((playerFire[1] >= leftLimit) && (fireYidx>=0) && (fireYidx<=lastActiveRow) && (fireXidx>=0) && (fireXidx<9)) {                    
          if (row[fireYidx][fireXidx] == 1) { 
             int xi;
            int lastActiveToClear = lastActiveRow; // if we kill the last alien on a row - we still need to clear that row (end of this fn)
            
            if (fireYidx == 2) deadOn2++;
            if (fireYidx == 1) deadOn1++;
            
            if (deadOn2 == 5) {lastActiveRow = 1;}
            if ((deadOn1 == 4) && (deadOn2 == 5)) {lastActiveRow = 0;}

            score = score + (int)((3-fireYidx) * 10);
                        
            aliensDead++;                      
            ssd1306_setpos(playerFire[1],alienRow+fireYidx+1);
            
            ssd1306_send_data_start();
            ssd1306_send_byte(B00000000);  
            ssd1306_send_data_stop();                                                          
            beep(30,100);
            
            fire = 0;
            playerFire[0] = 0;
            playerFire[1] = 0;
            playerFire[2] = 7;
            row[fireYidx][fireXidx] = 0;
            
            if (fireXidx == firstAlien) { // this is the first Alien - sweep back and reset to new first alien
              for (xi = lastAlien; xi>=firstAlien;xi--) {
                if ( (row[0][xi] == 1) || (row[1][xi] == 1) || (row[2][xi] == 1) ) newFirst = xi;
              }
              positionNow += newFirst - firstAlien;
              firstAlien = newFirst;
            }
            
            if (fireXidx == lastAlien) { // this is the last Alien - sweep thru and reset to new last alien
				
              for (xi = firstAlien; xi<=lastAlien;xi++) {
                if ( (row[0][xi] == 1) || (row[1][xi] == 1) || (row[2][xi] == 1) ) newLast = xi;
              }
              lastAlien = newLast;
            }
            clearAlienArea(alienRow,lastActiveToClear);
                                               
          }         
        }
      }


      // --- Deal with Alien Firing --- 
      for (afIndex = 0; afIndex<5; afIndex++) {
        if(alienFire[afIndex][0] == 1) {
          drawFire(alienFire[afIndex][1],alienFire[afIndex][2]);
          alienFire[afIndex][2] = alienFire[afIndex][2] + 1;

          if (alienFire[afIndex][2] >= 56) {
			  
            ssd1306_setpos(alienFire[afIndex][1],7);
            
            ssd1306_send_data_start();
            ssd1306_send_byte( B00000000);  
            ssd1306_send_data_stop();                                                          
            alienFire[afIndex][0] = 0; // the fire's got to the end
            if( ((alienFire[afIndex][1]) > player) && ( (alienFire[afIndex][1]) < player+platformWidth) ) { // you've been hit!!
              stopAnimate = 1;
              goto die;
              }
          }
        }                  
      } // end of aliens firing                  
    }

    if (aliensDead == 14) {
		int ai;
      for (ai = 0; ai <= 5; ai++) alienFire[ai][0] = 0;

      level++;
      if (level>15) level = 15;
      levelUp(level);

      resetAliens();
    }
    
    if( ((alienRow == 5) && (lastActiveRow == 2))||  ( (alienRow == 6) && (lastActiveRow == 1)) || ((alienRow == 7) && (lastActiveRow == 0)) || stopAnimate) {
      stopAnimate = 1;
      break;
    }
    // draw the player
    drawPlatform();
	while(millis()-lastFrame<SPEED);
    }
 }
die:
  topScoreB = 0;
  topScoreB = topScoreB << 8;
  topScoreB = topScoreB;

  if (score > topScoreB) { 
    topScoreB = score;
    
    newHigh = 1;
    }
  }

void drawPlatform(){
	byte pw ;
 ssd1306_setpos(player,7);
 ssd1306_send_data_start();
 ssd1306_send_byte(B00000000);   
 ssd1306_send_byte(B00000000);   
 for (pw = 0; pw <5; pw++){ssd1306_send_byte(B11000000);}                
 for (pw = 0; pw <4; pw++){ssd1306_send_byte(B11110000);}                
 for (pw = 0; pw <5; pw++){ssd1306_send_byte(B11000000);}                
 ssd1306_send_byte(B00000000);   
 ssd1306_send_byte(B00000000);   
 ssd1306_send_data_stop();  
}

void sendBlock(int fill){
  if (fill == 1) {
   ssd1306_send_byte(B10011000);
   ssd1306_send_byte(B01011100);
   ssd1306_send_byte(B10110110);
   ssd1306_send_byte(B01011111);
   ssd1306_send_byte(B01011111);
   ssd1306_send_byte(B10110110);
   ssd1306_send_byte(B01011100);
   ssd1306_send_byte(B10011000);
  } else if (fill == 2) {
   ssd1306_send_byte(B00110000);
   ssd1306_send_byte(B00111110);
   ssd1306_send_byte(B10110011);
   ssd1306_send_byte(B01011101);
   ssd1306_send_byte(B01011101);
   ssd1306_send_byte(B10110011);
   ssd1306_send_byte(B00111110);
   ssd1306_send_byte(B00110000);  
  } else if (fill == 3) {
   ssd1306_send_byte(B00011000);
   ssd1306_send_byte(B00111000);
   ssd1306_send_byte(B00110100);
   ssd1306_send_byte(B00110100);
   ssd1306_send_byte(B00110100);
   ssd1306_send_byte(B00110100);
   ssd1306_send_byte(B00111000);
   ssd1306_send_byte(B00011000);  
  } else if (fill == 0) {
   ssd1306_send_byte(B00000000);
   ssd1306_send_byte(B00000000);
   ssd1306_send_byte(B00000000);
   ssd1306_send_byte(B00000000);
   ssd1306_send_byte(B00000000);
   ssd1306_send_byte(B00000000);
   ssd1306_send_byte(B00000000);
   ssd1306_send_byte(B00000000);
  } 
}

void levelUp(int number) {
  int i;
  fire = 0; // make sure no fire
  playerFire[0] = 0; // player fire inactive
  aliencounter=0;
  firecounter=0;
  mothercounter=0;
  firstAlien = 0;
  lastAlien = 8;
  newFirst = 0;
  newLast = 8;
  lastActiveRow = 2;
  deadOn1 = 0;
  deadOn2 = 0;
  aliensDead = 0;      
  mothership = 0;
  alienRow = 0;
  positionNow = 0;
  alienDirection = 1;
  player = 64;

  ssd1306_fillscreen(0x00);
  ssd1306_char_f6x8(16, 3, "--------------");
  ssd1306_char_f6x8(16, 4, " L E V E L ");
  ssd1306_char_f6x8(16, 5, "--------------");
  doNumber(85,4,number);
  for (i = 800; i>200; i = i - 200){
  beep(30,i);
  }
  delay(700);    
  ssd1306_fillscreen(0x00);
}

void drawFire(int x, int y) {
  if (y%8!=0){
    ssd1306_setpos(x,y/8);
    ssd1306_send_data_start();
    doDrawLS(0,y%8);
    ssd1306_send_data_stop();
    
    ssd1306_setpos(x,y/8+1);
    ssd1306_send_data_start();
    doDrawRS(0,8-y%8);
    ssd1306_send_data_stop();
  } else {
    ssd1306_setpos(x,y/8);
    ssd1306_send_data_start();
    doDrawLS(0,0);
    ssd1306_send_data_stop();
  }
}

// Drawing routine for the player and fire - with right-shifts
void doDrawRS(long P1, byte P2) {
  ssd1306_send_byte((B01111110 | P1)>>P2);
}

// Drawing routine for the player and fire - with left-shifts
void doDrawLS(long P1, byte P2) {
  ssd1306_send_byte((B01111110 | P1)<<P2);
}

void clearAlienArea(int row, int lastActive) {
  // clear alien area
	int bl,inc;
  for (inc = row; inc <=row+lastActive; inc ++) {
    ssd1306_setpos(0,inc);
    ssd1306_send_data_start();
    for (bl = 0; bl <16; bl++){
      sendBlock(0);
      }   
      ssd1306_send_data_stop();
    }
}

void resetAliens(void) {
	byte i;
  for (i =0; i<10;i++){ // reset aliens
    row[0][i]=0; row[1][i]=0; row[2][i]=0;
  } 
  for (i =0; i<9;i+=2){ // reset aliens
    row[0][i]=1; row[2][i]=1;
  } 
  for (i =1; i<9;i+=2){ // reset aliens
    row[1][i]=1;
  }     
}




