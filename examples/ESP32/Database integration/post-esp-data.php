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
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $carb = test_input($_POST["carb"]);
        $carb_R = test_input($_POST["carb_R"]);
        $lip = test_input($_POST["lip"]);
        $lip_R = test_input($_POST["lip_R"]);
        $prot = test_input($_POST["prot"]);
        $prot_R = test_input($_POST["prot_R"]);
        $kcal = test_input($_POST["kcal"]);
        $peso = test_input($_POST["peso"]);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO comidas (carb, carb_R, lip, lip_R, prot, prot_R, kcal, peso)
        VALUES ('" . $carb . "', '" . $carb_R . "', '" . $lip . "', '" . $lip_R . "', '" . $prot . "', '" . $prot_R . "', '" . $kcal . "', '" . $peso . "')";
        

        //$sql = "INSERT INTO comidas (carb, carb_R, lip, lip_R, prot, prot_R, kcal, peso) VALUES (35.7, 3.5, 23.7, 3, 12.7, 1, 134.6, 234.67)";

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
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}