#ifndef SERIAL_FUNCTIONS_H
#define SERIAL_FUNCTIONS_H

#define SerialPC Serial
#define SerialESP32 Serial1

/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
// Comunicación Serial Due-ESP32
void            setupSerial();                                      // Inicializar comunicación serie con PC y ESP32
inline void     sendMsgToESP32(const String &msg);                  // Enviar 'msg' del Due al ESP32
inline bool     hayMsgFromESP32();                                  // Comprobar si hay mensajes del ESP32 disponibles
inline void     readMsgFromESP32(String &msgFromESP32);             // Leer mensaje del puerto serie Due-ESP32 y guardarlo en msgFromESP32

void            waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout);            // Espera la respuesta del ESP32, sea cual sea, y la devuelve en msgFromESP32.

inline bool     isTimeoutExceeded(unsigned long &startTime, unsigned long &timeout);            // Comprobar si se ha excedido el tiempo de espera

bool            processCharacter(String &tempBuffer, String &msgFromESP32);  // Procesar un carácter recibido del serial del ESP32 y construir un mensaje completo
/*-----------------------------------------------------------------------------*/




/*------------------------------------------------------------------------------
                           IMPLEMENTACIÓN FUNCIONES
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/**
 * @brief Configura la comunicación serial entre el PC y el ESP32.
 *  Esta función inicializa los puertos seriales y establece la velocidad de baudios
 *  en 115200 para ambos, el PC y el ESP32.
 */
/*-----------------------------------------------------------------------------*/
void setupSerial() {
    // Deben tener la misma velocidad de baudios que el PC y ESP32, respectivamente
    SerialPC.begin(115200);
    SerialESP32.begin(115200); 
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía un mensaje desde el Arduino Due al ESP32.
 * @param msg El mensaje a enviar.
 */
/*-----------------------------------------------------------------------------*/
inline void sendMsgToESP32(const String &msg) { 
    SerialESP32.println(msg); 
    SerialPC.println("Mensaje enviado al ESP32: " + msg);  // Depuración
    delay(100);
    }



/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba si hay mensajes del ESP32 disponibles en el puerto serial.
 * @return true si hay mensajes disponibles, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
inline bool hayMsgFromESP32() { return SerialESP32.available() > 0; }


/*-----------------------------------------------------------------------------*/
/**
 * Lee un mensaje desde el puerto serie con el ESP32 y lo guarda en la variable proporcionada.
 * 
 * @param msgFromESP32 La variable donde se guardará el mensaje leído.
 */
/*-----------------------------------------------------------------------------*/
inline void readMsgFromESP32(String &msgFromESP32) { 
    msgFromESP32 = SerialESP32.readStringUntil('\n'); 
    msgFromESP32.trim();  // Elimina espacios en blanco al principio y al final
}


/*---------------------------------------------------------------------------------------------------------*/
/**
 * Espera la respuesta del ESP32 durante un tiempo determinado.
 * 
 * @param msgFromESP32 Referencia a un objeto String donde se almacenará el mensaje recibido del ESP32.
 * @param timeout Tiempo máximo de espera en milisegundos.
 */
/*---------------------------------------------------------------------------------------------------------*/
/*void waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el ESP32 responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while(!hayMsgFromESP32() && !isTimeoutExceeded(startTime, timeout));

    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
    if (hayMsgFromESP32())  // Si el esp32 ha respondido
    {
        readMsgFromESP32(msgFromESP32); // Leer mensaje del puerto serie y guardarlo en msgFromESP32
        SerialPC.println("Respuesta del ESP32: " + msgFromESP32);  
    } 
    else // No se ha recibido respuesta del ESP32
    {
        //SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));

        // Se considera que no hay conexión WiFi
        msgFromESP32 = "TIMEOUT";;
    }

}*/
void waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual
    String tempBuffer = "";  // Buffer temporal para ensamblar el mensaje

    // Espera hasta que se reciba un mensaje o se exceda el tiempo de espera
    while (!isTimeoutExceeded(startTime, timeout)) 
    {
        if (hayMsgFromESP32())  // Si el esp32 ha respondido
        {
            if (processCharacter(tempBuffer, msgFromESP32)) 
                return;  // Sale de la función cuando se ha procesado un mensaje completo del ESP32
        }

        delay(50);  // Evita que el bucle sea demasiado intensivo
    }

    // Si se alcanza el tiempo de espera sin recibir un mensaje
    #if defined(SM_DEBUG)
        SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));
    #endif

    msgFromESP32 = "TIMEOUT";  // Marca el mensaje como TIMEOUT si no se recibió nada útil
}


/*-----------------------------------------------------------------------------*/
/**
 * Comprueba si se ha excedido el tiempo de espera.
 * 
 * Esta función compara el tiempo actual con el tiempo de inicio y verifica si se ha excedido el tiempo de espera especificado.
 * 
 * @param startTime El tiempo de inicio en milisegundos.
 * @param timeout El tiempo de espera en milisegundos.
 * @return true si se ha excedido el tiempo de espera, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
inline bool isTimeoutExceeded(unsigned long &startTime, unsigned long &timeout) 
{ 
    return millis() - startTime > timeout; 
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Procesa un carácter recibido del serial del ESP32 y construye un mensaje completo.
 *
 * Esta función lee un carácter del serial del ESP32 y lo acumula en un buffer temporal.
 * Si el carácter leído es un salto de línea ('\n'), se considera que el mensaje está completo.
 * En ese caso, se elimina cualquier espacio en blanco del buffer temporal, y si no está vacío,
 * se asigna el contenido del buffer al mensaje desde el ESP32 y se retorna true.
 *
 * @param tempBuffer Referencia al buffer temporal donde se acumulan los caracteres.
 * @param msgFromESP32 Referencia a la cadena donde se almacenará el mensaje completo del ESP32.
 * @return true Si se ha recibido y procesado un mensaje completo.
 * @return false Si aún no se ha recibido un mensaje completo.
 */
/*-----------------------------------------------------------------------------*/
bool processCharacter(String &tempBuffer, String &msgFromESP32) 
{
    char c = SerialESP32.read();  // Lee un carácter del serial del ESP32

    if (c == '\n') 
    {
        tempBuffer.trim();  // Elimina los espacios en blanco del buffer temporal

        if (tempBuffer.length() > 0)   // Solo procesa si no está vacío
        {
            msgFromESP32 = tempBuffer;
            #if defined(SM_DEBUG)
                SerialPC.print("\n---> Respuesta del ESP32: "); SerialPC.print("\"" + msgFromESP32); SerialPC.println("\"");
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