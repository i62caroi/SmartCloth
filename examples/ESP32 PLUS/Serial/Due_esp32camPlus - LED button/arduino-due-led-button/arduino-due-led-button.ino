/*
  Irene Casares Rodríguez
  07/04/24

  Al pulsar el botón (Lector), el Due envía por Serial al ESP32-CAM el mensaje "ON"
  para que encienda el led. Al recibir ese mensaje, el ESP32-CAM enciende el led y responde
  con "Led encendido". 
  Al volver a pulsar el botón, el Due le manda el mensaje "OFF" para que apague el led. Al 
  recibir ese mensaje, el ESP32-CAM apaga el led y responde con "Led apagado". 
*/

/*
    BOTON BARCODE conectado de la siguiente forma (indistintamente):
        -----------------
        | GND | Barcode |
        -----------------
        | GND |   53    |       
        -----------------
    ¡IMPORTANTE! El GND junto al D53 en el screw shield no está conectado a nada. Usar alguno
    de los que están junto a 5V o 3.3V.
*/

#define SerialPC Serial
#define SerialDueESP32 Serial1

const int pinBarcode = 53;   

bool ledState = false; // Estado del LED

String command = "OFF";
String msg = "";


void setup() {
    SerialPC.begin(115200);
    while (!SerialPC);
    delay(100);

    SerialDueESP32.begin(115200);
    while (!SerialDueESP32);
    delay(100);

    SerialPC.println("Arduino Due");

    // Inicializar botón
    pinMode(pinBarcode, INPUT_PULLUP); // Activo a la baja (LOW)
}

void loop() {
    // ----- ENVIAR MENSAJE AL PULSAR BOTÓN -----
    if (digitalRead(pinBarcode) == LOW) { // Si se pulsa el botón
      delay(10); // Esperar para evitar rebotes
      if (digitalRead(pinBarcode) == LOW) { // Comprobar que sigue pulsado
        ledState = !ledState; // Cambiar el estado del LED
        if(ledState) command = "ON"; // Encender led
        else command = "OFF"; // Apagar led
        while (digitalRead(pinBarcode) == LOW) {} // Esperar a que se suelte el botón

        SerialPC.print("Mensaje a enviar: "); SerialPC.println(command);
        SerialDueESP32.print(command); // Envía al ESP32-CAM
      }
    }

  
    // ----- RECIBIR MENSAJE -----
    if (SerialDueESP32.available() > 0) {
        msg = SerialDueESP32.readStringUntil('\n');

        SerialPC.print("Mensaje recibido: "); SerialPC.println(msg); 
    }

    delay(100);
}
