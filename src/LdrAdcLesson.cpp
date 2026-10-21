#include <Arduino.h>
#include <math.h>
#include "LdrAdcLesson.h"

namespace LdrAdcLesson
{    
    constexpr uint8_t LDR_PIN = 4;
    constexpr uint32_t SAMPLE_INTERVAL_MS = 100;
    constexpr uint16_t ADC_MAX = 4095;
    constexpr float U_REF_MV = 3100.0f;

    uint32_t lastSampleMs = 0;

    void setup()
    {
        Serial.begin(115200);
        pinMode(LDR_PIN, INPUT);  // Без підтяжки: напругу задає подільник

        lastSampleMs = millis();
    }

    void loop()
    {        
        const uint32_t nowMs = millis();
        if (nowMs - lastSampleMs < SAMPLE_INTERVAL_MS)
        {
            return;
        }
        lastSampleMs = nowMs;

        const uint16_t raw = analogRead(LDR_PIN);
        const float calculatedMv = raw * U_REF_MV / ADC_MAX;
        const uint32_t measuredMv = analogReadMilliVolts(LDR_PIN);

        // Повторюємо заголовок перед кожною групою з 10 вимірювань.
        static uint8_t rowsPrinted = 0;
        if (rowsPrinted == 0)
        {
            Serial.println("RAW\tU_calc (mV)\tU_ADC (mV)\tError (%)");
        }

        Serial.printf("%u\t%.2f\t\t%lu\t\t",
                      static_cast<unsigned>(raw), calculatedMv,
                      static_cast<unsigned long>(measuredMv));

        if (measuredMv > 0)
        {           
            const float errorPercent =
                fabsf(calculatedMv - measuredMv) / measuredMv * 100.0f;
            Serial.printf("%.2f\n", errorPercent);
        }
        else
        {
            Serial.println("N/A");
        }

        ++rowsPrinted;
        if (rowsPrinted == 10)
        {
            rowsPrinted = 0;
        }
    }
}
