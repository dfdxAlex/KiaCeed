#include <Arduino.h>
#include "infoChardge.h"  // Подключаем заголовочный файл

// Функция выводит состояние - зарядка - не зарядка в зависимости от пина digitalRead(9)
// То есть информация берется со светодиода зарядного устройства. 
bool infoChardge(bool chardgeRightNow)
{
    if (digitalRead(9) != HIGH) {
        chardgeRightNow = true;
        return true;
    }
    chardgeRightNow = false;
    return false;
}