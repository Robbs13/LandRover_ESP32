#ifndef MAPPERFUNCTION_H
#define MAPPERFUNCTION_H

#include <Arduino.h>



class MapperFunction {
    private:
        // Konstruktor privat -> niemand kann von außen ein Objekt erzeugen
        MapperFunction();

        //Destruktor
        ~MapperFunction();  

        // Kopieren und Zuweisung verbieten
        MapperFunction(const MapperFunction&) = delete;
        MapperFunction& operator=(const MapperFunction&) = delete;


    public:
        // Zugriff auf die einzige Instanz
        static MapperFunction& getInstance();

        void begin();      


};
#endif