#include <Arduino.h>
#include <TM1638plus.h>
#include "returnButtonArray.h"
#include "buttonSearch.h"
#include "requestButtonEnd.h"  // Подключаем заголовочный файл

// Функция сравнивает заданную комбинацию с реально нажатыми и отжатыми кнопками.
bool requestButtonEnd(uint8_t maskForRequest, bool* buttonPushArray, bool* buttonArray)
{
    if (maskForRequest == buttonSearch(returnButtonArray(), buttonPushArray, buttonArray)) 
        return true;
    return false;
}