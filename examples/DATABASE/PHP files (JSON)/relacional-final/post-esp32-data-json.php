<?php

$servername = "localhost";

$dbname = "smartclo_SM-database";
$username = "smartclo_irene";
$password = "SmartCloth.database/pass23";

// Keep this API Key value to be compatible with the ESP32 code provided in the project page. 
// If you change this value, the ESP32 sketch needs to match
$api_key_value = "apiKeySmartCloth";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $json = file_get_contents('php://input');
    $data = json_decode($json);

    $api_key = $data->api_key; // api_key en el body (no seguro)
    //$api_key = $_SERVER['HTTP_API_KEY']; // api_key en el header (más seguro)

    if($api_key == $api_key_value) {
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 

        $mac = $data->MAC;

        $sql = "SELECT dni FROM UsuarioMantel UM INNER JOIN Mantel M ON UM.id_mantel = M.id_mantel WHERE M.MAC = '$mac'";
        $result = $conn->query($sql);
        if ($result->num_rows > 0) {
            $row = $result->fetch_assoc();
            $dni = $row['dni'];

            foreach($data->comidas as $comida) {
                $fecha = $comida->fecha;
                $hora = $comida->hora;

                $sql = "INSERT INTO Comida (fecha, hora) VALUES ('$fecha', '$hora')";
                if ($conn->query($sql) === TRUE) {
                    $comida_id = $conn->insert_id;

                    foreach($comida->platos as $plato) {
                        $sql = "INSERT INTO Plato (id_comida) VALUES ($comida_id)";
                        if ($conn->query($sql) === TRUE) {
                            $plato_id = $conn->insert_id;

                            foreach($plato->alimentos as $alimento) {
                                $grupo = $alimento->grupo;
                                $peso = $alimento->peso;

                                $sql = "SELECT id_tipo FROM GrupoAlimento WHERE grupo = '$grupo'";
                                $result = $conn->query($sql);
                                if ($result->num_rows > 0) {
                                    $row = $result->fetch_assoc();
                                    $id_tipo = $row['id_tipo'];

                                    $sql = "INSERT INTO Alimento (peso, id_plato, id_tipo, dni) VALUES ('$peso', $plato_id, '$id_tipo', '$dni')";
                                    if ($conn->query($sql) == TRUE) {
                                        echo "Nuevo alimento creado\n";
                                    }
                                    else{
                                        echo "Error: " . $sql . "<br>" . $conn->error;
                                    }
                                }
                                else {
                                    echo "No se encontró un tipo de alimento correspondiente al grupo: " . $grupo;
                                }
                            }

                            echo "Nuevo plato creado\n";
                        } 
                        else {
                            echo "Error: " . $sql . "<br>" . $conn->error;
                        }
                    }

                    echo "Nueva comida creada\n";
                } 
                else {
                    echo "Error: " . $sql . "<br>" . $conn->error;
                }
            }
        }
        else {
            echo "No se encontró un usuario correspondiente a la MAC: " . $mac;
        }

        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}