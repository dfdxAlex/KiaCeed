#include <Arduino.h>
#include <TM1638plus.h>
#include "outInformationWithPause.h"  // Подключаем заголовочный файл
#include "global.h"

void outInformationWithPause()
{
    tm.displayText(outString);
    delay(100);
}
