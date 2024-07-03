<?php

$servername = "localhost";
$dbname = "smartclo_SM-database";
$username = "smartclo_irene";
$password = "SmartCloth.database/pass23";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
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
    $mac = $data->mac;

    // ---- USUARIO DEL SMARTCLOTH -----
    // Obtener el DNI del usuario basado en la MAC del mantel
    $sql = "SELECT dni FROM UsuarioMantel UM INNER JOIN Mantel M ON UM.id_mantel = M.id_mantel WHERE M.MAC = '$mac'";
    $result = $conn->query($sql);
    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        $dni = $row['dni'];

        // ----- COMIDA -----
        // Procesar cada comida en el JSON
        foreach($data->comidas as $comida) {
            date_default_timezone_set("UTC");
            $fechaUnix = $comida->fecha; // Fecha en timestamp Unix
            $fecha = date('Y-m-d H:i:s', $fechaUnix); // Convertir a formato de fecha y hora

            // Insertar la nueva comida en la base de datos
            $sql = "INSERT INTO Comida (fecha) VALUES ('$fecha')";
            if ($conn->query($sql) === TRUE) {
                $comida_id = $conn->insert_id;

                // ----- PLATO -----
                // Procesar cada plato de la comida
                foreach($comida->platos as $plato) {
                    $sql = "INSERT INTO Plato (id_comida) VALUES ($comida_id)";
                    if ($conn->query($sql) === TRUE) {
                        $plato_id = $conn->insert_id;

                        // ----- ALIMENTO -----
                        // Procesar cada alimento del plato
                        foreach($plato->alimentos as $alimento) {
                            $grupo = $alimento->grupo;
                            $peso = $alimento->peso;

                            // ----- ALIMENTO DE NUESTROS GRUPOS -------------------------
                            // Si el alimento pertenece a un grupo definido
                            if ($grupo != 50) {
                                // Obtener id_tipo del grupo de alimentos
                                $sql = "SELECT id_tipo FROM GrupoAlimento WHERE grupo = '$grupo'";
                                $result = $conn->query($sql);
                                if ($result->num_rows > 0) {
                                    $row = $result->fetch_assoc();
                                    $id_tipo = $row['id_tipo'];

                                    // Insertar el alimento en la base de datos
                                    $sql = "INSERT INTO Alimento (peso, id_plato, id_tipo, dni) VALUES ('$peso', $plato_id, '$id_tipo', '$dni')";
                                    if ($conn->query($sql) == TRUE) {
                                        echo "Nuevo alimento creado\n";
                                    } else {
                                        echo "Error: " . $sql . "<br>" . $conn->error;
                                    }
                                } else {
                                    echo "No se encontró un tipo de alimento correspondiente al grupo: " . $grupo;
                                }
                            } 
                            // -----------------------------------------------------------
                            // ----- ALIMENTO TIPO BARCODE -------------------------------
                            // Si el alimento es un producto con código de barras
                            else if (isset($alimento->ean)) {
                                $ean = $alimento->ean;

                                // ----- BARCODE LEIDO ANTERIORMENTE ---------
                                // Comprobar si el código de barras ya está en la base de datos
                                $sql = "SELECT id_tipo FROM Barcode WHERE EAN = '$ean'";
                                $result = $conn->query($sql);
                                if ($result->num_rows > 0) {
                                    $row = $result->fetch_assoc();
                                    $id_tipo = $row['id_tipo'];

                                    // ----- INSERTAR ALIMENTO -----
                                    // Insertar el alimento con el código de barras existente
                                    $sql = "INSERT INTO Alimento (peso, id_plato, id_tipo, dni) VALUES ('$peso', $plato_id, '$id_tipo', '$dni')";
                                    if ($conn->query($sql) == TRUE) {
                                        echo "Nuevo alimento con código de barras existente creado\n";
                                    } else {
                                        echo "Error: " . $sql . "<br>" . $conn->error;
                                    }
                                } 
                                // -------------------------------------------
                                // ----- NUNCA SE HA LEIDO ESTE BARCODE ------
                                else {
                                    // ----- BUSCAR INFO NUTRICIONAL -----
                                    // Obtener la información nutricional del producto desde OpenFoodFacts
                                    $productData = getNutritionalDataFromOpenFoodFacts($ean);
                                    if ($productData) {
                                        $nombre = $productData['nombre'];
                                        $carb_g = $productData['carb_g'] / 100.0;
                                        $kcal_g = $productData['kcal_g'] / 100.0;
                                        $lip_g = $productData['lip_g'] / 100.0;
                                        $prot_g = $productData['prot_g'] / 100.0;

                                        // ----- NUEVO TIPO ALIMENTO -----
                                        // Insertar el nuevo tipo de alimento en la base de datos
                                        $sql = "INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES ('$kcal_g', '$prot_g', '$lip_g', '$carb_g')";
                                        if ($conn->query($sql) === TRUE) {
                                            $id_tipo = $conn->insert_id;

                                            // ---- NUEVO PRODUCTO LEIDO -----
                                            // Insertar el código de barras en la base de datos
                                            $sql = "INSERT INTO Barcode (EAN, nombre_comercial, id_tipo) VALUES ('$ean', '$nombre', '$id_tipo')";
                                            if ($conn->query($sql) === TRUE) {
                                                // ----- INSERTAR ALIMENTO -----
                                                // Insertar el alimento con el nuevo tipo de alimento y código de barras
                                                $sql = "INSERT INTO Alimento (peso, id_plato, id_tipo, dni) VALUES ('$peso', $plato_id, '$id_tipo', '$dni')";
                                                if ($conn->query($sql) == TRUE) {
                                                    echo "Nuevo alimento con código de barras creado\n";
                                                } else {
                                                    echo "Error: " . $sql . "<br>" . $conn->error;
                                                }
                                            }
                                        }
                                    } else {
                                        echo "No se pudo obtener información del código de barras: " . $ean;
                                    }
                                }
                                // -------------------------------------------
                            }
                            // -----------------------------------------------------------
                        }

                        echo "Nuevo plato creado\n";
                    } else {
                        echo "Error: " . $sql . "<br>" . $conn->error;
                    }
                }

                echo "Nueva comida creada\n";
            } else {
                echo "Error: " . $sql . "<br>" . $conn->error;
            }
        }
    } else {
        echo "No se encontró un usuario correspondiente a la MAC: " . $mac;
    }

    $conn->close();
} else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}

// Función para obtener la información nutricional del producto desde OpenFoodFacts
function getNutritionalDataFromOpenFoodFacts($ean) {
    // Construir la URL de la solicitud
    $url = "https://world.openfoodfacts.org/api/v2/product/" . $ean . "?fields=product_name,product_name_es,carbohydrates_100g,energy-kcal_100g,fat_100g,proteins_100g";
    // Obtener la respuesta de la API
    $response = file_get_contents($url);
    // Decodificar la respuesta JSON
    $data = json_decode($response, true);

    // Verificar si se obtuvieron datos del producto
    if (isset($data['product'])) {
        $product = $data['product'];
        return [
            'nombre' => isset($product['product_name_es']) ? $product['product_name_es'] : $product['product_name'],
            'carb_g' => isset($product['carbohydrates_100g']) ? $product['carbohydrates_100g'] : 0,
            'kcal_g' => isset($product['energy-kcal_100g']) ? $product['energy-kcal_100g'] : 0,
            'lip_g' => isset($product['fat_100g']) ? $product['fat_100g'] : 0,
            'prot_g' => isset($product['proteins_100g']) ? $product['proteins_100g'] : 0
        ];
    } else {
        return false;
    }
}
?>