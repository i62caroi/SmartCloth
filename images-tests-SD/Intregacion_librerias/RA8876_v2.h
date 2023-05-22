/*
File Name : RA8876_v2.h                                   
Author    : Irene Casares Rodríguez                          
Edit Date : 19/05/2023
Version   : v2.0

    Version v1.0 obtenida de https://github.com/xlatb/ra8876/tree/master/src
    
    Se ha completado con funciones de RA8876_Lite de Raio, adaptándolas con las
    funciones de la librería base RA8876.

    Guía de RA8876_Lite: https://www.raio.com.tw/data_raio/RA887677/Arduino/RA8876_Lite_UserGuide_v1.0_Eng.pdf

    Raio RA8876 Technical documents: https://www.raio.com.tw/en/download_center.html 
*/

#include <cstdint>
#pragma GCC diagnostic warning "-Wall"

#ifndef RA8876_V2_H
#define RA8876_V2_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "COLORS.h"


//#define RA8876_DEBUG // Uncomment to enable debug messaging


#define RGB332(r, g, b) (((r) & 0xE0) | (((g) & 0xE0) >> 3) | (((b) & 0xE0) >> 6))
#define RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))


typedef uint8_t FontFlags;
#define RA8876_FONT_FLAG_XLAT_FULLWIDTH 0x01  // Translate ASCII to Unicode fullwidth forms

//enum CanvasMode
//{
// RA8876_CANVAS_LINEAR,
// RA8876_CANVAS_BLOCK
//};

// 1MHz. TODO: Figure out actual speed to use
// Data sheet section 5.2 says maximum SPI clock is 50MHz.
#define RA8876_SPI_SPEED      3000000  // 3MHz es lo máximo que permite mostrar texto sin problema
#define RA8876_SPI_SPEED_IMG  50000000 // 50MHz para mostrar imagen

// With SPI, the RA8876 expects an initial byte where the top two bits are meaningful. Bit 7
// is A0, bit 6 is WR#. See data sheet section 7.3.2 and section 19.
// A0: 0 for command/status, 1 for data
// WR#: 0 for write, 1 for read
#define RA8876_DATA_WRITE  0x80
#define RA8876_DATA_READ   0xC0
#define RA8876_CMD_WRITE   0x00
#define RA8876_STATUS_READ 0x40


/* Configuración de páginas (buffer de imágenes):

   El máximo número de páginas se basa en la capacidad de la SDRAM y en la profundidad de color (16bpp), 
   la anchura (width = 1024) y la altura (height = 600) de una página.

   Nuestra pantalla tiene una SDRAM modelo W9812G6KH-6, con capacidad de 128Mbit = 16MByte.
   La SDRAM se divide en varios buffers de imagen y el máximo número de buffers se limita según
   el tamaño de la memoria: 
        page_size = 1024*600*2byte(16bpp) = 1228800byte
        buffers_max_number = memory_size/page_size = 16/1.22 = 13.11

   Según datasheet 11.2.1 Configure Display Image Buffer, se debe cumplir que:

      width (1024) x height (600) x color depth (16) x maximum number of buffers (13.11) < SDRAM MBytes (16)

      128876544 bits < 16 MBytes
*/
/*vertical mulit page application*/
#define PAGE1_START_ADDR  0
#define PAGE2_START_ADDR  1024*600*2
#define PAGE3_START_ADDR  1024*600*2*2
#define PAGE4_START_ADDR  1024*600*2*3
#define PAGE5_START_ADDR  1024*600*2*4
#define PAGE6_START_ADDR  1024*600*2*5
#define PAGE7_START_ADDR  1024*600*2*6
#define PAGE8_START_ADDR  1024*600*2*7
#define PAGE9_START_ADDR  1024*600*2*8
#define PAGE10_START_ADDR 1024*600*2*9

#define PATTERN1_RAM_START_ADDR 1024*600*2*10
#define PATTERN2_RAM_START_ADDR (1024*600*2*10)+(16*16*2)
#define PATTERN3_RAM_START_ADDR (1024*600*2*10)+(16*16*2)+(16*16*2)

/*DMA picture data start address*/ 
// DMA se puede usar si se tiene memoria Flash. No es nuestro caso.
/*
#define WP1_ADDR  0
#define WP2_ADDR  1024*600*2
#define WP3_ADDR  1024*600*2*2
#define WP4_ADDR  1024*600*2*3
#define WP5_ADDR  1024*600*2*4
#define WP6_ADDR  1024*600*2*5
*/


//------ STRING LINE ------- ¿PA QUÉ? NO SÉ
#define STRING_LINE1    0
#define STRING_LINE2    32
#define STRING_LINE3    32*2
#define STRING_LINE4    32*3
#define STRING_LINE5    32*4
#define STRING_LINE6    32*5
#define STRING_LINE7    32*6
#define STRING_LINE8    32*7
#define STRING_LINE9    32*8
#define STRING_LINE10   32*9
#define STRING_LINE11   32*10
#define STRING_LINE12   32*11
#define STRING_LINE13   32*12
#define STRING_LINE14   32*13
#define STRING_LINE15   32*14
#define STRING_LINE16   32*15
#define STRING_LINE17   32*16
#define STRING_LINE18   32*17
#define STRING_LINE19   32*18
//---------------------------


// Data sheet 19.2: Chip configuration registers
#define RA8876_REG_SRR     0x00  // Software Reset Register
//#define RA8876_SOFTWARE_RESET  0xD7

#define RA8876_REG_CCR     0x01  // Chip Configuration 
//-values RA8876_REG_CCR-
#define RA8876_PLL_DISABLE          0  
#define RA8876_PLL_ENABLE           1
#define RA8876_WAIT_NO_MASK         0
#define RA8876_WAIT_MASK            1  
#define RA8876_KEY_SCAN_DISABLE     0
#define RA8876_KEY_SCAN_ENABLE      1
#define RA8876_TFT_OUTPUT24         0
#define RA8876_TFT_OUTPUT18         1
#define RA8876_TFT_OUTPUT16         3
#define RA8876_I2C_MASTER_DISABLE   0
#define RA8876_I2C_MASTER_ENABLE    1
#define RA8876_SERIAL_IF_DISABLE    0
#define RA8876_SERIAL_IF_ENABLE     1
#define RA8876_HOST_DATA_BUS_8BIT   0
#define RA8876_HOST_DATA_BUS_SERIAL 0
#define RA8876_HOST_DATA_BUS_16BIT  1
//-en values RA8876_REG_CCR-

#define RA8876_REG_MACR    0x02  // Memory Access Control Register
//-values RA8876_REG_MACR-
#define RA8876_DIRECT_WRITE             0  // (for all 8 bits MPU I/F, 16 bits MPU I/F with 8bpp data mode 1 & 2, 16 bits MPU I/F with 16/24-bpp data mode 1 & serial host interface)
#define RA8876_MASK_HIGH_BYTE           2  // (ex. 16 bit MPU I/F with 8-bpp data mode 1)
#define RA8876_MASK_HIGH_BYTE_EVEN_DATA 3  //(ex. 16 bit MPU I/F with 24-bpp data mode 2)
#define RA8876_READ_MEMORY_LRTB         0
#define RA8876_READ_MEMORY_RLTB         1
#define RA8876_READ_MEMORY_TBLR         2
#define RA8876_READ_MEMORY_BTLR         3
#define RA8876_WRITE_MEMORY_LRTB        0
#define RA8876_WRITE_MEMORY_RLTB        1
#define RA8876_WRITE_MEMORY_TBLR        2
#define RA8876_WRITE_MEMORY_BTLR        3
//-end values RA8876_REG_MACR-

#define RA8876_REG_ICR     0x03  // Input Control Register
//-values RA8876_REG_ICR-
#define RA8876_OUTPUT_INT_LEVEL_LOW           0
#define RA8876_OUTPUT_INT_LEVEL_HIGH          1
#define RA8876_EXT_INT_WITHOUT_DEBOUNCE       0
#define RA8876_EXT_INT_WITH_DEBOUNCE          1
#define RA8876_EXT_INT_LOW_LEVEL_TRIGGER      0
#define RA8876_EXT_INT_FALLING_EDGE_TRIGGER   1
#define RA8876_EXT_INT_HIGH_LEVEL_TRIGGER     2
#define RA8876_EXT_INT_RISING_EDGE_TRIGGER    3
#define RA8876_GRAPHIC_MODE                   0
#define RA8876_TEXT_MODE                      1
#define RA8876_MEMORY_SELECT_IMAGE            0
#define RA8876_MEMORY_SELECT_GAMMA            1
#define RA8876_MEMORY_SELECT_CURSOR_RAM       2
#define RA8876_MEMORY_SELECT_PALETTE_RAM      3
//-end values RA8876_REG_ICR-

#define RA8876_REG_MRWDP   0x04  // Memory Read/Write Data Port

// Data sheet 19.3: PLL setting registers
#define RA8876_REG_PPLLC1  0x05  // SCLK PLL control register 1
#define RA8876_REG_PPLLC2  0x06  // SCLK PLL control register 2
#define RA8876_REG_MPLLC1  0x07  // MCLK PLL control register 1
#define RA8876_REG_MPLLC2  0x08  // MCLK PLL control register 2
#define RA8876_REG_SPLLC1  0x09  // CCLK PLL control register 1
#define RA8876_REG_SPLLC2  0x0A  // CCLK PLL control register 2

//------ Interrupt Control Registers --------
// Data sheet 19.4: Interrupt Control Registers
#define RA8876_INTEN  0x0B // Interrupt Enable Register
#define RA8876_INTF   0x0C // Interrupt Event Flag Register
#define RA8876_MINTFR 0x0D // Mask Interrupt Flag Register
#define RA8876_PUENR  0x0E // Pull- high control Register
#define RA8876_PSFSR  0x0F // PDAT for PIO/Key Function Select Register
//--------------------------------------------

// Data sheet 19.5: LCD display control registers
#define RA8876_REG_MPWCTR  0x10  // Main/PIP Window Control Register
//-values RA8876_REG_MPWCTR-
#define RA8876_PIP1_WINDOW_DISABLE      0
#define RA8876_PIP1_WINDOW_ENABLE       1
#define RA8876_PIP2_WINDOW_DISABLE      0
#define RA8876_PIP2_WINDOW_ENABLE       1
#define RA8876_SELECT_CONFIG_PIP1       0
#define RA8876_SELECT_CONFIG_PIP2       1
#define RA8876_IMAGE_COLOCR_DEPTH_8BPP  0
#define RA8876_IMAGE_COLOCR_DEPTH_16BPP 1
#define RA8876_IMAGE_COLOCR_DEPTH_24BPP 2
#define RA8876_PANEL_SYNC_MODE          0
#define RA8876_PANEL_DE_MODE            1
//-end values RA8876_REG_MPWCTR-

#define RA8876_REG_PIPCDEP 0x11  // PIP Window Color Depth register
//-values RA8876_REG_PIPCDEP-
#define RA8876_PIP1_COLOR_DEPTH_8BPP   0
#define RA8876_PIP1_COLOR_DEPTH_16BPP  1
#define RA8876_PIP1_COLOR_DEPTH_24BPP  2
#define RA8876_PIP2_COLOR_DEPTH_8BPP   0
#define RA8876_PIP2_COLOR_DEPTH_16BPP  1
#define RA8876_PIP2_COLOR_DEPTH_24BPP  2
//-end values RA8876_REG_PIPCDEP-

#define RA8876_REG_DPCR    0x12  // Display configuration register
//-values RA8876_REG_DPCR-
#define RA8876_PCLK_NO_INVERSION  0
#define RA8876_PCLK_INVERSION     1
#define RA8876_DISPLAY_OFF        0
#define RA8876_DISPLAY_ON         1
#define RA8876_COLOR_BAR_DISABLE  0
#define RA8876_COLOR_BAR_ENABLE   1
#define RA8876_VDIR_TB            0
#define RA8876_VDIR_BT            1
#define RA8876_OUTPUT_RGB         0
#define RA8876_OUTPUT_RBG         1
#define RA8876_OUTPUT_GRB         2
#define RA8876_OUTPUT_GBR         3
#define RA8876_OUTPUT_BRG         4
#define RA8876_OUTPUT_BGR         5
#define RA8876_OUTPUT_GRAY        6
#define RA8876_OUTPUT_IDLE        7
//-end values RA8876_REG_DPCR-

#define RA8876_REG_PCSR    0x13  // Panel scan clock and data setting register
//-values RA8876_REG_PCSR-
#define RA8876_XHSYNC_LOW_ACTIVE    0
#define RA8876_XHSYNC_HIGH_ACTIVE   1
#define RA8876_XVSYNC_LOW_ACTIVE    0
#define RA8876_XVSYNC_HIGH_ACTIVE   1
#define RA8876_XDE_HIGH_ACTIVE      0
#define RA8876_XDE_LOW_ACTIVE       1
#define RA8876_XDE_IDLE_LOW         0
#define RA8876_XDE_IDLE_HIGH        1
#define RA8876_XPCLK_IDLE_LOW       0
#define RA8876_XPCLK_IDLE_HIGH      1
#define RA8876_XPDAT_IDLE_LOW       0
#define RA8876_XPDAT_IDLE_HIGH      1
#define RA8876_XHSYNC_IDLE_LOW      0
#define RA8876_XHSYNC_IDLE_HIGH     1
#define RA8876_XVSYNC_IDLE_LOW      0
#define RA8876_XVSYNC_IDLE_HIGH     1
//-end values RA8876_REG_PCSR-

#define RA8876_REG_HDWR    0x14  // Horizontal Display Width Register
#define RA8876_REG_HDWFTR  0x15  // Horizontal Display Width Fine Tuning Register
#define RA8876_REG_HNDR    0x16  // Horizontal Non-Display Period Register
#define RA8876_REG_HNDFTR  0x17  // Horizontal Non-Display Period Fine Tuning Register
#define RA8876_REG_HSTR    0x18  // HSYNC start position register
#define RA8876_REG_HPWR    0x19  // HSYNC Pulse Width Register

#define RA8876_REG_VDHR0   0x1A  // Vertical Display Height Register 0
#define RA8876_REG_VDHR1   0x1B  // Vertical Display Height Register 1
#define RA8876_REG_VNDR0   0x1C  // Vertical Non-Display Period Register 0
#define RA8876_REG_VNDR1   0x1D  // Vertical Non-Display Period Register 1
#define RA8876_REG_VSTR    0x1E  // VSYNC start position register
#define RA8876_REG_VPWR    0x1F  // VSYNC pulse width register

#define RA8876_REG_MISA0   0x20  // Main Image Start Address 0
#define RA8876_REG_MISA1   0x21  // Main Image Start Address 1
#define RA8876_REG_MISA2   0x22  // Main Image Start Address 2
#define RA8876_REG_MISA3   0x23  // Main Image Start Address 3

#define RA8876_REG_MIW0    0x24  // Main Image Width 0
#define RA8876_REG_MIW1    0x25  // Main Image Width 1
#define RA8876_REG_MWULX0  0x26  // Main Window Upper-Left X coordinate 0
#define RA8876_REG_MWULX1  0x27  // Main Window Upper-Left X coordinate 1
#define RA8876_REG_MWULY0  0x28  // Main Window Upper-Left Y coordinate 0
#define RA8876_REG_MWULY1  0x29  // Main Window Upper-Left Y coordinate 1

#define RA8876_PWDULX0  0x2A
#define RA8876_PWDULX1  0x2B
#define RA8876_PWDULY0  0x2C
#define RA8876_PWDULY1  0x2D
#define RA8876_PISA0    0x2E
#define RA8876_PISA1    0x2F
#define RA8876_PISA2    0x30
#define RA8876_PISA3    0x31
#define RA8876_PIW0     0x32
#define RA8876_PIW1     0x33
#define RA8876_PWIULX0  0x34
#define RA8876_PWIULX1  0x35
#define RA8876_PWIULY0  0x36
#define RA8876_PWIULY1  0x37
#define RA8876_PWW0     0x38
#define RA8876_PWW1     0x39
#define RA8876_PWH0     0x3A
#define RA8876_PWH1     0x3B

#define RA8876_GTCCR    0x3C
#define RA8876_BTCR     0x3D
#define RA8876_CURHS    0x3E
#define RA8876_CURVS    0x3F

#define RA8876_GCHP0    0x40
#define RA8876_GCHP1    0x41
#define RA8876_GCVP0    0x42
#define RA8876_GCVP1    0x43
#define RA8876_GCC0     0x44
#define RA8876_GCC1     0x45

// Data sheet 19.6: Geometric engine control registers
#define RA8876_REG_CVSSA0     0x50  // Canvas Start Address 0
#define RA8876_REG_CVSSA1     0x51  // Canvas Start Address 1
#define RA8876_REG_CVSSA2     0x52  // Canvas Start Address 2
#define RA8876_REG_CVSSA3     0x53  // Canvas Start Address 3
#define RA8876_REG_CVS_IMWTH0 0x54  // Canvas image width 0
#define RA8876_REG_CVS_IMWTH1 0x55  // Canvas image width 1

#define RA8876_REG_AWUL_X0    0x56  // Active Window Upper-Left X coordinate 0
#define RA8876_REG_AWUL_X1    0x57  // Active Window Upper-Left X coordinate 1
#define RA8876_REG_AWUL_Y0    0x58  // Active Window Upper-Left Y coordinate 0
#define RA8876_REG_AWUL_Y1    0x59  // Active Window Upper-Left Y coordinate 1
#define RA8876_REG_AW_WTH0    0x5A  // Active Window Width 0
#define RA8876_REG_AW_WTH1    0x5B  // Active Window Width 1
#define RA8876_REG_AW_HT0     0x5C  // Active Window Height 0
#define RA8876_REG_AW_HT1     0x5D  // Active Window Height 1

#define RA8876_REG_AW_COLOR   0x5E  // Color Depth of canvas & active window
//-values -
#define RA8876_CANVAS_BLOCK_MODE         0
#define RA8876_CANVAS_LINEAR_MODE        1
#define RA8876_CANVAS_COLOR_DEPTH_8BPP   0
#define RA8876_CANVAS_COLOR_DEPTH_16BPP  1
#define RA8876_CANVAS_COLOR_DEPTH_24BPP  2
//-end values -

#define RA8876_REG_CURH0      0x5F  // Graphic read/write horizontal position 0
#define RA8876_REG_CURH1      0x60  // Graphic read/write horizontal position 1
#define RA8876_REG_CURV0      0x61  // Graphic read/write vertical position 0
#define RA8876_REG_CURV1      0x62  // Graphic read/write vertical position 1

#define RA8876_REG_F_CURX0    0x63  // Text cursor X-coordinate register 0
#define RA8876_REG_F_CURX1    0x64  // Text cursor X-coordinate register 1
#define RA8876_REG_F_CURY0    0x65  // Text cursor Y-coordinate register 0
#define RA8876_REG_F_CURY1    0x66  // Text cursor Y-coordinate register 1

#define RA8876_REG_DCR0       0x67  // Draw shape control register 0
//-valores para dibujar 1 (linea y triangulo)-
#define RA8876_DRAW_LINE            0x80
#define RA8876_DRAW_TRIANGLE        0x82 
#define RA8876_DRAW_TRIANGLE_FILL   0xA2 
//-end valore para dibujar 1-

#define RA8876_REG_DLHSR0     0x68  // Draw shape point 1 X coordinate register 0
#define RA8876_REG_DLHSR1     0x69  // Draw shape point 1 X coordinate register 1
#define RA8876_REG_DLVSR0     0x6A  // Draw shape point 1 Y coordinate register 0
#define RA8876_REG_DLVSR1     0x6B  // Draw shape point 1 Y coordinate register 1

#define RA8876_REG_DLHER0     0x6C  // Draw shape point 2 X coordinate register 0
#define RA8876_REG_DLHER1     0x6D  // Draw shape point 2 X coordinate register 1
#define RA8876_REG_DLVER0     0x6E  // Draw shape point 2 Y coordinate register 0
#define RA8876_REG_DLVER1     0x6F  // Draw shape point 2 Y coordinate register 1

#define RA8876_REG_DTPH0      0x70  // Draw shape point 3 X coordinate register 0
#define RA8876_REG_DTPH1      0x71  // Draw shape point 3 X coordinate register 1
#define RA8876_REG_DTPV0      0x72  // Draw shape point 3 Y coordinate register 0
#define RA8876_REG_DTPV1      0x73  // Draw shape point 3 Y coordinate register 1

#define RA8876_REG_DCR1       0x76  // Draw shape control register 1
//-valores para dibujar 2 (circulo, elipse, cuadrado y cuadrado redondeado)-
#define RA8876_DRAW_CIRCLE                  0x80
#define RA8876_DRAW_CIRCLE_FILL             0xC0
#define RA8876_DRAW_ELLIPSE                 0x80
#define RA8876_DRAW_ELLIPSE_FILL            0xC0
#define RA8876_DRAW_BOTTOM_LEFT_CURVE       0x90
#define RA8876_DRAW_BOTTOM_LEFT_CURVE_FILL  0xD0
#define RA8876_DRAW_UPPER_LEFT_CURVE        0x91
#define RA8876_DRAW_UPPER_LEFT_CURVE_FILL   0xD1
#define RA8876_DRAW_UPPER_RIGHT_CURVE       0x92
#define RA8876_DRAW_UPPER_RIGHT_CURVE_FILL  0xD2
#define RA8876_DRAW_BOTTOM_RIGHT_CURVE      0x93
#define RA8876_DRAW_BOTTOM_RIGHT_CURVE_FILL 0xD3
#define RA8876_DRAW_SQUARE                  0xA0
#define RA8876_DRAW_SQUARE_FILL             0xE0
#define RA8876_DRAW_ROUND_RECT              0xB0
#define RA8876_DRAW_ROUND_RECT_FILL         0xF0
//-end valores dibujar 2-

#define RA8876_REG_ELL_A0     0x77  // Draw ellipse major radius 0
#define RA8876_REG_ELL_A1     0x78  // Draw ellipse major radius 1
#define RA8876_REG_ELL_B0     0x79  // Draw ellipse minor radius 0
#define RA8876_REG_ELL_B1     0x7A  // Draw ellipse minor radius 1
#define RA8876_REG_DEHR0      0x7B  // Draw ellipse centre X coordinate register 0
#define RA8876_REG_DEHR1      0x7C  // Draw ellipse centre X coordinate register 1
#define RA8876_REG_DEVR0      0x7D  // Draw ellipse centre Y coordinate register 0
#define RA8876_REG_DEVR1      0x7E  // Draw ellipse centre Y coordinate register 1

// Data sheet 19.7: PWM timer control registers
#define RA8876_REG_PSCLR      0x84  // PWM prescaler register
//#define RA8876_PRESCALER  0x03

#define RA8876_REG_PMUXR      0x85  // PWM clock mux register
//-values RA8876_REG_PMUXR-
#define RA8876_PWM_TIMER_DIV1             0
#define RA8876_PWM_TIMER_DIV2             1
#define RA8876_PWM_TIMER_DIV4             2
#define RA8876_PWM_TIMER_DIV8             3
#define RA8876_XPWM1_OUTPUT_ERROR_FLAG    0
#define RA8876_XPWM1_OUTPUT_PWM_TIMER1    2
#define RA8876_XPWM1_OUTPUT_OSC_CLK       3
#define RA8876_XPWM0_GPIO_C7              0
#define RA8876_XPWM0_OUTPUT_PWM_TIMER0    2
#define RA8876_XPWM0_OUTPUT_CORE_CLK      3
//-end values RA8876_REG_PMUXR-

#define RA8876_REG_PCFGR      0x86  // PWM configuration register
//-values RA8876_REG_PCFGR-
#define RA8876_PWM_TIMER1_INVERTER_OFF        0
#define RA8876_PWM_TIMER1_INVERTER_ON         1
#define RA8876_PWM_TIMER1_ONE_SHOT            0
#define RA8876_PWM_TIMER1_AUTO_RELOAD         1
#define RA8876_PWM_TIMER1_STOP                0
#define RA8876_PWM_TIMER1_START               1
#define RA8876_PWM_TIMER0_DEAD_ZONE_DISABLE   0
#define RA8876_PWM_TIMER0_DEAD_ZONE_ENABLE    1
#define RA8876_PWM_TIMER0_INVERTER_OFF        0
#define RA8876_PWM_TIMER0_INVERTER_ON         1
#define RA8876_PWM_TIMER0_ONE_SHOT            0
#define RA8876_PWM_TIMER0_AUTO_RELOAD         1
#define RA8876_PWM_TIMER0_STOP                0
#define RA8876_PWM_TIMER0_START               1
//-end values RA8876_REG_PCFGR-

#define RA8876_DZ_LENGTH   0x87
#define RA8876_TCMPB0L     0x88
#define RA8876_TCMPB0H     0x89
#define RA8876_TCNTB0L     0x8A
#define RA8876_TCNTB0H     0x8B
#define RA8876_TCMPB1L     0x8C
#define RA8876_TCMPB1H     0x8D
#define RA8876_TCNTB1L     0x8E
#define RA8876_TCNTB1F     0x8F


//----------------- BLOCK TRANSFER ENGINE (BTE) --------------------
// Data sheet 19.8: Block Transfer Engine (BTE) Control Registers
#define RA8876_BTE_CTRL0   0x90
//-values RA8876_BTE_CTRL0-
#define RA8876_BTE_ENABLE          1
#define RA8876_PATTERN_FORMAT8X8   0
#define RA8876_PATTERN_FORMAT16X16 1
//-end values RA8876_BTE_CTRL0-

#define RA8876_BTE_CTRL1  0x91
//-values RA8876_BTE_CTRL1-
#define RA8876_BTE_ROP_CODE_0       0   //0 ( Blackness ) 
#define RA8876_BTE_ROP_CODE_1       1   //~S0・~S1 or ~ ( S0+S1 ) 
#define RA8876_BTE_ROP_CODE_2       2   //~S0・S1
#define RA8876_BTE_ROP_CODE_3       3   //~S0
#define RA8876_BTE_ROP_CODE_4       4   //S0・~S1
#define RA8876_BTE_ROP_CODE_5       5   //~S1
#define RA8876_BTE_ROP_CODE_6       6   //S0^S1
#define RA8876_BTE_ROP_CODE_7       7   //~S0+~S1 or ~ ( S0・S1 ) 
#define RA8876_BTE_ROP_CODE_8       8   //S0・S1
#define RA8876_BTE_ROP_CODE_9       9   //~ ( S0^S1 ) 
#define RA8876_BTE_ROP_CODE_10      10  //S1
#define RA8876_BTE_ROP_CODE_11      11  //~S0+S1
#define RA8876_BTE_ROP_CODE_12      12  //S0
#define RA8876_BTE_ROP_CODE_13      13  //S0+~S1
#define RA8876_BTE_ROP_CODE_14      14  //S0+S1
#define RA8876_BTE_ROP_CODE_15      15  //1 ( Whiteness ) 
#define RA8876_BTE_ROP_BUS_WIDTH8   7
#define RA8876_BTE_ROP_BUS_WIDTH16  15
//-end values RA8876_BTE_CTRL1-

#define RA8876_BTE_MPU_WRITE_WITH_ROP                       0  
#define RA8876_BTE_MEMORY_COPY_WITH_ROP                     2
#define RA8876_BTE_MPU_WRITE_WITH_CHROMA                    4
#define RA8876_BTE_MEMORY_COPY_WITH_CHROMA                  5
#define RA8876_BTE_PATTERN_FILL_WITH_ROP                    6
#define RA8876_BTE_PATTERN_FILL_WITH_CHROMA                 7
#define RA8876_BTE_MPU_WRITE_COLOR_EXPANSION                8
#define RA8876_BTE_MPU_WRITE_COLOR_EXPANSION_WITH_CHROMA    9
#define RA8876_BTE_MEMORY_COPY_WITH_OPACITY                 10
#define RA8876_BTE_MPU_WRITE_WITH_OPACITY                   11
#define RA8876_BTE_SOLID_FILL                               12
#define RA8876_BTE_MEMORY_COPY_WITH_COLOR_EXPANSION         14
#define RA8876_BTE_MEMORY_COPY_WITH_COLOR_EXPANSION_CHROMA  15

#define RA8876_BTE_COLR  0x92
//-values RA8876_BTE_COLR-
#define RA8876_S0_COLOR_DEPTH_8BPP            0
#define RA8876_S0_COLOR_DEPTH_16BPP           1
#define RA8876_S0_COLOR_DEPTH_24BPP           2
#define RA8876_S1_COLOR_DEPTH_8BPP            0
#define RA8876_S1_COLOR_DEPTH_16BPP           1
#define RA8876_S1_COLOR_DEPTH_24BPP           2
#define RA8876_S1_CONSTANT_COLOR              3
#define RA8876_S1_8BIT_PIXEL_ALPHA_BLENDING   4
#define RA8876_S1_16BIT_PIXEL_ALPHA_BLENDING  5
#define RA8876_DESTINATION_COLOR_DEPTH_8BPP   0
#define RA8876_DESTINATION_COLOR_DEPTH_16BPP  1
#define RA8876_DESTINATION_COLOR_DEPTH_24BPP  2
//-end values RA8876_BTE_COLR-

#define RA8876_S0_STR0   0x93
#define RA8876_S0_STR1   0x94
#define RA8876_S0_STR2   0x95
#define RA8876_S0_STR3   0x96
#define RA8876_S0_WTH0   0x97
#define RA8876_S0_WTH1   0x98
#define RA8876_S0_X0     0x99
#define RA8876_S0_X1     0x9A
#define RA8876_S0_Y0     0x9B
#define RA8876_S0_Y1     0x9C
#define RA8876_S1_STR0   0x9D
#define RA8876_S1_STR1   0x9E
#define RA8876_S1_STR2   0x9F
#define RA8876_S1_RED    0x9D
#define RA8876_S1_GREEN  0x9E
#define RA8876_S1_BLUE   0x9F
#define RA8876_S1_STR3   0xA0
#define RA8876_S1_WTH0   0xA1
#define RA8876_S1_WTH1   0xA2

#define RA8876_S1_X0     0xA3
#define RA8876_S1_X1     0xA4
#define RA8876_S1_Y0     0xA5
#define RA8876_S1_Y1     0xA6
#define RA8876_DT_STR0   0xA7
#define RA8876_DT_STR1   0xA8
#define RA8876_DT_STR2   0xA9
#define RA8876_DT_STR3   0xAA
#define RA8876_DT_WTH0   0xAB
#define RA8876_DT_WTH1   0xAC
#define RA8876_DT_X0     0xAD
#define RA8876_DT_X1     0xAE
#define RA8876_DT_Y0     0xAF
#define RA8876_DT_Y1     0xB0
#define RA8876_BTE_WTH0  0xB1
#define RA8876_BTE_WTH1  0xB2
#define RA8876_BTE_HIG0  0xB3
#define RA8876_BTE_HIG1  0xB4
#define RA8876_APB_CTRL  0xB5
//------------------------------------------------------------------


// Data sheet 19.9: Serial flash & SPI master control registers
#define RA8876_DMA_CTRL  0xB6
//-values RA8876_DMA_CTRL-
#define RA8876_DMA_START  1
//-end values RA8876_DMA_CTRL-

#define RA8876_REG_SFL_CTRL   0xB7  // Serial flash/ROM control register
//-values RA8876_REG_SFL_CTRL-
#define RA8876_SERIAL_FLASH_SELECT0     0
#define RA8876_SERIAL_FLASH_SELECT1     1
#define RA8876_SERIAL_FLASH_FONT_MODE   0
#define RA8876_SERIAL_FLASH_DMA_MODE    1
#define RA8876_SERIAL_FLASH_ADDR_24BIT  0
#define RA8876_SERIAL_FLASH_ADDR_32BIT  1
#define RA8876_STANDARD_SPI_MODE        0
#define RA8876_FOLLOW_RA8876_MODE       1
#define RA8876_SPI_NORMAL_READ          0   //COMMAND 03h
#define RA8876_SPI_FAST_READ_8DUMMY     4   //COMMAND 0Bh
#define RA8876_SPI_FAST_READ_16DUMMY    8   //COMMAND 1Bh
#define RA8876_SPI_DUAL_READ_8DUMMY     2   //COMMAND 3Bh
#define RA8876_2XIO_READ_MODE_4DUMMY    3   //COMMAND BBh
//-end values RA8876_REG_SFL_CTRL-

#define RA8876_SPIDR  0xB8
#define RA8876_SPIMCR2   0xB9
//-values RA8876_SPIMCR2-
#define RA8876_SPIM_INT_DISABLE       0
#define RA8876_SPIM_INT_ENABLE        1
#define RA8876_SPIM_NSS_SELECT_0      0
#define RA8876_SPIM_NSS_SELECT_1      1
#define RA8876_SPIM_NSS_INACTIVE      0  //nSS port will goes high
#define RA8876_SPIM_NSS_ACTIVE        1  //nSS port will goes low
#define RA8876_SPIM_OVFIRQMSK_UNMASK  0
#define RA8876_SPIM_OVFIRQMSK_MASK    1 
#define RA8876_SPIM_EMTIRQMSK_UNMASK  0
#define RA8876_SPIM_EMTIRQMSK_MASK    1
#define RA8876_SPIM_MODE0             0
#define RA8876_SPIM_MODE1             1
#define RA8876_SPIM_MODE2             2
#define RA8876_SPIM_MODE3             3
//-end values RA8876_SPIMCR2-

#define RA8876_SPIMSR  0xBA

#define RA8876_REG_SPI_DIVSOR 0xBB  // SPI clock period
//-values RA8876_REG_SPI_DIVSOR-
#define RA8876_SPI_DIV2   0
#define RA8876_SPI_DIV4   1
#define RA8876_SPI_DIV6   2
#define RA8876_SPI_DIV8   3
#define RA8876_SPI_DIV10  4
//-end values RA8876_REG_SPI_DIVSOR-

#define RA8876_DMA_SSTR0  0xBC
#define RA8876_DMA_SSTR1  0xBD
#define RA8876_DMA_SSTR2  0xBE
#define RA8876_DMA_SSTR3  0xBF

#define RA8876_DMA_DX0    0xC0
#define RA8876_DMA_DX1    0xC1
#define RA8876_DMA_DY0    0xC2
#define RA8876_DMA_DY1    0xC3

#define RA8876_DMAW_WTH0  0xC6
#define RA8876_DMAW_WTH1  0xC7
#define RA8876_DMAW_HIGH0 0xC8
#define RA8876_DMAW_HIGH1 0xC9
#define RA8876_DMA_SWTH0  0xCA
#define RA8876_DMA_SWTH1  0xCB


// Data sheet 19.10: Text engine
#define RA8876_REG_CCR0         0xCC  // Datasheet 19.10: Text engine => REG[CCh] Character Control Register 0 (CCR0) 
//-values RA8876_REG_CCR0-
enum FontSource{                    // CCR0 ==> Character source selection ==> Bit 7-6
  RA8876_FONT_SOURCE_INTERNAL = 0x00,  // CGROM with four 8-bit ISO Latin variants
  RA8876_FONT_SOURCE_EXT_ROM  = 0x01,  // External font ROM chip
  RA8876_FONT_USER_DEFINED    = 0x02   // User-defined
};
enum FontSize{                      // CCR0 ==> Character Height Setting ==> Bit 5-4
  RA8876_FONT_SIZE_16         = 0x00,         // 8x16 / 16x16 / variable character width x 16
  RA8876_FONT_SIZE_24         = 0x01,         // 12x24 / 24x24 / variable character width x 24
  RA8876_FONT_SIZE_32         = 0x02          // 16x32 / 32x32 / variable character width x 32
};
enum CharacterSelection{            // CCR0 ==> Character Selection for internal CGROM ==> Bit 1-0
  RA8876_SELECT_8859_1        = 0x00,
  RA8876_SELECT_8859_2        = 0x01,
  RA8876_SELECT_8859_4        = 0x02,
  RA8876_SELECT_8859_5        = 0x03
};
//-end values RA8876_REG_CCR0-


#define RA8876_REG_CCR1       0xCD  // Character Control Register 1
//-values RA8876_REG_CCR1-
// Alineación entre caracteres
//#define RA8876_TEXT_FULL_ALIGN_DISABLE   0
//#define RA8876_TEXT_FULL_ALIGN_ENABLE    1
// Transparencia de background
#define RA8876_TEXT_TRANS_OFF   0 // Transparencia OFF: background especificado
#define RA8876_TEXT_TRANS_ON    1 // Transparencia ON: ignorar background
// Rotación
#define RA8876_TEXT_NO_ROTATION 0
#define RA8876_TEXT_ROTATION    1  
// Escala en anchura
#define RA8876_TEXT_W_SCALE_X1  0 // Escala x1 en anchura
#define RA8876_TEXT_W_SCALE_X2  1 // Escala x2 en anchura
#define RA8876_TEXT_W_SCALE_X3  2 // Escala x3 en anchura
#define RA8876_TEXT_W_SCALE_X4  3 // Escala x4 en anchura
// Escala en altura
#define RA8876_TEXT_H_SCALE_X1  0 // Escala x1 en altura
#define RA8876_TEXT_H_SCALE_X2  1 // Escala x2 en altura
#define RA8876_TEXT_H_SCALE_X3  2 // Escala x3 en altura
#define RA8876_TEXT_H_SCALE_X4  3 // Escala x4 en altura
//-end values RA8876_REG_CCR1-


#define RA8876_REG_GTFNT_SEL  0xCE  // REG[CEh] GT Character ROM Select (GTFNT_SEL)
//-values RA8876_REG_GTFNT_SEL-
enum ExternalFontRom{ // GTFNT_SEL ==> Genitop (GT) Serial Character ROM Select ==> Bit 7-5
  RA8876_FONT_ROM_GT21L16T1W = 0x00,
  RA8876_FONT_ROM_GT30L16U2W = 0x01, //ESTA ES LA DE NUESTRA PANTALLA
  RA8876_FONT_ROM_GT30L24T3Y = 0x02,
  RA8876_FONT_ROM_GT30L24M1Z = 0x03,
  RA8876_FONT_ROM_GT30L32S4W = 0x04,
  RA8876_FONT_ROM_GT20L24F6Y = 0x05,
  RA8876_FONT_ROM_GT21L24S1W = 0x06
};
//-end values RA8876_REG_GTFNT_SEL-


#define RA8876_REG_GTFNT_CR   0xCF  // Datasheet 19.10 Text Engine ==> REG[CFh] GT Character ROM Control register (GTFNT_CR)
//-values RA8876_REG_GTFNT_CR-
enum FontEncoding{ // GTFNT_CR ==> Character Sets ==> Bit 7-3
  RA8876_FONT_ENCODING_UNICODE  = 0x03,  // Unicode (UCS-2?)
  RA8876_FONT_ENCODING_ASCII    = 0x04,  // ASCII
  RA8876_FONT_ENCODING_8859_1   = 0x11,  // ISO 8859-1 (Latin 1)
  RA8876_FONT_ENCODING_8859_2   = 0x12,  // ISO 8859-2 (Latin 2: Eastern European)
  RA8876_FONT_ENCODING_8859_3   = 0x13,  // ISO 8859-3 (Latin 3: South European)
  RA8876_FONT_ENCODING_8859_4   = 0x14,  // ISO 8859-4 (Latin 4: Northern European)
  RA8876_FONT_ENCODING_8859_5   = 0x15,  // ISO 8859-5 (Latin/Cyrillic)
  RA8876_FONT_ENCODING_8859_7   = 0x16,  // ISO 8859-7 (Latin/Greek)
  RA8876_FONT_ENCODING_8859_8   = 0x17,  // ISO 8859-8 (Latin/Hebrew)
  RA8876_FONT_ENCODING_8859_9   = 0x18,  // ISO 8859-9 (Latin 5: Turkish)
  RA8876_FONT_ENCODING_8859_10  = 0x19,  // ISO 8859-10 (Latin 6: Nordic)
  RA8876_FONT_ENCODING_8859_11  = 0x1A,  // ISO 8859-11 (Latin/Thai)
  RA8876_FONT_ENCODING_8859_13  = 0x1B,  // ISO 8859-13 (Latin 7: Baltic Rim)
  RA8876_FONT_ENCODING_8859_14  = 0x1C,  // ISO 8859-14 (Latin 8: Celtic)
  RA8876_FONT_ENCODING_8859_15  = 0x1D,  // ISO 8859-15 (Latin 9: Western European)
  RA8876_FONT_ENCODING_8859_16  = 0x1E   // ISO 8859-16 (Latin 10: South-Eastern European)
};
enum ExternalFontFamily{ // GTFNT_CR ==> Character width setting ==> Bit 1-0 
                         // La relación entre 'Character sets' y 'Character width' marca el tipo de fuente (fixed, Arial, Times New Roman, Bold)
  RA8876_FONT_FAMILY_FIXED      = 0x00,
  RA8876_FONT_FAMILY_ARIAL      = 0x01,
  RA8876_FONT_FAMILY_TIMES      = 0x02,
  RA8876_FONT_FAMILY_FIXED_BOLD = 0x03
};
//-end values RA8876_REG_GTFNT_CR-


#define RA8876_REG_FLDR          0xD0  // Chracter line gap register
#define RA8876_REG_F2FSSR        0xD1  // Chracter to character space setting register
//---- Foreground color registers ----
#define RA8876_REG_FGCR          0xD2  // Foreground colour register - red
#define RA8876_REG_FGCG          0xD3  // Foreground colour register - green
#define RA8876_REG_FGCB          0xD4  // Foreground colour register - blue
//------------------------------------
//---- Background color registers ----
#define RA8876_REG_BGCR          0xD5 //Background Color Register - Red (BGCR)
#define RA8876_REG_BGCG          0xD6 //Background Color Register - Green (BGCG)
#define RA8876_REG_BGCB          0xD7 //Background Color Register - Blue (BGCB)
//------------------------------------
#define RA8876_CGRAM_STR0        0xDB
#define RA8876_CGRAM_STR1        0xDC
#define RA8876_CGRAM_STR2        0xDD
#define RA8876_CGRAM_STR3        0xDE

#define RA8876_PMU               0xDF
#define RA8876_POWER_NORMAL_STATE  0x00
#define RA8876_WAKE_UP           0x00
#define RA8876_STANDBY_MODE      0x81
#define RA8876_SUSPEND_MODE      0x82
#define RA8876_SLEEP_MODE        0x83


// Data sheet 19.12: SDRAM control registers
#define RA8876_REG_SDRAR         0xE0  // SDRAM attribute register
//-values RA8876_REG_SDRAR-
#define RA8876_SDRAM_POWER_DOWN   0
#define RA8876_SDRAM_SELF_REFRESH 1
#define RA8876_SDRAM_SDR          0
#define RA8876_SDRAM_MOBILE       1
#define RA8876_SDRAM_2BANKS       0
#define RA8876_SDRAM_4BANKS       1
#define RA8876_SDRAM_ROW_ADDR_2K  0 
#define RA8876_SDRAM_ROW_ADDR_4K  1
#define RA8876_SDRAM_ROW_ADDR_8K  2
#define RA8876_SDRAM_COLUMN_256   0
#define RA8876_SDRAM_COLUMN_512   1
#define RA8876_SDRAM_COLUMN_1024  2
#define RA8876_SDRAM_COLUMN_2048  3
#define RA8876_SDRAM_COLUMN_4096  4
//-end values RA8876_REG_SDRAR-

#define RA8876_REG_SDRMD         0xE1  // SDRAM mode & extended mode register
//-values RA8876_REG_SDRMD-
#define RA8876_SDRAM_FULL_ARRAY                 0
#define RA8876_SDRAM_HALF_ARRAY                 1
#define RA8876_SDRAM_QUARTER_ARRAY              2
#define RA8876_SDRAM_ONE_EIGHTH_ARRAY           5
#define RA8876_SDRAM_ONE_SIXTEENTH_ARRAY        6
#define RA8876_SDRAM_FULL_STRENGTH_DRIVER       0
#define RA8876_SDRAM_HALF_STRENGTH_DRIVER       1
#define RA8876_SDRAM_QUARTER_STRENGTH_DRIVER    2
#define RA8876_SDRAM_ONE_EIGHT_STRENGTH_DRIVER  3
#define RA8876_SDRAM_CAS_LATENCY_2              2
#define RA8876_SDRAM_CAS_LATENCY_3              3
//-end values RA8876_REG_SDRMD-

#define RA8876_REG_SDR_REF_ITVL0 0xE2  // SDRAM auto refresh interval 0
#define RA8876_REG_SDR_REF_ITVL1 0xE3  // SDRAM auto refresh interval 1
#define RA8876_REG_SDRCR         0xE4  // SDRAM control register
//-values RA8876_REG_SDRCR-
#define RA8876_SDRAM_BREAK_LEN_256        0
#define RA8876_SDRAM_BREAK_LEN_128        1
#define RA8876_SDRAM_BREAK_LEN_64         2
#define RA8876_SDRAM_BREAK_LEN_32         3
#define RA8876_SDRAM_BUS_WIDTH_16         0
#define RA8876_SDRAM_BUS_WIDTH_32         1
#define RA8876_SDRAM_XMCKE_DISABLE        0
#define RA8876_SDRAM_XMCKE_ENABLE         1
#define RA8876_SDRAM_DISABLE_WARNING      0
#define RA8876_SDRAM_ENABLE_WARNING       1
#define RA8876_SDRAM_TIMING_PARA_DISABLE  0
#define RA8876_SDRAM_TIMING_PARA_ENABLE   1
#define RA8876_SDRAM_ENTER_POWER_SAVING   1 //0 to 1 transition will enter power saving mode
#define RA8876_SDRAM_EXIT_POWER_SAVING    0  //1 to 0 transition will exit power saving mode
#define RA8876_SDRAM_INITIALIZE           1  // An 0 to 1 transition will execute SDRAM initialization procedure.
//-end values RA8876_REG_SDRCR-

#define RA8876_SDRAM_PARAMETER1   0xE0
#define RA8876_SDRAM_PARAMETER2   0xE1
#define RA8876_SDRAM_PARAMETER3   0xE2
#define RA8876_SDRAM_PARAMETER4   0xE3

// Data sheet 19.13: IIC Master registers
#define RA8876_I2CMCPR0   0xE5
#define RA8876_I2CMCPR1   0xE6
#define RA8876_I2CMTXR    0xE7
#define RA8876_I2CMRXR    0xE8
#define RA8876_I2CMCMDR   0xE9
#define RA8876_I2CMSTUR   0xEA

// Data sheet 19.14: GPI & GPIO register
#define RA8876_GPIOAD   0xF0
#define RA8876_GPIOA    0xF1
#define RA8876_GPIOB    0xF2
#define RA8876_GPIOCD   0xF3
#define RA8876_GPIOC    0xF4
#define RA8876_GPIODD   0xF5
#define RA8876_GPIOD    0xF6
#define RA8876_GPIOED   0xF7
#define RA8876_GPIOE    0xF8
#define RA8876_GPIOFD   0xF9
#define RA8876_GPIOF    0xFA

// Data sheet 19.15: Key-Scan Control registers
#define RA8876_KSCR1    0xFB
#define RA8876_KSCR2    0xFC
#define RA8876_KSDR0    0xFD
#define RA8876_KSDR1    0xFE
#define RA8876_KSDR2    0xFF



// --- COLORES ----
// 8bpp (1 byte/pixel ) => RGB332
// 16bpp (2 bytes/pixel) Colores en formato RGB565 ==> http://www.barth-dev.de/online/rgb565-color-picker/
// 24bpp (3 bytes/pixel o 4 bytes/pixel) ==> RGB888

#define NEGRO         0x0000
#define BLANCO        0xFFFF
#define ROJO          0xF800 //0xF920 | 0x8000 | 0xfc10 | 0xfa02
#define NARANJA       0xFC80
#define AMARILLO      0xFFC0
#define VERDE         0x07C0
#define CIAN          0x07FF
#define AZUL          0x019F
#define ROSA          0xFA1F
#define MORADO        0x9112
#define MARRON        0xABC8
//-----------------------------


#define BUFFPIXEL     40
//Increasing the buffer 
// size takes more of the Arduino's precious RAM but 
// makes loading a little faster.  20 pixels seems a 
// good balance. 







// Datasheet 8.1.2 SDRAM Connection
struct SdramInfo
{
  int       speed;       // MHz
  int       casLatency;  // CAS latency (2 or 3)
  int       banks;       // Banks (2 or 4)
  int       rowBits;     // Row addressing bits (11 to 13 bits)
  int       colBits;     // Column addressing bits (8 to 12 bits)
  int       refresh;     // Refresh time in microseconds
};

struct DisplayInfo
{
  uint16_t  width;     // Display width
  uint16_t  height;    // Display height
  
  uint32_t  dotClock;  // Pixel clock in kHz
  
  int       hFrontPorch;  // Will be rounded to the nearest multiple of 8
  int       hBackPorch;
  int       hPulseWidth;  // Will be rounded to the nearest multiple of 8
  
  int       vFrontPorch;
  int       vBackPorch;
  int       vPulseWidth;
};

// Data sheet section 6.1.
// Output frequency is: (_oscClock * (n + 1)) / (2 ** k)
// There is also a PLL parameter named 'm', but it's unclear how its value could ever be non-zero.
//  When it is zero, the divisor is (2 ** 0) = 1, so we simply ignore it.
struct PllParams
{
  uint32_t    freq;  // Frequency in kHz
  int         n;     // Multiplier less 1 (range 1..63)
  int         k;     // Divisor power of 2 (range 0..3 for CCLK/MCLK; range 0..7 for SCLK)
};

struct ExternalFontRomInfo
{
  bool        present;
  int         spiInterface;          // SPI interface that font ROM is connected to (0 or 1)
  int         spiClockDivisor;       // SPI interface clock divisor (2..512 in steps of 2)
  enum        ExternalFontRom chip;  // Chip type
};





class RA8876 : public Print
{
  
private:

  int                 _csPin;
  int                 _resetPin;

  uint16_t            _width;
  uint16_t            _height;
  uint8_t             _depth;

  uint32_t            _oscClock;    // OSC clock (external crystal) frequency in kHz

  PllParams           _memPll;      // MCLK (memory) PLL parameters
  PllParams           _corePll;     // CCLK (core) PLL parameters
  PllParams           _scanPll;     // SCLK (LCD panel scan) PLL parameters

  SPISettings         _spiSettings;

  SdramInfo           *_sdramInfo;

  DisplayInfo         *_displayInfo;

  ExternalFontRomInfo _fontRomInfo;

  uint16_t            _textForeColor;
  uint16_t            _textBackColor;
  uint8_t             _textBackTrans; //0 (OFF) o 1 (ON)

  uint16_t            _cursorX;
  uint16_t            _cursorY;

  uint8_t             _textScaleX;
  uint8_t             _textScaleY;

  uint8_t             _align;

  enum FontSource     _fontSource;
  enum FontSize       _fontSize;
  FontFlags           _fontFlags;

  // -------- RESET --------------------------------------------- */
  void      _hardReset(void);
  void      _softReset(void);
  // ------------------------------------------------------------ */

  // -------- ACCESS -------------------------------------------- */
  void      _writeCmd(uint8_t reg);                       // lcdRegWrite() en RA8876_Lite
  void      _writeData(uint8_t data);                     // lcdDataWrite() en RA8876_Lite
  void      _writeData16bbp(uint16_t data);               // lcdDataWrite16bbp() en RA8876_Lite
  void      _writeData64bits(uint64_t data);     
  void      _writeData256bits(uint64_t *data);   
  //void    _writeData1024bits(uint64_t *data); 
  //void    _writeData4096bits(uint64_t *data);  
  uint8_t   _readData(void);                              // lcdDataRead()  en RA8876_Lite
  uint8_t   _readStatus(void);                            // lcdStatusRead() en RA8876_Lite
  void      _writeReg(uint8_t reg, uint8_t data);         // lcdRegDataWrite() en RA8876_Lite
  void      _writeReg16(uint8_t reg, uint16_t data);
  void      _writeReg32(uint8_t reg, uint32_t data);
  uint8_t   _readReg(uint8_t reg);                        // lcdRegDataRead() en RA8876_Lite
  uint16_t  _readReg16(uint8_t reg);
  // ------------------------------------------------------------ */

  // -------- STATUS -------------------------------------------- */
  void      _checkWriteFifoNotFull(void);
  void      _checkWriteFifoEmpty(void);                 
  //void    _checkReadFifoNotFull(void);
  //void    _checkReadFifoFull(void);
  //void    _checkReadFifoNotEmpty(void);
  //void    _check2dBusy(void);
  //bool    _checkSdramReady(void);
  //bool    _checkIcReady(void);
 //----
  inline void _waitWriteFifo(void) { while (_readStatus() & 0x80); };           // checkWriteFifoNotFull() en RA8876_Lite, pero bloqueante
  inline void _waitTaskBusy(void) { while (_readStatus() & 0x08); };            // check2dBusy() en RA8876_Lite, pero bloqueante

  bool      _calcPllParams(uint32_t targetFreq, int kMax, PllParams *pll);      // Incluido en ra8876PllInitial() en RA8876_Lite
  bool      _calcClocks(void);                                                  // Incluido en ra8876PllInitial() en RA8876_Lite
  void      _dumpClocks(void);                                                  // Incluido en ra8876PllInitial() en RA8876_Lite

  bool      _initPLL(void);                                                     // ra8876PllInitial() en RA8876_Lite
  bool      _initMemory(SdramInfo *info);                                       // ra8876SdramInitial() en RA8876_Lite
  bool      _initDisplay(void);                                                 // ra8876Initialize() en RA8876_Lite
  // ------------------------------------------------------------ */

  // -------- TEXT ---------------------------------------------- */
  // Font utils
  uint8_t   _internalFontEncoding(enum FontEncoding enc);
  // ------------------------------------------------------------ */

  // -------- TEXT/GRAPHICS MODE --------------------------------- */
  void      _setTextMode(void);                                                 // textMode(bool) en RA8876_Lite
  void      _setGraphicsMode(void);
  // ------------------------------------------------------------ */

  // -------- BASIC SHAPES -------------------------------------- */
  // Low-level shapes
  void      _drawTwoPointShape(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint8_t reg, uint8_t cmd);                  // drawLine, drawRect, fillRect
  void      _drawThreePointShape(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color, uint8_t cmd);   // drawTriangle, fillTriangle
  void      _drawEllipseShape(uint16_t x, uint16_t y, uint16_t xrad, uint16_t yrad, uint16_t color, uint8_t cmd);                              // drawCircle, fillCircle
  void      _drawRoundRectShape(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t radius, uint16_t color, uint8_t cmd);             //drawRoundRect y fillRoundRect
  // ------------------------------------------------------------ */


  // These read 16- and 32-bit types from the SD card file.
  // BMP data is stored little-endian, Arduino is little-endian too.
  // May need to reverse subscript order if porting elsewhere.
  
  uint16_t  _read16(File f);
  uint32_t  _read32(File f);

  int       _getTextSizeY(void);

  
public:

  RA8876(int csPin, int resetPin = 0);

  // -------- INIT ------------------------------------------------ 
  bool      init(void);                                                                  // begin() en RA8876_Lite
  void      initExternalFontRom(int spiIf, enum ExternalFontRom chip);
  // ------------------------------------------------------------ 

  // -------- REGION ---------------------------------------------- 
  // Canvas region
  bool      setCanvasRegion(uint32_t address, uint16_t width = 0);                       // Parecido canvasImageStartAddress() en RA8876_Lite
  bool      setCanvasWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height);    // activeWindowXY() y activeWindowWH() en RA8876_Lite juntos

  void      displayImageStartAddress(uint32_t addr);                   // Lite
  void      displayImageWidth(uint16_t width);                         // Lite
  void      displayWindowStartXY(uint16_t x0,uint16_t y0);             // Lite
  void      canvasImageStartAddress(uint32_t addr);                    // Lite
  void      canvasImageWidth(uint16_t width);                          // Lite

  // Display region
  bool      setDisplayRegion(uint32_t address, uint16_t width);
  bool      setDisplayOffset(uint16_t x, uint16_t y);

  // Dimensions
  int       getWidth() { return _width; };
  int       getHeight() { return _height; };
  // ------------------------------------------------------------ 

  // -------- RANDOM -------------------------------------------- 
  // Test
  void      colorBarTest(bool enabled);

  // RAM Access Prepare
  void      ramAccessPrepare(void);
  // ------------------------------------------------------------ 


  // -------- TEXT -------------------------------------------- 
  // INTERNAL/EXTERNAL TEXT FONTS 
  void            selectInternalFont(enum FontSize size, enum FontEncoding enc = RA8876_FONT_ENCODING_8859_1);
  void            selectExternalFont(enum ExternalFontFamily family, enum FontSize size, enum FontEncoding enc, FontFlags flags = 0);

  // SCALE TEXT
  //void setTextScale(int scale) { setTextScale(scale, scale); };
  void            setTextScale(uint8_t X_scale, uint8_t Y_scale);
  inline uint8_t  getScaleX(void){ return _textScaleX; };
  inline uint8_t  getScaleY(void){ return _textScaleY; };

  // TEXT CURSOR
  void            setCursor(uint16_t x, uint16_t y);                 // setTextCursor() en RA8876_Lite
  inline uint16_t getCursorX(void){ return _cursorX; };
  inline uint16_t getCursorY(void){ return _cursorY; };
  
  
  // TEXT COLOR (FOREGROUND/BACKGROUND)
  void            setTextForegroundColor(uint16_t color);   // Color del texto
  void            setTextBackgroundColor(uint16_t color);   // Color del resaltado del texto
  void            setTextBackgroundTrans(uint8_t trans);
  void            setTextColor(uint16_t f_color, uint16_t b_color, uint8_t trans);  // Texto y resaltado, segúm flag de transparencia
  
  // WRITE TEXT
  inline void     putChar(char c) { putChars(&c, 1); };
  void            putChars(const char *buffer, size_t size);
  inline void     putChar16(uint16_t c) { putChars16(&c, 1); };
  void            putChars16(const uint16_t *buffer, unsigned int count);
  void            putString(uint16_t x0,uint16_t y0, char *str); // ESTE ES CLAVE // Lite
  //void          putDec(uint16_t x0,uint16_t y0,signed long vaule,uint8_t len,const char *flag);
  //void          putFloat(uint16_t x0,uint16_t y0,double vaule,uint8_t len,uint8_t precision, const char *flag);
  //void          putHex(uint16_t x0,uint16_t y0,uint32_t vaule,uint8_t len,const char *flag); 
  // Internal for Print class
  virtual size_t write(uint8_t c) { return write(&c, 1); };
  virtual size_t write(const uint8_t *buffer, size_t size);
  /* ------------------------------------------------------------ */
  

  /* -------- GRAPHIC -------------------------------------------- */
  // GRAPHIC CURSOR
  void            setPixelCursor(uint16_t x,uint16_t y);             
  
  // Drawing
  void            drawPixel(uint16_t x, uint16_t y, uint16_t color);
        // putPixel_16bpp() en RA8876_Lite
  inline void     drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) { _drawTwoPointShape(x1, y1, x2, y2, color, RA8876_REG_DCR0, RA8876_DRAW_LINE); }; //0x80
        // drawLine() en RA8876_Lite
  inline void     drawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) { _drawTwoPointShape(x1, y1, x2, y2, color, RA8876_REG_DCR1, RA8876_DRAW_SQUARE); }; //0xA0
        // drawSquare() en RA8876_Lite
  inline void     fillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) { _drawTwoPointShape(x1, y1, x2, y2, color, RA8876_REG_DCR1, RA8876_DRAW_SQUARE_FILL); }; //0xE0
        // drawSquareFill() en RA8876_Lite
  inline void     drawRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t radius, uint16_t color) { _drawRoundRectShape(x1, y1, x2, y2, radius, color, RA8876_DRAW_ROUND_RECT); }; //0xB0
        // drawCircleSquare() en RA8876_Lite
  inline void     fillRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t radius, uint16_t color) { _drawRoundRectShape(x1, y1, x2, y2, radius, color, RA8876_DRAW_ROUND_RECT_FILL); }; //0xF0
        // drawCircleSquareFill() en RA8876_Lite
  inline void     drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color) { _drawThreePointShape(x1, y1, x2, y2, x3, y3, color, 0xA2); }; //RA8876_DRAW_TRIANGLE => 0x82
        // drawTriangle() en RA8876_Lite
  inline void     fillTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color) { _drawThreePointShape(x1, y1, x2, y2, x3, y3, color, 0xE2); }; //RA8876_DRAW_TRIANGLE_FILL => 0xA2
        // drawTriangleFill() en RA8876_Lite
  inline void     drawCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color) { _drawEllipseShape(x, y, radius, radius, color, RA8876_DRAW_CIRCLE); }; //0x80
        // drawCircle() en RA8876_Lite  
  inline void     fillCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color) { _drawEllipseShape(x, y, radius, radius, color, RA8876_DRAW_CIRCLE_FILL); }; //0xC0
        // drawCircleFill() en RA8876_Lite
  inline void     drawEllipse(uint16_t x, uint16_t y, uint16_t radius, uint16_t color) { _drawEllipseShape(x, y, radius, radius, color, RA8876_DRAW_ELLIPSE); }; //0x80
        // drawEllipse() en RA8876_Lite  
  inline void     fillEllipse(uint16_t x, uint16_t y, uint16_t radius, uint16_t color) { _drawEllipseShape(x, y, radius, radius, color, RA8876_DRAW_ELLIPSE_FILL); }; //0xC0
        // drawEllipseFill() en RA8876_Lite

  // Limpiar pantalla ==> dibujar rectángulo que ocupe todo
  //inline void     clearScreen(uint16_t color) { setCursor(0, 0); fillRect(0, 0, _width, _height, color); };
  inline void     clearScreen(uint16_t color) { fillRect(0, 0, _width, _height, color); };


  // ------- BTE ------------
  void    bte_Source0_MemoryStartAddr(uint32_t addr);              // Lite
  void    bte_Source0_ImageWidth(uint16_t width);                  // Lite
  void    bte_Source0_WindowStartXY(uint16_t x0,uint16_t y0);      // Lite
  void    bte_Source1_MemoryStartAddr(uint32_t addr);              // Lite
  void    bte_Source1_ImageWidth(uint16_t width);                  // Lite
  void    bte_Source1_WindowStartXY(uint16_t x0,uint16_t y0);      // Lite
  void    bte_DestinationMemoryStartAddr(uint32_t addr);           // Lite
  void    bte_DestinationImageWidth(uint16_t width);               // Lite
  void    bte_DestinationWindowStartXY(uint16_t x0,uint16_t y0);   // Lite
  void    bte_WindowSize(uint16_t width, uint16_t height);         // Lite
  //--- todo Lite
  void    bteMemoryCopy(uint32_t s0_addr,uint16_t s0_image_width,uint16_t s0_x,uint16_t s0_y,uint32_t des_addr,uint16_t des_image_width, 
                    uint16_t des_x,uint16_t des_y,uint16_t copy_width,uint16_t copy_height);
  void    bteMemoryCopyWithROP(uint32_t s0_addr,uint16_t s0_image_width,uint16_t s0_x,uint16_t s0_y,uint32_t s1_addr,uint16_t s1_image_width,uint16_t s1_x,uint16_t s1_y,
                            uint32_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t copy_width,uint16_t copy_height,uint8_t rop_code);
  void    bteMemoryCopyWithChromaKey(uint32_t s0_addr,uint16_t s0_image_width,uint16_t s0_x,uint16_t s0_y,
                                uint32_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t copy_width,uint16_t copy_height,uint16_t chromakey_color);
  void    bteMpuWriteWithROP(uint32_t s1_addr,uint16_t s1_image_width,uint16_t s1_x,uint16_t s1_y,uint32_t des_addr,uint16_t des_image_width,
                         uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint8_t rop_code,const unsigned short *data);
  void    bteMpuWriteWithROP(uint32_t s1_addr,uint16_t s1_image_width,uint16_t s1_x,uint16_t s1_y,uint32_t des_addr,uint16_t des_image_width,
                         uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint8_t rop_code);                     
  void    bteMpuWriteWithChromaKey(uint32_t des_addr,uint16_t des_image_width, uint16_t des_x, uint16_t des_y, uint16_t width,uint16_t height,uint16_t chromakey_color,
                              const unsigned short *data);
  void    bteMpuWriteWithChromaKey(uint32_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t chromakey_color);
  void    bteMpuWriteColorExpansion(uint32_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t foreground_color,uint16_t background_color,const unsigned char *data);
  void    bteMpuWriteColorExpansion(uint32_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t foreground_color,uint16_t background_color);
  void    bteMpuWriteColorExpansionWithChromaKey(uint32_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,
                                             uint16_t foreground_color,uint16_t background_color,const unsigned char *data);
  void    bteMpuWriteColorExpansionWithChromaKey(uint32_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,
                                             uint16_t width,uint16_t height,uint16_t foreground_color,uint16_t background_color);

 /* void  btePatternFill(uint8_t p8x8or16x16, uint32_t s0_addr,uint16_t s0_image_width,uint16_t s0_x,uint16_t s0_y,
                    uint32_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height);
  void    btePatternFillWithChromaKey(uint8_t p8x8or16x16, uint32_t s0_addr,uint16_t s0_image_width,uint16_t s0_x,uint16_t s0_y,
                                 uint32_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t chromakey_color);
*/


  
  // ------ IMAGENES -------
  void    putPicture_16bpp(uint16_t x,uint16_t y,uint16_t width, uint16_t height, const unsigned short *data);

  // ------ SD -------------
  void    sdCardDraw16bppBIN(uint16_t x,uint16_t y,uint16_t width, uint16_t height,char *filename);
  void    sdCardDraw64bitsBIN(uint16_t x,uint16_t y,uint16_t width, uint16_t height,char *filename); 
  void    sdCardDraw256bitsBIN(uint16_t x,uint16_t y,uint16_t width, uint16_t height,char *filename); 
  //void  sdCardDraw1024bitsBIN(uint16_t x,uint16_t y,uint16_t width, uint16_t height,char *filename); 
  //void  sdCardDraw4096bitsBIN(uint16_t x,uint16_t y,uint16_t width, uint16_t height,char *filename); 
  //void    sdCardDraw24bppBMP(char *filename, int x, int y);
 /* ------------------------------------------------------------ */


};

#endif
