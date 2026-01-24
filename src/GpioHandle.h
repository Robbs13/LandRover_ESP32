#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Config.h" 


// Beispiel: 50 Hz Servo/LED PWM, 16 Bit Auflösung
static constexpr uint32_t PWM_FREQ = 5000;
static constexpr uint8_t  PWM_RES  = 8;
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

    void initFromConfig();                 // scan cfg_rcGpioMap und initialisiere alles
    void writePwm(uint8_t pin, uint16_t us);
    void initPwmPin(uint8_t pin);  

    //void buildFrame(bool failsafe, GpioData& out);
    void handleGPIOQueue();
    void handleGPIOOutput();
    //void writePwm(uint8_t pin, uint16_t value);
    void gpio_set(uint8_t pin, uint16_t value);
    void dac_write(uint8_t pin, uint16_t value);

private:
    TaskHandle_t    _taskHandle;
    uint8_t             _cycleTime;   

    GpioData _gpioFrame {};
    uint8_t pinToCh_[MAX_GPIO];                      // pin -> ledc channel
    uint8_t nextCh_ = 0;
    

};
