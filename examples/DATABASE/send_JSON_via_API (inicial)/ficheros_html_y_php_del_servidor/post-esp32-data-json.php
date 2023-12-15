<?php

/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

$servername = "localhost";

// REPLACE with your Database name
$dbname = "smartclo_SM-database";
// REPLACE with Database user
$username = "smartclo_irene";
// REPLACE with Database user password
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
        
        foreach($data->comidas as $comida) {
            $fecha = $comida->fecha;
            $hora = $comida->hora;

            $sql = "INSERT INTO comida (fecha, hora) VALUES ('$fecha', '$hora')";
            if ($conn->query($sql) === TRUE) {
                $comida_id = $conn->insert_id;

                foreach($comida->platos as $plato) {
                    $sql = "INSERT INTO plato (comida_id) VALUES ($comida_id)";
                    if ($conn->query($sql) === TRUE) {
                        $plato_id = $conn->insert_id;

                        foreach($plato->alimentos as $alimento) {
                            $grupo = $alimento->grupo;
                            $peso = $alimento->peso;

                            $sql = "INSERT INTO alimento (plato_id, grupo, peso) VALUES ($plato_id, '$grupo', '$peso')";
                            if ($conn->query($sql) == TRUE) {
                                echo "Nuevo alimento creado";
                            }
                            else{
                                echo "Error: " . $sql . "<br>" . $conn->error;
                            }
                        }

                        echo "Nuevo plato creado";
                    } 
                    else {
                        echo "Error: " . $sql . "<br>" . $conn->error;
                    }
                }

                echo "Nueva comida creada";
            } 
            else {
                echo "Error: " . $sql . "<br>" . $conn->error;
            }
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