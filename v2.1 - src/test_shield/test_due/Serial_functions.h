
/*
  --------- CONEXIÓN ESP32-CAM PLUS CON ARDUINO DUE -----------

 ESP32-CAM PLUS conectado por Serial (UART) de la siguiente forma:  

      ------------------------------------------------------
      |    ESP32-CAM PLUS        |  Arduino Due (Serial1)  |
      ------------------------------------------------------
      |    IO17 (Tx1) (4º izq)   |      pin 19 (Rx1)       |    
      |    IO16 (Rx1) (5º izq)   |      pin 18 (Tx1)       |
      |      3V3                 |        3V3              |
      |      GND                 |        GND              | 
      ------------------------------------------------------

-----------------------------------------------------

*/



#ifndef SERIAL_FUNCTIONS_H
#define SERIAL_FUNCTIONS_H

#define SerialPC Serial
#define SerialESP32 Serial1 // Comunicación Serial con ESP32



/******************************************************************************/
inline void     setupSerialPC() { SerialPC.begin(115200); };        // Configurar comunicación Serial con PC
inline void     setupSerialESP32() { SerialESP32.begin(115200); };  // Configurar comunicación Serial con ESP32
void            pingESP32();                                        // Comprobar conexión con ESP32

inline void     sendMsgToESP32(const String &msg) { SerialESP32.println(msg); };                  
inline bool     hayMsgFromESP32() { return SerialESP32.available() > 0; };                                 
inline bool     isESP32SerialEmpty(){ return !hayMsgFromESP32(); }  
inline void     readMsgFromSerialESP32(String &msgFromESP32) { msgFromESP32 = SerialESP32.readStringUntil('\n'); msgFromESP32.trim(); };       

void            waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout);            // Espera la respuesta del ESP32, sea cual sea, y la devuelve en msgFromESP32.
inline bool     isTimeoutExceeded(unsigned long &startTime, unsigned long &timeout);            // Comprobar si se ha excedido el tiempo de espera
/******************************************************************************/






/******************************************************************************/

/*-----------------------------------------------------------------------------*/
/**
 * Comprueba la conexión Serial con el ESP32.
 * 
 * Esta función envía un mensaje al ESP32 y espera una respuesta. 
 * Si la respuesta es "PONG", significa que la comunicación con el ESP32 se ha establecido correctamente.
 * Si la respuesta es "TIMEOUT", significa que no se ha recibido respuesta del ESP32.
 * Si la respuesta es cualquier otro mensaje, se considera desconocido y se establece la comunicación como no exitosa.
 */
/*-----------------------------------------------------------------------------*/
void pingESP32()
{
    // ---- COMPROBAR CONEXIÓN ---------------------------------
    // Enviar mensaje al ESP32 y esperar respuesta
    sendMsgToESP32("PING");
    // ---------------------------------------------------------

    // ---- RESPUESTA DEL ESP32 --------------------------------
    String msgFromESP32;
    unsigned long timeout = 3000; // Espera máxima de 3 segundos
    waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32

    if (msgFromESP32 == "PONG")             SerialPC.println("  ESP32: Saludos recibidos!");
    else if (msgFromESP32 == "TIMEOUT")     SerialPC.println("  ESP32: TIMEOUT");
    else                                    SerialPC.println("  ESP32: UNKNOWN MESSAGE");
    // ---------------------------------------------------------
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



/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Espera la respuesta del ESP32 y la devuelve
 * 
 * @return La respuesta del ESP32.
 */
/*---------------------------------------------------------------------------------------------------------*/
void waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el ESP32 responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while(isESP32SerialEmpty() && !isTimeoutExceeded(startTime, timeout));

    // Cuando se recibe mensaje o se pasa el timeout, entonces se comprueba la respuesta
    if (hayMsgFromESP32())   readMsgFromSerialESP32(msgFromESP32); // Leer mensaje del puerto serie y guardarlo en msgFromESP32
    else                     msgFromESP32 = "TIMEOUT"; // No se ha recibido respuesta del ESP32 en 'timeout' segundos

}


/******************************************************************************/
/******************************************************************************/

#endif