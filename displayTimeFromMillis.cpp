// #include <Arduino.h>
#include <TM1638plus.h>
#include "displayTimeFromMillis.h"  // Подключаем заголовочный файл
#include "global.h"

// функция переводит секунды в нормальный вид: часы:минуты
// https://github.com/dfdxAlex/KiaCeed.git
// https://youtu.be/u3vY8uXiEkI
void displayTimeFromMillis(unsigned long ms) {

  unsigned long totalSeconds = ms / 1000;       // Перевести миллисекунды в секунды
  uint8_t seconds = totalSeconds % 60;          // Посчитать число секунд
  uint8_t minutes = (totalSeconds / 60) % 60;   // Посчитать число минут
  uint16_t hours   = (totalSeconds / 3600);     // Посчитать число часов

  // Ограничим отображение максимумом 2 цифр:
  if (hours > 99) hours = 99;                   // Максимальное число часов 99

// sprintf(outString, "%02d.%02d.%02d  ", hours, minutes, seconds);
snprintf(outString, sizeof(outString), "%02d.%02d.%02d  ", hours, minutes, seconds);
}