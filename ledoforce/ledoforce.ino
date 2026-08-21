#include <MD_MAX72xx.h> 

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 
#define MAX_DEVICES 4
#define CS_PIN 10
#define LED_INTENSITY 4
enum States {
  POMODORO,
  CLOCK,
  LIGHTSABER
}; 
const States actualState = POMODORO;
unsigned long lastExecutedMillis = 0;

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);



typedef struct {
  int height;
  int width;
  uint64_t characters[10];
  bool is_number;
} font_t;

void drawCharacter(int startX, int num, font_t used_font);
font_t used_font_num;

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

font_t FONT_BREAK{
  7,
  5,
  {
  0x001f1515151a0000,
  0x001f0505051a0000,
  0x001f151515150000,
  0x001e0505051e0000,
  0x001f040406190000
  },
  true
};


void drawCharacter(int startX, int num, font_t used_font) {
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

// LIGHTSABER STATE
void drawLightsaber() {
  for (int i = 2; i < 32; i++) {
    for(int y = 2; y <7; y++){
      mx.setPoint(y, i, true);  
    }
    
    delay(15); 
  }
}
int randTime = random(500, 3000);
void lighsaberLoop(unsigned long currentMillis){
  if (currentMillis - lastExecutedMillis >= randTime) {
    lastExecutedMillis = currentMillis; 
    mx.control(MD_MAX72XX::INTENSITY, random(1,8));
    randTime = random(500, 3000);
  }
}
// POMODORO STATE
#define POMODORO_WORK 3000
#define POMODORO_BREAK 600

int pomodoro_seconds = POMODORO_BREAK;
bool pomodoro_is_break = true;
bool pomodoro_break_blink = true;
void pomodoroLoop(unsigned long currentMillis){
  if (currentMillis - lastExecutedMillis >= 1000) {
      lastExecutedMillis = currentMillis; 
      
      drawPomodoroTime(pomodoro_seconds);
      if(pomodoro_seconds == 0){
        if(pomodoro_is_break){
          pomodoro_seconds = POMODORO_WORK;
          pomodoro_is_break = false;
        }else{
          pomodoro_seconds = POMODORO_BREAK;
          pomodoro_is_break = true;
        }
      }
      pomodoro_seconds--;
    }
}
void drawPomodoroTime(int seconds){  
  
  int timeSeconds    = (seconds % 60) % 10;
  int timeTenSeconds = (seconds % 60) / 10;
  int timeMin        = (seconds / 60) % 10; 
  int timeTenMin     = (seconds / 60) / 10; 

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.clear(); 
  if(pomodoro_is_break){

    if(pomodoro_break_blink){
      for(int i = 0; i < 5; i++)
      {
        drawCharacter(31 - (i*used_font_num.width) - i, i, FONT_BREAK);
      }
      pomodoro_break_blink = false;
    }else{
      drawCharacter( 17 + (2 * used_font_num.width) + 2, timeTenMin, used_font_num);
      drawCharacter( 17 + used_font_num.width + 1, timeMin, used_font_num);
      drawCharacter(14, timeTenSeconds, used_font_num);
      drawCharacter(14 - used_font_num.width - 1, timeSeconds, used_font_num);
      drawSeparator();
      pomodoro_break_blink = true;
    }
    

  } else{
    
    drawCharacter( 17 + (2 * used_font_num.width) + 2, timeTenMin, used_font_num);
    drawCharacter( 17 + used_font_num.width + 1, timeMin, used_font_num);
    drawCharacter(14, timeTenSeconds, used_font_num);
    drawCharacter(14 - used_font_num.width - 1, timeSeconds, used_font_num);

    drawSeparator();

  }
  
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}


void setup()
{
  used_font_num = FONT_NUM_VAD;
  randomSeed(analogRead(0));
//pinMode(BUZZER_PIN, OUTPUT);
  mx.begin();     
                        
  mx.control(MD_MAX72XX::INTENSITY, LED_INTENSITY); 
  //pinMode(BUZZER_PIN, OUTPUT);
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  randomSeed(analogRead(0));
  
  mx.clear(); 
  delay(2000);
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  drawLightsaber();
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  // for testing
  delay(2000);
  switch (actualState) {
    case POMODORO:
      
      break;
    case CLOCK:
      break;

  }
  
}

void loop()
{
  
  unsigned long currentMillis = millis(); //it will be clock module in the future

  switch (actualState) {
    case POMODORO:
      pomodoroLoop(currentMillis);
      break;
    case CLOCK:
      break;
    case LIGHTSABER:
      lighsaberLoop(currentMillis);
      break;

  }
  if(actualState == POMODORO ){
    
  }
  
}