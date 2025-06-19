#include <Arduino.h>
// #include <TM1638plus.h>
#include "requestButtonEnd.h"
#include "viewTimeChardge.h"  // Подключаем заголовочный файл
#include "displayTimeFromMillis.h"  // Подключаем заголовочный файл
#include "global.h"

// Показывает заданные значения заряда-разряда/ S7-показывает время заряда, S8 - разряда
bool viewTimeChardge()
{
    // Показать заданное время разряда
    if (requestButtonEnd(0b10000000) || requestButtonEnd(0b10010000)  || requestButtonEnd(0b10010010)  || requestButtonEnd(0b10010100)) {
        displayTimeFromMillis(tWork);
        return true;
    }

    // показать заданное время заряда
    if (requestButtonEnd(0b01000000) || requestButtonEnd(0b01010000) || requestButtonEnd(0b01010010) || requestButtonEnd(0b01010100)) {
        displayTimeFromMillis(tChardge);
        return true;
    }

    return false;
}
