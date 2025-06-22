#include <Arduino.h>
#include <TM1638plus.h>
#include "welcome.h"  // Подключаем заголовочный файл
#include "global.h"

// Функция запускается при старте чтобы показать модификацию программы и нагнать понтов.
void welcome()
{
        for (char i=2; i<10; i++) {
        if (i % 2 == 0)
            tm.displayText("V-2.2   ");
        else
            tm.displayText("READY   ");
        delay(800);
    }
}
