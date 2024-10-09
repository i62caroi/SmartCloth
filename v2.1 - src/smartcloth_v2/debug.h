#ifndef DEBUG_H
#define DEBUG_H

#define SM_DEBUG // Descomentar para habilitar mensajes de depuración entre Due y PC

#if defined(SM_DEBUG)
    #define SerialPC Serial
#endif


#endif