<img src="images/smartLogo.png" width="400">

## Descripción

SmartCloth es un dispositivo inteligente diseñado para ayudar a los usuarios diabéticos a realizar un seguimiento de su ingesta alimentaria y obtener información nutricional detallada. Este proyecto incluye tanto el hardware como el software necesarios para utilizar SmartCloth de manera efectiva.

## SmartCloth 1.0
### Características Principales 
- **Seguimiento Nutricional:** SmartCloth utiliza sensores integrados para pesar los alimentos y calcular su valor nutricional.
- **Interfaz de Usuario Intuitiva:** La interfaz de usuario proporciona una experiencia fácil de usar para los usuarios, con opciones claras y accesibles.

### Comunicaciones
<img src="Docs/Diseño-SmartCloth/Hardware/Diagrama_de_comunicaciones_v1.png" width="600">

### Interfaz de Usuario
#### Dashboard
<img src="Docs/Diseño-SmartCloth/Interfaz/Dashboards/Dashboard2.png" width="400">

#### Botonera de Grupos de Alimentos
<img src="Docs/Diseño-SmartCloth/Interfaz/Iconografía/Botonera_5X4_con_bordes.png" width="300">

#### Botonera de Acciones
<img src="Docs/Diseño-SmartCloth/Interfaz/Iconografía/Botonera_5X1_con_bordes.png" width="300">

### Aspecto externo
<img src="Docs/Diseño-SmartCloth/Interfaz/Aspecto_externo/SmartCloth_v1.jpg" width="600">

## SmartCloth 2.0
### Características adicionales
- **Lectura de códigos de barras:** Utiliza un lector de códigos de barras para procesar alimentos precocinados o envasados.
- **Conectividad Wi-Fi:** El dispositivo puede conectarse a Internet para almacenar información de las comidas en la base de datos y acceder a información actualizada sobre productos comerciales.

### Comunicaciones 
<img src="Docs/Diseño-SmartCloth/Hardware/Diagrama_de_comunicaciones_v2.png" width="600">

### Aspecto externo v2.0
<img src="Docs/Diseño-SmartCloth/Interfaz/Aspecto_externo/SmartCloth_v2_0.jpeg" width="600">

### Aspecto externo v2.1
<img src="Docs/Diseño-SmartCloth/Interfaz/Aspecto_externo/SmartCloth_v2_1.jpeg" width="600">

### Montaje del Hardware
Para ensamblar el hardware de SmartCloth 2.0, por favor consulte el [documento de montaje](./Docs/SmartCloth%20assembly%20document%20V0.2.pdf).

### Programación del dispositivo
1. Guardar imágenes en SD ([contenido_SD](v2.1%20-%20src/contenido_SD)) manteniendo la estructura de ficheros
2. Poner RTC en hora (programa [RTC_set_time](./v2.1%20-%20src/RTC/RTC_set_time/RTC_set_time.ino))
3. Calibrar báscula (programa [calibrate_scale](./v2.1%20-%20src/calibrate_scale/calibrate_scale.ino)) 
   1. Poner peso conocido (p.ej. celda de carga que pesa 30gr)
   2. 𝑒𝑠𝑐𝑎𝑙𝑎 𝑖𝑛𝑖𝑐𝑖𝑎𝑙 ∗ 𝑔𝑟 𝑜𝑏𝑡𝑒𝑛𝑖𝑑𝑜𝑠 𝑎𝑙 𝑝𝑒𝑠𝑎𝑟 = 𝑋
   3. 𝑋/30 𝑔𝑟 = 𝑛𝑢𝑒𝑣𝑎 𝑒𝑠𝑐𝑎𝑙𝑎 (𝑏𝑎𝑠𝑐𝑢𝑙𝑎 𝑐𝑎𝑙𝑖𝑏𝑟𝑎𝑑𝑎)
   4. Modificar programa Arduino con nueva escala calculada ([scale.set_scale(escala)](https://github.com/i62caroi/SmartCloth/blob/main/v2.1%20-%20src/smartcloth_v2/Scale.h#L87) en la función [setupScale()](https://github.com/i62caroi/SmartCloth/blob/main/v2.1%20-%20src/smartcloth_v2/Scale.h#L68)) del programa del Arduino
4. Programar Arduino ([smartcloth_v2](v2.1%20-%20src/smartcloth_v2/smartcloth_v2.ino))
5. Programar ESP32 ([esp32cam-v1](v2.1%20-%20src/esp32cam-v1/esp32cam-v1.ino)) y añadir MAC del ESP32 a la database

## Contacto
Para obtener más información, visite nuestro sitio web oficial en [smartcloth.org](https://smartcloth.org).
