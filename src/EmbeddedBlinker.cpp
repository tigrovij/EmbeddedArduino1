#include <Arduino.h>
#include "EmbeddedBlinker.h"

namespace EmbeddedBlinker
{
    struct Config
    {
        static constexpr uint8_t LED_PIN = 7;
        static constexpr uint8_t BUTTON_PIN = 16;
        static constexpr uint32_t BLINK_INTERVAL_MS = 500;
        static constexpr uint32_t DEBOUNCE_MS = 30;
        static constexpr uint32_t SERIAL_BAUD = 115200;
        static constexpr uint32_t REPORT_ITERATIONS = 1000;
    };

    enum class LedState { Off, On };
    enum class Mode { Blinking, AlwaysOn, AlwaysOff };

    class Led
    {
    public:
        explicit Led(uint8_t pin) : pin_(pin) {}

        void init()
        {            
            digitalWrite(pin_, LOW);
            pinMode(pin_, OUTPUT);
        }

        void set(LedState state)
        {
            digitalWrite(pin_, state == LedState::On ? HIGH : LOW);
        }

    private:
        const uint8_t pin_;  // Номер піну не змінюється після створення LED.
    };

    // Один об'єкт живе весь час роботи, без new і глобального об'єкта LED.
    static Led& led()
    {
        static Led instance(Config::LED_PIN);
        return instance;
    }

    // Лише ці дані потрібні для обміну між ISR і головним циклом.
    static volatile bool buttonEvent = false;
    static portMUX_TYPE buttonMux = portMUX_INITIALIZER_UNLOCKED;

    static void IRAM_ATTR buttonIsr()
    {
        portENTER_CRITICAL_ISR(&buttonMux);
        buttonEvent = true;
        portEXIT_CRITICAL_ISR(&buttonMux);
    }

    void setup()
    {
        Serial.begin(Config::SERIAL_BAUD);
        led().init();
        // Кнопка з'єднує GPIO16 із GND.
        pinMode(Config::BUTTON_PIN, INPUT_PULLUP);
        // CHANGE потрібен для відстеження брязкоту і при відпусканні.
        attachInterrupt(digitalPinToInterrupt(Config::BUTTON_PIN), buttonIsr, CHANGE);
        Serial.println("Mode: blinking");
    }

    void loop()
    {
        const uint32_t startedUs = micros();

        // static зберігає значення між викликами loop(), без динамічної пам'яті.
        static Mode mode = Mode::Blinking;
        static LedState state = LedState::Off;
        static uint32_t lastBlinkMs = millis();
        static int lastRaw = digitalRead(Config::BUTTON_PIN);
        static int stableButton = lastRaw;
        static uint32_t lastChangeMs = millis();

        portENTER_CRITICAL(&buttonMux);
        const bool event = buttonEvent;
        buttonEvent = false;
        portEXIT_CRITICAL(&buttonMux);

        const int raw = digitalRead(Config::BUTTON_PIN);
        const uint32_t nowMs = millis();
        if (event || raw != lastRaw)
        {
            lastRaw = raw;
            lastChangeMs = nowMs;
        }

        const bool stableChange = raw != stableButton &&
            nowMs - lastChangeMs >= Config::DEBOUNCE_MS;
        bool modeChanged = false;
        if (stableChange)
        {
            stableButton = raw;
            if (stableButton == LOW)
            {
                modeChanged = true;
                switch (mode)
                {
                    case Mode::Blinking:
                        mode = Mode::AlwaysOn;
                        state = LedState::On;
                        break;
                    case Mode::AlwaysOn:
                        mode = Mode::AlwaysOff;
                        state = LedState::Off;
                        break;
                    case Mode::AlwaysOff:
                        mode = Mode::Blinking;
                        state = LedState::Off;
                        lastBlinkMs = nowMs;
                        break;
                }
                led().set(state);
            }
        }

        // Віднімання unsigned коректне і при переповненні millis().
        if (mode == Mode::Blinking && nowMs - lastBlinkMs >= Config::BLINK_INTERVAL_MS)
        {
            lastBlinkMs = nowMs;
            state = state == LedState::Off ? LedState::On : LedState::Off;
            led().set(state);
        }

        static uint64_t totalUs = 0;
        static uint32_t iterations = 0;
        totalUs += static_cast<uint32_t>(micros() - startedUs);
        ++iterations;

        if (modeChanged)
        {
            switch (mode)
            {
                case Mode::Blinking: Serial.println("Mode: blinking"); break;
                case Mode::AlwaysOn: Serial.println("Mode: always on"); break;
                case Mode::AlwaysOff: Serial.println("Mode: always off"); break;
            }
        }
        if (iterations == Config::REPORT_ITERATIONS)
        {
            const double averageUs = static_cast<double>(totalUs) / iterations;
            Serial.printf("Average loop: %.2f us (%lu iterations, Serial excluded)\n",
                          averageUs, static_cast<unsigned long>(iterations));
            totalUs = 0;
            iterations = 0;
        }
    }
}
