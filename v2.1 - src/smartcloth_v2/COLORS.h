/**
 * @file Colors.h
 * @brief Librería para definir colores en formato RGB 5:6:5 de 16 bpp (2 bytes/pixel)
 *
 * @author Irene Casares Rodríguez
 * @date 12/07/2023
 * @version 1.0
 * 
 * Este archivo contiene la definición de varios colores en formato RGB 5:6:5 de 16 bpp.
 * También se incluyen enlaces útiles para obtener códigos HEX de colores y una colección
 * de colores predefinidos con sus respectivos códigos HEX para rgb565.
 * 
 * Los colores definidos se pueden utilizar en aplicaciones de interfaz gráfica o visualización de datos.
 * 
 * @note Los colores se definen en formato RGB 5:6:5, donde los 5 bits superiores corresponden a la
 *       intensidad del componente rojo, los siguientes 6 bits corresponden al componente verde y los
 *       5 bits inferiores corresponden al componente azul.
 * 
 * @see http://www.rinkydinkelectronics.com/calc_rgb565.php Calculadora de códigos RGB565 a partir de otros códigos
 * @see https://github.com/newdigate/rgb565_colors Colección de colores con códigos HEX para rgb565
 * @see http://www.barth-dev.de/online/rgb565-color-picker/ Herramienta para obtener el código HEX de un color seleccionado
 * @see La herramienta "Medidor de Color Digital" del MacBook permite obtener el HEX de cualquier color en pantalla
 * 
 */

#ifndef COLORS_H
#define COLORS_H


// ---------------- COLORES GENERALES ---------
#define   BLACK     0x0000  // Limpiar páginas antes de escribir o copiar imágenes
#define   WHITE     0xFFFF  // Texto y wireframes de carga y arranque
#define   YELLOW    0xFFE0  // Texto "Comida guardada" en STATE_Init si se acaba de guardar la comida
// -------------------------------------------------


// ---------------- COLORES PANTALLAS TRANSITORIAS --------------
#define   VERDE_PEDIR                 0x6468  // Wireframes pedir recipiente, grupo y crudo-cocinado
#define   VERDE_BORDE_PEDIR_COCCION   0x0184  // Borde de pantalla pedir crudo-cocinado superpuesta al dashboard

#define   AMARILLO_CONFIRM_Y_AVISO    0xFEC4  // Wireframes de aviso y para confirmar acción de añadir, eliminar y guardar
#define   ROJO_TEXTO_CONFIRM_Y_AVISO  0x9883  // Wireframes de aviso y para confirmar acción de añadir, eliminar y guardar

#define   DARKORANGE                  0xCB20 // 0xC86406 en RGB-8:8:8 ==> aviso

#define   RED_ERROR_Y_CANCEL          0xF800  // Wireframes de error y cancelar
#define   RED                         0xF800  // Rojo general

#define   RED_BUTTON                  0xB8C3  // Recuadro que aparece alrededor de los botones de los wireframes de añadir, eliminar, guardar y grupos

#define   DARKPURPLE                  0x30C6  // Mini mensaje cuando guardado completo (local y database)

#define   AZUL_SINCRONIZANDO          0x1B9F  // Pantalla de sincronización
#define   AZUL_RECUADRO_SINCRONIZANDO 0x0010  // Recuadro de la pantalla de sincronización
// --------------------------------------------------------------


// ---------------- COLORES DASHBOARD --------------
#define   AZUL_FONDO            0x2126 
#define   GRIS_CUADROS          0x29A8 
#define   AZUL_BORDE_CUADRO     0xC6DE 
#define   GRIS_CUADROS_VALORES  0x426C 
#define   ROJO_PESO             0xF902 
#define   AZUL_CARB             0x059E 
#define   NARANJA_PROT          0xEA85 
#define   AMARILLO_GRASAS       0xEEC5 
#define   ROJO_KCAL             0xF801 

#define   COLOR_G1              0xEB4C    // ROJO         
#define   COLOR_G2              0x2DC7    // VERDE
#define   COLOR_G3              0x8E5E    // AZUL
#define   COLOR_G4              0xFE5A    // ROSA
#define   COLOR_G5              0xFE40    // AMARILLO
#define   COLOR_G6              0xBC3C    // MORADO
#define   COLOR_G7              0x5E40    // VERDE
#define   COLOR_G8              0xFF9A    // BEIGE
#define   COLOR_G9              0xD4CD    // MARRON
#define   COLOR_G10             0xFD7D    // ROSA
#define   COLOR_G11             0xD6E9    // VERDE
#define   COLOR_G12             0xFE80    // AMARILLO
#define   COLOR_G13             0xFF4D    // AMARILLO
#define   COLOR_G14             0xF6C1    // AMARILLO
#define   COLOR_G15             0xE476    // ROSA
#define   COLOR_G16             0x8DB7    // GRIS
#define   COLOR_G17             0xFA30    // ROSA
#define   COLOR_G18             0xFACA    // NARANJA
#define   COLOR_G19             0xFEE8    // AMARILLO
#define   COLOR_G20             0xFF1A    // BEIGE

#define   COLOR_G50             0xD6E9    // VERDE
// -------------------------------------------------








// ------------- COLORES EJEMPLO ----------------------

/*

// ROJO
#define   LIGHTRED      0xFC10
#define   DARKRED       0x8000
  // NARANJA
#define   ORANGE        0xFC80


// VERDE
#define   GREEN         0x07E0
#define   GREEN_HAND    0x07E2
#define   GREEN2        0x07C0
#define   LIGHTGREEN    0x87F0
#define   DARKGREEN     0x0400

//AZUL
#define   BLUE          0x001F
#define   BLUE2         0x051F
#define   BLUE3         0x019F
#define   LIGHTBLUE     0x841F
#define   DARKBLUE      0x0010
  // CYAN
#define   CYAN          0x07FF
#define   LIGHTCYAN     0x87FF
#define   DARKCYAN      0x0410

// AMARILLO
#define   YELLOW        0xFFE0
#define   YELLOW2       0xFFC0
#define   LIGHTYELLOW   0xFFF0
#define   DARKYELLOW    0x8400

// ROSA
#define   PINK          0xFA1F

// MAGENTA
#define   MAGENTA       0xF81F
#define   LIGHTMAGENTA  0xFC1F
#define   DARKMAGENTA   0x8010

// MORADO
#define   PURPLE        0x9112
#define   DARKPURPLE    0x30C6

// MARRON
#define   BROWN         0xA145
#define   BROWN2        0xABC8

// ESCALA DE GRISES
#define   GRAYSCALE1    2113
#define   GRAYSCALE2    2113*2
#define   GRAYSCALE3    2113*3
#define   GRAYSCALE4    2113*4
#define   GRAYSCALE5    2113*5
#define   GRAYSCALE6    2113*6
#define   GRAYSCALE7    2113*7
#define   GRAYSCALE8    2113*8
#define   GRAYSCALE9    2113*9
#define   GRAYSCALE10   2113*10
#define   GRAYSCALE11   2113*11
#define   GRAYSCALE12   2113*12
#define   GRAYSCALE13   2113*13
#define   GRAYSCALE14   2113*14
#define   GRAYSCALE15   2113*15
#define   GRAYSCALE16   2113*16
#define   GRAYSCALE17   2113*17
#define   GRAYSCALE18   2113*18
#define   GRAYSCALE19   2113*19
#define   GRAYSCALE20   2113*20
#define   GRAYSCALE21   2113*21
#define   GRAYSCALE22   2113*22
#define   GRAYSCALE23   2113*23
#define   GRAYSCALE24   2113*24
#define   GRAYSCALE25   2113*25
#define   GRAYSCALE26   2113*26
#define   GRAYSCALE27   2113*27
#define   GRAYSCALE28   2113*28
#define   GRAYSCALE29   2113*29
#define   GRAYSCALE30   2113*30

*/




#endif