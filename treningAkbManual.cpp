#include <Arduino.h>
#include "treningAkbManual.h"
#include "requestButtonEnd.h"
#include "global.h"
#include "calculateDischargeTime.h"
#include "unshift.h"

void treningAkbManual()
{

      if (requestButtonEnd(0b00000001)) { // Сюда заходим если нажата только S1
          
          // Если заполнился массив, то принять решение о дальнейшем поведении
          // Либо если слишком много (20) нулевых результатов
          if (unshift(mAh, 5)) {
              long tDischarge = (long)(unshift(mAh) / 5);
              // Подсчитаь новое время зарядки по формуле, просто понты, можно без логорифмов
              tChardge = 2* tChardge * (1 + log(tDischarge + 1));
              if (isBatteryBad) {
                //   isBatteryBad = false; // снять признак плохого аккумулятора
                //   unshift();            // обнулить массив с данными
                  tChardge = tChardge * 2;
                //   return;
              }

              isBatteryBad = false; // снять признак плохого аккумулятора
              unshift();            // обнулить массив с данными
          }
          // Режим заряд/разряд по времени
          // Здесь только процесс сравнивания с текущим состоянием работы одного из режимов заряд/разряд и если времени прошло больше заданного, то переключаемся
          if (timeAccumulator > tChardge)                  // Если превышено время разрядки
              if (digitalRead(LED_BUILTIN) != HIGH) {      // Если ещё не включили разрядку
                  digitalWrite(LED_BUILTIN, HIGH);         // Включить разрядку
                  calculateDischargeTime(true);            // Начала отсчёта времени разрядки
                  milisek = millis();                      // Обнуление переменной хранящей контрольную точку таймера
                  timeAccumulator = 0;                     // Обнулить аккумулятор
                  delay(100);
                  return;
              }
          if (analogRead(A0) < lowChardg)                              // Если превышено время зарядки
              if (digitalRead(LED_BUILTIN) != LOW) {                   // Если ещё не включена зарядка
                  int rezult = (int)(calculateDischargeTime()/1000);   // Зафиксировать время разрядки
                  unshift(rezult, mAh, 5);                             // Добавить данные в массив
                  digitalWrite(LED_BUILTIN, LOW);                      // Включить зарядку
                  milisek = millis();                                  // Обнуление переменной хранящей контрольную точку таймера
                  timeAccumulator = 0;                                 // Обнулить аккумулятор
                  delay(100);
              }

      }

}