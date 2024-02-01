/*
    Irene Casares Rodríguez
    12/01/2024

    Funciones para enviar un String como simulación del esp32File del Due al ESP32 
    por comunicación Serial.
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "defines.h" // Seriales


// Simular la lectura del archivo
String string1 = "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,53.50\n"
                "ALIMENTO,9,53.50\n"
                "FIN-COMIDA,19.01.2024,17:55:36\n"
                "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,24.40\n"
                "FIN-COMIDA,19.01.2024,18:25:25\n";
                
                

String string2 = "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,53.50\n"
                "ALIMENTO,9,53.50\n"
                "FIN-COMIDA,19.01.2024,17:55:36\n"
                "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,24.40\n"
                "FIN-COMIDA,19.01.2024,18:25:25\n"
                "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,53.50\n"
                "ALIMENTO,9,53.50\n"
                "INICIO-PLATO\n"
                "ALIMENTO,3,32.07\n"
                "ALIMENTO,27,46.65\n"
                "ALIMENTO,10,16.23\n"
                "FIN-COMIDA,19.01.2024,10:39:36\n";


/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía el fichero (String de simulación) por Serial al esp32
 */
/*-----------------------------------------------------------------------------*/
void sendStringSimulationToEsp32(String fileContent)
{
    int start = 0;
    int end = fileContent.indexOf('\n');

    while (end != -1) 
    {
        String line = fileContent.substring(start, end);
        line.trim();

        // Envía la línea al ESP32 a través de Serial
        SerialDueESP32.println(line);

        start = end + 1;
        end = fileContent.indexOf('\n', start);
    }

    // Envía la última línea si existe
    if (start < fileContent.length()) {
        String line = fileContent.substring(start);
        line.trim();
        SerialDueESP32.println(line);
    }

    SerialDueESP32.println("FIN-TRANSMISION");
}



/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Comprueba la conexión WiFi del ESP32.
 * 
 * @return true si hay conexión WiFi, false si no la hay.
 */
/*---------------------------------------------------------------------------------------------------------*/
bool checkWifiConnection() {
    SerialPC.println("Comprobando la conexión WiFi del ESP32...");

    String command = "CHECK-WIFI";
    SerialPC.print("Cadena enviada al esp32: "); SerialPC.println(command);
    SerialDueESP32.print(command); //Envía la cadena al esp32

    unsigned long timeout = 10000;  // Espera máxima de 10 segundos
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    while (true) { // Bloquea el arduino en este bucle hasta que se reciba respuesta o se pase el TIMEOUT
        // Comprueba si hay datos disponibles en el puerto serie del ESP32
        if (SerialDueESP32.available() > 0) { // Si el esp32 ha respondido
            String responseFromESP32 = SerialDueESP32.readStringUntil('\n');
            responseFromESP32.trim();  // Elimina espacios en blanco al principio y al final
            SerialPC.print("Respuesta del ESP32: ");  SerialPC.println(responseFromESP32);

            if (responseFromESP32 == "WIFI") {
                // Respuesta OK, hay conexión WiFi
                SerialPC.println("Dice que hay wifi");
                return true;
            } 
            else if (responseFromESP32 == "NO-WIFI") {
                // Respuesta NO-WIFI, no hay conexión WiFi
                SerialPC.println("Dice que NO hay wifi");
                return false;
            }
        }

        // Comprueba si ha pasado un tiempo límite sin respuesta del esp32
        if (millis() - startTime > timeout) {
            // Tiempo de espera agotado, se considera que no hay conexión WiFi
            SerialPC.println("Timeout");
            return false;
        }
    }

    // Si sale del while (no debería), se considera que no hay conexión WiFi
    return false;
}



#endif