<?php

/**
 * Archivo PHP para recibir y procesar datos JSON enviados desde un dispositivo ESP32.
 * Este archivo se encarga de establecer la conexión con la base de datos y recibir los datos enviados por el ESP32.
 * Los datos recibidos se guardan en la base de datos para su posterior procesamiento.
 *
 * @package SmartCloth
 * @author Irene Casares
 * @version 2.1
 */

// Configuración de la conexión a la base de datos
$servername = "localhost";
$dbname = "smartclo_SM-database";
$username = "smartclo_irene";
$password = "SmartCloth.database/pass23";

if ($_SERVER["REQUEST_METHOD"] == "POST") 
{
    // Leer el contenido de la solicitud POST
    $json = file_get_contents('php://input');
    $data = json_decode($json);
        
    // Crear conexión a la base de datos
    $conn = new mysqli($servername, $username, $password, $dbname);
    // Verificar conexión
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    } 

    // ----- MAC DEL SMARTCLOTH -----
    // Obtener la dirección MAC del JSON recibido
    $mac = validateInput($data->mac);

    // ---- USUARIO DEL SMARTCLOTH -----
    // Obtener el DNI del usuario basado en la MAC del mantel
    $sql = "SELECT user_id FROM mantels WHERE mac = '$mac'";

    $result = $conn->query($sql);
    if ($result->num_rows > 0) 
    {
        $row = $result->fetch_assoc();
        $user_id = $row['user_id'];

        // ----- COMIDA -----
        // Procesar cada comida en el JSON
        foreach($data->comidas as $comida) 
        {
            date_default_timezone_set("UTC");
            $fechaUnix = $comida->fecha; // Fecha en timestamp Unix
            $fecha = date('Y-m-d H:i:s', $fechaUnix); // Convertir a formato de fecha y hora

            // Insertar la nueva comida en la base de datos
            $sql = "INSERT INTO comidas (fecha) VALUES ('$fecha')";

            if ($conn->query($sql) === TRUE) 
            {
                $comida_id = $conn->insert_id;

                // ----- PLATO -----
                // Procesar cada plato de la comida
                foreach($comida->platos as $plato) 
                {
                    // Insertar el nuevo plato en la base de datos
                    $sql = "INSERT INTO platos (comida_id) VALUES ($comida_id)";

                    if ($conn->query($sql) === TRUE) 
                    {
                        $plato_id = $conn->insert_id;

                        // ----- ALIMENTO -----
                        // Procesar cada alimento del plato
                        foreach($plato->alimentos as $alimento) 
                        {
                            // Obtener grupo y peso del alimento
                            $grupo = validateInput($alimento->grupo);
                            $peso = validateInput($alimento->peso);
                            $tipoalimento_id = null; // Se modificará según el caso (alimento de tipo grupo o de tipo barcode)
                            $mensaje = null; // Mensaje a mostrar en la consola según el caso

                            // ----- ALIMENTO DE NUESTROS GRUPOS -------------------------
                            // Si el alimento pertenece a un grupo definido (distinto de 50, que es el grupo barcode)
                            if ($grupo != 50) 
                            {
                                // Obtener id del tipo de alimento referente al grupo de alimentos indicado
                                $sql = "SELECT tipoalimento_id FROM grupos WHERE cod_grupo = '$grupo'";

                                $result = $conn->query($sql);
                                if ($result->num_rows > 0) 
                                {
                                    $row = $result->fetch_assoc();
                                    $tipoalimento_id = $row['tipoalimento_id'];
                                    $mensaje = "Nuevo alimento creado\n";
                                } 
                                else 
                                {
                                    echo "No se encontró un tipo de alimento correspondiente al grupo: " . $grupo;
                                    continue;
                                }
                            } 
                            // -----------------------------------------------------------
                            
                            // ----- ALIMENTO TIPO BARCODE -------------------------------
                            // Si el alimento es un producto con código de barras
                            else if (isset($alimento->ean))  // El ID del grupo es 50 y el JSON contiene un campo EAN, entonces es un barcode
                            {   
                                // Obtener el código de barras del alimento
                                $ean = validateInput($alimento->ean);

                                // Comprobar si el código de barras ya está en la base de datos
                                $sql = "SELECT tipoalimento_id FROM barcodes WHERE ean = '$ean'";

                                // ----- BARCODE LEIDO ANTERIORMENTE ---------
                                $result = $conn->query($sql);
                                if ($result->num_rows > 0) 
                                {
                                    // Obtener id_tipo del barcode, que se ha leído otras veces
                                    $row = $result->fetch_assoc();
                                    $tipoalimento_id = $row['tipoalimento_id'];
                                    $mensaje = "Nuevo alimento creado con código de barras ya existente\n";
                                } 
                                // -------------------------------------------

                                // ----- NUNCA SE HA LEIDO ESTE BARCODE ------
                                else 
                                {
                                    echo "Producto barcode nuevo. Buscando informacion...\n";

                                    // ----- BUSCAR INFO NUTRICIONAL -----
                                    // Obtener la información nutricional del producto desde OpenFoodFacts
                                    $productData = getNutritionalDataFromOpenFoodFacts($ean);
                                    if ($productData) 
                                    {
                                        echo "Producto encontrado\n";

                                        $nombre = $productData['nombre'];
                                        $carb_g = $productData['carb_g'] / 100.0;
                                        $kcal_g = $productData['kcal_g'] / 100.0;
                                        $lip_g = $productData['lip_g'] / 100.0;
                                        $prot_g = $productData['prot_g'] / 100.0;

                                        // ----- NUEVO TIPO ALIMENTO -----
                                        // Insertar el nuevo tipo de alimento en la base de datos
                                        $sql = "INSERT INTO tipo_alimentos (kcal_g, prot_g, lip_g, carb_g, tipo) VALUES ('$kcal_g', '$prot_g', '$lip_g', '$carb_g', 'barcode')";

                                        if ($conn->query($sql) === TRUE) 
                                        {
                                            echo "Creado nuevo tipo de alimentos\n";

                                            // Obtener el id_tipo del nuevo tipo de alimento
                                            $tipoalimento_id = $conn->insert_id;

                                            // ---- NUEVO PRODUCTO LEIDO -----
                                            // Insertar el código de barras en la base de datos
                                            $sql = "INSERT INTO barcodes (ean, nombre, tipoalimento_id) VALUES ('$ean', '$nombre', '$tipoalimento_id')";

                                            if ($conn->query($sql) !== TRUE) 
                                            {
                                                echo "Error: " . $sql . "<br>" . $conn->error;
                                                continue;
                                            }

                                            $mensaje = "Nuevo alimento creado con nuevo código de barras\n";
                                        } 
                                        else 
                                        {
                                            echo "Error: " . $sql . "<br>" . $conn->error;
                                            continue;
                                        }
                                    } 
                                    else 
                                    {
                                        echo "No se pudo obtener información del código de barras: " . $ean;
                                        continue;
                                    }
                                }
                                // -------------------------------------------
                            }

                            // ----- INSERTAR ALIMENTO -----
                            if ($tipoalimento_id !== null) 
                            {
                                $sql = "INSERT INTO alimentos (peso, plato_id, tipoalimento_id, user_id) VALUES ('$peso', $plato_id, '$tipoalimento_id', '$user_id')";

                                if ($conn->query($sql) == TRUE) {
                                    echo $mensaje;
                                } 
                                else 
                                {
                                    echo "Error: " . $sql . "<br>" . $conn->error;
                                }
                            }
                            // -----------------------------------------------------------
                        }

                        echo "Nuevo plato creado\n";

                    } 
                    else 
                    {
                        echo "Error: " . $sql . "<br>" . $conn->error;
                    }
                }

                echo "Nueva comida creada\n";

            } 
            else 
            {
                echo "Error: " . $sql . "<br>" . $conn->error;
            }
        }
    } 
    else 
    {
        echo "No se encontró un usuario correspondiente a la MAC: " . $mac;
    }

    // Cerrar la conexión a la base de datos
    $conn->close();

} 
else 
{
    echo "No data posted with HTTP POST.";
}



/**
 * Función para validar y limpiar los datos de entrada.
 *
 * Esta función toma un dato como entrada y realiza las siguientes operaciones:
 * - Elimina los espacios en blanco al principio y al final del dato.
 * - Elimina las barras invertidas (\) del dato.
 * - Convierte los caracteres especiales en entidades HTML.
 *
 * @param string $data El dato a validar y limpiar.
 * @return string El dato validado y limpiado.
 */
function validateInput($data) 
{
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}





/**
 * Obtiene los datos nutricionales de Open Food Facts para un código EAN dado.
 *
 * @param string $ean El código EAN del producto.
 * @return array|false Un array con los datos nutricionales del producto si se obtuvieron datos, o false si no se obtuvieron datos.
 */
function getNutritionalDataFromOpenFoodFacts($ean) 
{
    // Construir la URL de la solicitud
    $url = "https://world.openfoodfacts.org/api/v2/product/" . $ean . "?fields=product_name,product_name_es,carbohydrates_100g,energy-kcal_100g,fat_100g,proteins_100g";
    
    $response = file_get_contents($url);    // Obtener la respuesta de la API de Open Food Facts
    $data = json_decode($response, true);   // Decodificar la respuesta JSON

    // Verificar si se obtuvieron datos del producto
    if (isset($data['product'])) 
    {
        $product = $data['product'];   // Obtener los datos del producto

        // Crear un array con los datos nutricionales del producto
        return [
            'nombre' => isset($product['product_name_es']) ? $product['product_name_es'] : $product['product_name'],    // Tomar el nombre en español si está disponible
            'carb_g' => isset($product['carbohydrates_100g']) ? $product['carbohydrates_100g'] : 0.0,                   // Si no hay datos de carbohidratos, establecer a 0
            'kcal_g' => isset($product['energy-kcal_100g']) ? $product['energy-kcal_100g'] : 0.0,                       // Si no hay datos de calorías, establecer a 0
            'lip_g' => isset($product['fat_100g']) ? $product['fat_100g'] : 0.0,                                        // Si no hay datos de grasas, establecer a 0
            'prot_g' => isset($product['proteins_100g']) ? $product['proteins_100g'] : 0.0                              // Si no hay datos de proteínas, establecer a 0
        ];
    } 
    else {
        return false;
    }
}


?>