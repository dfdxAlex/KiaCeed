#include <Arduino.h>
#include "outputMonitor.h"  // Подключаем заголовочный файл
#include "requestButtonEnd.h"  // Подключаем заголовочный файл
#include "global.h"

// функция выводит результат на монитор
void outputMonitor() 
{
    if (requestButtonEnd(0b00000001) || requestButtonEnd(0b00001001)) {
        if (digitalRead(LED_BUILTIN) == LOW) {
            testOutInfo = 1;                     // маркер для отладки
            strcpy(outString, "C-UP    ");
        }
        else {
            testOutInfo = 2;                     // маркер для отладки
            strcpy(outString, "C-DontUP");
        }
    }
}
