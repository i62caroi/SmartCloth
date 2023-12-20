
/*
    Para insertar información de un grupo de alimentos se siguen estos pasos:
        1 - Insertar información base (nombre y ejemplos) del grupo en 'GrupoAlimentoBase'
        2 - Crear tipos de alimentos para grupos
            Bucle:
                2.1. - Crear tipo de alimentos con valores nutricionales de un grupo
                2.2. - Obtener último id_tipo creado (auto incremental)
                2.3. - Crear grupo asignando el id_tipo (tiene sus valores nutricionales) y el id_base (tiene su nombre y ejemplos)

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


-- --- CREAR TIPOS DE ALIMENTOS CON VALORES NUTRICIONALES ----------------------------------
-- --- Y ASIGNARLES LOS GRUPOS DE ALIMENTOS ------------------------------------------------
-- Procedimiento almacenado para reducir la cantidad de consultas
DELIMITER //
CREATE PROCEDURE InsertarGrupo(IN kcal_g FLOAT, IN prot_g FLOAT, IN lip_g FLOAT, IN carb_g FLOAT, IN grupo INT, IN id_base INT)
BEGIN
    INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (kcal_g, prot_g, lip_g, carb_g);
    SET @id_tipo = LAST_INSERT_ID();
    INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (grupo, id_base, @id_tipo);
END //
DELIMITER ;

CALL InsertarGrupo(0.684059925, 0.038014981, 0.037910112, 0.047191011, 1, 1);
CALL InsertarGrupo(0.465, 0.033, 0.016, 0.046, 2, 2);
CALL InsertarGrupo(0.391466667, 0.041822222, 0.0016, 0.052133333, 3, 3);
CALL InsertarGrupo(0.841395349, 0.032483721, 0.02067907, 0.131274419, 4, 4);
CALL InsertarGrupo(1.130612245, 0.036653061, 0.026938776, 0.184489796, 5, 5);
CALL InsertarGrupo(0.455913978, 0.00719086, 0.001747312, 0.102822581, 6, 6);
CALL InsertarGrupo(0.224860335, 0.01673743, 0.003128492, 0.035195531, 7, 7);
CALL InsertarGrupo(2.095873684, 0.063073684, 0.015621053, 0.4256, 8, 8);
CALL InsertarGrupo(3.01, 0.219, 0.03, 0.465, 9, 9);
CALL InsertarGrupo(4.193949153, 0.071186441, 0.211423729, 0.501864407, 10, 10);
CALL InsertarGrupo(6.517826087, 0.053478261, 0.679565217, 0.045652174, 11, 11);
CALL InsertarGrupo(0.87375, 0.0110625, 0.0879375, 0.00975, 12, 12);
CALL InsertarGrupo(3.625396825, 0.029365079, 0.367460317, 0.049206349, 13, 13);
CALL InsertarGrupo(6.08, 0.0272, 0.6624, 0.0032, 14, 14);
CALL InsertarGrupo(3.491368421, 0.021052632, 0.018526316, 0.810947368, 15, 15);
CALL InsertarGrupo(0.368903088, 0.073801917, 0.006709265, 0.002555911, 16, 16);
CALL InsertarGrupo(0.73488, 0.11456, 0.03024, 0.00096, 17, 17);
CALL InsertarGrupo(1.706813187, 0.153186813, 0.113186813, 0.008351648, 18, 18);
CALL InsertarGrupo(3.601914894, 0.222765957, 0.299361702, 0.004468085, 19, 19);
CALL InsertarGrupo(3.419606061, 0.145090909, 0.308212121, 0.016333333, 20, 20);
CALL InsertarGrupo(0.237068966, 0.017646177, 0.003298351, 0.037106447, 27, 7);
CALL InsertarGrupo(1.170534979, 0.035226337, 0.00872428, 0.237695473, 28, 8);
CALL InsertarGrupo(1.119421488, 0.081446281, 0.011157025, 0.172933884, 29, 9);
CALL InsertarGrupo(0.836939597, 0.167436242, 0.015221477, 0.005798658, 36, 16);
CALL InsertarGrupo(1.321273973, 0.205972603, 0.054369863, 0.001726027, 37, 17);
CALL InsertarGrupo(1.897038168, 0.170259542, 0.125801527, 0.009282443, 38, 18);

-- -----------------------------------------------------------------------------------------




/*
-- --- CREAR TIPOS DE ALIMENTOS CON VALORES NUTRICIONALES ----------------------------------
-- --- Y ASIGNARLES LOS GRUPOS DE ALIMENTOS ------------------------------------------------
-- GRUPO 1
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (0.684059925, 0.038014981, 0.037910112, 0.047191011);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (1, 1, @id_tipo);

-- GRUPO 2
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (0.465, 0.033, 0.016, 0.046);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (2, 2, @id_tipo);

-- GRUPO 3
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (0.391466667, 0.041822222, 0.0016, 0.052133333);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (3, 3, @id_tipo);

-- GRUPO 4
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (0.841395349, 0.032483721, 0.02067907, 0.131274419);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (4, 4, @id_tipo);

-- GRUPO 5
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (1.130612245, 0.036653061, 0.026938776, 0.184489796);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (5, 5, @id_tipo);

-- GRUPO 6
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (0.455913978, 0.00719086, 0.001747312, 0.102822581);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (6, 6, @id_tipo);

-- GRUPO 7
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (0.224860335, 0.01673743, 0.003128492, 0.035195531);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (7, 7, @id_tipo);

-- GRUPO 8
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (2.095873684, 0.063073684, 0.015621053, 0.4256);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (8, 8, @id_tipo);

-- GRUPO 9
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (3.01, 0.219, 0.03, 0.465);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (9, 9, @id_tipo);

-- GRUPO 10
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (4.193949153, 0.071186441, 0.211423729, 0.501864407);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (10, 10, @id_tipo);

-- GRUPO 11
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (6.517826087, 0.053478261, 0.679565217, 0.045652174);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (11, 11, @id_tipo);

-- GRUPO 12
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (0.87375, 0.0110625, 0.0879375, 0.00975);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (12, 12, @id_tipo);

-- GRUPO 13
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (3.625396825, 0.029365079, 0.367460317, 0.049206349);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (13, 13, @id_tipo);

-- GRUPO 14
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (6.08, 0.0272, 0.6624, 0.0032);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (14, 14, @id_tipo);

-- GRUPO 15
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (3.491368421, 0.021052632, 0.018526316, 0.810947368);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (15, 15, @id_tipo);

-- GRUPO 16
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (0.368903088, 0.073801917, 0.006709265, 0.002555911);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (16, 16, @id_tipo);

-- GRUPO 17
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (0.73488, 0.11456, 0.03024, 0.00096);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (17, 17, @id_tipo);

-- GRUPO 18
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (1.706813187, 0.153186813, 0.113186813, 0.008351648);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (18, 18, @id_tipo);

-- GRUPO 19
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (3.601914894, 0.222765957, 0.299361702, 0.004468085);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (19, 19, @id_tipo);

-- GRUPO 20
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (3.419606061, 0.145090909, 0.308212121, 0.016333333);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (20, 20, @id_tipo);

-- GRUPO 27
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (0.237068966, 0.017646177, 0.003298351, 0.037106447);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (27, 7, @id_tipo);

-- GRUPO 28
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (1.170534979, 0.035226337, 0.00872428, 0.237695473);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (28, 8, @id_tipo);

-- GRUPO 29
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (1.119421488, 0.081446281, 0.011157025, 0.172933884);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (29, 9, @id_tipo);

-- GRUPO 36
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (0.836939597, 0.167436242, 0.015221477, 0.005798658);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (36, 16, @id_tipo);

-- GRUPO 37
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (1.321273973, 0.205972603, 0.054369863, 0.001726027);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (37, 17, @id_tipo);

-- GRUPO 38
INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (1.897038168, 0.170259542, 0.125801527, 0.009282443);
SET @id_tipo = LAST_INSERT_ID(); -- Obtener el id_tipo que acabamos de insertar
INSERT INTO GrupoAlimento (grupo, id_base, id_tipo) VALUES (38, 18, @id_tipo);
-- -----------------------------------------------------------------------------------------


*/

