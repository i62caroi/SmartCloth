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
#define   VERDE_PEDIR_Y_EXITO                 0x6468  // Wireframes pedir recipiente, grupo y crudo-cocinado
#define   VERDE_BORDE_PEDIR_COCCION   0x0184  // Borde de pantalla pedir crudo-cocinado superpuesta al dashboard

#define   AMARILLO_CONFIRM_Y_AVISO    0xFEC4  // Wireframes de aviso y para confirmar acción de añadir, eliminar y guardar
#define   ROJO_TEXTO_CONFIRM_Y_AVISO  0x9883  // Wireframes de aviso y para confirmar acción de añadir, eliminar y guardar

#define   DARKORANGE                  0xCB20 // 0xC86406 en RGB-8:8:8 ==> aviso

#define   RED_ERROR_Y_CANCEL          0xF800  // Wireframes de error y cancelar
#define   RED                         0xF800  // Rojo general

#define   RED_BUTTON                  0xB8C3  // Recuadro que aparece alrededor de los botones de los wireframes de añadir, eliminar, guardar y grupos

#define   DARKPURPLE                  0x30C6  // Mini mensaje cuando guardado completo (local y database)

//#define   AZUL_PROCESO                0x1B9F  // Pantalla de sincronización, de escanear producto y de buscar info producto
#define   AZUL_PROCESO                0x037F  // Pantalla de guardando, sincronización, de escanear producto y de buscar info producto
#define   AZUL_RECUADRO_PROCESO       0x0010  // Recuadro para el texto
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



#define   BLUE          0x001F
#define   GRAY          0x7BEF





#endif