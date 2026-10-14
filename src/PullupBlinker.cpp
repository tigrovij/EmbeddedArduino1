#include <Arduino.h>
#include "PullupBlinker.h"

namespace PullupBlinker
{
    constexpr int LED1_PIN = 15;
    constexpr int LED2_PIN = 16;
    constexpr int EXTERNAL_BUTTON_PIN = 21;
    constexpr int BOOT_BUTTON_PIN = 0;

    int mode = 1;

    void setup()
    {
        pinMode(LED1_PIN, OUTPUT);
        pinMode(LED2_PIN, OUTPUT);

        pinMode(EXTERNAL_BUTTON_PIN, INPUT_PULLUP);
        pinMode(BOOT_BUTTON_PIN, INPUT);
    }

    void loop()
    {
        if (digitalRead(EXTERNAL_BUTTON_PIN) == LOW)
        {
            mode = 1;
        }

        if (digitalRead(BOOT_BUTTON_PIN) == LOW)
        {
            mode = 2;
        }

        if (mode == 1)
        {
            digitalWrite(LED1_PIN, HIGH);
            digitalWrite(LED2_PIN, HIGH);
            delay(200);

            digitalWrite(LED1_PIN, LOW);
            digitalWrite(LED2_PIN, LOW);
            delay(200);
        }
        else
        {
            digitalWrite(LED1_PIN, HIGH);
            digitalWrite(LED2_PIN, LOW);
            delay(1000);

            digitalWrite(LED1_PIN, LOW);
            digitalWrite(LED2_PIN, HIGH);
            delay(1000);
        }
    }
}