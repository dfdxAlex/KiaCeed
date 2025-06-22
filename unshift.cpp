#include <Arduino.h>
#include "unshift.h"  // Подключаем заголовочный файл

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
    for (char i = 0; i < size; i++) {
        if (array[i] == 0) return false;
    }

    if ((array[4]*100/array[0])-100 > 5) return false;

    return true;
}
