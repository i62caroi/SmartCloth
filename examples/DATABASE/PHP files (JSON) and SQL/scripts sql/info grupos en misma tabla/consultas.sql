
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