/*
  Irene Casares Rodríguez
  30/10/23

  Al pulsar el botón (el de guardar), el Due envía por Serial al ESP32-CAM el mensaje "save"
  para guarde la información. Al recibir ese mensaje, el ESP32-CAM genera valores aleatorios 
  de macronutrientes y hace una petición POST para guardarlos en la base de datos.
*/

#define SerialPC Serial
#define SerialDueESP32 Serial1

const int pinGuardar = 25;   // Morado 

String command = "save";
String msg = "";


void setup() {
    SerialPC.begin(115200);
    while (!SerialPC);

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
        SerialPC.print("Mensaje a enviar: "); SerialPC.println(command); //"save"
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
