#include <Wire.h> // библиотека для протокола I2C
#include <LiquidCrystal_I2C.h> // подключаем библиотеку для LCD 1602
#include <DHT.h> // подключаем библиотеку для датчика
#include <LCD_1602_RUS.h> //Библотека русских символов для LCD 1602
#include <microDS3231.h>
#include <Adafruit_Sensor.h>
MicroDS3231 rtc;


LCD_1602_RUS LCD(0x27,16,2); // присваиваем имя LCD для дисплея
DHT dht(4, DHT22); // сообщаем на каком порту будет датчик

#define PERIOD_STATE 25
#define PERIOD_OTHER 3000
#define LCD_DISPLAY 3000

#define ON_TIME 20 //Время включения в формате 8:00 = 800, 23:54 = 2354
#define OFF_TIME 21 //Время выключения


bool isHum;
bool isLight;

void lightTurnOff();
void lightTurnOn();

void setup() {
  pinMode(6, OUTPUT);
  isHum = true;
  isLight = true;
  Serial.begin(9600);
  Wire.begin();
  
  LCD.init(); // инициализация LCD дисплея
  LCD.backlight(); // включение подсветки дисплея
  dht.begin(); // запускаем датчик DHT22
  rtc.setTime(COMPILE_TIME);
}

void lightTurnOn() {
  digitalWrite(6, LOW);
  isLight = true;
}

void lightTurnOff() {
  digitalWrite(6, HIGH);
  isLight = false;
}

uint8_t non_stop_program1(uint16_t span) {
  static uint32_t future = 0;
  if (millis()<future) return 0;
  future += span;
  return 1;
}

void showLine(String title, float val, String postfix) {
  LCD.setCursor(0,0);
  LCD.print(" " + title +": ");
  
  LCD.setCursor(0,1);
  LCD.print(" " + postfix);
  LCD.print(val);
}

void showHum(){
  float h = dht.readHumidity();
  showLine("Влага воздуха", h, "    % ");
}

void showTemp(){
  float t = dht.readTemperature();
  showLine("Темп. воздуха", t, "    t°");
}

void change() {
  if(isHum) {
    showTemp();
  } else {
    showHum();
  }
  isHum = !isHum;
}

void debugTime(int currentTime) {
  Serial.print(rtc.getHours());
  Serial.print(":");
  Serial.print(rtc.getMinutes());
  Serial.print(":");
  Serial.print(rtc.getSeconds());
  Serial.print(" ");
  Serial.print("Status: ");
  Serial.print(isLight ? "on" : "off");
  Serial.print(" ");
  Serial.println(currentTime);
}

void loop() 
{
  int currentTime = rtc.getHours()*100 + rtc.getMinutes();

  debugTime(currentTime);
  
  if(currentTime >= ON_TIME && currentTime < OFF_TIME && !isLight) {
    lightTurnOn();
  }

  if((currentTime >= OFF_TIME || currentTime < ON_TIME) && isLight) {
    lightTurnOff();
  }

  if (non_stop_program1(3750)) {
    change();
  }
}