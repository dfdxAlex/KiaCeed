#include <Arduino.h>
#include <TM1638plus.h>
#include "welcome.h"  // Подключаем заголовочный файл

// Функция запускается при старте чтобы показать модификацию программы и нагнать понтов.
void welcome(TM1638plus &tm, char tik, int propertyForDelay)
{
        for (char i=2; i<tik; i++) {
        if (i % 2 == 0)
            tm.displayText("V-1.6   ");
        else
            tm.displayText("READY   ");
        delay(propertyForDelay);
    }
}