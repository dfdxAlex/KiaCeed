#include <Arduino.h>
#include <TM1638plus.h>
#include "listingmAh.h"  // Подключаем заголовочный файл
#include "global.h"
#include "requestButtonEnd.h"
#include "buttonClosed.h"
#include "displayTimeFromMillis.h"
#include "outInformationWithPause.h"

// Функция выводит в цикле содержимое массива с миллиамперами разряда
void listingmAh()
{
    if (viewArray()) return;

    char localOutString[9];
    for (char i=0; i<5; i++) {
        sprintf(localOutString, "%06d.%01s%01d", mAh[i], " ", i);
        tm.displayText(localOutString);
        delay(900);
    }

    buttonClosed(7);
}

bool openSmartTrening()
{
  // Если включена умная тренировка hightChardg
  if (requestButtonEnd(0b00001001)             // Включена просто тренировка без индикации
     || requestButtonEnd(0b00001111)           // Включена тренировка с индикации в вольтах
        || requestButtonEnd(0b00001011)        // Включена тренировка с индикацией в цифрах 0-1023
           || requestButtonEnd(0b00001101)     // Включена тренировка с индикацией цели в цифрах 0-1023
              || requestButtonEnd(0b00011101)
                  || requestButtonEnd(0b00011011)
                      || requestButtonEnd(0b00011001)
                          || requestButtonEnd(0b00011111)) 
                             return true;
  return false;
}

bool viewArray()
{
    if (!requestButtonEnd(0b10001001) 
        && !requestButtonEnd(0b10001011)
            && !requestButtonEnd(0b10001101)
                && !requestButtonEnd(0b10001111)
                    && !requestButtonEnd(0b10011001) 
                        && !requestButtonEnd(0b10011011)
                            && !requestButtonEnd(0b10011101)
                                && !requestButtonEnd(0b10011111)) return true;
    return false;
}