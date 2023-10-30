#define SerialPC Serial
#define SerialDueESP32 Serial1

void setup() {
  SerialPC.begin(9600);
  while (!SerialPC);

  SerialDueESP32.begin(9600);
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
