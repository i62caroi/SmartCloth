
-- Obtener código de grupo, nombre, ejemplos y valores nutricionales del grupo 38 (18 cocinado)
SELECT GrupoAlimento.grupo, GrupoAlimento.nombre, GrupoAlimento.ejemplos, TipoAlimento.id_tipo, TipoAlimento.kcal_g, TipoAlimento.prot_g, TipoAlimento.lip_g, TipoAlimento.carb_g 
FROM GrupoAlimento 
JOIN TipoAlimento ON GrupoAlimento.id_tipo = TipoAlimento.id_tipo 
WHERE GrupoAlimento.grupo = 38;

/*
Salida:

grupo   |   nombre    |   ejemplos  |   id_tipo |   kcal_g  |   prot_g  |   lip_g   |   carb_g
38  |   Alimentos proteicos semigrasos  |   Chuletas de cerdo, cordero, anchoas, atún o sardin...   |   26  |   1.89704 |   0.17026 |   0.125802    |   0.00928244


*/





-- Obtener comidas realizadas para cada usuario
SELECT um.dni, m.MAC, c.id_comida, c.fecha, c.hora, p.id_plato, a.id_alimento, g.grupo, a.peso 
FROM UsuarioMantel um
JOIN Mantel m ON um.id_mantel = m.id_mantel
JOIN Alimento a ON um.dni = a.dni
JOIN TipoAlimento t ON a.id_tipo = t.id_tipo
JOIN GrupoAlimento g ON t.id_tipo = g.id_tipo
JOIN Plato p ON a.id_plato = p.id_plato
JOIN Comida c ON p.id_comida = c.id_comida 
ORDER BY um.dni, m.MAC, c.id_comida DESC, p.id_plato DESC, a.id_alimento DESC

/*
SALIDA:


dni |   MAC |   id_comida   |   fecha   |   hora    |   id_plato    |   id_alimento |   grupo   |   peso
30987645F   |   08:D1:F9:CB:A1:EC   |   3   |   20.12.2023  |   10:39:36    |   4   |   8   |   10  |   16.23
30987645F   |   08:D1:F9:CB:A1:EC   |   3   |   20.12.2023  |   10:39:36    |   4   |   7   |   27  |   46.65  
30987645F   |   08:D1:F9:CB:A1:EC   |   3   |   20.12.2023  |   10:39:36    |   4   |   6   |   3   |   32.07
30987645F   |   08:D1:F9:CB:A1:EC   |   3   |   20.12.2023  |   10:39:36    |   3   |   5   |   9   |   53.5
30987645F   |   08:D1:F9:CB:A1:EC   |   3   |   20.12.2023  |   10:39:36    |   3   |   4   |   15  |   53.5
30987645F   |   08:D1:F9:CB:A1:EC   |   2   |   05.12.2023  |   20:25:25    |   2   |   3   |   15  |   24.4
30987645F   |   08:D1:F9:CB:A1:EC   |   1   |   05.12.2023  |   17:55:36    |   1   |   2   |   9   |   53.5
30987645F   |   08:D1:F9:CB:A1:EC   |   1   |   05.12.2023  |   17:55:36    |   1   |   1   |   15  |   53.5
38042651J   |   08:D1:F9:CB:4C:24   |   6   |   20.12.2023  |   18:39:36    |   8   |   14  |   3   |   32.07
38042651J   |   08:D1:F9:CB:4C:24   |   6   |   20.12.2023  |   18:39:36    |   7   |   13  |   9   |   53.5
38042651J   |   08:D1:F9:CB:4C:24   |   6   |   20.12.2023  |   18:39:36    |   7   |   12  |   15  |   53.5
38042651J   |   08:D1:F9:CB:4C:24   |   5   |   20.12.2023  |   10:25:25    |   6   |   11  |   15  |   24.4
38042651J   |   08:D1:F9:CB:4C:24   |   4   |   05.12.2023  |   17:55:36    |   5   |   10  |   9   |   53.5


*/