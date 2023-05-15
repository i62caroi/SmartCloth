#ifndef SCREEN_H
#define SCREEN_H

#include "RA8876_v2.h"

/* ---------------- COLORES ----------------------------------------- */
// 16bpp (2 bytes/pixel) Colores en formato RGB565 ==> http://www.barth-dev.de/online/rgb565-color-picker/

#define   NEGRO             0x0000
#define   BLANCO            0xFFFF
#define   ROJO              0xF800  // 0xF920 | 0xfa02
#define   ROJO_CLARO        0xfC10
#define   ROJO_OSCURO       0x8000
#define   NARANJA           0xFC80
#define   AMARILLO          0xFFC0
#define   AMARILLO_CLARO    0xfff0
#define   AMARILLO_OSCURO   0x8400
#define   VERDE             0x07E0
#define   VERDE_CLARO       0x87f0
#define   VERDE_OSCURO      0x0400
#define   CIAN              0x07FF
#define   CIAN_CLARO        0x87ff
#define   CIAN_OSCURO       0x0410
#define   AZUL              0x001F
#define   AZUL2             0x051f
#define   AZUL_CLARO        0x841f
#define   AZUL_OSCURO       0x0010
#define   ROSA              0xFA1F
#define   MORADO            0x9112
#define   MAGENTA           0xf81f
#define   MAGENTA_CLARO     0xfc1f
#define   MAGENTA_OSCURO    0x8010
#define   MARRON            0xABC8
#define   MARRON2           0xA145

//--------------------------------

#define COLOR65K_GRAYSCALE1    2113
#define COLOR65K_GRAYSCALE2    2113*2
#define COLOR65K_GRAYSCALE3    2113*3
#define COLOR65K_GRAYSCALE4    2113*4
#define COLOR65K_GRAYSCALE5    2113*5
#define COLOR65K_GRAYSCALE6    2113*6
#define COLOR65K_GRAYSCALE7    2113*7
#define COLOR65K_GRAYSCALE8    2113*8
#define COLOR65K_GRAYSCALE9    2113*9
#define COLOR65K_GRAYSCALE10   2113*10
#define COLOR65K_GRAYSCALE11   2113*11
#define COLOR65K_GRAYSCALE12   2113*12
#define COLOR65K_GRAYSCALE13   2113*13
#define COLOR65K_GRAYSCALE14   2113*14
#define COLOR65K_GRAYSCALE15   2113*15
#define COLOR65K_GRAYSCALE16   2113*16
#define COLOR65K_GRAYSCALE17   2113*17
#define COLOR65K_GRAYSCALE18   2113*18
#define COLOR65K_GRAYSCALE19   2113*19
#define COLOR65K_GRAYSCALE20   2113*20
#define COLOR65K_GRAYSCALE21   2113*21
#define COLOR65K_GRAYSCALE22   2113*22
#define COLOR65K_GRAYSCALE23   2113*23
#define COLOR65K_GRAYSCALE24   2113*24
#define COLOR65K_GRAYSCALE25   2113*25
#define COLOR65K_GRAYSCALE26   2113*26
#define COLOR65K_GRAYSCALE27   2113*27
#define COLOR65K_GRAYSCALE28   2113*28
#define COLOR65K_GRAYSCALE29   2113*29
#define COLOR65K_GRAYSCALE30   2113*30
/* ------------------------------------------------------------------- */

int ANCHO; // X (1024)
int ALTO; // Y (600)




void Arranque(){
    ANCHO = tft.getWidth(); // X
    ALTO = tft.getHeight(); // Y

    //PAG 1 ==> fondo blanco
    tft.clearScreen(BLANCO);
    delay(300);

    //PAG 2 ==> círculo gris
    tft.fillCircle(100,300,50,COLOR65K_GRAYSCALE13);
    delay(300);

    //PAG 3 ==> cuadrado esquinas redondeadas girado
    tft.clearArea(30, 200, 200, 400, BLANCO);
    //tft.clearScreen(BLANCO);
    tft.fillRoundRect(30,300,100,300,10,COLOR65K_GRAYSCALE26);
    delay(300);

    //PAG 4 ==> cuadrado girado
    tft.clearArea(30, 200, 200, 400, BLANCO);
    //tft.clearScreen(BLANCO);
    tft.fillRect(30,300,100,300,COLOR65K_GRAYSCALE26);
    delay(300);

    //PAG 5 ==> cuadrado normal
    tft.clearArea(30, 200, 200, 400, BLANCO);
    //tft.clearScreen(BLANCO);
    tft.fillRect(50,250,100,350,COLOR65K_GRAYSCALE26);
    delay(300);

    //PAG 6 => symbol muy tenue ==> DIFICIL

    //PAG 7 => symbol (142x150)
    //tft.clearArea(20, 200, 300, 500, BLANCO);
    tft.clearScreen(BLANCO);
    tft.sdCardDraw256bitsBIN(30,250,142,150,"bin/arranque/symbol.bin");
    //delay(200);

    //PAG 8 => letra S (88x154) => x = 30 + symbol(142) = 172
    tft.sdCardDraw256bitsBIN(172,250,88,154,"bin/arranque/S.bin");
    //delay(200);

    //PAG 9 => letra T1 (95x154) => x = <S(172) + S(88) + M(96) + A(95) + R(77) = 528
    tft.sdCardDraw256bitsBIN(528,250,95,154,"bin/arranque/T.bin");
    //delay(200);

    //PAG 10 => letra O (77x154) => x = <T1(528) + T1(95) + C(77) + L(77) = 777
    tft.sdCardDraw256bitsBIN(777,250,77,154,"bin/arranque/O.bin");
    //delay(200);

    //PAG 11 => letra T2 (95x154) => x = <O(777) + O(77) = 854
    tft.sdCardDraw256bitsBIN(854,250,95,154,"bin/arranque/T.bin");
    //delay(200);

    //PAG 12 => letra M (96x154) => x = <S(172) + S(88) = 260
    tft.sdCardDraw256bitsBIN(260,250,96,154,"bin/arranque/M.bin");
    //delay(200);

    //PAG 13 => letra L (77x154) => x = <T1(528) + T1(95) + C(77) = 700
    tft.sdCardDraw256bitsBIN(700,250,77,154,"bin/arranque/L.bin");
    //delay(200);

    //PAG 14 => letra R (77x154) => x = <M(260) + M(96) + A(95) = 451
    tft.sdCardDraw256bitsBIN(451,250,77,154,"bin/arranque/R.bin");
    //delay(200);

    //PAG 15 => letra A (95x154) => x = <M(260) + M(96) = 356
    tft.sdCardDraw256bitsBIN(356,250,95,154,"bin/arranque/A.bin");
    //delay(200);

    //PAG 16 => letra C (77x154) => x = <T1(528) + T1(95) = 623
    tft.sdCardDraw256bitsBIN(623,250,77,154,"bin/arranque/C.bin");
    //delay(200);

    //PAG 17 => letra H (77x154) => x = <T2(854) + T2(95) = 949
    tft.sdCardDraw256bitsBIN(949,250,77,154,"bin/arranque/H.bin");
    //delay(200);
}



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
    dealy(300);

    //PAG 4 ==> cuadro 2
    tft.sdCardDraw256bitsBIN(402,307,167,160,"bin/grupo/cuadro2.bin");
    dealy(300);

    //PAG 5 ==> cuadro 3
    tft.sdCardDraw256bitsBIN(574,307,167,160,"bin/grupo/cuadro3.bin");
    dealy(300);

    //PAG 6 ==> cuadro 4
    tft.sdCardDraw256bitsBIN(746,307,167,160,"bin/grupo/cuadro4.bin");
    dealy(300);

    //PAG 7 ==> mano
    tft.sdCardDraw256bitsBIN(746,227,142,137,"bin/grupo/mano.bin"); 

    //PAG 8 ==> lineas alrededor cuadro3 simulando pulsacion
    //TODO
}

#endif