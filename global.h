// Хранит состояния нажатых кнопок на мониторе
extern bool buttonArray[8];
// служебный, для устранения изменения состояния кнопки при удержании её в нажатом состоянии
extern bool buttonPushArray[8];

// переменная хранит в себе информацию, которую нужно будет в конце скетча вывести на екран
extern char outString[9];

// Программирование времени зарядки-разрядки
// Переменная хранит шаг изменения при повышении или понижении времени зарядки и разрядки
extern long stepIncrementDecrementTimeChardge;

extern long tChardge;   // длина времени зарядки
extern long tWork;     // длина времени разрядки

#include <TM1638plus.h>  // обязательно, чтобы тип был известен
extern TM1638plus tm;

extern unsigned long timeAccumulator;

extern unsigned long milisek;

extern int lockFlagDown;

extern int lockFlagUp;

extern int testOutInfo;

extern int lowChardg;
extern int hightChardg;   

extern int mAh[5];
extern char ticChargeZero;
extern unsigned long milisecForSmartChardgeStart;
extern unsigned long milisecForSmartChardgeFinish;

extern unsigned long propertyForDischargeTime;
extern bool isBatteryBad;