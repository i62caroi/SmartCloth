#include <SoftwareSerial.h>
/*
#define rxPin 19
#define txPin 18

SoftwareSerial espSerial =  SoftwareSerial(rxPin, txPin);
*/

#define SerialPC Serial
#define SerialMegaESP32 Serial3

void setup() {
  SerialPC.begin(9600);
  while (!SerialPC);

  SerialMegaESP32.begin(9600);
  while (!SerialMegaESP32);

/*
  // Define pin modes for TX and RX
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  espSerial.begin(115200); // Inicializa la comunicación serial con el ESP32-CAM
  while (!espSerial);
  */
}

void loop() {
  if (SerialPC.available() > 0) {
    String command = SerialPC.readStringUntil('\n');

    SerialPC.print("Mensaje a enviar: "); SerialPC.println(command);
    SerialMegaESP32.print(command); // Envía al ESP32-CAM
  }

}
