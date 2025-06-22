#include <TM1638plus.h> // include the library
#include "welcome.h"
#include "displayTimeFromMillis.h"
#include "buttonSearch.h"
#include "requestButtonEnd.h"
#include "global.h"
#include "buttonClosed.h"
#include "setTimeChardge.h"
#include "viewTimeChardge.h"
#include "outInformationWithPause.h"
#include "outputMonitor.h"
#include "treningAKB.h"

TM1638plus tm(10, 11 , 12, false);

// Переменная будет содержать в сете номер функции, которая изменила переменную outString
// Переменная для отладки, после отладки можно закоментировать её
int testOutInfo = 0;

long tChardge = 36000000;   // длина времени зарядки
long tWork = 3*3600000;     // длина времени разрядки
unsigned long timeAccumulator = 0;   // аккумулятор хранит число милиссекунд от последнего изменения состояния зарядка/разрядка аккумулятора

char outString[9];        // Переменная хранит в себе текст, который нужно вывести;

// Хранит состояния нажатых кнопок на мониторе
bool buttonArray[8] = {false, false, false, false, false, false, false, false};
// служебный, для устранения изменения состояния кнопки при удержании её в нажатом состоянии
bool buttonPushArray[8] = {false, false, false, false, false, false, false, false};

unsigned long milisek = millis(); // Запоминает число миллисекунд нп новом цыкле для аккумулятора

// хранит время последнего заряда
unsigned long timeLastChardge = 1;
// хранит время последнего разряда
unsigned long timeLastDisChardge = 2;

// Работа с автоматическим контролем разряда
// Массив будет хранить последние данные о времени разряда батареи
int mAh[5] = {0,0,0,0,0};
int lowChardg = 185;               // Аналог 0.9 вольт, допустимая разрядка аккамулятора 0.9/5*1024
int hightChardg = 286;             // Аналог 1.4 вольт, допустимая зарядка аккамулятора 1.4/5*1024
bool statusChardgDisChardg = true; // Состояние зарядки или разрядки. Должен перейти в true если упали ниже 0.9 Вольт или в false если поднялись выше 1.4 Вольт
int lockFlagUp = 0;                // Переменная нужна для организации плавного вывода данных счётчика
int lockFlagDown = 2000;           // Переменная нужна для организации плавного вывода данных счётчика

// Программирование времени зарядки-разрядки
// Переменная хранит шаг изменения при повышении или понижении времени зарядки и разрядки
long stepIncrementDecrementTimeChardge = 1;

void setup() {

    // Serial.begin(9600); // скорость передачи, должна совпадать с монитором
    // Serial.println("Привет, монитор!");

    pinMode(LED_BUILTIN, OUTPUT); // Настройка пина светодиода

    pinMode(9, INPUT); // Настройка пина светодиода

    tm.displayBegin(); 

    tm.setLED(0, false);

    welcome();

}

void loop() {
    testOutInfo = 0;
    strcpy(outString, "        ");

    // Функция выводит C_UP или CdontUp
    outputMonitor();

    treningAKB();

    buttonSearch(); // проверяет кнопки и записывает нажатые в переменные

    // Если есть условия то показать заданное время заряда либо разряда
    viewTimeChardge();
    // Функция задает время зарядки акб для режима 1 (ручной режим)
    setTimeChardge();

    timeAccumulator = millis() - milisek; // Аккумулятор времени, для цикла зарядки-разрядки аккумулятора

    // Serial.println(testOutInfo);
    // Функция выводит данные в индикатор из глобальной переменной и должна запускаться в конце программы.
    outInformationWithPause();
}