
#include <TM1638plus.h> // include the library

#include "welcome.h"
#include "infoChardge.h"
#include "displayTimeFromMillis.h"
#include "buttonSearch.h"
#include "requestButtonEnd.h"

#define  STROBE_TM 10 // strobe = GPIO connected to strobe line of module
#define  CLOCK_TM 11  // clock = GPIO connected to clock line of module
#define  DIO_TM 12 // data = GPIO connected to data line of module
bool high_freq = false; // default false, If using a high freq CPU > ~100 MHZ set to true. 

TM1638plus tm(STROBE_TM, CLOCK_TM , DIO_TM, high_freq);

long tChardge = 36000000;   // длина времени зарядки
long tWork = 3*3600000;     // длина времени разрядки
unsigned long timeAccumulator = 0;   // аккумулятор хранит число милиссекунд от последнего изменения состояния зарядка/разрядка аккумулятора

bool chardgeRightNow = false;        // показывает идёт ли зарядка прямо сейчас
char* outString = "        ";        // Переменная хранит в себе текст, который нужно вывести;

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

    Serial.begin(9600); // скорость передачи, должна совпадать с монитором
    // Serial.println("Привет, монитор!");

    pinMode(LED_BUILTIN, OUTPUT); // Настройка пина светодиода

    pinMode(9, INPUT); // Настройка пина светодиода

    tm.displayBegin(); 

    tm.setLED(0, false);

    welcome(tm, 10, 800);

}

void loop() {
    // outString = "        ";
    treningAKB();

    infoChardge(chardgeRightNow);
    
    buttonSearch(tm, buttonPushArray, buttonArray); // проверяет кнопки и записывает нажатые в переменные

    // Если есть условия то показать время заряда либо разряда
    if (!viewTimeChardge()) {outputMonitor();} //временно закоментировал, чтобы не мешала функция outputMonitor
    // viewTimeChardge();

    // Функция задает время зарядки акб для режима 1
    setTimeChardge();

    timeAccumulator = millis() - milisek; // Аккумулятор времени, для цикла зарядки-разрядки аккумулятора

    // Функция выводит данные в индикатор из глобальной переменной и должна запускаться в конце программы.
    outInformationWithPause();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

// Функция тренирует акб
bool treningAKB()
{
  if (!buttonArray[0]) {
      digitalWrite(LED_BUILTIN, LOW);  // Зарядка акамулятора
      return false;
  }

  if (requestButtonEnd(0b00000001, buttonPushArray, buttonArray)) { // Сюда заходим если нажата только S1
      if (digitalRead(LED_BUILTIN) == HIGH) 
         chardgeRightNow = true;
      if (digitalRead(LED_BUILTIN) == LOW) 
         chardgeRightNow = false;

      // Режим заряд/разряд по времени
      // Здесь только процесс сравнивания с текущим состоянием работы одного из режимов заряд/разряд и если времени прошло больше заданного, то переключаемся
      if (timeAccumulator > tChardge)                     // Если превышено время разрядки
          if (digitalRead(LED_BUILTIN) != HIGH) {      // Если ещё не включили разрядку
              digitalWrite(LED_BUILTIN, HIGH);         // Включить разрядку
              milisek = millis();                      // Обнуление переменной хранящей контрольную точку таймера
              timeAccumulator = 0;                     // Обнулить аккумулятор
          }
      if (timeAccumulator > tWork)                  // Если превышено время зарядки
          if (digitalRead(LED_BUILTIN) != LOW) {       // Если ещё не включена зарядка
              digitalWrite(LED_BUILTIN, LOW);          // Включить зарядку
              milisek = millis();                      // Обнуление переменной хранящей контрольную точку таймера
              timeAccumulator = 0;                     // Обнулить аккумулятор
          }

    
  }
  
  // Режим заряд/разряд по уровням напряжения
  if (requestButtonEnd(0b00001001, buttonPushArray, buttonArray)) {  // Если включена умная тренировка hightChardg
      // Проверить массив, он должен быть заполнен

      int intPart;
      int fracPart;
      

      if (buttonArray[0] && buttonArray[1] && buttonArray[2]) {

        // Блок индикации заряда-разряда. Используется если в смарт тренировке нажаты S2 и S3 
        if (digitalRead(LED_BUILTIN) == HIGH) {
            if (lockFlagDown > analogRead(A0)) lockFlagDown = analogRead(A0);
            float lockFlagDownF = (float)lockFlagDown/1024*5;
            intPart = (int)lockFlagDownF;                         // Целая часть
            fracPart = (int)((lockFlagDownF - intPart) * 1000);    // Дробная часть
            sprintf(outString, "%01d.%03d", intPart, fracPart);
        }
        if (digitalRead(LED_BUILTIN) == LOW) {
          if (lockFlagUp < analogRead(A1)) lockFlagUp = analogRead(A1);
          float lockFlagUpF = (float)lockFlagUp/1024*5;
          intPart = (int)lockFlagUpF;                         // Целая часть
          fracPart = (int)((lockFlagUpF - intPart) * 1000);    // Дробная часть
          sprintf(outString, "%01d.%03d", intPart, fracPart);
        }
      }

      // Если аккумулятор разрядка
    //   if (digitalRead(LED_BUILTIN) == HIGH) {
    //       if (analogRead(A0) < lowChardg) {
    //           digitalWrite(LED_BUILTIN, LOW);
    //           chardgeRightNow = true;
    //           Serial.println("Смарт разрядка");
    //       }
    //   }
    //   // Если аккумулятор зарядка
    //   if (digitalRead(LED_BUILTIN) == LOW) {
    //     Serial.println("Смарт зарядка 1");
    //        if (!chardgeRightNow) {
    //         digitalWrite(LED_BUILTIN, HIGH);
    //         chardgeRightNow = false;
    //         Serial.println("Смарт зарядка 2");
    //        } 
    //   }
  }
}

// функция выводит результат на монитор
bool outputMonitor() 
{
    // Если нажаты кнопки S1, S2, S3, S4 - это значит что идёт смарт зарядка с включенным монитором процессов
    if (buttonArray[0] && buttonArray[1] && buttonArray[2] && buttonArray[3] ) {
        //    && (buttonArray[4] || buttonArray[5] || buttonArray[6] || buttonArray[7])) {
        return false;
    }

    if (requestButtonEnd(0b00000001, buttonPushArray, buttonArray)) {
        if (chardgeRightNow == true) {
            outString = "C-UP    ";
        }
        else {
            outString = "C-DontUP";
        }
    }
    return false;
}

void outInformationWithPause()
{
    tm.displayText(outString);
    delay(100);
}

// Показывает заданные значения заряда-разряда/ S7-показывает время заряда, S8 - разряда
bool viewTimeChardge()
{
    // Показать заданное время разряда
    if (requestButtonEnd(0b10000000, buttonPushArray, buttonArray) || requestButtonEnd(0b10010000, buttonPushArray, buttonArray)  || requestButtonEnd(0b10010010, buttonPushArray, buttonArray)  || requestButtonEnd(0b10010100, buttonPushArray, buttonArray)) {
        displayTimeFromMillis(outString, tWork);
        return true;
    }

    // показать заданное время заряда
    if (requestButtonEnd(0b01000000, buttonPushArray, buttonArray) || requestButtonEnd(0b01010000, buttonPushArray, buttonArray) || requestButtonEnd(0b01010010, buttonPushArray, buttonArray) || requestButtonEnd(0b01010100, buttonPushArray, buttonArray)) {
        displayTimeFromMillis(outString, tChardge);
        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////Ниже отлаженные функции/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Функция опрашивает состояние кнопок и присваивает труе соотвттвующему индексу в массиве buttonArray[]
// Массив buttonPushArray[] служебный, для избавления от эффекта многократного изменения состояния кнопки при удержании ее. 
// Благодаря этому массиву состояние кнопки изменится после отпускания кнопки. То есть одно нажатие - одно изменение состояния кнопки сколько бы не держали кнопку нажатой.
void buttonClosed(uint8_t number)
{
    buttonArray[number] = 0;
    tm.setLED(number, 0);
}

