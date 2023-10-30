#include <SoftwareSerial.h>

#define SerialPC Serial
#define SerialMegaESP32 Serial3

void setup() {
  SerialPC.begin(9600);
  while (!SerialPC);

  SerialMegaESP32.begin(9600);
  while (!SerialMegaESP32);
}

void loop() {
  if (SerialPC.available() > 0) {
    String command = SerialPC.readStringUntil('\n');

    SerialPC.print("Mensaje a enviar: "); SerialPC.println(command);
    SerialMegaESP32.print(command); // Envía al ESP32-CAM
  }

  if (SerialMegaESP32.available() > 0) {
    String msg = SerialMegaESP32.readStringUntil('\n');

    SerialPC.print("Mensaje recibido: "); SerialPC.println(msg); 
  }

}
