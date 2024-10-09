#ifndef SERIAL_FUNCTIONS_H
#define SERIAL_FUNCTIONS_H


// ESP32-CAM
//#define RXD1 14
//#define TXD1 15

// ESP32-CAM PLUS
#define RXD1 16
#define TXD1 17

#define SerialPC Serial
#define SerialDue Serial1


#define SERIAL_DUE 1
#define SERIAL_BR  2



/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
// Comunicación Serial ESP32-Due
void            setupAllSerial();                             // Inicializar comunicación serie con PC y Due
inline void     sendMsgToDue(const String &msg);              // Enviar 'msg' del ESP32 al Due
inline bool     hayMsgFromDue();                              // Comprobar si hay mensajes del Due disponibles
inline void     readMsgFromSerialDue(String &msgFromDue);     // Leer mensaje del puerto serie ESP32-Due

void            waitMsgFromDue(String &msgFromESP32, unsigned long &timeout);   

inline bool    isTimeoutExceeded(unsigned long startTime, unsigned long timeout); // Comprobar si se ha excedido el tiempo de espera
bool           processCharacter(String &tempBuffer, String &msg, byte serialPort); // Procesa buffer del Serial (Due o BR) caracter a caracter hasta completar mensaje con "\n"
/*-----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
                           IMPLEMENTACIÓN FUNCIONES
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/**
 * @brief Configura la comunicación serial entre el PC y el Due.
 *  Esta función inicializa los puertos seriales y establece la velocidad de baudios
 *  en 115200 para ambos, el PC y el Due.
 */
/*-----------------------------------------------------------------------------*/
void setupAllSerial() {
    // Deben tener la misma velocidad de baudios que el PC y Due, respectivamente
    SerialPC.begin(115200);
    SerialDue.begin(115200, SERIAL_8N1, RXD1, TXD1); 
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía un mensaje desde el ESP32 al Arduino Due.
 * @param msg El mensaje a enviar.
 */
/*-----------------------------------------------------------------------------*/
inline void sendMsgToDue(const String &msg) { SerialDue.println(msg); delay(100); }



/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba si hay mensajes del Due disponibles en el puerto serial.
 * @return true si hay mensajes disponibles, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
inline bool hayMsgFromDue() { return SerialDue.available() > 0; }



/*-----------------------------------------------------------------------------*/
/**
 * Lee un mensaje desde el puerto serie con el Due y lo guarda en la variable proporcionada.
 * 
 * @param msgFromDue La variable donde se guardará el mensaje leído.
 */
/*-----------------------------------------------------------------------------*/
inline void readMsgFromSerialDue(String &msgFromDue) { 
    msgFromDue = SerialDue.readStringUntil('\n');
    msgFromDue.trim(); // Elimina espacios en blanco al principio y al final
}


/*---------------------------------------------------------------------------------------------------------*/
/**
 * Espera un mensaje del Due durante un tiempo determinado.
 * 
 * @param msgFromESP32 Referencia a un objeto String donde se almacenará el mensaje recibido del Due.
 * @param timeout Tiempo máximo de espera en milisegundos.
 */
/*---------------------------------------------------------------------------------------------------------*/
/*void waitMsgFromDue(String &msgFromDue, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el Due responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while(!hayMsgFromDue() && !isTimeoutExceeded(startTime, timeout));

    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
    if (hayMsgFromDue())  // Si el Due ha respondido
    {
        readMsgFromSerialDue(msgFromDue); // Leer mensaje del puerto serie y guardarlo en msgFromDue
        SerialPC.println("Respuesta del Due: " + msgFromDue);  
    } 
    else // No se ha recibido respuesta del Due
    {
        SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del Due"));

        // Se considera que no hay conexión WiFi
        msgFromDue = "TIMEOUT";;
    }

}*/
// Procesando caracter a caracter
void waitMsgFromDue(String &msgFromDue, unsigned long &timeout) 
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual
    String tempBuffer = "";  // Buffer temporal para ensamblar el mensaje

    // Esperar 'timeout' segundos a que el Due responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while (!isTimeoutExceeded(startTime, timeout)) 
    {
        if (hayMsgFromDue())  // Si el Due ha respondido
        {
            if (processCharacter(tempBuffer, msgFromDue, SERIAL_DUE)) 
                return;  // Sale cuando se ha procesado un mensaje completo
        }
        delay(50);  // Evita que el bucle sea demasiado intensivo
    }

    SerialPC.println("Mensaje recibido del Due: " + msgFromDue);

    // Si se alcanza el tiempo de espera sin recibir un mensaje
    SerialPC.println(F("TIMEOUT-DUE. No se ha recibido respuesta del Due"));

    msgFromDue = "TIMEOUT-DUE";  // Marca el mensaje como TIMEOUT si no se recibió nada útil
}

/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba si se ha excedido el tiempo de espera.
 * 
 * Esta función compara el tiempo transcurrido desde el inicio con el tiempo de espera especificado.
 * 
 * @param startTime El tiempo de inicio en milisegundos.
 * @param timeout El tiempo de espera en milisegundos.
 * @return true si el tiempo transcurrido es menor que el tiempo de espera, false de lo contrario.
 */
/*-----------------------------------------------------------------------------*/
inline bool isTimeoutExceeded(unsigned long startTime, unsigned long timeout) {
    return millis() - startTime > timeout;
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Procesa un carácter recibido por el puerto serial y construye un mensaje completo.
 *
 * Esta función lee un carácter del puerto serial especificado y lo acumula en un buffer temporal.
 * Si se recibe un carácter de nueva línea ('\n'), se considera que el mensaje está completo y se 
 * procesa el contenido del buffer temporal.
 *
 * @param tempBuffer Referencia al buffer temporal donde se acumulan los caracteres recibidos.
 * @param msg Referencia a la cadena donde se almacenará el mensaje completo procesado.
 * @param serialPort Puerto serial desde el cual se lee el carácter. Puede ser SERIAL_DUE o SERIAL_BR.
 * @return true si se ha recibido y procesado un mensaje completo, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool processCharacter(String &tempBuffer, String &msg, byte serialPort) 
{
    char c;
    if (serialPort == SERIAL_DUE) c = SerialDue.read();  // Lee un carácter del serial del Due
    //else c = SerialBR.read();  // Lee un carácter del serial del lector BR

    if (c == '\n') {
        tempBuffer.trim();  // Elimina los espacios en blanco del buffer temporal

        if (tempBuffer.length() > 0) {  // Solo procesa si no está vacío
            msg = tempBuffer;
            #if defined(SM_DEBUG)
                if (serialPort == SERIAL_DUE) SerialPC.println("\nRespuesta del Due: " + msg);
                else SerialPC.println("\nBuffer BR leido: " + msg);
            #endif
            return true;  // Mensaje completo procesado
        }
        tempBuffer = "";  // Si estaba vacío, se resetea el buffer
    } 
    else {
        tempBuffer += c;  // Acumula el carácter en el buffer temporal
    }
    return false;  // Aún no se ha recibido un mensaje completo
}


#endif