#include <Arduino.h>
#include <TM1638plus.h>
#include "treningAKB.h"  // Подключаем заголовочный файл
#include "global.h"
#include "requestButtonEnd.h"
#include "unshift.h"
#include "buttonClosed.h"
#include "listingmAh.h"

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

   // Функция проверяет нажата ли одна из комбинаций кнопок, для работы тренировки аккумуляторов
   if (openSmartTrening()) {  
      // Показывать значение заряда-разряда в миливольтах если нажаты все 4 кнопки
        if (!requestButtonEnd(0b00001001) && !requestButtonEnd(0b00011001))
              {
          // Блок индикации заряда-разряда. Используется если в смарт тренировке нажаты S2 и S3 
          // Если наался процесс разрядки аккумулятора, то берем напряжение с входа 0, этот 
          // вход подключен к разрядному резистору
          int localAnalogRead = 0;
          if (digitalRead(LED_BUILTIN) == HIGH) {
              lockFlagDown = analogRead(A0);  // Читаем один раз данные с входа
            //   if (lockFlagDown > localAnalogRead) lockFlagDown = localAnalogRead; // Если прочитанное значение меньше предыдущего, то тогда уменьшаем значение
              if (requestButtonEnd(0b00001111) || requestButtonEnd(0b00011111)) {
                  sprintf(outString, "%01d.%03d%04s", intFracPart(intFracPart(lockFlagDown)), intFracPart(intFracPart(lockFlagDown), true), "    ");
              }
              if (requestButtonEnd(0b00001011) || requestButtonEnd(0b00011011)) {
                  sprintf(outString, "%04d.%04d", analogRead(A0), lowChardg);
              }
              if (requestButtonEnd(0b00001101) || requestButtonEnd(0b00011101)) {
                  sprintf(outString, "%04d.%04d", lowChardg, analogRead(A0));
              }
          }
          // Если выключен выход 13, значит не разряжаем, значит используем напряжение с выхода 1
          // Этот выход подключен а аккумулятору когда он не РАзряжается
          if (digitalRead(LED_BUILTIN) == LOW) {
            //   localAnalogRead = analogRead(A1);   // Читаем один раз данные с входа
            lockFlagUp = analogRead(A1);   // Читаем один раз данные с входа
            //   if (lockFlagUp < localAnalogRead) lockFlagUp = localAnalogRead; // Если прочитанное значение больше предыдущего, то тогда увеличиваем значение
              if (requestButtonEnd(0b00001111) || requestButtonEnd(0b00011111)) {
                  sprintf(outString, "%01d.%03d%04s", intFracPart(intFracPart(lockFlagUp)), intFracPart(intFracPart(lockFlagUp), true), "    ");
              }
              if (requestButtonEnd(0b00001011) || requestButtonEnd(0b00011011)) {
                  sprintf(outString, "%04d.%04d", analogRead(A1), hightChardg);
              }
              if (requestButtonEnd(0b00001101) || requestButtonEnd(0b00011101)) {
                  sprintf(outString, "%04d.%04d", hightChardg, analogRead(A1));
              }
          }
      }

      //Блок отключения разрядки аккумулятора по нижнему напряжению, заданному в переменной lowChardg
      //Если аккумулятор разрядка
      if (digitalRead(LED_BUILTIN) == HIGH) {
          // если время финиша больше, чем время старта, значит новый отсчёт ещё не начат, начинаем его
          if (milisecForSmartChardgeFinish > milisecForSmartChardgeStart)
              milisecForSmartChardgeStart = millis();

          // Если аккумулятор разрядился до нужного уровня
          if (analogRead(A0) < lowChardg) {
              digitalWrite(LED_BUILTIN, LOW);
          }
      }
      // Блок отклюения зарядки аккумулятора по достижению напряжения их переменной hightChardg
      // и по отключению работы зарядного устройства
      // Если аккумулятор зарядка
      if (digitalRead(LED_BUILTIN) == LOW) {
          // если время финиша меньше, чем время старта, значит очередной отсчёт ещё не закончен, заканчиваем его
          if (milisecForSmartChardgeFinish < milisecForSmartChardgeStart) {
              milisecForSmartChardgeFinish = millis();
              int rezult = (int)((milisecForSmartChardgeFinish - milisecForSmartChardgeStart) / 1000);
              unshift(rezult, mAh, 5);
          }
              
          // Если напряжении на аккумуляторе больше заданного и если процесс зарядки закончился от зарядного
          if ((analogRead(A1) > hightChardg && digitalRead(9)) 
              || (analogRead(A1) > hightChardg && buttonArray[4])) {
            digitalWrite(LED_BUILTIN, HIGH);
          }
      }

      // Если функция вернула true, то эффекта от тренировки больше нет и пора выходить из тренировки
      if (unshift(mAh, 5)) {
          // Выключить кнопки запускающие тренировки
          // При выключенных кнопках реле отключится и подключит зарядное, которое зарядит аккумулятор
          // и само отключится от него.
          unshift(0, mAh, 5);
          if (buttonArray[4]) 
              buttonClosed(4);
          else {
            //   buttonClosed(0);
              buttonClosed(1);
              buttonClosed(2);
              buttonClosed(3);
          }
          
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
// https://youtu.be/RO-C_Q_-JQE?si=1suaRJcM3O2U9Da6
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
    return (int)((flag - intFracPart(flag)) * 1000);
}