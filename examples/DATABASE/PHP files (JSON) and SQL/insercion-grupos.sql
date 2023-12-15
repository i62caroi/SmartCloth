
/*
    Para insertar información de un grupo de alimentos se siguen estos pasos:
        1 - Insertar información base (nombre y ejemplos) del grupo en 'GrupoAlimentoBase'
        2 - Insertar ID de grupo (id_grupo) e ID de la info que le corresponde (id_base) en 'GrupoAlimento' 
        3 - Crear tipo de alimento insertando el ID del grupo relacionado (id_grupo) en 'TipoAlimento'
        4 - Insertar valores nutricionales para los tipos de alimentos creados (id_tipo) en 'ValoresNutricionales'
            4.1 - Obtener el 'id_tipo' del tipo de alimento creado para cada 'id_grupo'
            4.2 - Insertar los valores nutricionales de cada grupo según el 'id_tipo' asignado

*/



-- ---------- 1 - INSERTAR INFORMACIÓN DE GRUPOS -------------------------------------------
INSERT INTO GrupoAlimentoBase (id_base, nombre_grupo, ejemplos_grupo) VALUES
    (1, 'Lácteos enteros', 'Leche entera de vaca (pasteurizada o UHT), de oveja, de cabra, yogurt natural entero, cuajada, etc.'),
    (2, 'Lácteos semidesnatados', 'Leche semidesnatada pasteurizada y UHT'),
    (3, 'Lácteos desnatados', 'Leche desnatada pasteurizada y UHT, natural, con frutas, yogurt desnatado, yogurt desnatado de sabores, etc.'),
    (4, 'Lácteos azucarados','Batidos lácteos de cacao y otros sabores, leche entera fermentada con frutas, yogures enteros de sabores y azucarados, yogures líquidos de sabores y azucarados'),
    (5,'Postres lácteos','Arroz con leche, flan de huevo, flan de vainilla y natillas...'),
    (6,'Frutas frescas, desecadas y zumos','Albaricoque, arándanos, cerezas, ciruelas, dátil seco, fresa, granada, higos, kiwi, mandarina,manzana, melocotón, melón, naranja, pera, piña, plátano, sandía, uvas...'),
    (7,'Verduras y hortalizas','Acelgas, apio, alcachofa, berenjena, brócoli, calabacín, calabaza, champiñones, col, espárragos, espinacas, guisantes, lechuga, judías, pimientos, tomate, zanahoria, etc.'),
    (8,'Cereales y tubérculos','Arroz, avena, boniato, castaña, cereales de desayuno ricos en fibra, copos de maíz, harina, maíz, pan, pasta, patata, sémola de trigo, etc.'),
    (9,'Legumbres','Alubias, garbanzos, lentejas, etc.'),
    (10,'Repostería, pastelería y otros','Bizcocho, bollo, croissant, ensaimada, galletas (de cualquier tipo), magdalena, muesli, pan de pasas, tartas, pasteles, bollería industrial...'),
    (11,'Alimentos ricos en grasas saludables','Aceites de cacahuete, de oliva y de hígado de bacalao, aceitunas, aguacate, almendras, avellanas, cacahuetes, pistachos, mayonesa de aceite de oliva, etc.'),
    (12,'Alimentos ricos en grasas vegetales','Aceite de girasol, aceite de maíz, aceite de soja, mayonesa light, nueces, piñones, etc.'),
    (13,'Alimentos ricos en grasas saturadas','Coco fresco o seco, aceite de coco, mantequilla, nata líquida para cocinar o montar, etc.'),
    (14,'Alimentos muy grasos (mezclas)','Margarina light, margarina vegetal enriquecida, manteca y tocino de cerdo'),
    (15,'Azúcares y dulces','Azúcar blanco y moreno, miel, leche condensada, cacao soluble azucarado, confitura de fruta baja en calorías'),
    (16,'Alimentos proteicos con muy poca grasa','Pavo, pollo, ternera (entrecot y solomillo), jamón cocido, atún natural, pescado no graso, marisco, queso granulado, clara de huevo, etc.'),
    (17,'Alimentos proteicos con poca grasa','Lomo de cerdo, pollo sin piel, bistec de vaca/buey, jamón curado (sin grasa), pescados grasos (atún, sardina, trucha, boquerón...), vísceras, pato sin piel, codorniz, etc.'),
    (18,'Alimentos proteicos semigrasos','Chuletas de cerdo, cordero, anchoas, atún o sardinas en aceite, caballa, salmón, jamón curado con grasa, huevo, queso fresco, requesón, queso en porciones, etc.'),
    (19,'Alimentos proteicos grasos','Chuletas/costillas de cordero, chorizo, salchichas, fuet, quesos (azul, babybel, camembert, cheddar, de cabra, emmental, gouda, gruyer, manchego...)'),
    (20,'Alimentos proteicos muy grasos','Carne picada sazonada, panceta de cerdo, morcilla, mortadela, paté, salami, salchichón, etc.');
-- -----------------------------------------------------------------------------------------




-- ---------- 2 - INSERTAR IDs GRUPOS DE ALIMENTOS Y ASIGNAR INFORMACIÓN -------------------
--  Se asignan los primeros 20 grupos con un id_base igual que su id_grupo, pero los últimos 6 grupos (27, 28, 29, 36, 37 y 38),
--  que son cocinados, se asignan con los id_base de sus contrapartes crudas (7, 8, 9, 16, 17 y 18, respectivamente)
INSERT INTO GrupoAlimento (id_grupo, id_base) VALUES
    (1, 1), (2, 2), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8), (9, 9), (10, 10),
    (11, 11), (12, 12), (13, 13), (14, 14), (15, 15), (16, 16), (17, 17), (18, 18), (19, 19), (20, 20),
    (27, 7), (28, 8), (29, 9), (36, 16), (37, 17), (38, 18);
-- ----------------------------------------------------------------------------------------





-- ------- 3 - CREAR TIPOS DE ALIMENTOS PARA CADA GRUPO ------------------------------------
INSERT INTO TipoAlimento (id_grupo) VALUES
    (1), (2), (3), (4), (5), (6), (7), (8), (9), (10),
    (11), (12), (13), (14), (15), (16), (17), (18), (19), (20),
    (27), (28), (29), (36), (37), (38);
-- -----------------------------------------------------------------------------------------






-- ---------- 4 - INSERTAR VALORES NUTRICIONALES DE GRUPOS DE ALIMENRTOS -------------------
/*
    Para insertar valores nutricionales de un grupo de alimentos, necesitamos el 'id_tipo'
    que se ha asignado a ese grupo. Como'id_tipo' es auto-incremental, si la tabla 'TipoAlimento' 
    estaba vacía al comenzar la inserción de los grupos (paso anterior de CREAR TIPOS DE ALIMENTOS DE TIPO 'GRUPO'), 
    estos se habrán asignado de esta manera:
        grupo 1 -> tipo 1
        grupo 2 -> tipo 2 
            ...
        grupo 20 -> tipo 20
        grupo 27 -> tipo 21
        grupo 28 -> tipo 22
        grupo 29 -> tipo 23
        grupo 36 -> tipo 24
        grupo 37 -> tipo 25
        grupo 38 -> tipo 26

    Sabiendo esto, podríamos escribir manualmente el valor de 'id_tipo' que creemos que tendrá 
    asignado cada grupo al insertar sus valores nutricionales en la tabla 'ValoresNutricionales', 
    pero lo ideal es hacer una consulta a la tabla 'TipoAlimento' para obtener el 'id_tipo' asignado 
    a cada 'id_grupo' antes de insertar los macronutrientes por gramo.
*/

-- 4.1 - Obtener el 'id_tipo' del nuevo tipo de alimento creado para cada 'id_grupo'
SET @id_tipo_g1 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 1);
SET @id_tipo_g2 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 2);
SET @id_tipo_g3 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 3);
SET @id_tipo_g4 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 4);
SET @id_tipo_g5 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 5);
SET @id_tipo_g6 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 6);
SET @id_tipo_g7 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 7);
SET @id_tipo_g8 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 8);
SET @id_tipo_g9 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 9);
SET @id_tipo_g10 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 10);
SET @id_tipo_g11 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 11);
SET @id_tipo_g12 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 12);
SET @id_tipo_g13 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 13);
SET @id_tipo_g14 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 14);
SET @id_tipo_g15 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 15);
SET @id_tipo_g16 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 16);
SET @id_tipo_g17 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 17);
SET @id_tipo_g18 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 18);
SET @id_tipo_g19 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 19);
SET @id_tipo_g20 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 20);
SET @id_tipo_g27 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 27);
SET @id_tipo_g28 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 28);
SET @id_tipo_g29 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 29);
SET @id_tipo_g36 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 36);
SET @id_tipo_g37 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 37);
SET @id_tipo_g38 = (SELECT id_tipo FROM TipoAlimento WHERE id_grupo = 38);


-- 4.2 - Insertar los valores nutricionales de cada grupo según su 'id_tipo'
INSERT INTO ValoresNutricionales (id_tipo, kcal_g, prot_g, lip_g, carb_g) VALUES
    (@id_tipo_g1, 0.684059925, 0.038014981, 0.037910112, 0.047191011),
    (@id_tipo_g2, 0.465, 0.033, 0.016, 0.046),
    (@id_tipo_g3, 0.391466667, 0.041822222, 0.0016, 0.052133333),
    (@id_tipo_g4, 0.841395349, 0.032483721, 0.02067907, 0.131274419),
    (@id_tipo_g5, 1.130612245, 0.036653061, 0.026938776, 0.184489796),
    (@id_tipo_g6, 0.455913978, 0.00719086, 0.001747312, 0.102822581),
    (@id_tipo_g7, 0.224860335, 0.01673743, 0.003128492, 0.035195531),
    (@id_tipo_g8, 2.095873684, 0.063073684, 0.015621053, 0.4256),
    (@id_tipo_g9, 3.01, 0.219, 0.03, 0.465),
    (@id_tipo_g10, 4.193949153, 0.071186441, 0.211423729, 0.501864407),
    (@id_tipo_g11, 6.517826087, 0.053478261, 0.679565217, 0.045652174),
    (@id_tipo_g12, 0.87375, 0.0110625, 0.0879375, 0.00975),
    (@id_tipo_g13, 3.625396825, 0.029365079, 0.367460317, 0.049206349),
    (@id_tipo_g14, 6.08, 0.0272, 0.6624, 0.0032),
    (@id_tipo_g15, 3.491368421, 0.021052632, 0.018526316, 0.810947368),
    (@id_tipo_g16, 0.368903088, 0.073801917, 0.006709265, 0.002555911),
    (@id_tipo_g17, 0.73488, 0.11456, 0.03024, 0.00096),
    (@id_tipo_g18, 1.706813187, 0.153186813, 0.113186813, 0.008351648),
    (@id_tipo_g19, 3.601914894, 0.222765957, 0.299361702, 0.004468085),
    (@id_tipo_g20, 3.419606061, 0.145090909, 0.308212121, 0.016333333),
    (@id_tipo_g27, 0.237068966, 0.017646177, 0.003298351, 0.037106447),
    (@id_tipo_g28, 1.170534979, 0.035226337, 0.00872428, 0.237695473),
    (@id_tipo_g29, 1.119421488, 0.081446281, 0.011157025, 0.172933884),
    (@id_tipo_g36, 0.836939597, 0.167436242, 0.015221477, 0.005798658),
    (@id_tipo_g37, 1.321273973, 0.205972603, 0.054369863, 0.001726027),
    (@id_tipo_g38, 1.897038168, 0.170259542, 0.125801527, 0.009282443);
-- ----------------------------------------------------------------------------------------



