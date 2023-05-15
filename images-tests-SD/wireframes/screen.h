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

    //pag 1
    tft.clearScreen(BLANCO);
    delay(500);
    //pag 2
    tft.drawCircleFill(100,300,50,COLOR65K_GRAYSCALE13);
    delay(500);
    //pag 3
    tft.clearScreen(BLANCO);
    tft.fillRoundRect(30,300,100,300,10,COLOR65K_GRAYSCALE26);
    delay(200);
    //pag 4
    tft.clearScreen(BLANCO);
    tft.drawRect(30,300,100,300,COLOR65K_GRAYSCALE26);
    delay(200);
    //pag 5
    tft.clearScreen(BLANCO);
    tft.drawRect(50,250,100,350,COLOR65K_GRAYSCALE26);
    delay(200);

}

#endif