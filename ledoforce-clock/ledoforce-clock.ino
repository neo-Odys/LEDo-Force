#include <MD_MAX72xx.h> 
#include "Arduino.h"
#include "uRTCLib.h"


#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 
#define MAX_DEVICES 4
#define CS_PIN 10
#define LED_INTENSITY 4
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);


enum States {
  CLOCK, 
  LIGHTSABER
}; 
const States actualState = CLOCK;
unsigned long lastExecutedMillis = 0;

// FONTS
typedef struct {
  int height;
  int width;
  uint64_t characters[10];
  bool is_number;
} font_t;


//https://xantorohara.github.io/led-matrix-editor/#001f1111111f0000|0011111f10100000|001d151515170000|00151515151f0000|00070404041f0000|00171515151d0000|001f1515151d0000|0001011905030000|001f1515151f0000|00171515151f0000
font_t FONT_NUM_VAD{
  7,
  5,
  {
  0x001f1111111f0000,
  0x0011111f10100000,
  0x001d151515170000,
  0x00151515151f0000,
  0x00070404041f0000,
  0x00171515151d0000,
  0x001f1515151d0000,
  0x0001011905030000,
  0x001f1515151f0000,
  0x00171515151f0000
  },
  true
};
const font_t used_font_num = FONT_NUM_VAD;


// DRAWING 
void drawCharacter(int startX, int num, const font_t& used_font) {
  uint64_t character = used_font.characters[num];
  
  // AI MADE THIS:
  for (int row = (8 - used_font.height); row < 8; row++) {
    byte line = (character >> (row * 8)) & 0xFF;
    for (int col = 0; col < used_font.width; col++) {
      bool pointInflamed = bitRead(line, col);
      mx.setPoint(row, startX - col, pointInflamed);
    }
  }
  // END OF AI
}
void drawSeparator() {
    mx.setPoint(5, 16, true);
    mx.setPoint(5, 17, true);
    mx.setPoint(3, 16, true);
    mx.setPoint(3, 17, true);
}

//
// LIGHTSABER STATE
//

void drawLightsaber() {
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
    
  for (int i = 2; i < 32; i++) {
    for(int y = 2; y <7; y++){
      mx.setPoint(y, i, true);  
    }
    
    delay(15); 
  }
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
}
int randTime = 0;
void lighsaberLoop(unsigned long currentMillis){
  if (currentMillis - lastExecutedMillis >= randTime) {
    lastExecutedMillis = currentMillis; 
    mx.control(MD_MAX72XX::INTENSITY, random(1,8));
    randTime = random(500, 3000);
  }
}

//
// CLOCK STATE
//

uRTCLib rtc(0x68);
void drawClockTime(){  
  

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.clear(); 
  int hour = rtc.hour();
  int minutes = rtc.minute();
  drawCharacter( 17 + (2 * used_font_num.width) + 2, (hour)/10, used_font_num);
  drawCharacter( 17 + used_font_num.width + 1, hour%10, used_font_num);
  drawCharacter(14, minutes/10, used_font_num);
  drawCharacter(14 - used_font_num.width - 1, minutes%10, used_font_num);
  drawSeparator();

  
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

//



void setClock(){
  // Comment out below line once you set the date & time.
  // Following line sets the RTC with an explicit date & time
  // for example to set April 14 2025 at 12:56 you would call:
  rtc.set(0, 43, 12, 2, 14, 4, 25);
  // rtc.set(second, minute, hour, dayOfWeek, dayOfMonth, month, year)
  // set day of week (1=Sunday, 7=Saturday)
}

void setup()
{

  mx.begin();     
  mx.control(MD_MAX72XX::INTENSITY, LED_INTENSITY); 
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.clear();

  setClock();


  
}

void loop()
{
  
  unsigned long currentMillis = millis(); //it will be clock module in the future (maybe)
  // like for pomodoro usage it wouldn't be too big problem
  if (currentMillis - lastExecutedMillis >= 1000) {
    rtc.refresh();
    drawClockTime();
  }

  
}