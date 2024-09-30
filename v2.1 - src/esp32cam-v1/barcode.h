/** 
 * @file barcode.h
 * @brief Estrategia para la lectura y validación de códigos de barras
 * 
 * PROBLEMA:
 * A veces, al leer el Serial con el lector se obtiene una cadena que incluye varios códigos de barras o basura, muy probablemente porque el usuario 
 * haya pasado demasiadas veces el producto por el lector en lugar de esperar a que se procese y se muestre su información. Inicialmente se pensó en 
 * simplemente limpiar el buffer antes de leer el producto, pero el sistema no sabe cuándo es "antes". Además, tras la limpieza el usuario podría 
 * igualmente pasar el producto demasiadas veces. 
 * 
 * OpenFoodFacts admite los formatos EAN-13 (13 caracteres), UPC-A (12 caracteres) y EAN-8 (8 caracteres), entre otros. 
 * 
 * SOLUCION:
 * Extraer el código de barras de lo leído en el buffer, independientemente de si hay basura o varios códigos concatenados, e intentar validar los 
 * últimos caracteres leídos del buffer como un EAN-13, UPC-A o EAN-8, los formatos más comunes, siguiendo estos pasos:
 * 1. Obtener los últimos 13 caracteres y comprobar si es un EAN-13 válido. 
 * 2. Si no lo es, obtener los últimos 12 caracteres y comprobar si es un UPC-A.
 * 3. Si tampoco lo es, obtener los últimos 8 caracteres y comprobar si es un EAN-8. 
 * 4. Si no es ninguno de estos formatos, los más comunes, se considerará que no hay un código de barras válido.
 * 
 * Para comprobar si el código es un EAN-13, UPC-A o EAN-8 válido se han realizado funciones que realizan el checksum de cada uno de estos códigos para comprobar el
 * dígito de control. Si el dígito de control calculado es igual al último dígito del código de barras, entonces el código es válido para el formato correspondiente.
 * 
 * Es cierto que si se lee un código que no sea EAN-13, UPC-A o EAN-8, no se podrá obtener la información del producto, pero es preferible evitar enviar solicitudes 
 * incorrectas o innecesarias a OpenFoodFacts para optimizar el rendimiento del sistema.
 * 
 */
#ifndef BARCODE_H
#define BARCODE_H


#include "wifi_functions.h"         // Incluye Serial_functions.h y debug.h
inline bool   hayConexionWiFi();
void          getFoodData(String barcode);


#define EAN13_LENGTH 13 // Longitud de un EAN-13
#define UPC_A_LENGTH 12 // Longitud de un UPC-A
#define EAN8_LENGTH  8  // Longitud de un EAN-8

#define MULT_PAR   0 // Multiplicar por 3 los dígitos en posiciones pares (perspectiva vector)
#define MULT_IMPAR 1 // Multiplicar por 3 los dígitos en posiciones impares (perspectiva vector)





/*-----------------------------------------------------------------------------
                           DECLARACIÓN FUNCIONES
-----------------------------------------------------------------------------*/
void            tryGetBarcode();                                 // Comprobar si hay WiFi y leer código de barras
void            getBarcode();                                    // Leer código de barras 

// Comprobación códigos de barras
//inline bool    hayVariosCodigos(const String &input){ return input.length() > 13; };            // Comprobar si hay varios códigos de barras concatenados en el buffer del lector

bool            hasMin13Chars(const String &input){ return input.length() >= EAN13_LENGTH; }; // Comprobar si la cadena tiene al menos 13 caracteres
bool            hasMin12Chars(const String &input){ return input.length() >= UPC_A_LENGTH; }; // Comprobar si la cadena tiene al menos 12 caracteres
bool            hasMin8Chars(const String &input){ return input.length() >= EAN8_LENGTH; };   // Comprobar si la cadena tiene al menos 8 caracteres

inline String   takeLast13(const String &input){ return input.substring(input.length() - 13); }; // Devolver los últimos 13 caracteres del mensaje
inline String   takeLast12(const String &input){ return input.substring(input.length() - 12); }; // Devolver los últimos 12 caracteres del mensaje
inline String   takeLast8(const String &input){ return input.substring(input.length() - 8); };   // Devolver los últimos 8 caracteres del mensaje

bool            isValidEAN13(const String &barcode);             // Comprobar si la cadena es un EAN-13 válido
bool            isValidUPCA(const String &barcode);              // Comprobar si la cadena es un UPC-A válido
bool            isValidEAN8(const String &barcode);              // Comprobar si la cadena es un EAN-8 válido

bool            extractAndValidateBarcode(String &buffer, String &barcode);                  // Comprobar si la cadena es un EAN-13, UPC-A o EAN-8 válido
int             calculateCheckDigit(const String& barcode, bool multImpar);  // Calcular el dígito de comprobación de un código de barras
/*-----------------------------------------------------------------------------*/






/*------------------------------------------------------------------------------
                           IMPLEMENTACIÓN FUNCIONES
/*-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------*/
/**
 * @brief Intenta obtener el código de barras.
 * 
 * Si hay una conexión WiFi disponible, se intentará leer el código de barras.
 * Si no hay conexión WiFi, se mostrará un mensaje de error y se enviará un mensaje al dispositivo Due indicando la falta de conexión.
 */
/*-----------------------------------------------------------------------------*/
void tryGetBarcode()
{
    // ---- LIMPIAR BUFFER -------------------------------------
    // Se limpia el buffer de recepción (Rx) antes de enviar para asegurar que se procesa la respuesta 
    // al mensaje que se va a enviar y no otros enviados anteriormente
    limpiarBufferBR();
    // ---------------------------------------------------------

    // Ya se pregunta si hay conexión antes de enviar el mensaje "GET-BARCODE"
    
    //if(hayConexionWiFi())
    //{
        #if defined(SM_DEBUG)
            SerialPC.println("Leyendo codigo de barras...");
        #endif
        getBarcode(); // Lee barcode con BR 
    /*}
    else  
    {
        #if defined(SM_DEBUG)
            SerialPC.println("No hay conexión a Internet. No se puede buscar la info del producto.");
        #endif
        sendMsgToDue(F("NO-WIFI"));
    }*/
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Obtiene el códido de barras leído por el lector BR (Barcode Reader) y 
 *          envía la información del producto al Due.
 *
 *      Si no se lee ningún código de barras en un tiempo determinado, se envía un 
 *      mensaje de error al Due.
 * 
 * @note Esta función asume que hay una comunicación establecida con el lector de código de barras y el dispositivo Due.
 */
/*-----------------------------------------------------------------------------*/
void getBarcode()
{
    String buffer = BUFFER_EMPTY; // Cadena inicialmente vacía con "-"
    String barcode = BUFFER_EMPTY; // Código de barras por leer

    /*unsigned long startMillis = millis();
    unsigned long timeToRead = TIME_TO_READ_BARCODE; // 30 segundos para leer el código de barras

    while((barcode == "-") && !isTimeoutExceeded(startMillis, timeToRead))  // Se da 30 segundos para leer el producto
    {
        // ----- PRODUCTO DETECTADO ------------------------
        if (hayMsgFromBR())  // El BR leyó algo
        {
            // ---- OBTENER CÓDIGO DE BARRAS -----------
            readMsgFromSerialBR(barcode); // Obtener código de barras en 'barcode'
            #if defined(SM_DEBUG)
                SerialPC.print("\nBarcode leido: "); SerialPC.println(barcode);
            #endif
            // -----------------------------------------
        }
        // --------------------------------------------------
    }*/


    // --- LIMPIAR BUFFER --------------------------------
    // Limpiar el buffer antes de esperar hasta 30 segundos a que se detecte algo.
    // Si con esta limpieza se borrara el código que justo se ha leído, el usuario lo notaría
    // porque el sistema no lo buscaría, así que lo pasaría de nuevo y ya sí se podrá detectar

    //limpiarBufferBR(); // Ya se hace en tryGetBarcode()
    // ---------------------------------------------------


    // --- ESPERAR LECTURA DE CÓDIGO DE BARRAS -----------
    // Tras limpiar el buffer, se dan 30 segundos al usuario para que coloque el producto sobre el lector.
    // Si se recibe "CANCEL-BARCODE" del Due antes de leer código de barras, significa que el usuario ha cancelado la lectura. 
    waitForBarcode(buffer); // Esperar 30 segundos a que haya algo en el buffer del lector
    // ---------------------------------------------------
    

    // ---- BARCODE DETECTADO ----------------------------
    if(buffer != BUFFER_EMPTY)  // Si había algo en el buffer, se ha guardado en 'buffer'
    {   
        // ----- LECTURA CANCELADA ------------------------
        if(buffer == "CANCEL-BARCODE")  // Mientras se esperaba barcode, el usuario ha cancelado la lectura pulsando un botón
                                        // y el Due ha avisado al ESP32 de que cancele la lectura
        {
            #if defined(SM_DEBUG)
                SerialPC.println("Cancelada lectura de codigo de barras...");
            #endif
        }
        // -------------------------------------------------
        // ----- PRODUCTO DETECTADO ------------------------
        else
        {
            // ---- COMPROBAR VALIDEZ CODIGO -------------
            // Se comprueba el contenido del buffer por si se ha unido el código con residuos de lecturas previas
            // o si se ha leído varias veces seguidas, habiéndose concatenado varias veces el mismo código. Si es así, 
            // se toma el último código leído y se valida si es un EAN-13, UPC-A o EAN-8 válido, los más comunes.
            
            if(extractAndValidateBarcode(buffer, barcode)) // Comprobar si en la parte final del buffer hay un código válido (EAN-13, UPC-A o EAN-8)
            {                                              // y devolverlo en 'barcode'
                // ---- ENVIAR BARCODE AL DUE ---
                // Avisar al Due de que se ha leído el barcode
                String msgToDue = "BARCODE:" + barcode; 
                #if defined(SM_DEBUG)
                    SerialPC.print("Enviando barcode al Due: "); SerialPC.print("\"" + msgToDue); SerialPC.println("\"");
                #endif
                sendMsgToDue(msgToDue);
                //delay(500); // Creo que estaba para darle tiempo al Due a saber que se ha leído el barcode, pero que el ESP32 no obtuviera su info demasiado rápido.
                            // Al cambiarlo para que hasta que no reciba el mensaje "GET-PRODUCT:<barcode>" no busque la info, no hace falta.
                // ------------------------------

                // ---- BUSCAR PRODUCTO ---------
                //getFoodData(barcode); // Obtener información del 'barcode' en OpenFoodFacts
                // ------------------------------
            }
            else
            {
                #if defined(SM_DEBUG)
                    SerialPC.println("Código no válido");
                #endif
                sendMsgToDue(F("NO-BARCODE")); // Enviar mensaje al Due de que no se ha leído un código de barras válido
            }
            // --------------------------------------------
        }
        // -------------------------------------------------
    } 
    // ---------------------------------------------------
    // ---- NO DETECTADO ---------------------------------
    else // Si han pasado 30 segundos sin leer barcode (no se ve bien o el lector no responde)
    {
        #if defined(SM_DEBUG)
            SerialPC.println("\nNo se ha leido ningún código de barras");
        #endif
        sendMsgToDue(F("NO-BARCODE"));
    }
    // -------------------------------------------

}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Verifica si un código de barras es válido según el estándar EAN-13.
 *        Se suman todos los valores, multiplicando por 3 aquellos en posición PAR (perspectiva humana). 
 *        Luego, se hace el módulo 10 de la suma total y se resta el resultado de 10. Si el dígito de
 *        comprobación calculado es igual al último dígito del código de barras, entonces el
 *        código de barras es un EAN-13 válido.
 *
 *        Desde la perspectiva humana, la primera posición (1) es impar, pero desde la perspectiva
 *        de un vector en programación la primera posición (0) es par. Por tanto, multiplicamos por 3
 *        los dígitos en posiciones IMPARES en el vector.
 * 
 * @param barcode El código de barras a verificar.
 * @return true si el código de barras es un EAN-13 válido, false de lo contrario.
 * 
 * @see http://www.grupoalquerque.es/ferias/2012/archivos/digitos/codigo_barras.pdf
 * @see https://www.codigos-qr.com/calcular-digito-de-control-ean13-gtin13/
 */
/*-----------------------------------------------------------------------------*/
bool isValidEAN13(const String &barcode) 
{
    // Si no tiene 13 caracteres, no puede ser EAN-13
    if (barcode.length() != EAN13_LENGTH) {
        SerialPC.println("No tiene 13 digitos");
        return false;
    }
    
    // Calcular el dígito de comprobación esperado
    int checkDigit = calculateCheckDigit(barcode, MULT_IMPAR); // Multiplicar por 3 los dígitos en posiciones impares (perspectiva vector)

    // Comparar el dígito de comprobación calculado con el último dígito del código de barras
    // Si son iguales, el código de barras es un EAN-13 válido
    return (barcode[EAN13_LENGTH-1] - '0') == checkDigit;
}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Verifica si un código de barras es válido según el estándar UPC-A.
*        Se suman todos los valores, multiplicando por 3 aquellos en posición IMPAR (perspectiva humana). 
 *        Luego, se hace el módulo 10 de la suma total y se resta el resultado de 10. Si el dígito de
 *        comprobación calculado es igual al último dígito del código de barras, entonces el
 *        código de barras es un EAN-8 válido.
 *
 *        Desde la perspectiva humana, la primera posición (1) es impar, pero desde la perspectiva
 *        de un vector en programación la primera posición (0) es par. Por tanto, multiplicamos por 3
 *        los dígitos en posiciones PARES en el vector.
 * 
 * @param barcode El código de barras a verificar.
 * @return true si el código de barras es un UPC-A válido, false de lo contrario.
 * 
 * @see https://es.wikipedia.org/wiki/Código_Universal_de_Producto#Cálculo_de_Dígito_Verificador
 */
/*-----------------------------------------------------------------------------*/
bool isValidUPCA(const String &barcode) 
{
    // Si no tiene 12 caracteres, no puede ser UPC-A
    if (barcode.length() != UPC_A_LENGTH) {
        SerialPC.println("No tiene 12 digitos");
        return false;
    }

    // Calcula el dígito de comprobación esperado
    int checkDigit = calculateCheckDigit(barcode, MULT_PAR); // Multiplicar por 3 los dígitos en posiciones pares (perspectiva vector)
    
    // Compara el dígito de comprobación calculado con el último dígito del código de barras
    // Si son iguales, el código de barras es un UPC-A válido
    return (barcode[UPC_A_LENGTH-1] - '0') == checkDigit;
}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Verifica si un código de barras es válido según el estándar EAN-8.
 *        Se suman todos los valores, multiplicando por 3 aquellos en posición IMPAR (perspectiva humana). 
 *        Luego, se hace el módulo 10 de la suma total y se resta el resultado de 10. Si el dígito de
 *        comprobación calculado es igual al último dígito del código de barras, entonces el
 *        código de barras es un EAN-8 válido.
 *
 *        Desde la perspectiva humana, la primera posición (1) es impar, pero desde la perspectiva
 *        de un vector en programación la primera posición (0) es par. Por tanto, multiplicamos por 3
 *        los dígitos en posiciones PARES en el vector.
 * 
 * @param barcode El código de barras a verificar.
 * @return true si el código de barras es un EAN-8 válido, false de lo contrario.
 * 
 * @see https://softmatic.com/codigo-de-barras-ean-8.html
 */
/*-----------------------------------------------------------------------------*/
bool isValidEAN8(const String &barcode) 
{
    // Si no tiene 8 caracteres, no puede ser EAN-8
    if (barcode.length() != EAN8_LENGTH) {
        SerialPC.println("No tiene 8 digitos");
        return false;
    }

    // Calcula el dígito de comprobación esperado
    int checkDigit = calculateCheckDigit(barcode, MULT_PAR); // Multiplicar por 3 los dígitos en posiciones pares (perspectiva vector)
    
    // Compara el dígito de comprobación calculado con el último dígito del código de barras
    // Si son iguales, el código de barras es un EAN-8 válido
    return (barcode[EAN8_LENGTH-1] - '0') == checkDigit;
}






/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba el formato del código de barras para saber si es válido.
 * 
 * Primero toma los últimos 13 caracteres y comprueba si es un EAN-13.
 * Si no lo es, toma los 12 últimos caracteres y comprueba si es un UPC-A.
 * Si no lo es, toma los 8 últimos caracteres y comprueba si es un EAN-8.
 * Si no es ninguno de los tres, se indica como código no válido.
 * 
 * Al validar un código, se sale de la función sin seguir comprobando los demás formatos.
 * 
 * @param buffer El contenido leído del buffer a comprobar.
 * @param barcode El código de barras extraído y validado. Si no se valida, será "-"
 * @return true si el código de barras es válido, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool extractAndValidateBarcode(String &buffer, String &barcode)
{   
    #if defined(SM_DEBUG)
        SerialPC.println("Extrayendo y validando código");
    #endif

    String auxCad; // Cadena auxiliar para almacenar el código de barras extraído

    // ---- MINIMO 13 CARACTERES -------------
    if (hasMin13Chars(buffer)) // Verificar si la longitud del buffer es al menos 13 caracteres
    {                           // Si hay varios códigos concatenadaos, tomará el último
        auxCad = takeLast13(buffer); // Tomar los últimos 13 caracteres
        if (isValidEAN13(auxCad)) 
        {
            #if defined(SM_DEBUG)
                SerialPC.println("Código EAN-13 válido");
            #endif

            barcode = auxCad; // Es un EAN-13. Tomar ese código y procesarlo
            return true;
        }
    }
    // --------------------------------------

    // ---- MINIMO 12 CARACTERES -------------
    if (hasMin12Chars(buffer)) // Verificar si la longitud del buffer es al menos 12 caracteres
    {
        auxCad = takeLast12(buffer); // Tomar los últimos 12 caracteres
        if (isValidUPCA(auxCad)) 
        {
            #if defined(SM_DEBUG)
                SerialPC.println("Código UPC-A válido");
            #endif

            barcode = auxCad; // Es un UPC-A. Tomar ese código y procesarlo
            return true;
        }
    }
    // --------------------------------------

    // ---- MINIMO 8 CARACTERES -------------
    if (hasMin8Chars(buffer)) // Verificar si la longitud del buffer es al menos 8 caracteres
    {
        auxCad = takeLast8(buffer); // Tomar los últimos 8 caracteres
        if (isValidEAN8(auxCad)) {
            SerialPC.println("Código EAN-8 válido");
            barcode = auxCad; // Es un EAN-8. Tomar ese código y procesarlo
            return true;
        }
    }
    // --------------------------------------


    #if defined(SM_DEBUG)
        SerialPC.println("CÓDIGO INVÁLIDO");
    #endif

    // No es un código de barras válido
    //barcode = "-"; // No hace falta, si no es válido, ya está en "-" por defecto

    return false; // No es un código de barras válido
}




/*-----------------------------------------------------------------------------*/
/**
 * Calcula el dígito de comprobación para un código de barras dado.
 * 
 * @param barcode El código de barras para el cual se calculará el dígito de comprobación.
 * @param multImpar Indica si se debe multiplicar por 3 los dígitos en posiciones impares (true) o pares (false).
 * @return El dígito de comprobación calculado.
 */
/*-----------------------------------------------------------------------------*/
int calculateCheckDigit(const String& barcode, bool multImpar) 
{
    int sum = 0;

    for (int i = 0; i < barcode.length() - 1; ++i)  // Itera sobre los dígitos del código de barras, excepto el último (check digit)
    {
        int digit = barcode[i] - '0';                       // Convierte el carácter actual a un dígito entero
        sum += (i % 2 == multImpar) ? digit * 3 : digit;    // Suma el dígito a la suma total, multiplicando por 3 si está en una posición impar (perspectiva vector)
        // La primera posición se considera par porque es posición 0 (0 % 2 = 0). Se toma la perspectiva del vector, 
        // no del humano, que tomaría la primera posición como impar (1 % 2 != 0)
    }

     // Calcula y devuelve el dígito de comprobación esperado
    return (10 - (sum % 10)) % 10;
}





#endif