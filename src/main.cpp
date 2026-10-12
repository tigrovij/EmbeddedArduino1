#include <Arduino.h>

#define BLUE_LED_PIN 4
#define RED_LED_PIN 2

// Змінні для блимавки зі змінною швидкістю
int blinkDelay = 500;
unsigned long lastSpeedChange = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(BLUE_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);

    Serial.println("Program started");
}

// Зменшення кількості пінів: замість підключення BLUE_LED_PIN і RED_LED_PIN до GND, можна підключити їх одне до одного,
// під'єднавши діоди паралельно в різному напрямку. Таким чином, коли струм тече в один бік, то горить один діод,
// в зворотному напрямку - інший. Той самий код працює при обох конфігураціях ланцюга.
void police_blinker(int blinkDelay)
{
    Serial.println("RED");
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(BLUE_LED_PIN, LOW);
    delay(blinkDelay);

    Serial.println("BLUE");
    digitalWrite(BLUE_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
    delay(blinkDelay);
}

// Блимавка зі змінною швидкістю - 3 секунди блимає швидко, 3 секунди повільно і т. д.
void alternating_speed_blinker()
{    
    if (millis() - lastSpeedChange >= 3000)
    {
        lastSpeedChange = millis();

        if (blinkDelay == 500)
            blinkDelay = 100;
        else
            blinkDelay = 500;
    }

    police_blinker(blinkDelay);
}

void loop()
{
    // стандартна поліцейська блимавка
    // police_blinker(250);

    // блимавка зі змінною швидкістю
     alternating_speed_blinker();
}

