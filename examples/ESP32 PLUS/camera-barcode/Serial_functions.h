
/** 
 * @file Serial_functions.h
 * A veces, al leer el Serial con el lector se obtiene una cadena que incluye varios códigos de barras o basura,
 * muy probablemente porque el usuario haya pasado demasiadas veces el producto por el lector en lugar de esperar
 * a que se procese y se muestre su información. Inicialmente se pensó en simplemente limpiar el buffer antes de leer
 * el producto, pero el sistema no sabe cuándo es "antes". Además, tras la limpieza el usuario podría igualmente pasar
 * el producto demasiadas veces. 
 *
 * Entonces, se pensó en comprobar si había códigos concatenados en el buffer chequeando si había más de 13 caracteres 
 * (longitud de EAN-13). Se tomarían los últimos 13 caracteres y se comprobaría si es un EAN-13. Si no, se tomarían los 
 * últimos 12 caracteres y se comprobaría si es un UPC-A. Si tampoco lo fuera, se tomarían los últimos 8 caracteres y se 
 * comprobaría si es un EAN-8. Si no fuera ninguno de esos formatos, los más comunes en OpenFoodFacts, se indicaría como 
 * código inválido. El problema es que si hubiera algo de basura en el buffer antes de leer un EAN-8, podría haber menos 
 * de 13 caracteres pero no ser un código válido (por ejemplo si hubiera 10 caracteres). 
 * 
 * Finalmente, se decidió que lo mejor sería siempre extraer y validar el código, sin comprobar previamente si hay más de 13 caracteres.
 * 
 * Es cierto que si se lee un código que no sea EAN-13, UPC-A o EAN-8, no se podrá obtener la información del producto, 
 * pero al menos no se enviará basura a OpenFoodFacts. Aunque en realidad no pasa nada si se envía basura, porque OpenFoodFacts 
 * no devuelve nada si no encuentra el código, pero es mejor no enviar basura.
 * 
 * En resumen, se extrae y valida el código siempre, sin comprobar previamente si hay más de 13 caracteres. Si no es un 
 * código válido (EAN-13, UPC-A o EAN-8), se indica como inválido.
 * 
*/



#ifndef SERIAL_FUNCTIONS_H
#define SERIAL_FUNCTIONS_H


// ----- ESP32 <--> PC ----------
#define SerialPC Serial
// ------------------------------


// ----- ESP32 <-- BR -----------
// BR: Barcode Reader
#include <SoftwareSerial.h>

#define RXD2 19 // Tx (naranja lector)
#define TXD2 21 // Rx (azul lector)

SoftwareSerial mySerial(RXD2, TXD2); // RX, TX
#define SerialBR mySerial // Renombrar mySerial
// ------------------------------


#define EAN13_LENGTH 13 // Longitud de un EAN-13
#define UPC_A_LENGTH 12 // Longitud de un UPC-A
#define EAN8_LENGTH  8  // Longitud de un EAN-8

#define MULT_PAR   0 // Multiplicar por 3 los dígitos en posiciones pares (perspectiva vector)
#define MULT_IMPAR 1 // Multiplicar por 3 los dígitos en posiciones impares (perspectiva vector)


/*-----------------------------------------------------------------------------
                           DECLARACIÓN FUNCIONES
-----------------------------------------------------------------------------*/
void            setupAllSerial();                             // Inicializar comunicación serie con PC, Due y lector de código de barras

// Comunicación Serial ESP32-PC
inline bool     hayMsgFromPC(){ return SerialPC.available() > 0; };   // Comprobar si hay mensajes del PC (escritos en terminal)
inline void     readMsgFromSerialPC(String &msgFromPC);       // Leer mensaje del puerto serie ESP32-PC 

// Comunicación Serial ESP32-BR
inline bool     hayMsgFromBR(){ return SerialBR.available() > 0; };   // Comprobar si hay mensajes del BR (Barcode Reader) disponibles (se ha leído código)
inline void     readMsgFromSerialBR(String &msgFromBR);       // Leer mensaje del puerto serie ESP32-BR (leer el código de barras)
inline void     limpiarBufferBR(){ while(SerialBR.available() > 0) { SerialBR.read(); } }; // Limpiar buffer del lector de códigos de barras


// Comprobación códigos de barras
//inline bool    hayVariosCodigos(const String &input){ return input.length() > EAN13_LENGTH; };   // Comprobar si hay varios códigos de barras concatenados en el buffer del lector

bool            hasMin13Chars(const String &input){ return input.length() >= EAN13_LENGTH; }; // Comprobar si la cadena tiene al menos 13 caracteres
bool            hasMin12Chars(const String &input){ return input.length() >= UPC_A_LENGTH; }; // Comprobar si la cadena tiene al menos 12 caracteres
bool            hasMin8Chars(const String &input){ return input.length() >= EAN8_LENGTH; };   // Comprobar si la cadena tiene al menos 8 caracteres

inline String  takeLast13(const String &input){ return input.substring(input.length() - EAN13_LENGTH); }; // Devolver los últimos 13 caracteres del mensaje
inline String  takeLast12(const String &input){ return input.substring(input.length() - UPC_A_LENGTH); }; // Devolver los últimos 12 caracteres del mensaje
inline String  takeLast8(const String &input){ return input.substring(input.length() - EAN8_LENGTH); };   // Devolver los últimos 8 caracteres del mensaje

bool           isValidEAN13(const String &barcode); // Comprobar si la cadena es un EAN-13 válido
bool           isValidUPCA(const String &barcode);  // Comprobar si la cadena es un UPC-A válido
bool           isValidEAN8(const String &barcode);  // Comprobar si la cadena es un EAN-8 válido

bool           extractAndValidateBarcode(String &barcode);                  // Comprobar si la cadena es un EAN-13, UPC-A o EAN-8 válido
int            calculateCheckDigit(const String& barcode, bool multImpar);  // Calcular el dígito de comprobación de un código de barras
/*-----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
                           DEFINICIÓN FUNCIONES
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/**
 * @brief Configura la comunicación serial entre el ESP32 y los demás dispositivos (PC, Due y lector).
 *  Esta función inicializa los puertos seriales y establece la velocidad de baudios
 *  en 115200 para el PC y el Due, y en 9600 para el lector de código de barras.
 */
/*-----------------------------------------------------------------------------*/
void setupAllSerial() 
{
    // Deben tener la misma velocidad de baudios que el PC, el Due y el Barcode Reader, respectivamente
    
    // ----- ESP32 - PC ----------
    SerialPC.begin(115200);
    delay(100);
    // ---------------------------

    // ----- ESP32 - BR ------
    SerialBR.begin(9600); // Debe ser 9600 porque así trabaja el lector
    delay(100);
    // ---------------------------
}



/*-----------------------------------------------------------------------------*/
/**
 * Lee un mensaje desde el puerto serie con el BR y lo guarda en la variable proporcionada.
 * 
 * @param msgFromBR La variable donde se guardará el mensaje leído.
 */
/*-----------------------------------------------------------------------------*/
inline void readMsgFromSerialBR(String &msgFromBR) { 
    msgFromBR = SerialBR.readStringUntil('\n');
    msgFromBR.trim(); // Elimina espacios en blanco al principio y al final
    limpiarBufferBR(); // Limpiar buffer del lector de códigos de barras
}




/*-----------------------------------------------------------------------------*/
/**
 * Lee un mensaje desde el puerto serie con el PC y lo guarda en la variable proporcionada.
 * 
 * @param msgFromPC La variable donde se guardará el mensaje leído.
 */
/*-----------------------------------------------------------------------------*/
inline void readMsgFromSerialPC(String &msgFromPC) { 
    msgFromPC = SerialPC.readStringUntil('\n');
    msgFromPC.trim(); // Elimina espacios en blanco al principio y al final
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
 * @param barcode El código de barras a comprobar.
 * @return true si el código de barras es válido, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool extractAndValidateBarcode(String &barcode)
{   
    // ---- MINIMO 13 CARACTERES -------------
    if (hasMin13Chars(barcode)) // Verificar si la longitud del código es al menos 13 caracteres
    {                           // Si hay varios códigos concatenadaos, tomará el último
        SerialPC.println("Tiene al menos 13 caracteres");
        String auxBarcode = takeLast13(barcode); // Tomar los últimos 13 caracteres
        if (isValidEAN13(auxBarcode)) 
        {
            SerialPC.println("Código EAN-13 válido");
            barcode = auxBarcode; // Es un EAN-13. Tomar ese código y procesarlo
            return true;
        }
        else
            SerialPC.println("NO es un EAN-13");
    }
    // --------------------------------------

    // ---- MINIMO 12 CARACTERES -------------
    if (hasMin12Chars(barcode)) // Verificar si la longitud del código es al menos 12 caracteres
    {
        SerialPC.println("Tiene al menos 12 caracteres");
        String auxBarcode = takeLast12(barcode); // Tomar los últimos 12 caracteres
        if (isValidUPCA(auxBarcode)) {
            SerialPC.println("Código UPC-A válido");
            barcode = auxBarcode; // Es un UPC-A. Tomar ese código y procesarlo
            return true;
        }
        else
            SerialPC.println("NO es un UPC-A");
    }
    // --------------------------------------

    // ---- MINIMO 8 CARACTERES -------------
    if (hasMin8Chars(barcode)) // Verificar si la longitud del código es al menos 8 caracteres
    {
        SerialPC.println("Tiene al menos 8 caracteres");
        String auxBarcode = takeLast8(barcode); // Tomar los últimos 8 caracteres
        if (isValidEAN8(auxBarcode)) {
            SerialPC.println("Código EAN-8 válido");
            barcode = auxBarcode; // Es un EAN-8. Tomar ese código y procesarlo
            return true;
        }
        else
            SerialPC.println("NO es un EAN-8");
    }
    // --------------------------------------

    // No es un código de barras válido
    return false;
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