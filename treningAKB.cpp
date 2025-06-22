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
  // Если включена умная тренировка hightChardg
  if (requestButtonEnd(0b00001001)             // Включена просто тренировка без индикации
     || requestButtonEnd(0b00001111)           // Включена тренировка с индикации в вольтах
        || requestButtonEnd(0b00001011)        // Включена тренировка с индикацией в цифрах 0-1023
           || requestButtonEnd(0b00001101)) {  // Включена тренировка с индикацией цели в цифрах 0-1023
      // Проверить массив, он должен быть заполнен

      // Показывать значение заряда-разряда в миливольтах если нажаты все 4 кнопки
      if (requestButtonEnd(0b00001111) || requestButtonEnd(0b00001011) || requestButtonEnd(0b00001101)) {
        // Блок индикации заряда-разряда. Используется если в смарт тренировке нажаты S2 и S3 
        // Если наался процесс разрядки аккумулятора, то берем напряжение с входа 0, этот 
        // вход подключен к разрядному резистору
        if (digitalRead(LED_BUILTIN) == HIGH) {
            if (lockFlagDown > analogRead(A0)) lockFlagDown = analogRead(A0);
            if (requestButtonEnd(0b00001111)) {
                sprintf(outString, "%01d.%03d%04s", intFracPart(intFracPart(lockFlagDown)), intFracPart(intFracPart(lockFlagDown), true), "    ");
            }
            if (requestButtonEnd(0b00001011)) {
                sprintf(outString, "%04d.%04d", analogRead(A0), lowChardg);
            }
            if (requestButtonEnd(0b00001101)) {
                sprintf(outString, "%04d.%04d", lowChardg, analogRead(A0));
            }
        }
        // Если выключен выход 13, значит не разряжаем, значит используем напряжение с выхода 1
        // Этот выход подключен а аккумулятору когда он не РАзряжается
        if (digitalRead(LED_BUILTIN) == LOW) {
            if (lockFlagUp < analogRead(A1)) lockFlagUp = analogRead(A1);
            if (requestButtonEnd(0b00001111)) {
                sprintf(outString, "%01d.%03d%04s", intFracPart(intFracPart(lockFlagUp)), intFracPart(intFracPart(lockFlagUp), true), "    ");
            }
            if (requestButtonEnd(0b00001011)) {
                sprintf(outString, "%04d.%04d", analogRead(A1), hightChardg);
            }
            if (requestButtonEnd(0b00001101)) {
                sprintf(outString, "%04d.%04d", hightChardg, analogRead(A1));
            }
        }
      }

      //Если аккумулятор разрядка
      if (digitalRead(LED_BUILTIN) == HIGH) {
          // Если аккумулятор разрядился до нужного уровня
          if (analogRead(A0) < lowChardg) {
              digitalWrite(LED_BUILTIN, LOW);
          }
      }
      // Если аккумулятор зарядка
      if (digitalRead(LED_BUILTIN) == LOW)
          // Если напряжении на аккумуляторе больше заданного и если процесс зарядки закончился от зарядного
          if (analogRead(A1) > hightChardg && digitalRead(9)) {
            digitalWrite(LED_BUILTIN, HIGH);
      }
  }
}

// Ради развлечения реализована перегрузка функций.
// Функция при получении целого числа пересчитывает его в десятичное.
// Целое - это макс 1024 при 5 вольтах на входе.
// Дробное - это пересчёт текущего напряжения
// ******************************************
// Если в функцию заходит int, это значит что пришёл int с аналогового входа 0-1023
// Значит нужно преобразовать это значение в число дробное, показывающее вольты 
// Стандартный способ пересчёта АЦП в напряжение на Arduino
float intFracPart(int flag)
{
    return (float)flag/1023*5;
}

// Если на вход приходит только одно дробное, то вернуть целюю часть
int intFracPart(float flag)
{
    return (int)flag;
}

// Если на вход приходит одно дробное и любой boolean, то вернуть дробную часть
int intFracPart(float flag, bool x)
{
    return (int)round((flag - intFracPart(flag)) * 1000);
}