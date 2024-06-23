#ifndef DEBUG_H
#define DEBUG_H

#define SM_DEBUG // Descomentar para habilitar mensajes de depuración entre Due y PC

#if defined(SM_DEBUG)
    #define SerialPC Serial
#endif


// Escoger versión para establecer los pines de conexión:
// SmartCloth v2.1 (cartón)
#define SM_V2_1
// SmartCloth v2.2 (3D)
//#define SM_V2_2

#endif