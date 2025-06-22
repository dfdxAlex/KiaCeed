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
  if (requestButtonEnd(0b00001001) || requestButtonEnd(0b00001111)) {  // Если включена умная тренировка hightChardg
      // Проверить массив, он должен быть заполнен

      // Показывать значение заряда-разряда в миливольтах если нажаты все 4 кнопки
      if (requestButtonEnd(0b00001111)) {
        // Блок индикации заряда-разряда. Используется если в смарт тренировке нажаты S2 и S3 
        // Если наался процесс разрядки аккумулятора, то берем напряжение с входа 0, этот 
        // вход подключен к разрядному резистору
        if (digitalRead(LED_BUILTIN) == HIGH) {
            if (lockFlagDown > analogRead(A0)) lockFlagDown = analogRead(A0);
            sprintf(outString, "%01d.%03d", intFracPart(intFracPart(lockFlagDown)), intFracPart(intFracPart(lockFlagDown), true));
        }
        // Если выключен выход 13, значит не разряжаем, значит используем напряжение с выхода 1
        // Этот выход подключен а аккумулятору когда он не РАзряжается
        if (digitalRead(LED_BUILTIN) == LOW) {
        //   Serial.println(analogRead(A1));
          if (lockFlagUp < analogRead(A1)) lockFlagUp = analogRead(A1);
          sprintf(outString, "%01d.%03d", intFracPart(intFracPart(lockFlagUp)), intFracPart(intFracPart(lockFlagUp), true));
        }
      }

      //Если аккумулятор разрядка
      if (digitalRead(LED_BUILTIN) == HIGH) {
          if (analogRead(A0) < lowChardg) {
              digitalWrite(LED_BUILTIN, LOW);
          }
      }
      // Если аккумулятор зарядка
      if (digitalRead(LED_BUILTIN) == LOW && analogRead(A1) > hightChardg) {
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