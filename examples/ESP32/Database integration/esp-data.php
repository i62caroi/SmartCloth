<!DOCTYPE html>
<html>
<head>
 <style>
  table {
   border-collapse: collapse;
   width: 100%;
   margin-bottom: 1em;
   color: #333333;
   font-family: Arial, sans-serif;
   font-size: 14px;
   text-align: left;
   background-color: #F5F5F5;
  }

  table td, table th {
   padding: 8px;
   border: 1px solid #DDDDDD;
  }

  table th {
   background-color: #B3B3B3;
   color: #FFFFFF;
   font-weight: bold;
  }

  table tr:nth-child(even) {
   background-color: #EFEFEF;
  }

  table tr:nth-child(odd) {
   background-color: #FFFFFF;
  }

    h1 {
    text-align: center;
  }
 </style>
</head>  
<body>
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


// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT ID, fechaHora, carb, carb_R, lip, lip_R, prot, prot_R, kcal, peso FROM comidas ORDER BY ID DESC";

echo '<h1>ESP32 Database</h1>
      <table cellspacing="5" cellpadding="5">
      <tr> 
        <td>ID</td> 
        <td>TimeStamp</td> 
        <td>Carbohidratos</td> 
        <td>Raciones Carbohidratos</td> 
        <td>Lipidos</td>
        <td>Raciones Lipidos</td> 
        <td>Proteinas</td> 
        <td>Raciones Proteinas</td> 
        <td>Kilocalorias</td> 
        <td>Peso (gr)</td> 
      </tr>';

 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["ID"];
        $row_time = $row["fechaHora"];
        $row_carb = $row["carb"];
        $row_carb_R = $row["carb_R"];
        $row_lip = $row["lip"]; 
        $row_lip_R = $row["lip_R"]; 
        $row_prot = $row["prot"];
        $row_prot_R = $row["prot_R"];
        $row_kcal = $row["kcal"];
        $row_peso = $row["peso"];
        // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
        $row_time = date("Y-m-d H:i:s", strtotime("$row_time - 1 hours"));
      
        // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 4 hours"));
      
        echo '<tr> 
                <td>' . $row_id . '</td> 
                <td>' . $row_time . '</td> 
                <td>' . $row_carb . '</td> 
                <td>' . $row_carb_R . '</td> 
                <td>' . $row_lip . '</td>
                <td>' . $row_lip_R . '</td> 
                <td>' . $row_prot . '</td> 
                <td>' . $row_prot_R . '</td> 
                <td>' . $row_kcal . '</td> 
                <td>' . $row_peso . '</td> 
              </tr>';
    }
    $result->free();
}

$conn->close();
?> 
</table>
</body>
</html>