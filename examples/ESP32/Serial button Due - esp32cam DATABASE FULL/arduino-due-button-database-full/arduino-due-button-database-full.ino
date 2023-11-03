/*
  Irene Casares Rodríguez
  30/10/23

  Al pulsar el botón (el de guardar), el Due envía por Serial al ESP32-CAM el mensaje "save"
  para guarde la información. Al recibir ese mensaje, el ESP32-CAM genera valores aleatorios 
  de macronutrientes y hace una petición POST para guardarlos en la base de datos.
*/

//#define SerialPC Serial //No es necesario
//#define SerialDueESP32 Serial1 // Ya definido en Serial_esp32cam.h

#include "Serial_esp32cam.h" //Comunicación con ESP32-CAM
#include "random_values.h" // Generar valores aleatorios y conformar query

const int pinGuardar = 25;   // Morado 


void setup() {
    Serial.begin(115200);
    while (!Serial);

    SerialDueESP32.begin(115200);
    while (!SerialDueESP32);

    // Inicializar botón
    pinMode(pinGuardar, INPUT); // Pull-down con resistencias externas, activo al alta (HIGH)
}

void loop() {
    // ----- ENVIAR MENSAJE AL PULSAR BOTÓN -----
    if (digitalRead(pinGuardar) == HIGH) { // Si se pulsa el botón
      delay(10); // Esperar para evitar rebotes
      if (digitalRead(pinGuardar) == HIGH) { // Comprobar que sigue pulsado
        while (digitalRead(pinGuardar) == HIGH) {} // Esperar a que se suelte el botón
        
        // Generar valores aleatorios y conformar la query
        /*String msgToESP32 = generarValores_y_generarQuery();

        Serial.print("Mensaje a enviar: "); Serial.println(msgToESP32); //"save"
        SerialDueESP32.print(msgToESP32); // Envía al ESP32-CAM*/

        saveComidaDatabase(); 
      }
    }

  
    // ----- RECIBIR MENSAJE -----
    /*if (SerialDueESP32.available() > 0) {
        String msgFromESP32 = SerialDueESP32.readStringUntil('\n');

        Serial.print("Mensaje recibido: "); Serial.println(msgFromESP32); 
    }*/

    delay(1);
}
