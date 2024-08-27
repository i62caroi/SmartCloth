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

$sql = "SELECT m.user_id, m.mac, c.id as comida_id, c.fecha, p.id as plato_id, a.id as alimento_id, g.cod_grupo, a.peso 
        FROM mantels m 
        JOIN alimentos a ON m.user_id = a.user_id
        JOIN tipo_alimentos t ON a.tipoalimento_id = t.id
        JOIN grupos g ON t.id = g.tipoalimento_id
        JOIN platos p ON a.plato_id = p.id
        JOIN comidas c ON p.comida_id = c.id 
        ORDER BY m.user_id, m.mac, c.id DESC, p.id DESC, a.id DESC";
        

echo '<h1>ESP32 Database</h1>
      <table cellspacing="5" cellpadding="5">
      <tr> 
        <td>DNI Usuario</td> 
        <td>MAC Mantel</td>
        <td>ID Comida</td> 
        <td>Fecha</td> 
        <td>ID Plato</td> 
        <td>ID Alimento</td> 
        <td>Grupo Alimento</td> 
        <td>Peso Alimento</td> 
      </tr>';

if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $user_id = $row["user_id"];
        $mac = $row["mac"];
        $id_comida = $row["comida_id"];
        $fecha = $row["fecha"];
        $id_plato = $row["plato_id"];
        $id_alimento = $row["alimento_id"];
        $grupo = $row["cod_grupo"];
        $peso = $row["peso"];

        echo '<tr> 
                <td>' . $user_id . '</td> 
                <td>' . $mac . '</td> 
                <td>' . $id_comida . '</td> 
                <td>' . $fecha . '</td> 
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