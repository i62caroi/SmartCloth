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

// Se recomienda usar JOIN en lugar de WHERE porque, si los criterios de WHERE no son suficientes
// para relacionar correctamente las tablas, puede dar resultados incorrectos y un rendimiento deficiente.
$sql = "SELECT c.id as comida_id, c.fecha, c.hora, p.id as plato_id, a.id as alimento_id, a.grupo, a.peso 
        FROM comida c 
        JOIN plato p ON c.id = p.comida_id 
        JOIN alimento a ON p.id = a.plato_id 
        ORDER BY c.id DESC, p.id DESC, a.id DESC";

echo '<h1>ESP32 Database</h1>
      <table cellspacing="5" cellpadding="5">
      <tr> 
        <td>ID Comida</td> 
        <td>Fecha</td> 
        <td>Hora</td> 
        <td>ID Plato</td> 
        <td>ID Alimento</td> 
        <td>Grupo Alimento</td> 
        <td>Peso Alimento</td> 
      </tr>';

 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $comida_id = $row["comida_id"];
        $fecha = $row["fecha"];
        $hora = $row["hora"];
        $plato_id = $row["plato_id"];
        $alimento_id = $row["alimento_id"];
        $grupo = $row["grupo"];
        $peso = $row["peso"];

        echo '<tr> 
                <td>' . $comida_id . '</td> 
                <td>' . $fecha . '</td> 
                <td>' . $hora . '</td> 
                <td>' . $plato_id . '</td> 
                <td>' . $alimento_id . '</td> 
                <td>' . $grupo . '</td> 
                <td>' . $peso . '</td> 
              </tr>';
    }
    $result->free();
}

$conn->close();
?> 
</table>
</body>
</html>