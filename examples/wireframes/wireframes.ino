/*
-------------- MEMORIA SDRAM --------------------------------------------------------------------
  * Memoria SDRAM tipo W9812G6KH-6 [3]:
	  		- 2 M words x 4 banks x 16 bits (128 Mbits)
	  		- 166 MHz de frecuencia de reloj
	  		- modo auto-refresh
	  		- 64 ms de tiempo de refresh
	  		- CAS Latency: 2 y 3
	  		- Velocidad de acceso CL3 (3 ciclos de reloj)
        - Row address: A0-A11
	  		- Column address: A0-A8

  	  Esto se corresponde con los siguientes valores de Datasheet 8.1.2 SDRAM Connection del RA8876:
        Density: 128 Mb (4 banks) 
        Addressing => row: [A0-A11]
                   => column: [A0-A8]  
  -------------------------------------------------------------------------------------------------
*/

#include "RA8876_v2.h"
#include "COLORS.h"

// Screen circuit wiring
#define RA8876_CS        12
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10

#define SD_CARD_SCS   4

RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);


void arranque();
void screen_INI();
void select_Grupo();


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
    arranque();  // OK
    delay(1000);
    
    //tft.clearScreen(0); 
    delay(500);
/*
    Serial.println("\nPANTALLA INICIAL");
    screen_INI();
    delay(1000);

    tft.clearScreen(0);
    delay(500);

    Serial.println("\nESCOGER GRUPO");
    select_Grupo();
    delay(1000);
*/


}





void loop() {
     
}



void arranque(){

    //PAG 1 ==> fondo blanco
    tft.clearScreen(WHITE);
    delay(300);
/*
    // Al colocar el logo (symbolo) abajo, ya no hace falta esta parte?
    // Porque ahora es lo último que sale

    //PAG 2 ==> círculo gris
    tft.fillCircle(100,300,50,COLOR65K_GRAYSCALE20);
    delay(300);

    //PAG 3 ==> cuadrado esquinas redondeadas girado
    tft.clearArea(30, 200, 200, 400, BLANCO);
    //tft.clearScreen(BLANCO);
    tft.fillRoundRect(30,300,100,300,10,COLOR65K_GRAYSCALE26);
    delay(300);

    //PAG 4 ==> cuadrado girado
    tft.clearArea(30, 200, 200, 400, BLANCO);
    //tft.clearScreen(BLANCO);
    tft.fillRect(30,300,100,300,COLOR65K_GRAYSCALE16);
    delay(300);

    //PAG 5 ==> cuadrado normal
    tft.clearArea(30, 200, 200, 400, BLANCO);
    //tft.clearScreen(BLANCO);
    tft.fillRect(50,250,100,350,COLOR65K_GRAYSCALE26);
    delay(300);

    //PAG 6 => symbol muy tenue ==> DIFICIL
*/
    

    // -------- LETRAS ------- 

    /*
      S (95x159)    =>  x  =  <S  =  40                          -->   tft.sdCardDraw256bitsBIN(40,150,95,159,"bin/arranque/S.bin");
        
      M (104x154)   =>  x  =  <M  =  <S(40) + S(95)     =  135   -->   tft.sdCardDraw256bitsBIN(135,150,104,159,"bin/arranque/M.bin");

      A (104x159)   =>  x  =  <A  =  <M(135) + M(104)   =  239   -->   tft.sdCardDraw256bitsBIN(239,150,104,159,"bin/arranque/A.bin");

      R (85x159)    =>  x  =  <R  =  <A(239) + A(104)   =  343   -->   tft.sdCardDraw256bitsBIN(343,150,85,159,"bin/arranque/R.bin");

      T1 (104x159)  =>  x  =  <T1 =  <R(343) + R(85)    =  428   -->   tft.sdCardDraw256bitsBIN(428,150,104,159,"bin/arranque/T.bin");

      C (85x159)    =>  x  =  <C  =  <T1(428) + T1(104) =  532   -->   tft.sdCardDraw256bitsBIN(532,150,85,159,"bin/arranque/C.bin");

      L (85x159)    =>  x  =  <L  =  <C(532) + C(85)    =  617   -->   tft.sdCardDraw256bitsBIN(617,150,85,159,"bin/arranque/L.bin");

      O (85x159)    =>  x  =  <O  =  <L(617) + L(85)    =  702   -->   tft.sdCardDraw256bitsBIN(702,150,85,159,"bin/arranque/O.bin");

      T2 (104x159)  =>  x  =  <T2 =  <O(702) + O(85)    =  787   -->   tft.sdCardDraw256bitsBIN(787,150,104,159,"bin/arranque/T.bin");

      H (85x159)    =>  x  =  <H  =  <T2(787) + T2(104) =  891   -->   tft.sdCardDraw256bitsBIN(891,150,85,159,"bin/arranque/H.bin");

    */

    // S M A R T C L O T H ==> S T O T M L R A C H 

    // S (95x159)
    tft.sdCardDraw256bitsBIN(40,150,95,159,"bin/arranque/S.bin");
    delay(300);

    // T1 (104x159)
    tft.sdCardDraw256bitsBIN(428,150,104,159,"bin/arranque/T.bin");
    delay(300);

    // O (85x159)
    tft.sdCardDraw256bitsBIN(702,150,85,159,"bin/arranque/O.bin");
    delay(300);

    // T2 (104x159)
    tft.sdCardDraw256bitsBIN(787,150,104,159,"bin/arranque/T.bin");
    delay(300);

    // M (104x154)
    tft.sdCardDraw256bitsBIN(135,150,104,159,"bin/arranque/M.bin");
    delay(300);

    // L (85x159)
    tft.sdCardDraw256bitsBIN(617,150,85,159,"bin/arranque/L.bin");
    delay(300);

    // R (85x159)
    tft.sdCardDraw256bitsBIN(343,150,85,159,"bin/arranque/R.bin");
    delay(300); 

    // A (104x159)
    tft.sdCardDraw256bitsBIN(239,150,104,159,"bin/arranque/A.bin");
    delay(300);

    // C (85x159)
    tft.sdCardDraw256bitsBIN(532,150,85,159,"bin/arranque/C.bin");
    delay(300);

    // H (85x159)
    tft.sdCardDraw256bitsBIN(891,150,85,159,"bin/arranque/H.bin");
    delay(300);    


    // ------- LOGO (SYMBOL)------

    // Logo (162x169) ==> debajo
    tft.sdCardDraw256bitsBIN(450,350,162,169,"bin/arranque/Log.bin");
    delay(300);
    
}
/*
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void screen_INI(){
    String cad = "COLOQUE UN RECIPIENTE \n        EN LA ZONA DE PESADA";

    //PAG 1 ==> fondo rojo y texto
    tft.clearScreen(ROJO);                       // Fondo rojo
    tft.selectInternalFont(RA8876_FONT_SIZE_32); // Tamaño letra
    tft.setTextScale(1);                         // Escala texto
    tft.setTextColor(BLANCO);                    // Color texto
    tft.setCursor(100, 150);                     // Posición cursor
    tft.print(cad);
    delay(300);

    //PAG 2 ==> Círculo
    tft.fillCircle(512,400,50,BLANCO);
    delay(300);

    //PAG 3 ==> cuadrado esquinas redondeadas girado
    tft.clearArea(400, 300, 650, 500, ROJO);
    tft.fillRoundRect(477,400,547,400,10,BLANCO);
    delay(300);

    //PAG 4 ==> cuadrado esquinas redondeadas normal
    tft.clearArea(400, 300, 650, 500, ROJO);
    tft.fillRoundRect(487,300,537,400,5,BLANCO);
    delay(300);

    //PAG 5 ==> palitos alrededor cuadrado (fillRect o drawLine??)
    // Palitos izquierda 
    tft.fillRect(484, 346, 487, 348, BLANCO); // Arriba (3x2)
    tft.fillRect(482, 349, 487, 351, BLANCO); // Central (5x2)
    tft.fillRect(484, 352, 487, 353, BLANCO); // Abajo (3x2)
    // Palitos derecha 
    tft.fillRect(537, 346, 540, 348, BLANCO); // Arriba (3x2)
    tft.fillRect(537, 349, 542, 351, BLANCO); // Central (5x2)
    tft.fillRect(537, 352, 540, 353, BLANCO); // Abajo (3x2)
    // Palitos arriba 
    tft.fillRect(508, 297, 510, 300, BLANCO); // Izquierda (2x3)
    tft.fillRect(511, 295, 513, 300, BLANCO); // Central (2x5)
    tft.fillRect(515, 297, 517, 300, BLANCO); // Derecha (2x3)
    // Palitos abajo 
    tft.fillRect(508, 400, 510, 403, BLANCO); // Izquierda (2x3)
    tft.fillRect(511, 400, 513, 405, BLANCO); // Central (2x5)
    tft.fillRect(515, 400, 517, 403, BLANCO); // Derecha (2x3)
    delay(300);

    //PAG 6 => cerebro --> centrar en el cuadrado blanco
    tft.sdCardDraw256bitsBIN(475,320,74,61,"bin/inicial/brain.bin");
    //delay(200);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------


void select_Grupo(){
    String cad = "SELECCIONE UN GRUPO \n            DE ALIMENTOS";

    //PAG 1 ==> fondo rojo y texto
    tft.clearScreen(ROJO);                       // Fondo rojo
    tft.selectInternalFont(RA8876_FONT_SIZE_32); // Tamaño letra
    tft.setTextScale(1);                         // Escala texto
    tft.setTextColor(BLANCO);                    // Color texto
    tft.setCursor(100, 80);                     // Posición cursor
    tft.print(cad);
    delay(300);

    //PAG 2 ==> líneas
    tft.drawLine(0,250,240,250,BLANCO); //draLine o fillRect??
    tft.drawLine(0,150,512,150,BLANCO); //drawLine o fillRect??
    delay(300);

    //PAG 3 ==> cuadro 1
    tft.sdCardDraw256bitsBIN(230,307,167,160,"bin/grupo/cuadro1.bin");
    delay(300);

    //PAG 4 ==> cuadro 2
    tft.sdCardDraw256bitsBIN(402,307,167,160,"bin/grupo/cuadro2.bin");
    delay(300);

    //PAG 5 ==> cuadro 3
    tft.sdCardDraw256bitsBIN(574,307,167,160,"bin/grupo/cuadro3.bin");
    delay(300);

    //PAG 6 ==> cuadro 4
    tft.sdCardDraw256bitsBIN(746,307,167,160,"bin/grupo/cuadro4.bin");
    delay(300);

    //PAG 7 ==> mano
    tft.sdCardDraw256bitsBIN(746,227,142,137,"bin/grupo/mano.bin"); 

    //PAG 8 ==> lineas alrededor cuadro3 simulando pulsacion
    //TODO
}*/

