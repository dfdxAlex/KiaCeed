#include <Arduino.h>
#include <TM1638plus.h>
#include "calculateDischargeTime.h"  // Подключаем заголовочный файл
#include "global.h"

void calculateDischargeTime(bool propertiForSignature)
{
    propertyForDischargeTime = millis();
}

unsigned long calculateDischargeTime()
{
    unsigned long rezult = millis() - propertyForDischargeTime;
    if (rezult < 2000) ticChargeZero++;
    return rezult;
}
