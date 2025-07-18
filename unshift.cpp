#include <Arduino.h>
#include "unshift.h"  // Подключаем заголовочный файл
#include "global.h"

// Функция сдвигает все элементы массива вправо и добавляет один элемент в начало массива
void unshift(int el, int* array, char size)
{
    for (char i = size - 1; i > 0; --i) {
        array[i] = array[i-1];
    }

    array[0] = el;
}

// Функция проверяет растёт ли время разряда хотябы на 5 процентов за время наполнения массива
// Также массив проверяется на заполнение
// Если функция возвращает true, тренировку можно заканчивать
bool unshift(int* array, int size)
{
    if (ticChargeZero > 20) {
        ticChargeZero = 0;
        isBatteryBad = true;
        return true;
    }

    for (char i = 0; i < size; i++) {
        if (array[i] == 0) return false;
    }

    if ((array[0]*100/array[4])-100 > 5) return false;
    
    return true;
}

// Функция без параметров обнуляет массив
void unshift()
{
    for (char i = 0; i < 5; i++) {
        mAh[i] = 0;
    }
}


long unshift(int* array)
{
    long rezult = 0;
    for (char i = 0; i < 5; i++) {
        rezult += mAh[i];
    }
    return (long)rezult;
}
