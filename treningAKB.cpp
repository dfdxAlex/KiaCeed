#include <Arduino.h>
#include <TM1638plus.h>
#include "treningAKB.h"  // Подключаем заголовочный файл
#include "global.h"
#include "requestButtonEnd.h"

// Функция тренирует акб
bool treningAKB()
{
  if (!buttonArray[0]) {
      digitalWrite(LED_BUILTIN, LOW);  // Зарядка акамулятора
      return false;
  }

  if (requestButtonEnd(0b00000001)) { // Сюда заходим если нажата только S1
      if (digitalRead(LED_BUILTIN) == HIGH) 
         chardgeRightNow = true;
      if (digitalRead(LED_BUILTIN) == LOW) 
         chardgeRightNow = false;

      // Режим заряд/разряд по времени
      // Здесь только процесс сравнивания с текущим состоянием работы одного из режимов заряд/разряд и если времени прошло больше заданного, то переключаемся
      if (timeAccumulator > tChardge)                     // Если превышено время разрядки
          if (digitalRead(LED_BUILTIN) != HIGH) {      // Если ещё не включили разрядку
              digitalWrite(LED_BUILTIN, HIGH);         // Включить разрядку
              milisek = millis();                      // Обнуление переменной хранящей контрольную точку таймера
              timeAccumulator = 0;                     // Обнулить аккумулятор
          }
      if (timeAccumulator > tWork)                  // Если превышено время зарядки
          if (digitalRead(LED_BUILTIN) != LOW) {       // Если ещё не включена зарядка
              digitalWrite(LED_BUILTIN, LOW);          // Включить зарядку
              milisek = millis();                      // Обнуление переменной хранящей контрольную точку таймера
              timeAccumulator = 0;                     // Обнулить аккумулятор
          }

    
  }
  
  // Режим заряд/разряд по уровням напряжения
  if (requestButtonEnd(0b00001001) || requestButtonEnd(0b00001111)) {  // Если включена умная тренировка hightChardg
      // Проверить массив, он должен быть заполнен

      int intPart;
      int fracPart;

      if (buttonArray[0] && buttonArray[1] && buttonArray[2]) {

        // Блок индикации заряда-разряда. Используется если в смарт тренировке нажаты S2 и S3 
        if (digitalRead(LED_BUILTIN) == HIGH) {
            if (lockFlagDown > analogRead(A0)) lockFlagDown = analogRead(A0);
            // float lockFlagDownF = (float)lockFlagDown/1024*5;
            // float lockFlagDownF = intFracPart(lockFlagDown);
            // intPart = (int)lockFlagDownF;                         // Целая часть
            // fracPart = (int)((lockFlagDownF - intPart) * 1000);    // Дробная часть
            // intPart = intFracPart(intFracPart(lockFlagDown));                        // Целая часть
            // fracPart = intFracPart(intFracPart(lockFlagDown), true);    // Дробная часть
            sprintf(outString, "%01d.%03d", intFracPart(intFracPart(lockFlagDown)), intFracPart(intFracPart(lockFlagDown), true));
        }
        if (digitalRead(LED_BUILTIN) == LOW) {
          if (lockFlagUp < analogRead(A1)) lockFlagUp = analogRead(A1);
          // float lockFlagUpF = (float)lockFlagUp/1024*5;
          // float lockFlagUpF = intFracPart(lockFlagUp);
          // intPart = (int)lockFlagUpF; 
          // intPart = intFracPart(lockFlagUpF);                         // Целая часть
          // fracPart = (int)((lockFlagUpF - intPart) * 1000);    // Дробная часть
          // fracPart = intFracPart(lockFlagUpF, true);    // Дробная часть
          sprintf(outString, "%01d.%03d", intFracPart(intFracPart(lockFlagUp)), intFracPart(intFracPart(lockFlagUp), true));
        }
      }

      // Если аккумулятор разрядка
    //   if (digitalRead(LED_BUILTIN) == HIGH) {
    //       if (analogRead(A0) < lowChardg) {
    //           digitalWrite(LED_BUILTIN, LOW);
    //           chardgeRightNow = true;
    //           Serial.println("Смарт разрядка");
    //       }
    //   }
    //   // Если аккумулятор зарядка
    //   if (digitalRead(LED_BUILTIN) == LOW) {
    //     Serial.println("Смарт зарядка 1");
    //        if (!chardgeRightNow) {
    //         digitalWrite(LED_BUILTIN, HIGH);
    //         chardgeRightNow = false;
    //         Serial.println("Смарт зарядка 2");
    //        } 
    //   }
  }
}

// Ради развлечения реализована перегрузка функций.
// Функция при получении целого числа пересчитывает его в десятичное.
// Целое - это макс 1024 при 5 вольтах на входе.
// Дробное - это пересчёт текущего напряжения
float intFracPart(int flag)
{
    return (float)flag/1024*5;
}

// Если на вход приходит только одно дробное, то вернуть целюю часть
int intFracPart(float flag)
{
    return (int)flag;
}

// Если на вход приходит одно дробное и любой boolean, то вернуть дробную часть
int intFracPart(float flag, bool x)
{
    return (int)((flag - intFracPart(flag)) * 1000);
}