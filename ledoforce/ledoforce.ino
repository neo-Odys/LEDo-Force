#include <MD_MAX72xx.h> 

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 
#define MAX_DEVICES 4
#define CS_PIN 10

enum States {
  POMODORO,
  CLOCK,
  LIGHSABER
}; 

States actualState = POMODORO;
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void drawLightsaber() {
  for (int i = 2; i < 32; i++) {
    for(int y = 2; y <7; y++){
      mx.setPoint(y, i, true);  
    }
    
    delay(15); 
  }
}


typedef struct {
  int height;
  int width;
  uint64_t characters[10];
  bool is_number;
} font_t;

void drawCharacter(int startX, int num, font_t used_font);
int seconds = 1186;
font_t used_font_num;

//https://xantorohara.github.io/led-matrix-editor/#001f1111111f0000|0011111f10100000|001d151515170000|00151515151f0000|00070404041f0000|00171515151d0000|001f1515151d0000|0001011905030000|001f1515151f0000|00171515151f0000
font_t FONT_NUM_VAD{
  7,
  6,
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
void drawPomodoroTime(int seconds){  
  
  int timeSeconds    = (seconds % 60) % 10;
  int timeTenSeconds = (seconds % 60) / 10;
  int timeMin        = (seconds / 60) % 10; 
  int timeTenMin     = (seconds / 60) / 10; 

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.clear(); 

  drawCharacter( 17 + (2 * used_font_num.width) + 2, timeTenMin, used_font_num);
  drawCharacter( 17 + used_font_num.width + 1, timeMin, used_font_num);
  drawCharacter(14, timeTenSeconds, used_font_num);
  drawCharacter(14 - used_font_num.width - 1, timeSeconds, used_font_num);

  drawSeparator();

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}


void setup()
{
  used_font_num = FONT_NUM_VAD;
  actualState = POMODORO;
//  pinMode(BUZZER_PIN, OUTPUT);
  mx.begin();     
                        
  mx.control(MD_MAX72XX::INTENSITY, 8); 
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
      seconds = 100;
      break;
    case CLOCK:
      break;

  }
  
}

unsigned long lastExecutedMillis = 0;
bool pomodoro_is_break = false;
void loop()
{
  
  unsigned long currentMillis = millis(); //it will be clock module in the future

  if(actualState == POMODORO ){
    if (currentMillis - lastExecutedMillis >= 1000) {
      lastExecutedMillis = currentMillis; 
      
      drawPomodoroTime(seconds);
      if(seconds == 0){
        if(pomodoro_is_break){
          seconds = 100;
          pomodoro_is_break = false;
        }else{
          seconds = 30;
          pomodoro_is_break = true;
        }
      }
      seconds--;
    }
  }
  

  /*
  int size = sizeof(durations) / sizeof(int);

  for (int note = 0; note < size; note++) {
    //to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int duration = 1000 / durations[note];
    tone(BUZZER_PIN, melody[note], duration);

    //to distinguish the notes, set a minimum time between them.
    //the note's duration + 30% seems to work well:
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);

    //stop the tone playing:
    noTone(BUZZER_PIN);
  }*/
}