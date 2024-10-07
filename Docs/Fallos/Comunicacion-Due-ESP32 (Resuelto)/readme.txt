Inicialmente los mensajes se recibían y leían en el Due de esta forma:

void waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el ESP32 responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while(!hayMsgFromESP32() && !isTimeoutExceeded(startTime, timeout));

    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
    if (hayMsgFromESP32())  // Si el Due ha respondido
    {
        readMsgFromSerialESP32(msgFromESP32); // Leer mensaje del puerto serie y guardarlo en msgFromESP32
        SerialPC.print("\n---> Respuesta del ESP32: "); SerialPC.print("\"" + msgFromESP32); SerialPC.println("\"");
    } 
    else // No se ha recibido respuesta del ESP32
    {
        // Si se alcanza el tiempo de espera sin recibir un mensaje no vacío
        SerialPC.println(F("TIMEOUT del ESP32"));

        msgFromESP32 = "TIMEOUT";
    }
}




Eso hacía que de forma aleatoria hubiera mensajes que no se leyeran correctamente, aunque el ESP32 los hubiera mandado bien.
Muy probablemente sería debido a la función hayMsgFromESP32(), que devolvía 'true' si había algo en el buffer:

bool hayMsgFromESP32() { 
	return SerialESP32.available() > 0; 
}

Lo que provocaba que en cuanto se detectara al menos 1 byte en el buffer del Serial, se leyera el contenido y se asumiera que
eso era el mensaje enviado por el ESP32. Con lo cual, había veces en los que el ESP32 enviaba, por ejemplo, "BARCODE:<barcode>"
y el Due leía "" (mensaje vacío), así que creía que no se había leído el código de barras. Esto no es muy grave si se puede leer
de nuevo, pero en casos como el guardado de la comida (que el ESP32 envíe "SAVED-OK" pero el Due reciba "") sí es grave porque
el Due creería que no se ha guardado la comida y la copiaría en el TXT para intentar guardarla de nuevo más tarde, aunque en
realidad ya se haya subido a la base de datos.


Para resolverlo, se modificó la función de espera y lectura de mensajes para procesar los caracteres uno por uno hasta encontrar 
el carácter de fin de mensaje (\n), almacenando los caracteres en un buffer temporal. De este modo, el Due solo considera que ha 
recibido un mensaje válido cuando el buffer temporal contiene datos terminados en \n. Si solo se recibe un carácter de control 
como \n sin más contenido, no se procesa como un mensaje válido. Esta solución garantiza que el Due no trate como válido un mensaje 
vacío o incompleto. Así queda la función de espera de mensaje del ESP32:

void waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual
    String tempBuffer = "";  // Buffer temporal para ensamblar el mensaje

    // Espera hasta que se reciba un mensaje o se exceda el tiempo de espera
    while (!isTimeoutExceeded(startTime, timeout)) 
    {
        if (hayMsgFromESP32())  // Si el esp32 ha respondido
        {
            char c = SerialESP32.read();  // Lee un carácter del serial
            if (c == '\n') 
            {
                tempBuffer.trim();  // Elimina los espacios en blanco del buffer temporal

                if (tempBuffer.length() > 0)  // Solo procesa si no está vacío
                {
                    msgFromESP32 = tempBuffer;
                    #if defined(SM_DEBUG)
                        SerialPC.print("\n---> Respuesta del ESP32: "); SerialPC.print("\"" + msgFromESP32); SerialPC.println("\"");
                    #endif
                    return;  // Sale de la función con un mensaje válido
                }
                tempBuffer = "";  // Si estaba vacío, se resetea el buffer
            }
            else 
            {
                tempBuffer += c;  // Acumula el carácter en el buffer temporal
            }
        }

        delay(50);  // Evita que el bucle sea demasiado intensivo
    }

    // Si se alcanza el tiempo de espera sin recibir un mensaje
    #if defined(SM_DEBUG)
        SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));
    #endif

    msgFromESP32 = "TIMEOUT";  // Marca el mensaje como TIMEOUT si no se recibió nada útil
}




Además, la parte del procesamiento caracter a caracter se ha encapsulado en una función por claridad del código:



bool processCharacter(String &tempBuffer, String &msgFromESP32) 
{
    char c = SerialESP32.read();  // Lee un carácter del serial del ESP32

    if (c == '\n') 
    {
        tempBuffer.trim();  // Elimina los espacios en blanco del buffer temporal

        if (tempBuffer.length() > 0) {  // Solo procesa si no está vacío
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






Estos cambios también se han aplicado a la función de espera de mensaje que atiende a interrupciones, usada para cancelar la lectura del barcode
si se detecta pulsación:


Versión inicial:

void waitResponseFromESP32WithEvents(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el ESP32 responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while(!hayMsgFromESP32() && !isTimeoutExceeded(startTime, timeout))
    {
        if(eventOccurred()) 
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Interrupcion mientras se leia barcode"));
            #endif
            msgFromESP32 = "INTERRUPTION"; // Señalar que se ha interrumpido
            return; // Salir de la función si se detecta interrupción (cancelación manual de la lectura)
        }
        delay(50); // Pequeño retraso para evitar una espera activa intensa
    }

    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
    if (hayMsgFromESP32())  // Si el esp32 ha respondido
    {
        readMsgFromSerialESP32(msgFromESP32); // Leer mensaje del puerto serie y guardarlo en msgFromESP32
        #if defined(SM_DEBUG)
            SerialPC.println("\n---> Respuesta del ESP32 (attempt 1): " + msgFromESP32);  
        #endif
    } 
    else // No se ha recibido respuesta del ESP32
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32 durante lectura de barcode"));
        #endif

        // Se considera que no se ha detectado barcode
        msgFromESP32 = "TIMEOUT";
    }

}



Versión con procesamiento caracter a caracter:


void waitResponseFromESP32WithEvents(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual
    String tempBuffer = "";  // Buffer temporal para acumular datos

    // Espera hasta que se reciba un mensaje o se exceda el tiempo de espera
    while (!isTimeoutExceeded(startTime, timeout)) 
    {
        if (hayMsgFromESP32())  // Si el esp32 ha respondido
        {
            if (processCharacter(tempBuffer, msgFromESP32)) 
                return;  // Sale de la función cuando se ha procesado un mensaje completo del ESP32
        }


         if(eventOccurred()) 
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Interrupcion mientras se leia barcode"));
            #endif
            msgFromESP32 = "INTERRUPTION"; // Señalar que se ha interrumpido
            return; // Salir de la función si se detecta interrupción (cancelación manual de la lectura)
        }

        delay(50);  // Evita que el bucle sea demasiado intensivo
    }


    // Si se alcanza el tiempo de espera sin recibir un mensaje
    #if defined(SM_DEBUG)
        SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));
    #endif

    msgFromESP32 = "TIMEOUT";  // Marca el mensaje como TIMEOUT si no se recibió nada útil

}

