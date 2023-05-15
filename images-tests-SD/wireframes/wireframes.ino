#include "screen.h"   

// Screen circuit wiring
#define RA8876_CS        12
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10

#define SD_CARD_SCS   4

RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);



void setup() {
    Serial.begin(115200);
    while (!Serial);
    delay(1000);

    
    Serial.println("\nStarting up...");

    //setting up the screen
    pinMode(RA8876_BACKLIGHT, OUTPUT);  // Set backlight pin to OUTPUT mode
    digitalWrite(RA8876_BACKLIGHT, HIGH);  // Turn on backlight

    if(SD.begin(SD_CARD_SCS))
    Serial.println("SD card initialized");
    else
    Serial.println("SD card failure");
    
    if (!tft.init())
    {
      Serial.println("Could not initialize RA8876");
    }
    
    Serial.println("Startup complete...");

    Serial.println("\nARRANQUE");
    Arranque();  // Wi
    delay(1000);
    
    tft.clearScreen(0); 
    delay(500);

    Serial.println("\nPANTALLA INICIAL");
    screen_INI();
    delay(1000);

    tft.clearScreen(0);
    delay(500);

    Serial.println("\nESCOGER GRUPO");
    select_Grupo();
    delay(1000);



}




/*---------------------------------------------------------------------------------------------------------
   loop(): Función principal ejecutada continuamente
----------------------------------------------------------------------------------------------------------*/
void loop() {
    
   
    
}
