/*
  Irene Casares Rodríguez
  30/10/23

  Al pulsar el botón (el de guardar), el Due envía por Serial al ESP32-CAM el mensaje "ON"
  para que encienda el led. Al recibir ese mensaje, el ESP32-CAM enciende el led y responde
  con "Led encendido". 
  Al volver a pulsar el botón, el Due le manda el mensaje "OFF" para que apague el led. Al 
  recibir ese mensaje, el ESP32-CAM apaga el led y responde con "Led apagado". 
*/

#define SerialPC Serial
#define SerialDueESP32 Serial1

const int pinGuardar = 25;   // Morado 

bool ledState = false; // Estado del LED

String command = "OFF";
String msg = "";


void setup() {
    SerialPC.begin(9600);
    while (!SerialPC);

    SerialDueESP32.begin(9600);
    while (!SerialDueESP32);

    // Inicializar botón
    pinMode(pinGuardar, INPUT); // Pull-down, activo al alta (HIGH)
}

void loop() {
    // ----- ENVIAR MENSAJE AL PULSAR BOTÓN -----
    if (digitalRead(pinGuardar) == HIGH) { // Si se pulsa el botón
      delay(10); // Esperar para evitar rebotes
      if (digitalRead(pinGuardar) == HIGH) { // Comprobar que sigue pulsado
        ledState = !ledState; // Cambiar el estado del LED
        if(ledState) command = "ON"; // Encender led
        else command = "OFF"; // Apagar led
        while (digitalRead(pinGuardar) == HIGH) {} // Esperar a que se suelte el botón

        SerialPC.print("Mensaje a enviar: "); SerialPC.println(command);
        SerialDueESP32.print(command); // Envía al ESP32-CAM
      }
    }

  
    // ----- RECIBIR MENSAJE -----
    if (SerialDueESP32.available() > 0) {
        msg = SerialDueESP32.readStringUntil('\n');

        SerialPC.print("Mensaje recibido: "); SerialPC.println(msg); 
    }

    delay(1);
}
