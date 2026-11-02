#include <Arduino.h>
#include "SmartStreetLight.h"

namespace SmartStreetLight
{
    constexpr int LDR_PIN = 4;
    constexpr int LED_PIN = 7;
    constexpr int BUTTON_PIN = 21;

    constexpr int PWM_CHANNEL = 0;
    constexpr int PWM_FREQUENCY = 5000;
    constexpr int PWM_RESOLUTION = 8;
    constexpr int DIM_BRIGHTNESS = 26; // about 10% of 255
    constexpr int FULL_BRIGHTNESS = 255;
    
    constexpr int DARK_THRESHOLD = 1600;
    constexpr int LIGHT_THRESHOLD = 2200;
    constexpr unsigned long SENSOR_INTERVAL = 50;
    constexpr unsigned long DEBOUNCE_DELAY = 30;
    constexpr unsigned long REPORT_INTERVAL = 500;

    bool isDark = false;
    int lightLevel = 0;
    int lastButtonReading = HIGH;
    int stableButton = HIGH;
    unsigned long lastButtonChange = 0;
    unsigned long lastSensorRead = 0;
    unsigned long lastReport = 0;

    void setup()
    {
        Serial.begin(115200);
        pinMode(LDR_PIN, INPUT);
        pinMode(BUTTON_PIN, INPUT_PULLUP);

        ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
        ledcAttachPin(LED_PIN, PWM_CHANNEL);
        ledcWrite(PWM_CHANNEL, 0);

        lightLevel = analogRead(LDR_PIN);
        isDark = lightLevel < DARK_THRESHOLD;
        Serial.println("Smart street light: GPIO4=LDR, GPIO7=LED, GPIO21=button");
    }

    void loop()
    {
        const unsigned long now = millis();

        const int buttonReading = digitalRead(BUTTON_PIN);
        if (buttonReading != lastButtonReading)
        {
            lastButtonChange = now;
            lastButtonReading = buttonReading;
        }
        if (now - lastButtonChange >= DEBOUNCE_DELAY)
        {
            stableButton = buttonReading;
        }

        if (now - lastSensorRead >= SENSOR_INTERVAL)
        {
            lastSensorRead = now;
            lightLevel = analogRead(LDR_PIN);

            if (lightLevel < DARK_THRESHOLD)
            {
                isDark = true;
            }
            else if (lightLevel > LIGHT_THRESHOLD)
            {
                isDark = false;
            }
        }

        int brightness = 0;
        if (isDark)
        {
            brightness = (stableButton == LOW) ? FULL_BRIGHTNESS : DIM_BRIGHTNESS;
        }
        ledcWrite(PWM_CHANNEL, brightness);

        if (now - lastReport >= REPORT_INTERVAL)
        {
            lastReport = now;
            Serial.printf("ADC: %d | Dark: %d | Button: %d | PWM: %d\n",
                          lightLevel, isDark, stableButton == LOW, brightness);
        }
    }
}
