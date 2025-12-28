#include "TaskWrapper.h"

class TaskFunction : public TaskWrapper {
public:
    TaskFunction(uint8_t pin)
    : TaskWrapper("Blinker", 2048, 1, 1), pin(pin) {}

protected:
    void run() override {
        pinMode(pin, OUTPUT);
        while (true) {
            digitalWrite(pin, HIGH);
            vTaskDelay(pdMS_TO_TICKS(500));
            digitalWrite(pin, LOW);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }

private:
    uint8_t pin;
};