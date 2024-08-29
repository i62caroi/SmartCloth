/*
// Función para comprobar si hay concatenación de varios códigos de barras en el buffer del lector
// OpenFoodFacts admite los formatos EAN-13 (13 caracteres), UPC-A (12 caracteres) y EAN-8 (8 caracteres),
// entre otros. Estos son los más comunes y los que se tendrán en cuenta al comprobar si se han concatenado
// varios códigos por haber pasado el producto varias veces por el lector sin dejar que se procese.
//
// Entonces, si la longitud del mensaje es mayor a 13, es que hay más de un código de barras, porque
// el más largo es el EAN-13. Si hubiera más de un código, se tomarían los últimos 13 caracteres y se 
// comprobaría si es un EAN-13 válido. Si no lo es, se tomarían los últimos 12 caracteres y se comprobaría
// si es un UPC-A válido. Si tampoco lo es, se tomarían los últimos 8 caracteres y se comprobaría si es un
// EAN-8 válido. Si no es ninguno de estos, se considera que no hay un código de barras válido.

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
void            limpiarBufferBR();                            // Limpiar buffer del lector de códigos de barras

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
 * @brief Limpia el buffer de entrada de la comunicación serial con el lector de códigos de barras,
 *        para evitar acumular varios códigos seguidos.  
 */
/*-----------------------------------------------------------------------------*/
void limpiarBufferBR() 
{
    while(SerialBR.available() > 0) 
    {
        SerialBR.read();
    }
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

    /*int sum = 0;
    
    for (int i = 0; i < EAN13_LENGTH-1; i++)  // Iterar sobre los primeros 12 dígitos del código de barras. El último es el check digit
    {
        int digit = barcode[i] - '0';               // Convertir el carácter actual a un dígito entero
        sum += (i % 2 == 0) ? digit : digit * 3;    // Sumar el dígito a la suma total, multiplicando por 3 si está en una posición impar (perspectiva vector)
        // La primera posición se considera par porque es posición 0 (0 % 2 = 0). Se toma la perspectiva del vector, 
        // no del humano, que tomaría la primera posición como impar (1 % 2 != 0)
    }

    // Calcular el dígito de comprobación esperado
    int checkDigit = (10 - (sum % 10)) % 10; */
    
    int checkDigit = calculateCheckDigit(barcode, MULT_IMPAR);

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

    /*int sum = 0;
    
    for (int i = 0; i < UPC_A_LENGTH-1; i++) // Itera sobre los primeros 11 dígitos del código de barras. El último es el check digit
    {
        int digit = barcode[i] - '0';               // Convierte el carácter actual a un dígito entero
        sum += (i % 2 == 0) ? digit * 3 : digit;    // Suma el dígito a la suma total, multiplicando por 3 si está en una posición par (perspectiva vector)
        // La primera posición se considera par porque es posición 0 (0 % 2 = 0). Se toma la perspectiva del vector, 
        // no del humano, que tomaría la primera posición como impar (1 % 2 != 0)
    }
    
    // Calcula el dígito de comprobación esperado
    int checkDigit = (10 - (sum % 10)) % 10;*/

    int checkDigit = calculateCheckDigit(barcode, MULT_PAR);
    
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

    /*int sum = 0;
    
    for (int i = 0; i < EAN8_LENGTH-1; i++)  // Itera sobre los primeros 7 dígitos del código de barras. El último es el check digit
    {
        int digit = barcode[i] - '0';               // Convierte el carácter actual a un dígito entero
        sum += (i % 2 == 0) ? digit * 3: digit;    // Suma el dígito a la suma total, multiplicando por 3 si está en una posición par (perspectiva vector)
        // La primera posición se considera par porque es posición 0 (0 % 2 = 0). Se toma la perspectiva del vector, 
        // no del humano, que tomaría la primera posición como impar (1 % 2 != 0)
    }

    // Calcula el dígito de comprobación esperado
    int checkDigit = (10 - (sum % 10)) % 10;*/

    int checkDigit = calculateCheckDigit(barcode, MULT_PAR);
    
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
 * @param barcode El código de barras a comprobar.
 * @return true si el código de barras es válido, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
/*bool isValidFormat(String &barcode)
{   

    String auxBarcode = takeLast13(barcode); // Tomar los últimos 13 caracteres
    if(isValidEAN13(auxBarcode))
    { 
        SerialPC.println("Código EAN-13 válido");
        barcode = auxBarcode; // Es un EAN-13. Tomar ese código y procesarlo
        return true;
    }
    else
    {
        auxBarcode = takeLast12(barcode); // Tomar los últimos 12 caracteres
        if(isValidUPCA(auxBarcode))
        { 
            SerialPC.println("Código UPC-A válido");
            barcode = auxBarcode; // Es un UPC-A. Tomar ese código y procesarlo
            return true;
        }
        else
        {
            auxBarcode = takeLast8(barcode); // Tomar los últimos 8 caracteres
            if(isValidEAN8(auxBarcode))
            { 
                SerialPC.println("Código EAN-8 válido");
                barcode = auxBarcode; // Es un EAN-8. Tomar ese código y procesarlo
                return true;
            }
            else
                return false; // No es un código de barras válido
        }
    }
}*/
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






int calculateCheckDigit(const String& barcode, bool multImpar) 
{
    int sum = 0;
    for (int i = 0; i < barcode.length() - 1; ++i) {
        int digit = barcode[i] - '0';
        sum += (i % 2 == multImpar) ? digit * 3 : digit;
    }
    return (10 - (sum % 10)) % 10;
}


#endif