#include "TaskWrapper.h"

TaskWrapper::TaskWrapper(const char* name, uint16_t stackSize, UBaseType_t priority, BaseType_t core) {
  xTaskCreatePinnedToCore(
    taskEntry,   // Einstiegspunkt
    name,        // Task-Name
    stackSize,   // Stack-Größe
    this,        // Parameter = Instanzzeiger
    priority,    // Priorität
    nullptr,     // Kein Handle gespeichert
    core         // Core (0 oder 1)
  );
}

void TaskWrapper::taskEntry(void* param) {
  TaskWrapper* instance = static_cast<TaskWrapper*>(param);
  instance->run(); // Virtuelle Methode starten
}