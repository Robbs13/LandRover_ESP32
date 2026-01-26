# LandRover_ESP32
RC ESP32 Controller

Eingänge:
    16 Channels mit 11bit in 1000 - 2000us Signal
    Eingelesen mit UART CRSF Protokoll

Ausgänge:
    Servo und Motor mit ESC werden mit ledc 1000-2000us verarbeitet
    LEDs werden mit ledc 0-4095 Helligkeit gesteuert


PWF-Zustände:
    Parken = Keine RC Verbindung -> Fahrzeug aktiviert Warnblinken und setzt alle GPIO Werte auf 1000 - Failsafe
    Wohnen = Lichtsteuerung aktiv
    Fahren = Fahrfunktionen aktiv

Licht-Zustände:
    Off = Aus... überraschend
    Flash = Lichthupe
    LightOn = Abblendlicht, Rücklicht, Positionslicht und Tacho
    BeamFlash = LightOn mit Lichthupe
    BeamOn = LightOn mit Fernlicht

    Overlay Funktionen:
        Bremse über Rücklicht
        Bei Startversuch bringt Anlasserstrom die Beleuchtung zum Flackern nach abnehmender Amplitude und Zykluszeit plus Randomflackerkurve

Fahr-Zustände:


