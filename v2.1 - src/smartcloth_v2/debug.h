#ifndef DEBUG_H
#define DEBUG_H

// ---- SERIAL CON PC ----
#define SM_DEBUG // Descomentar para habilitar mensajes de depuración entre Due y PC

#if defined(SM_DEBUG)
    #define SerialPC Serial
#endif
// -----------------------


// ----- BORRADO CSV -----
#define BORRADO_INFO_USUARIO // Descomentar para habilitar el borrado de la info del usuario en ficheros CSV (acumulado), TXT (comidas a subir) y CSV (productos barcode leídos)
// -----------------------


#endif