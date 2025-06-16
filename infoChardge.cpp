#include <Arduino.h>
#include "infoChardge.h"  // Подключаем заголовочный файл
#include "global.h"

// Функция выводит состояние - зарядка - не зарядка в зависимости от пина digitalRead(9)
// То есть информация берется со светодиода зарядного устройства. 
bool infoChardge()
{
    if (digitalRead(9) != HIGH) {
        chardgeRightNow = true;
        return true;
    }
    chardgeRightNow = false;
    return false;
}