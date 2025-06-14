#include <Arduino.h>
#include <TM1638plus.h>
#include "buttonSearch.h"  // Подключаем заголовочный файл

uint8_t buttonSearch(TM1638plus tm, bool* buttonPushArray, bool* buttonArray)
{
    uint8_t buttons = tm.readButtons();
    for (char i = 0; i < 8; i++) {
        if (!buttonPushArray[i] && (buttons & (1 << i)) ) {
                buttonArray[i] = !buttonArray[i];
                tm.setLED(i, buttonArray[i]);
                buttonPushArray[i] = true;
        }
        // Здесь проверить отпущена ли кнопка, которая проверяется на текущей итерации цикла. 
        // button - это маска показывающая какие кнопки включены а какие выключены.
        if (!(buttons & (1 << i))) {
            buttonPushArray[i] = false;
        }
    } 

    // Создать маску из массива с данными об включенных и отключенных кнопках.
    return (uint8_t(buttonArray[0]) << 0) |
           (uint8_t(buttonArray[1]) << 1) |
           (uint8_t(buttonArray[2]) << 2) |
           (uint8_t(buttonArray[3]) << 3) |
           (uint8_t(buttonArray[4]) << 4) |
           (uint8_t(buttonArray[5]) << 5) |
           (uint8_t(buttonArray[6]) << 6) |
           (uint8_t(buttonArray[7]) << 7);
}