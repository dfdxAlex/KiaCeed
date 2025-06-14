#include <Arduino.h>
#include <TM1638plus.h>
#include "requestButtonEnd.h"  // Подключаем заголовочный файл

// Функция сравнивает заданную комбинацию с реально нажатыми и отжатыми кнопками.
bool requestButtonEnd(uint8_t maskForRequest, TM1638plus tm, bool* buttonPushArray, bool* buttonArray, uint8_t (*buttonSearch)(TM1638plus, bool*, bool*))
{
    if (maskForRequest == buttonSearch(tm, buttonPushArray, buttonArray)) 
        return true;
    return false;
}