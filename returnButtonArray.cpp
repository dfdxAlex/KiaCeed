// #include <Arduino.h>
#include <TM1638plus.h>
#include "returnButtonArray.h"  // Подключаем заголовочный файл

// Функция запускается при старте чтобы показать модификацию программы и нагнать понтов.
TM1638plus returnButtonArray()
{
    TM1638plus tm(10, 11, 12, false);
    return tm;
}
