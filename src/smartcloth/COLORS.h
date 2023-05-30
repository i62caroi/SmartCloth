/*

Tratamos con colores en formato RGB 5:6:5 de 16bpp (2Byte/pixel).

En este enlace hay una colección de colores con su respectivo código HEX para rgb565: https://github.com/newdigate/rgb565_colors

En este enlace se puede obtener el código HEX de un color seleccionado: http://www.barth-dev.de/online/rgb565-color-picker/

*/

#ifndef COLORS_H
#define COLORS_H


// ---------------- COLORES DASHBOARD --------------
#define AZUL_FONDO            0x2126
#define GRIS_CUADROS          0x29A8
#define GRIS_BORDE_CUADRO     0xC6DE
#define GRIS_CUADROS_VALORES  0x426C
#define ROJO_PESO             0xF902
#define AZUL_CARB             0x059E
#define NARANJA_PROT          0xF2A5
#define AMARILLO_GRASAS       0xEEC5
#define ROJO_KCAL             0xF801

#define COLOR_G1        0xEB4C    // ROJO
#define COLOR_G1E       0xFB0B

#define COLOR_G2        0x5DAA    // VERDE
#define COLOR_G2E       0x2DC7

#define COLOR_G3_G3E    0x8E5E    // AZUL

#define COLOR_G4        0xFE74    // ROSA
#define COLOR_G4E       0xFE5A

#define COLOR_G5        0xFE44    // AMARILLO
#define COLOR_G5E       0xFE40

#define COLOR_G6        0xB45B    // MORADO
#define COLOR_G6E       0xBC3C

#define COLOR_G7        0x7E25    // VERDE
#define COLOR_G7E       0x5E40

#define COLOR_G8        0xFF7B    // BEIGE
#define COLOR_G8E       0xFF7A

#define COLOR_G9        0xCCCE    // MARRON
#define COLOR_G9E       0xD4CD

#define COLOR_G10_G10E  0xFD7D    // ROSA

#define COLOR_G11       0xCEC9    // VERDE
#define COLOR_G11E      0xD6E9

#define COLOR_G12_G12E  0xFE80    // AMARILLO

#define COLOR_G13_G13E  0xFF4D    // AMARILLO

// Falta G14

#define COLOR_G15       0xE476    // ROSA
#define COLOR_G15E      0xEC37

#define COLOR_G16       0x95B7    // GRIS
#define COLOR_G16E      0x8DB7

#define COLOR_G17       0xFAD0    // ROSA
#define COLOR_G17E      0xFA30

#define COLOR_G18       0xFB2B    // NARANJA
#define COLOR_G18E      0xFAEA

#define COLOR_G19       0xFEEC    // AMARILLO
#define COLOR_G19E      0xFEE8

#define COLOR_G20_G20E  0xFF1A    // BEIGE
// -------------------------------------------------



// ------------- COLORES EJEMPLO ----------------------

// NEGRO
#define BLACK         0x0000

// BLANCO
#define WHITE         0xFFFF // Wireframes arranque, carga

// ROJO
#define RED           0xF800 // Wireframes añadir, eliminar, guardar, recipiente, grupos, crudo-cocinado
#define LIGHTRED      0xFC10
#define DARKRED       0x8000
  // NARANJA
#define ORANGE        0xFC80

// VERDE
#define GREEN         0x07E0
#define GREEN2        0x07C0
#define LIGHTGREEN    0x87F0
#define DARKGREEN     0x0400

//AZUL
#define BLUE          0x001F
#define BLUE2         0x051F
#define BLUE3         0x019F
#define LIGHTBLUE     0x841F
#define DARKBLUE      0x0010
  // CYAN
#define CYAN          0x07FF
#define LIGHTCYAN     0x87FF
#define DARKCYAN      0x0410

// AMARILLO
#define YELLOW        0xFFE0
#define YELLOW2       0xFFC0
#define LIGHTYELLOW   0xFFF0
#define DARKYELLOW    0x8400

// ROSA
#define PINK          0xFA1F
  // MAGENTA
#define MAGENTA       0xF81F
#define LIGHTMAGENTA  0xFC1F
#define DARKMAGENTA   0x8010
  // MORADO
#define PURPLE        0x9112
#define DARKPURPLE    0x30C6

// MARRON
#define BROWN         0xA145
#define BROWN2        0xABC8

// ESCALA DE GRISES
#define GRAYSCALE1    2113
#define GRAYSCALE2    2113*2
#define GRAYSCALE3    2113*3
#define GRAYSCALE4    2113*4
#define GRAYSCALE5    2113*5
#define GRAYSCALE6    2113*6
#define GRAYSCALE7    2113*7
#define GRAYSCALE8    2113*8
#define GRAYSCALE9    2113*9
#define GRAYSCALE10   2113*10
#define GRAYSCALE11   2113*11
#define GRAYSCALE12   2113*12
#define GRAYSCALE13   2113*13
#define GRAYSCALE14   2113*14
#define GRAYSCALE15   2113*15
#define GRAYSCALE16   2113*16
#define GRAYSCALE17   2113*17
#define GRAYSCALE18   2113*18
#define GRAYSCALE19   2113*19
#define GRAYSCALE20   2113*20
#define GRAYSCALE21   2113*21
#define GRAYSCALE22   2113*22
#define GRAYSCALE23   2113*23
#define GRAYSCALE24   2113*24
#define GRAYSCALE25   2113*25
#define GRAYSCALE26   2113*26
#define GRAYSCALE27   2113*27
#define GRAYSCALE28   2113*28
#define GRAYSCALE29   2113*29
#define GRAYSCALE30   2113*30


/*
// --- COLORES ----
// 8bpp (1 byte/pixel ) => RGB332
// 16bpp (2 bytes/pixel) Colores en formato RGB565 ==> http://www.barth-dev.de/online/rgb565-color-picker/
// 24bpp (3 bytes/pixel o 4 bytes/pixel) ==> RGB888

#define   NEGRO         0x0000
#define   BLANCO        0xFFFF
#define   ROJO          0xF800 //0xF920 | 0x8000 | 0xfc10 | 0xfa02
#define   NARANJA       0xFC80
#define   AMARILLO      0xFFC0
#define   VERDE         0x07C0
#define   CIAN          0x07FF
#define   AZUL          0x019F
#define   ROSA          0xFA1F
#define   MORADO        0x9112
#define   MARRON        0xABC8

*/


#endif