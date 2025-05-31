
#include <TM1638plus.h> // include the library

#define  STROBE_TM 10 // strobe = GPIO connected to strobe line of module
#define  CLOCK_TM 11  // clock = GPIO connected to clock line of module
#define  DIO_TM 12 // data = GPIO connected to data line of module
bool high_freq = false; // default false, If using a high freq CPU > ~100 MHZ set to true. 

TM1638plus tm(STROBE_TM, CLOCK_TM , DIO_TM, high_freq);

unsigned long tChardge = 36000000; // длина времени зарядки
unsigned long tWork = 3*3600000;     // длина времени разрядки
// unsigned long tChardge = 10000; // длина времени зарядки
// unsigned long tWork = 5000;     // длина времени разрядки
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
}

void loop() {
    
    unsigned long milisek = millis(); // Запоминает число миллисекунд нп новом цыкле для аккумулятора

    treningAKB();

    infoChardge();
    
    // outputMonitor();

    uint8_t buttons = tm.readButtons();
    buttonSearch(buttons); // проверяет кнопки и записывает нажатые в переменные

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
    if (buttonArray[7] != true || buttonArray[6] != true) return false;

    // Если обе кнопки вверх таймер и вних таймер выключены, то обнулить шаг декремента и инкремента
    if (buttonArray[1] == !true && buttonArray[2] == !true) stepIncrementDecrementTimeChardge = 1000;
    else stepIncrementDecrementTimeChardge+=10;
    if (buttonArray[5] == true) {
        if (buttonArray[1] == true && (tChardge + stepIncrementDecrementTimeChardge) < 180000000) {
            tChardge+=stepIncrementDecrementTimeChardge;
        }
        if (buttonArray[2] == true && (tChardge + stepIncrementDecrementTimeChardge) > 60000) {
            tChardge-=stepIncrementDecrementTimeChardge;
        }
    }

    if (buttonArray[5] != true) {
        if (buttonArray[1] == true && (tWork + + stepIncrementDecrementTimeChardge) < 180000000) {
            tWork+=stepIncrementDecrementTimeChardge;
        }
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

      if (timeAccumulator > tWork)          // Если превышено время разрядки
          if (digitalRead(LED_BUILTIN) != HIGH) {
              digitalWrite(LED_BUILTIN, HIGH); // Разрядка акамулятора
              timeAccumulator = 0;             // Если перешли в режим зарядки, то обновить аккумулятор
          }
      if (timeAccumulator > tChardge)          // Если превышено время зарядки
          if (digitalRead(LED_BUILTIN) != LOW) {
              digitalWrite(LED_BUILTIN, LOW);  // Зарядка акамулятора
              timeAccumulator = 0;             // Если перешли в режим разрядки, то обновить аккумулятор
          }
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

    if (chardgeRightNow == true)
        tm.displayText("C-UP    ");
    else 
        tm.displayText("C-DontUP");

    return false;
}

// Показывает значения времени последнего заряда-разряда
bool viewTimeChardge()
{
    // Показать заданное время разряда и задать режим программирования этого времени
    if (buttonArray[7] == true && buttonArray[6] == true  && buttonArray[5] != true) {
        displayTimeFromMillis(timeLastDisChardge);
        return true;
    }
    // Показать заданное время заряда и задать режим программирования этого времени
    if (buttonArray[7] == true && buttonArray[6] == true  && buttonArray[5] == true) {
        displayTimeFromMillis(timeLastChardge);
        return true;
    }

    // показать заданное время заряда
    if (buttonArray[7] == false && buttonArray[6] == true) {
        displayTimeFromMillis(timeLastChardge);
        return true;
    }

    // показать заданное время разряда
    if (buttonArray[6] == false && buttonArray[7] == true) {
        displayTimeFromMillis(timeLastDisChardge);
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
void buttonSearch(uint8_t buttons)
{
    for (char i = 0; i < 8; i++) {
        if (!buttonPushArray[i] && (buttons & (1 << i)) ) {
                buttonArray[i] = !buttonArray[i];
                tm.setLED(i, buttonArray[i]);
                buttonPushArray[i] = true;
        } //end if (!buttonPushArray[i])
        // Здесь проверить отпущена ли кнопка, которая проверяется на текущей итерации цикла. 
        // button - это маска показывающая какие кнопки включены а какие выключены.
        if (!(buttons & (1 << i))) {
            buttonPushArray[i] = false;
        }
    } // end for
} // end function

// функция переводит секунды в нормальный вид: часы:минуты
void displayTimeFromMillis(unsigned long ms) {
  unsigned long totalSeconds = ms / 1000;
  uint8_t seconds = totalSeconds % 60;
  uint8_t minutes = (totalSeconds / 60) % 60;
  uint16_t hours   = (totalSeconds / 3600);  // теперь до 65535 часов (если нужно)

  // Ограничим отображение максимумом 2 цифр:
  if (hours > 99) hours = 99;

  tm.displayASCII(0, '0' + hours / 10);
  tm.displayASCIIwDot(1, '0' + hours % 10);
  tm.displayASCII(2, '0' + minutes / 10);
  tm.displayASCIIwDot(3, '0' + minutes % 10);
  tm.displayASCII(4, '0' + seconds / 10);
  tm.displayASCII(5, '0' + seconds % 10);
  tm.displayASCII(6, ' ');
  tm.displayASCII(7, ' ');
}
