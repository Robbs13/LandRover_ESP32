/*
 * File: Controller.h
 *
 * Description:
 *      Header File für Controller Attribute
 * Usage:
 *
 * Notes:
 *
 */

 #define ESP32AZDelivery = 1;

/*
 * Description:    
 *      ESP32 AZDelivery
 *      https://www.amazon.de/dp/B07Z83MF5W?ref_=ppx_hzsearch_conn_dt_b_fed_asin_title_1&th=1
 *      AZDelivery ESP32 Dev Kit C V4 NodeMCU WLAN WiFi Development Board kompatibel mit Arduino inklusive E-Book! (Nachfolger Modul von ESP32 Dev Kit C V2)
 * 
 * Usage:
 *      MainController
 *
 * Notes:
 *      16 PWM PINs
 *
 */

 //PWM Output
 #define PIN_THROTTLE 14
 #define PIN_STEERING 13
 #define PIN_TRANSMISSION 12
 #define PIN_WINCH 27
 #define PIN_SHAKER 23
 #define PIN_HEADLIGHTS 3
 #define PIN_REARLIGHTS 4
 #define PIN_BLINKRIGHT 5
 #define PIN_BLINKLEFT 15
 
 //Digital Output
 #define PIN_POSLIGHTS 18
 #define PIN_REVERSELIGHT 2
 #define PIN_WORKLIGHT 16
 #define PIN_TACHOLIGHT 19

 //Digital Analog Output
 #define PIN_SOUND_DAC1 25
 #define PIN_SOUND_DAC2 26
 
 //I2C 
 #define PIN_I2C_1 21
 #define PIN_I2C_2 22

