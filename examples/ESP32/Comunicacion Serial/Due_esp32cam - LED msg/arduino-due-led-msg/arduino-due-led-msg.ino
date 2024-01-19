
#define SerialPC Serial
#define SerialDueESP32 Serial1

void setup() {
  SerialPC.begin(115200);
  while (!SerialPC);

  // DEBE TENER LA MISMA VELOCIDAD EN BAUDIOS QUE EL ESP32 (p.ej. 115200)
  SerialDueESP32.begin(115200);
  while (!SerialDueESP32);
}

void loop() {
  if (SerialPC.available() > 0) {
    String command = SerialPC.readStringUntil('\n');

    SerialPC.print("Mensaje a enviar: "); SerialPC.println(command);
    SerialDueESP32.print(command); // Envía al ESP32-CAM
  }

  if (SerialDueESP32.available() > 0) {
    String msg = SerialDueESP32.readStringUntil('\n');

    SerialPC.print("Mensaje recibido: "); SerialPC.println(msg); 
  }

}
