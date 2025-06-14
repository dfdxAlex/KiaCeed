#ifndef REQUESTBUTTONEND_H  // Защита от повторного включения
#define REQUESTBUTTONEND_H

// Описание функции
bool requestButtonEnd(uint8_t maskForRequest, TM1638plus tm, bool* buttonPushArray, bool* buttonArray, uint8_t (*buttonSearch)(TM1638plus, bool*, bool*));

#endif