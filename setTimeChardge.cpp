#include <Arduino.h>
#include <TM1638plus.h>
#include "setTimeChardge.h"  // Подключаем заголовочный файл
#include "global.h"
#include "buttonClosed.h"

// Функция изменяет время зарядки для режима 1
void setTimeChardge()
{
    // Если одна из кнопок не нажата, значит выходим, решим настроек времени заряда или разряда не активирован
    // Если не нажата S5, то выходим
    if (!buttonArray[4]) return false;

    // Если отключены обе кнопки прокрутки таймера вверх-вниз отпущены, то обнулить переменную скорости прокрутки
    if (!buttonArray[1] && !buttonArray[2]) { 
        stepIncrementDecrementTimeChardge = 1000;
    } // Если одна из кнопок включена, то продолжается процесс увеличения скорости прокрутки настроек
    else {
             stepIncrementDecrementTimeChardge+=150;
         }

    // Если нажата кнопка 6(№7) - это означает что активирован режим изменения переменных зарядки по времени
    if (buttonArray[6]) { 
        // Если включена кнопка 1(№2), то увеличиваем время зарядки на величину шага (скорости) время зарядки плюс шаг должно быть меньше 180000000 миллисекунд
        if (buttonArray[1] && (tChardge + stepIncrementDecrementTimeChardge < 180000000)) {
            tChardge+=stepIncrementDecrementTimeChardge;
        }
        // Если нажата кнопка 2(№3), то уменьшаем время зарядки на величину шага (скорости) - время зарядки плюс шаг должно быть больше минуты
        if (buttonArray[2]) {
            if ((tChardge-stepIncrementDecrementTimeChardge) <= 60000) {
                tChardge = 60000;
                buttonClosed(2);
            } else {
                tChardge-=stepIncrementDecrementTimeChardge;
            }
        }
    } else if (buttonArray[7]) {
                //Если включена кнопка 1(№2), то увеличиваем время разрядки на величину шага (скорости) время разрядки плюс шаг должно быть меньше 180000000 миллисекунд
                if (buttonArray[1] && (tWork + stepIncrementDecrementTimeChardge < 180000000)) {
                    tWork+=stepIncrementDecrementTimeChardge;
                }
                // Если нажата кнопка 2(№3), то уменьшаем время разрядки на величину шага (скорости) - время разрядки плюс шаг должно быть больше минуты
                    if (buttonArray[2]) {
                        if (tWork-stepIncrementDecrementTimeChardge <= 60000) {
                        tWork = 60000;
                        buttonClosed(2);
                    } else {
                        tWork-=stepIncrementDecrementTimeChardge;
                    }
                }
            }
}
