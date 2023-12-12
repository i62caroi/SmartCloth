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

$api_key= $carb = $carb_R = $lip = $lip_R = $prot = $prot_R = $kcal = $peso = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    //$api_key = test_input($_POST["api_key"]); // api_key en el body (no seguro)
    $api_key = $_SERVER['HTTP_API_KEY']; // api_key en el header (más seguro)
    if($api_key == $api_key_value) {

        // Formatear los valores con dos decimales para evitar que se trunquen si terminan en .0 --> no funciona
        /*$carb = sprintf("%.2f", test_input($_POST["carb"]));        // $carb = test_input($_POST["carb"]);      $carb = sprintf("%.2f", $carb);
        $carb_R = sprintf("%.2f", test_input($_POST["carb_R"]));    // $carb_R = test_input($_POST["carb_R"]);  $carb_R = sprintf("%.2f", $carb_R);
        $lip = sprintf("%.2f", test_input($_POST["lip"]));          // $lip = test_input($_POST["lip"]);        $lip = sprintf("%.2f", $lip);
        $lip_R = sprintf("%.2f", test_input($_POST["lip_R"]));      // $lip_R = test_input($_POST["lip_R"]);    $lip_R = sprintf("%.2f", $lip_R);
        $prot = sprintf("%.2f", test_input($_POST["prot"]));        // $prot = test_input($_POST["prot"]);      $prot = sprintf("%.2f", $prot);
        $prot_R = sprintf("%.2f", test_input($_POST["prot_R"]));    // $prot_R = test_input($_POST["prot_R"]);  $prot_R = sprintf("%.2f", $prot_R);
        $kcal = sprintf("%.2f", test_input($_POST["kcal"]));        // $kcal = test_input($_POST["kcal"]);      $kcal = sprintf("%.2f", $kcal);
        $peso = sprintf("%.2f", test_input($_POST["peso"]));        // $peso = test_input($_POST["peso"]);      $peso = sprintf("%.2f", $peso);
        */

        // Datos sin formatear
        $carb = test_input($_POST["carb"]);
        $carb_R = test_input($_POST["carb_R"]);
        $lip = test_input($_POST["lip"]);
        $lip_R = test_input($_POST["lip_R"]);
        $prot = test_input($_POST["prot"]);
        $prot_R = test_input($_POST["prot_R"]);
        $kcal = test_input($_POST["kcal"]);
        $peso = test_input($_POST["peso"]);

        // Agrega un mensaje al archivo de log
        $log_message = "Received POST request. Values to insert: carb: $carb, carb_R: $carb_R, lip: $lip, lip_R: $lip_R, prot: $prot, prot_R: $prot_R, kcal: $kcal, peso: $peso";
        file_put_contents('log.txt', $log_message . "\n", FILE_APPEND);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        // Si los valores terminan en .00, se truncan y se inserta solo la parte entera. 
        $sql = "INSERT INTO comidas (carb, carb_R, lip, lip_R, prot, prot_R, kcal, peso)
        VALUES ('" . $carb . "', '" . $carb_R . "', '" . $lip . "', '" . $lip_R . "', '" . $prot . "', '" . $prot_R . "', '" . $kcal . "', '" . $peso . "')";

        // No sé qué hacen las comillas simples, porque la siguiente versión sin comillas simples hace lo mismo que la anterior:
        /*$sql = "INSERT INTO comidas (carb, carb_R, lip, lip_R, prot, prot_R, kcal, peso)
        VALUES (" . $carb . ", " . $carb_R . ", " . $lip . ", " . $lip_R . ", " . $prot . ", " . $prot_R . ", " . $kcal . ", " . $peso . ")";*/



        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
    echo "('" . $carb . "', '" . $carb_R . "', '" . $lip . "', '" . $lip_R . "', '" . $prot . "', '" . $prot_R . "', '" . $kcal . "', '" . $peso . "')";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}