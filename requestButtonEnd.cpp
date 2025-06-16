#include <Arduino.h>
#include <TM1638plus.h>
#include "buttonSearch.h"
#include "requestButtonEnd.h"  // Подключаем заголовочный файл
#include "global.h"

// Функция сравнивает заданную комбинацию с реально нажатыми и отжатыми кнопками.
bool requestButtonEnd(uint8_t maskForRequest)
{
    if (maskForRequest == buttonSearch()) 
        return true;
    return false;
}