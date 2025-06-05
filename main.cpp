
#include <TM1638plus.h> // include the library

#define  STROBE_TM 10 // strobe = GPIO connected to strobe line of module
#define  CLOCK_TM 11  // clock = GPIO connected to clock line of module
#define  DIO_TM 12 // data = GPIO connected to data line of module
bool high_freq = false; // default false, If using a high freq CPU > ~100 MHZ set to true. 

TM1638plus tm(STROBE_TM, CLOCK_TM , DIO_TM, high_freq);

unsigned long tChardge = 36000000;   // длина времени зарядки
unsigned long tWork = 3*3600000;     // длина времени разрядки
// unsigned long tChardge = 10000;   // длина времени зарядки
// unsigned long tWork = 5000;       // длина времени разрядки
unsigned long timeAccumulator = 0;   // аккумулятор хранит число милиссекунд от последнего изменения состояния зарядка/разрядка аккумулятора

bool chardgeRightNow = false; // показывает идёт ли зарядка прямо сейчас

// Хранит состояния нажатых кнопок на мониторе
bool buttonArray[8] = {false, false, false, false, false, false, false, false};
// служебный, для устранения изменения состояния кнопки при удержании её в нажатом состоянии
bool buttonPushArray[8] = {false, false, false, false, false, false, false, false};
// хранит время последнего заряда
unsigned long timeLastChardge = 1;
// хранит время последнего разряда
unsigned long timeLastDisChardge = 2;

// Работа с автоматическим контролем разряда
// Массив будет хранить последние данные о времени разряда батареи
int mAh[5] = {0,0,0,0,0};
int lowChardg = 185;            // Аналог 0.9 вольт, допустимая разрядка аккамулятора 0.9/5*1024
int hightChardg = 286;          // Аналог 1.4 вольт, допустимая зарядка аккамулятора 1.4/5*1024
bool statusChardgDisChardg = true; // Состояние зарядки или разрядки. Должен перейти в true если упали ниже 0.9 Вольт или в false если поднялись выше 1.4 Вольт
int lockFlagUp = 0;             // Переменная нужна для организации плавного вывода данных счётчика
int lockFlagDown = 2000;        // Переменная нужна для организации плавного вывода данных счётчика

// Программирование времени зарядки-разрядки
// Переменная хранит шаг изменения при повышении или понижении времени зарядки и разрядки
unsigned long stepIncrementDecrementTimeChardge = 1;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT); // Настройка пина светодиода

    pinMode(9, INPUT); // Настройка пина светодиода

    tm.displayBegin(); 

    tm.setLED(0, false);

    welcome(10, 800);

}

void loop() {
    
    

    unsigned long milisek = millis(); // Запоминает число миллисекунд нп новом цыкле для аккумулятора

    treningAKB();

    infoChardge();
    
    // outputMonitor();

    //uint8_t buttons = tm.readButtons();
    buttonSearch(); // проверяет кнопки и записывает нажатые в переменные

    // Если есть условия то показать время заряда либо разряда
    if (!viewTimeChardge()) outputMonitor();

    // Функция задает время зарядки акб для режима 1
    setTimeChardge();

    delay(10);
    timeAccumulator += millis() - milisek; // Аккумулятор времени, для цикла зарядки-разрядки аккумулятора
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Функция изменяет время зарядки для режима 1
void setTimeChardge()
{
    // Если одна из кнопок не нажата, значит выходим, решим настроек времени заряда или разряда не активирован
    if (buttonArray[7] != true || buttonArray[6] != true) return false;

    // Если отключены обе кнопки прокрутки таймера вверх-вниз отпущены, то обнулить переменную скорости прокрутки
    if (buttonArray[1] == !true && buttonArray[2] == !true) { 
        stepIncrementDecrementTimeChardge = 1000;
    } // Если одна из кнопок включена, то продолжается процесс увеличения скорости прокрутки настроек
    else {
             stepIncrementDecrementTimeChardge+=10;
         }

    // Если нажата кнопка 5(№6) - это означает что активирован режим изменения переменных зарядки по времени
    if (buttonArray[5] == true) {
        // Если включена кнопка 1(№2), то увеличиваем время зарядки на величину шага (скорости) время зарядки плюс шаг должно быть меньше 180000000 миллисекунд
        if (buttonArray[1] == true && (tChardge + stepIncrementDecrementTimeChardge) < 180000000) {
            tChardge+=stepIncrementDecrementTimeChardge;
        }
        // Если нажата кнопка 2(№3), то уменьшаем время зарядки на величину шага (скорости) - время зарядки плюс шаг должно быть больше минуты
        if (buttonArray[2] == true && (tChardge + stepIncrementDecrementTimeChardge) > 60000) {
            tChardge-=stepIncrementDecrementTimeChardge;
        }
    } else  {
                //Если включена кнопка 1(№2), то увеличиваем время разрядки на величину шага (скорости) время разрядки плюс шаг должно быть меньше 180000000 миллисекунд
                if (buttonArray[1] == true && (tWork + stepIncrementDecrementTimeChardge) < 180000000) {
                    tWork+=stepIncrementDecrementTimeChardge;
                }
                // Если нажата кнопка 2(№3), то уменьшаем время разрядки на величину шага (скорости) - время разрядки плюс шаг должно быть больше минуты
                if (buttonArray[2] == true && (tWork + stepIncrementDecrementTimeChardge) > 60000) {
                    tWork-=stepIncrementDecrementTimeChardge;
                }
            }
    delay(10);
}

// Функция выводит состояние - зарядка - не зарядка в зависимости от пина digitalRead(9)
bool infoChardge()
{
    if (digitalRead(9) != HIGH) {
        chardgeRightNow = true;
        return true;
    }
    chardgeRightNow = false;
    return false;
}

// Функция тренирует акб по заранее заданному времени
bool treningAKB()
{
  if (!buttonArray[0]) {
      digitalWrite(LED_BUILTIN, LOW);  // Зарядка акамулятора
      return false;
  }

  // Если работает режим 1, то значения зарядки/разрядки берутся из задания
  timeLastChardge = tChardge;
  timeLastDisChardge = tWork;



  if (!buttonArray[3]) { // Сюда заходим если нажата только S1
      if (digitalRead(LED_BUILTIN) == HIGH) 
         chardgeRightNow = true;
      if (digitalRead(LED_BUILTIN) == LOW) 
         chardgeRightNow = false;

      // Режим заряд/разряд по времени
      if (timeAccumulator > tWork)                     // Если превышено время разрядки
          if (digitalRead(LED_BUILTIN) != HIGH) {      // Если ещё не включили разрядку
              digitalWrite(LED_BUILTIN, HIGH);         // Включить разрядку
              timeAccumulator = 0;                     // Обнулить аккумулятор
          }
      if (timeAccumulator > tChardge)                  // Если превышено время зарядки
          if (digitalRead(LED_BUILTIN) != LOW) {       // Если ещё не включена зарядка
              digitalWrite(LED_BUILTIN, LOW);          // Включить зарядку
              timeAccumulator = 0;                     // Обнулить аккумулятор
          }

    // Режим заряд/разряд по уровням напряжения
  } else if (buttonArray[3]) {  // Если включена умная тренировка hightChardg
      // Проверить массив, он должен быть заполнен

      if (buttonArray[0] && buttonArray[1] && buttonArray[2] && buttonArray[3]) {
        // Блок индикации заряда-разряда. Используется если в смарт тренировке нажаты S2 и S3 
        if (digitalRead(LED_BUILTIN) == HIGH) {
          if (lockFlagDown > analogRead(A0)) lockFlagDown = analogRead(A0);
          tm.displayIntNum(lockFlagDown, true, 4);
        }
        if (digitalRead(LED_BUILTIN) == LOW) {
          if (lockFlagUp < analogRead(A1)) lockFlagUp = analogRead(A1);
          tm.displayIntNum(lockFlagUp, true, 4);
        }
          delay(10);
      }

      // Если аккумулятор разрядка
      if (digitalRead(LED_BUILTIN) == HIGH) {
          if (analogRead(A0) < lowChardg) {
              digitalWrite(LED_BUILTIN, LOW);
              chardgeRightNow = true;
              delay(100);
          }
      }
      // Если аккумулятор зарядка
      if (digitalRead(LED_BUILTIN) == LOW) {
           if (!chardgeRightNow) {
            digitalWrite(LED_BUILTIN, HIGH);
           } 
      }
  }
}

// функция выводит результат на монитор
bool outputMonitor() 
{
    // Если нажаты кнопки S1, S2, S3, S4 - это значит что идёт смарт зарядка с включенным монитором процессов
    if (buttonArray[0] && buttonArray[1] && buttonArray[2] && buttonArray[3]) {
        return false;
    }

    if (buttonArray[5] && !buttonArray[6] && !buttonArray[7]) {
        tm.displayText("-SETUP- ");
        return false;
    }

    // if (buttonArray[5] || buttonArray[6] || buttonArray[7]) {
    if (buttonArray[0]) {
        if (chardgeRightNow == true)
            tm.displayText("C-UP    ");
        else 
            tm.displayText("C-DontUP");
        }

    return false;
}

// Показывает заданные значения заряда-разряда
bool viewTimeChardge()
{
    // Показать заданное время разряда
    if (buttonArray[7] == true && buttonArray[6] == true  && buttonArray[5] != true) {
        displayTimeFromMillis(timeLastDisChardge);
        return true;
    }

    // показать заданное время заряда
    if (buttonArray[7] && buttonArray[6]  && buttonArray[5]) {
        displayTimeFromMillis(timeLastChardge);
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
void buttonSearch()
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
    } // end for
} // end function

// uint8_t bubuttonSearchInt()
// {
//     return false;
// }

// функция переводит секунды в нормальный вид: часы:минуты
// https://github.com/dfdxAlex/KiaCeed.git
void displayTimeFromMillis(unsigned long ms) {
  unsigned long totalSeconds = ms / 1000;       // Перевести миллисекунды в секунды
  uint8_t seconds = totalSeconds % 60;          // Посчитать число секунд
  uint8_t minutes = (totalSeconds / 60) % 60;   // Посчитать число минут
  uint16_t hours   = (totalSeconds / 3600);     // Посчитать число часов

  // Ограничим отображение максимумом 2 цифр:
  if (hours > 99) hours = 99;                   // Максимальное число часов 99

  tm.displayASCII(0, '0' + hours / 10);         // Функция требует символ для вывода в нулевую позицию. Нужно вывести цифру. Берем символ 0 равный 48 и к нему добавляем число часов, так получаем символ нужной цифры
  tm.displayASCIIwDot(1, '0' + hours % 10);     // То же самое что и выше но с точкой
  tm.displayASCII(2, '0' + minutes / 10);       // Описание вначале этого блока
  tm.displayASCIIwDot(3, '0' + minutes % 10);   // То же самое что и выше но с точкой
  tm.displayASCII(4, '0' + seconds / 10);       // Описание вначале этого блока
  tm.displayASCII(5, '0' + seconds % 10);       // Описание вначале этого блока
  tm.displayASCII(6, ' ');                      // Пустой символ
  tm.displayASCII(7, ' ');                      // Пустой символ
}

// Функция запускается при старте чтобы показать модификацию программы и нагнать понтов.
void welcome(char tik, int propertyForDelay)
{
        for (char i=2; i<tik; i++) {
        if (i % 2 == 0)
            tm.displayText("V-1.1   ");
        else
            tm.displayText("READY   ");
        delay(propertyForDelay);
    }
}