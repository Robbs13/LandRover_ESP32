#ifndef TASKWRAPPER_H
#define TASKWRAPPER_H

#include <Arduino.h>

class TaskWrapper {
public:
  TaskWrapper(const char* name, uint16_t stackSize, UBaseType_t priority, BaseType_t core);

protected:
  virtual void run() = 0; // Muss in Unterklasse implementiert werden

private:
  static void taskEntry(void* param); // Statische Einstiegspunkt-Funktion
};

#endif