/**
 * @file calibrate_scale.ino
 * @brief Programa para calibrar una celda de carga HX711.
 * 
 * Este programa permite calibrar una celda de carga HX711 utilizando un objeto de peso conocido. 
 * Realiza múltiples lecturas y calcula una nueva escala para la celda de carga, mostrando los 
 * resultados en el monitor serie.
 *
 * @author Irene Casares Rodríguez
 * @date 22/04/25
 */


#include "HX711.h"


// -------------------------------------------------------------------------------------------------------
/**
 * @brief Definición de constantes y variables globales.
 * 
 * Se definen los pines de conexión de la celda de carga, los parámetros de calibración, 
 * y las variables necesarias para almacenar las medidas y controlar el proceso de calibración.
 */

// Pines de conexión de la celda de carga
const byte LOADCELL_DOUT_PIN = 2;
const byte LOADCELL_SCK_PIN = 3;

// Objeto HX711 para la celda de carga
HX711 scale; 

// Parámetros de calibración
const float PESO_CONOCIDO = 226.0;          // Peso del objeto de referencia en gramos (p.ej. mi móvil pesa 226gr)
float ESCALA_ACTUAL = 856.4875;            // Escala inicial (este valor inicial en realidad no importa)
const int   MUESTRAS_CALIBRACION = 5;       // Número de muestras para calibrar
const int   TIEMPO_ENTRE_MEDICIONES = 500;  // Tiempo entre mediciones en ms

// Variables para la calibración automática
float         medidas[MUESTRAS_CALIBRACION];
int           contador_medidas = 0;
unsigned long ultima_medicion = 0;
bool          modo_calibracion = true;
// -------------------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------------------------------------
// Declaración de funciones
// -------------------------------------------------------------------------------------------------------
void  setupScale();           // Configura la celda de carga
float weighScale();           // Devuelve el peso en gramos 
void  calcularNuevaEscala();  // Calcula la nueva escala
// -------------------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------------------------------------
/**
 * @brief Configuración inicial del programa.
 * 
 * Esta función se ejecuta una vez al inicio del programa. Se encarga de inicializar la 
 * comunicación serie, configurar la celda de carga y solicitar al usuario que coloque un 
 * objeto de peso conocido en la báscula para iniciar el proceso de calibración automática.
 */
void setup()
{
  Serial.begin(115200);
  while (!Serial);
  delay(100);

  Serial.println("\nIniciando calibración automática...");
  Serial.println("Báscula inicializada. Tomando medidas para calibración...");

  setupScale(); 

  Serial.println("Coloque un objeto de peso conocido en la báscula");
  Serial.print("Peso de referencia: "); Serial.print(PESO_CONOCIDO); Serial.println(" g\n\n");
}
// -------------------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------------------------------------
/**
 * @brief Bucle principal del programa.
 * 
 * Este bucle se ejecuta continuamente y se encarga de medir el peso en la balanza, 
 * almacenar las medidas y calcular una nueva escala de calibración cuando se han 
 * tomado suficientes muestras. También muestra el progreso de la calibración en 
 * el monitor serie.
 */
void loop()
{
  float peso = weighScale();
  Serial.print("\nPeso: "); Serial.print(peso); Serial.println(" g");
  
  // Guardar la medida actual
  medidas[contador_medidas] = peso;
  contador_medidas++;
  
  // Mostrar progreso
  Serial.print("  Medida "); Serial.print(contador_medidas); Serial.print(" de "); Serial.println(MUESTRAS_CALIBRACION);
  
  // Si ya tenemos todas las medidas, calcular nueva escala
  if (contador_medidas >= MUESTRAS_CALIBRACION) {
    calcularNuevaEscala();
    // Reiniciar para la siguiente calibración
    contador_medidas = 0;
    Serial.println("Listo para una nueva calibración. Coloque el objeto nuevamente.");
  }
  
  delay(TIEMPO_ENTRE_MEDICIONES);
}
// -------------------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------------------------------------
/**
 * @brief Configura la celda de carga HX711.
 * 
 * Esta función inicializa la celda de carga HX711, establece la escala inicial y realiza una tara 
 * (ajuste a cero) para eliminar cualquier peso residual. Se utiliza la biblioteca HX711 para 
 * interactuar con el hardware de la celda de carga.
 */
void setupScale()
{
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(ESCALA_ACTUAL); 
    scale.tare(5);  // Media de 5 medidas (5 por defecto)
    //scale.get_units(10);
    Serial.println(F("Scale initialized"));
}
// -------------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------------
/**
 * @brief Obtiene el peso actual de la balanza.
 * 
 * Esta función utiliza la biblioteca HX711 para obtener el peso en gramos de la balanza. 
 * Se realiza una lectura de la balanza y se devuelve el valor obtenido.
 * 
 * @return float El peso actual en gramos.
 */
float weighScale()
{
    return scale.get_units(5);
}
// -------------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------------
/**
 * @brief Calcula una nueva escala para la calibración de una balanza basada en medidas obtenidas.
 * 
 * Esta función toma un conjunto de medidas almacenadas en el vector `medidas`, calcula su media, 
 * y utiliza una fórmula para determinar una nueva escala de calibración. La fórmula utilizada es:
 * 
 *     nueva_escala = (ESCALA_ACTUAL * media) / PESO_CONOCIDO
 * 
 * Donde:
 * - `ESCALA_ACTUAL` es la escala previamente configurada.
 * - `media` es la media de las medidas obtenidas durante la calibración.
 * - `PESO_CONOCIDO` es el peso real conocido utilizado como referencia.
 * 
 * Una vez calculada la nueva escala, se muestran los resultados en el monitor serie, incluyendo 
 * la media de las medidas, la escala anterior y la nueva escala calculada. También se proporciona 
 * una instrucción para actualizar la constante `ESCALA_ACTUAL` con el nuevo valor calculado.
 * 
 * @note La función incluye una pausa de 3 segundos (`delay(3000)`) para permitir al usuario leer 
 * los resultados en el monitor serie.
 */
void calcularNuevaEscala()
{
  // Calcular la media de las medidas
  float suma = 0;
  for (int i = 0; i < MUESTRAS_CALIBRACION; i++) {
    suma += medidas[i];
  }
  float media = suma / MUESTRAS_CALIBRACION;
  
  // Aplicar la fórmula: (escala_inicial * gramos_obtenidos) / peso_real
  float nueva_escala = (ESCALA_ACTUAL * media) / PESO_CONOCIDO;
  
  // Mostrar resultados
  Serial.println("\n----- RESULTADO DE CALIBRACIÓN -----");
  Serial.print("Media de las medidas: ");   Serial.print(media);              Serial.println(" g");
  Serial.print("Escala anterior: ");        Serial.println(ESCALA_ACTUAL);
  Serial.print("Nueva escala calculada: "); Serial.println(nueva_escala, 4); // Mostrar 4 decimales
  Serial.println("Para aplicar esta calibración, actualiza la constante ESCALA_ACTUAL con este valor");
  
  // Aplicar la nueva escala para las siguientes mediciones
  ESCALA_ACTUAL = nueva_escala;
  scale.set_scale(ESCALA_ACTUAL);
  
  Serial.println("Nueva escala aplicada a la báscula.");
  Serial.println("-------------------------------\n");
  
  // Pequeña pausa para leer los resultados
  delay(3000);
}
// -------------------------------------------------------------------------------------------------------