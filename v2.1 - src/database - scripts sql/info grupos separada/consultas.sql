
-- Obtener código de grupo, nombre, ejemplos y valores nutricionales del grupo 38 (18 cocinado)
SELECT GrupoAlimento.grupo, GrupoAlimentoBase.nombre_grupo, GrupoAlimentoBase.ejemplos_grupo, TipoAlimento.kcal_g, TipoAlimento.prot_g, TipoAlimento.lip_g, TipoAlimento.carb_g
FROM GrupoAlimento
INNER JOIN TipoAlimento ON GrupoAlimento.id_tipo = TipoAlimento.id_tipo
INNER JOIN GrupoAlimentoBase ON GrupoAlimento.id_base = GrupoAlimentoBase.id_base
WHERE GrupoAlimento.grupo = 38;

/*
Salida:


grupo   |   nombre_grupo    |   ejemplos_grupo  |   kcal_g  |   prot_g  |   lip_g   |   carb_g
38  |   Alimentos proteicos semigrasos  |   Chuletas de cerdo, cordero, anchoas, atún o sardin...   |   1.89704 |   0.17026 |   0.125802    |   0.00928244

*/