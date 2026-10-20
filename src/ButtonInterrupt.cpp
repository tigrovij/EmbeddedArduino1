#include <Arduino.h>
#include "ButtonInterrupt.h"

namespace ButtonInterrupt
{
    #define BUTTON_IN 16  // Кнопка підключена до GPIO 16

    constexpr uint8_t ISR_MONITOR_PIN = 17;  // D1 аналізатора: кожен перехід = один виклик ISR
    uint8_t isrMonitorState = LOW;  // Після setup() змінюється лише в ISR

    volatile uint32_t button_counter = 0;  // Змінюється всередині ISR
    uint32_t last_count = 0;  // Відстежує зміни у головному циклі

    // Обробник переривання розміщується в оперативній пам'яті (IRAM).
    void IRAM_ATTR button_isr()
    {
        button_counter++;

        // Перемикаємо рівень: рахуємо обидва типи фронтів на цьому виході.
        isrMonitorState ^= 1U;
        digitalWrite(ISR_MONITOR_PIN, isrMonitorState);
    }

    void setup()
    {
        // Ініціалізуємо діагностичний вихід до підключення переривання.
        pinMode(ISR_MONITOR_PIN, OUTPUT);
        digitalWrite(ISR_MONITOR_PIN, LOW);

        pinMode(BUTTON_IN, INPUT);
        Serial.begin(115200);

        // FALLING: натискання кнопки, перехід з 1 у 0.
        attachInterrupt(digitalPinToInterrupt(BUTTON_IN), button_isr, FALLING);
    }

    void loop()
    {
        if (button_counter != last_count)
        {
            last_count = button_counter;
            Serial.print("Button Pressed! Count: ");
            Serial.println(last_count);
        }

        delay(10);
    }
}
