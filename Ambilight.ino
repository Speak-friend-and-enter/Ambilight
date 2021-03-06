/*
Copyright (c) <2017> <https://github.com/RanzQ/serial-fastled>
*/
//----------------------НАСТРОЙКИ-----------------------
#define NUM_LEDS 63          // число светодиодов в ленте
#define DI_PIN 13            // пин, к которому подключена лента

#define auto_bright 0        // автоматическая подстройка яркости от уровня внешнего освещения (1 - включить, 0 - выключить)
#define max_bright 255       // максимальная яркость (0 - 255)
#define min_bright 50        // минимальная яркость (0 - 255)
#define bright_constant 200  // константа усиления от внешнего света (0 - 1023)

unsigned long delay_time;
// чем МЕНЬШЕ константа, тем "резче" будет прибавляться яркость
//----------------------НАСТРОЙКИ-----------------------

int new_bright;
unsigned long bright_timer;

#define serialRate 115200  // скорость связи с ПК
uint8_t prefix[] = {'A', 'd', 'a'}, hi, lo, chk, i;  // кодовое слово Ada для связи
#include <FastLED.h>
CRGB leds[NUM_LEDS];  // создаём ленту

void setup()
{
  FastLED.addLeds<WS2812, DI_PIN, GRB>(leds, NUM_LEDS);  // инициализация светодиодов
  //LEDS.setBrightness(max_bright);
  Serial.begin(serialRate);
  Serial.print("Ada\n");     // Связаться с компом
}

void loop() {
  
  if (auto_bright) {                         // если включена адаптивная яркость
    if (millis() - bright_timer > 100) {     // каждые 100 мс
      bright_timer = millis();               // сброить таймер
      new_bright = map(analogRead(3), 0, bright_constant, min_bright, max_bright);   // считать показания с фоторезистора, перевести диапазон
      new_bright = constrain(new_bright, min_bright, max_bright);
      LEDS.setBrightness(new_bright);        // установить новую яркость
    }
  }
  

  
  for (i = 0; i < sizeof prefix; ++i) {
waitLoop: while (!Serial.available()) ;;
    if (prefix[i] == Serial.read()) continue;
    i = 0;
    goto waitLoop;
  }

  while (!Serial.available()) ;;
  hi = Serial.read();
  while (!Serial.available()) ;;
  lo = Serial.read();
  while (!Serial.available()) ;;
  chk = Serial.read();
  if (chk != (hi ^ lo ^ 0x55))
  {
    i = 0;
    goto waitLoop;
  }

  memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    byte r, g, b;
    // читаем данные для каждого цвета
    while (!Serial.available());
    r = Serial.read();
    while (!Serial.available());
    g = Serial.read();
    while (!Serial.available());
    b = Serial.read();
      
    leds[i].r = r;
    leds[i].g = g;
    leds[i].b = b;
  }
  
  FastLED.show();  // записываем цвета в ленту
}
