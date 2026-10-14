#include <Arduino.h>
#include "PullupBlinkerImproved.h"

namespace PullupBlinkerImproved
{
    constexpr int LED1_PIN = 15;
    constexpr int LED2_PIN = 16;
    constexpr int EXTERNAL_BUTTON_PIN = 21;
    constexpr int BOOT_BUTTON_PIN = 0;

    constexpr unsigned long FAST_INTERVAL = 200;
    constexpr unsigned long SLOW_INTERVAL = 1000;
    constexpr unsigned long DEBOUNCE_DELAY = 30;

    int mode = 1;
    bool ledState = false;
    unsigned long lastUpdate = 0;

    // Button state tracking
    struct Button
    {
        int pin;
        bool lastReading = HIGH;
        bool stableState = HIGH;
        unsigned long lastChange = 0;

        Button(int buttonPin) : pin(buttonPin) {}
    };

    Button externalButton{EXTERNAL_BUTTON_PIN};
    Button bootButton{BOOT_BUTTON_PIN};

    // Returns true only when a valid button press is detected
    bool wasPressed(Button& button)
    {
        bool reading = digitalRead(button.pin);

        if (reading != button.lastReading)
        {
            button.lastChange = millis();
        }

        button.lastReading = reading;

        if (millis() - button.lastChange >= DEBOUNCE_DELAY)
        {
            if (reading != button.stableState)
            {
                button.stableState = reading;

                if (reading == LOW)
                {
                    return true;
                }
            }
        }

        return false;
    }

    void setup()
    {
        pinMode(LED1_PIN, OUTPUT);
        pinMode(LED2_PIN, OUTPUT);

        pinMode(EXTERNAL_BUTTON_PIN, INPUT_PULLUP);
        pinMode(BOOT_BUTTON_PIN, INPUT);
    }

    void loop()
    {
        // Detect debounced button presses
        bool externalPressed = wasPressed(externalButton);
        bool bootPressed = wasPressed(bootButton);

        if (externalPressed)
        {
            mode = 1;
            ledState = false;
            lastUpdate = millis();
        }
        else if (bootPressed)
        {
            mode = 2;
            ledState = false;
            lastUpdate = millis();
        }

        // Blinking logic
        unsigned long interval =
            (mode == 1) ? FAST_INTERVAL : SLOW_INTERVAL;

        if (millis() - lastUpdate >= interval)
        {
            lastUpdate = millis();
            ledState = !ledState;
        }

        if (mode == 1)
        {
            digitalWrite(LED1_PIN, ledState ? HIGH : LOW);
            digitalWrite(LED2_PIN, ledState ? HIGH : LOW);
        }
        else
        {
            digitalWrite(LED1_PIN, ledState ? HIGH : LOW);
            digitalWrite(LED2_PIN, ledState ? LOW : HIGH);
        }
    }
}