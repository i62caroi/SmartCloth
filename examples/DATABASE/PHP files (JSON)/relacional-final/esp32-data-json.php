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
$servername = "localhost";
$dbname = "smartclo_SM-database";
$username = "smartclo_irene";
$password = "SmartCloth.database/pass23";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT um.dni, m.MAC, c.id_comida, c.fecha, c.hora, p.id_plato, a.id_alimento, g.grupo, a.peso 
        FROM UsuarioMantel um
        JOIN Mantel m ON um.id_mantel = m.id_mantel
        JOIN Alimento a ON um.dni = a.dni
        JOIN TipoAlimento t ON a.id_tipo = t.id_tipo
        JOIN GrupoAlimento g ON t.id_tipo = g.id_tipo
        JOIN Plato p ON a.id_plato = p.id_plato
        JOIN Comida c ON p.id_comida = c.id_comida 
        ORDER BY um.dni, m.MAC, c.id_comida DESC, p.id_plato DESC, a.id_alimento DESC";
        

echo '<h1>ESP32 Database</h1>
      <table cellspacing="5" cellpadding="5">
      <tr> 
        <td>DNI Usuario</td> 
        <td>MAC Mantel</td>
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
        $dni = $row["dni"];
        $mac = $row["MAC"];
        $id_comida = $row["id_comida"];
        $fecha = $row["fecha"];
        $hora = $row["hora"];
        $id_plato = $row["id_plato"];
        $id_alimento = $row["id_alimento"];
        $grupo = $row["grupo"];
        $peso = $row["peso"];

        echo '<tr> 
                <td>' . $dni . '</td> 
                <td>' . $mac . '</td> 
                <td>' . $id_comida . '</td> 
                <td>' . $fecha . '</td> 
                <td>' . $hora . '</td> 
                <td>' . $id_plato . '</td> 
                <td>' . $id_alimento . '</td> 
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