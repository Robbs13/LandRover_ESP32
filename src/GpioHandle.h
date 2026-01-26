#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Config.h" 


// Beispiel: 50 Hz Servo/LED PWM, 16 Bit Auflösung
static constexpr uint32_t PWM_SERV_MOT_FREQ = 50;
static constexpr uint8_t  PWM_SERV_MOT_RES  = 16;
static constexpr uint32_t PWM_LED_FREQ = 5000;
static constexpr uint8_t  PWM_LED_RES  = 12;

static constexpr uint8_t INVALID_CH = 0xFF;
static constexpr uint8_t MAX_GPIO = 40;          // ESP32: 0..39
static constexpr uint8_t MAX_LEDC_CH = 16;       // ESP32 LEDC channels


class GpioHandle {
public:
    explicit GpioHandle(int n);

    // UART Initialisierung
    void begin(); 

    // Startet den FreeRTOS-Task für diese Instanz
    bool startTask(UBaseType_t priority, BaseType_t core, uint8_t time);

private:
    // Die eigentliche Task-Schleife (instanzbezogen)
    void taskLoop();

    // Statischer „Trampolin“-Entry für FreeRTOS
    static void taskTrampoline(void *pvParameters);

    void gpioTaskInit();
    void initPwmPin(uint8_t pin, uint32_t freq, uint8_t res, int &pwmChannel);  

    void handleGPIOQueue();
    void handleGPIOOutput();

    //uint16_t mapUsValueToLedValue(uint16_t value_us);

    uint32_t rcToLedcDuty_ServoEsc(uint16_t rc_1000_2000,
                              uint32_t servo_freq_hz,
                              uint8_t  servo_res_bits);
    uint32_t rcToLedcDuty_LedDim(uint16_t rc_1000_2000, uint8_t led_res_bits);


private:
    TaskHandle_t    _taskHandle;
    uint8_t         _cycleTime;   
    GpioData        _gpioFrame {};
    static constexpr int8_t INVALID_CH = -1;
    int8_t          _pinToCh[MAX_GPIO];                  // pin -> ledc channel
    uint8_t         _nextCh = 0;
    

};

static inline uint16_t clampU16(uint16_t v, uint16_t lo, uint16_t hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}
