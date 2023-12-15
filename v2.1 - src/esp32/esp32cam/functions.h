#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <ArduinoJson.h> 

#define SerialESP32Due Serial1

DynamicJsonDocument doc(1024);

/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte en JSON el fichero txt recibido por comunicación Serial 
 *        desde el Arduino Due.
 */
/*-----------------------------------------------------------------------------*/
/*void textToJSON()
{
        DynamicJsonDocument doc(1024);

        JsonArray comidas = doc.createNestedArray("comidas");
        JsonObject comida;

        JsonArray platos;
        JsonObject plato;

        JsonArray alimentos;

        while (SerialESP32Due.available()) 
        {
            String line = SerialESP32Due.readStringUntil('\n');
            line.trim();

            if (line == "INICIO-COMIDA") 
            {
                comida = comidas.createNestedObject();
                platos = comida.createNestedArray("platos");
            } 
            else if (line == "INICIO-PLATO") 
            {
                plato = platos.createNestedObject();
                alimentos = plato.createNestedArray("alimentos");
            } 
            else if (line.startsWith("ALIMENTO")) 
            {
                JsonObject alimento = alimentos.createNestedObject();
                // Aquí asumimos que los datos del alimento están separados por comas
                int firstCommaIndex = line.indexOf(',');
                int secondCommaIndex = line.lastIndexOf(',');
                alimento["grupo"] = line.substring(firstCommaIndex + 1, secondCommaIndex);
                alimento["peso"] = line.substring(secondCommaIndex + 1);
            } 
            else if (line.startsWith("FIN-COMIDA")) 
            {
                int firstCommaIndex = line.indexOf(',');
                int secondCommaIndex = line.lastIndexOf(',');
                comida["fecha"] = line.substring(firstCommaIndex + 1, secondCommaIndex);
                comida["hora"] = line.substring(secondCommaIndex + 1);
            }
        }

        //serializeJson(doc, Serial);
        serializeJsonPretty(doc, Serial);


}*/


void createJson(String &msgFromDue)
{
    JsonArray comidas = doc.createNestedArray("comidas");
    JsonArray platos;
    JsonArray alimentos;
    JsonObject comida; // Se define fuera para que persista en las iteraciones y esté disponible
                       // para "INICIO-COMIDA" y "FIN-COMIDA"

    // Se mantiene escuchando hasta recibir SAVE-FIN
    while (!msgFromDue.startsWith("SAVE-FIN")) 
    {
        // --- MENSAJE RECIBIDO ---
        msgFromDue = SerialESP32Due.readStringUntil('\n');
        msgFromDue.trim();
        SerialPC.print("Mensaje fichero txt: "); SerialPC.println(msgFromDue); SerialPC.println();

        if (msgFromDue.startsWith("INICIO-COMIDA")) 
        {
            comida = comidas.createNestedObject();
            platos = comida.createNestedArray("platos");
        } 
        else if (msgFromDue.startsWith("INICIO-PLATO"))
        {
            JsonObject plato = platos.createNestedObject();
            alimentos = plato.createNestedArray("alimentos");
        } 
        else if (msgFromDue.startsWith("ALIMENTO")) 
        {
            JsonObject alimento = alimentos.createNestedObject();
            int firstCommaIndex = msgFromDue.indexOf(',');
            int secondCommaIndex = msgFromDue.lastIndexOf(',');
            alimento["grupo"] = msgFromDue.substring(firstCommaIndex + 1, secondCommaIndex);
            alimento["peso"] = msgFromDue.substring(secondCommaIndex + 1);
        } 
        else if (msgFromDue.startsWith("FIN-COMIDA")) 
        {
            int firstCommaIndex = msgFromDue.indexOf(',');
            int secondCommaIndex = msgFromDue.lastIndexOf(',');
            comida["fecha"] = msgFromDue.substring(firstCommaIndex + 1, secondCommaIndex);
            comida["hora"] = msgFromDue.substring(secondCommaIndex + 1);
        }
    }
}

#endif