import pandas as pd

# Lista de llamadas a procedimientos
calls = [
    "CALL InsertarGrupoYTipo(1; 0.684059925; 0.038014981; 0.037910112; 0.047191011; 'Lácteos enteros'; 'Leche entera de vaca (pasteurizada o UHT), de oveja, de cabra, yogurt natural entero, cuajada, etc.')",
    "CALL InsertarGrupoYTipo(2; 0.465; 0.033; 0.016; 0.046; 'Lácteos desnatados'; 'Leche desnatada, yogurt natural desnatado, etc.')",
    "CALL InsertarGrupoYTipo(3; 0.391466667; 0.041822222; 0.0016; 0.052133333; 'Lácteos desnatados'; 'Leche desnatada pasteurizada y UHT, natural, con frutas, yogurt desnatado, yogurt desnatado de sabores, etc.')",
    "CALL InsertarGrupoYTipo(4; 0.841395349; 0.032483721; 0.02067907; 0.131274419; 'Lácteos azucarados'; 'Batidos lácteos de cacao y otros sabores, leche entera fermentada con frutas, yogures enteros de sabores y azucarados, yogures líquidos de sabores y azucarados')",
    "CALL InsertarGrupoYTipo(5; 1.130612245; 0.036653061; 0.026938776; 0.184489796; 'Postres lácteos'; 'Arroz con leche, flan de huevo, flan de vainilla y natillas...')",
    "CALL InsertarGrupoYTipo(6; 0.455913978; 0.00719086; 0.001747312; 0.102822581; 'Frutas frescas, desecadas y zumos'; 'Albaricoque, arándanos, cerezas, ciruelas, dátil seco, fresa, granada, higos, kiwi, mandarina,manzana, melocotón, melón, naranja, pera, piña, plátano, sandía, uvas...')",
    "CALL InsertarGrupoYTipo(7; 0.224860335; 0.01673743; 0.003128492; 0.035195531; 'Verduras y hortalizas'; 'Acelgas, apio, alcachofa, berenjena, brócoli, calabacín, calabaza, champiñones, col, espárragos, espinacas, guisantes, lechuga, judías, pimientos, tomate, zanahoria, etc.')",
    "CALL InsertarGrupoYTipo(8; 2.095873684; 0.063073684; 0.015621053; 0.4256; 'Cereales y tubérculos'; 'Arroz, avena, boniato, castaña, cereales de desayuno ricos en fibra, copos de maíz, harina, maíz, pan, pasta, patata, sémola de trigo, etc.')",
    "CALL InsertarGrupoYTipo(9; 3.01; 0.219; 0.03; 0.465; 'Legumbres'; 'Alubias, garbanzos, lentejas, etc.')",
    "CALL InsertarGrupoYTipo(10; 4.193949153; 0.071186441; 0.211423729; 0.501864407; 'Repostería, pastelería y otros'; 'Bizcocho, bollo, croissant, ensaimada, galletas (de cualquier tipo), magdalena, muesli, pan de pasas, tartas, pasteles, bollería industrial...')",
    "CALL InsertarGrupoYTipo(11; 6.517826087; 0.053478261; 0.679565217; 0.045652174; 'Alimentos ricos en grasas saludables'; 'Aceites de cacahuete, de oliva y de hígado de bacalao, aceitunas, aguacate, almendras, avellanas, cacahuetes, pistachos, mayonesa de aceite de oliva, etc.')",
    "CALL InsertarGrupoYTipo(12; 0.87375; 0.0110625; 0.0879375; 0.00975; 'Alimentos ricos en grasas vegetales'; 'Aceite de girasol, aceite de maíz, aceite de soja, mayonesa light, nueces, piñones, etc.')",
    "CALL InsertarGrupoYTipo(13; 3.625396825; 0.029365079; 0.367460317; 0.049206349; 'Alimentos ricos en grasas saturadas'; 'Coco fresco o seco, aceite de coco, mantequilla, nata líquida para cocinar o montar, etc.')",
    "CALL InsertarGrupoYTipo(14; 6.08; 0.0272; 0.6624; 0.0032; 'Alimentos muy grasos (mezclas)'; 'Margarina light, margarina vegetal enriquecida, manteca y tocino de cerdo')",
    "CALL InsertarGrupoYTipo(15; 3.491368421; 0.021052632; 0.018526316; 0.810947368; 'Azúcares y dulces'; 'Azúcar blanco y moreno, miel, leche condensada, cacao soluble azucarado, confitura de fruta baja en calorías')",
    "CALL InsertarGrupoYTipo(16; 0.368903088; 0.073801917; 0.006709265; 0.002555911; 'Alimentos proteicos con muy poca grasa'; 'Pavo, pollo, ternera (entrecot y solomillo), jamón cocido, atún natural, pescado no graso, marisco, queso granulado, clara de huevo, etc.')",
    "CALL InsertarGrupoYTipo(17; 0.73488; 0.11456; 0.03024; 0.00096; 'Alimentos proteicos con poca grasa'; 'Lomo de cerdo, pollo sin piel, bistec de vaca/buey, jamón curado (sin grasa), pescados grasos (atún, sardina, trucha, boquerón...), vísceras, pato sin piel, codorniz, etc.')",
    "CALL InsertarGrupoYTipo(18; 1.706813187; 0.153186813; 0.113186813; 0.008351648; 'Alimentos proteicos semigrasos'; 'Chuletas de cerdo, cordero, anchoas, atún o sardinas en aceite, caballa, salmón, jamón curado con grasa, huevo, queso fresco, requesón, queso en porciones, etc.')",
    "CALL InsertarGrupoYTipo(19; 3.601914894; 0.222765957; 0.299361702; 0.004468085; 'Alimentos proteicos grasos'; 'Chuletas/costillas de cordero, chorizo, salchichas, fuet, quesos (azul, babybel, camembert, cheddar, de cabra, emmental, gouda, gruyer, manchego...)')",
    "CALL InsertarGrupoYTipo(20; 3.419606061; 0.145090909; 0.308212121; 0.016333333; 'Alimentos proteicos muy grasos'; 'Carne picada sazonada, panceta de cerdo, morcilla, mortadela, paté, salami, salchichón, etc.')",
    "CALL InsertarGrupoYTipo(27; 0.237068966; 0.017646177; 0.003298351; 0.037106447; 'Verduras y hortalizas'; 'Acelgas, apio, alcachofa, berenjena, brócoli, calabacín, calabaza, champiñones, col, espárragos, espinacas, guisantes, lechuga, judías, pimientos, tomate, zanahoria, etc.')",
    "CALL InsertarGrupoYTipo(28; 1.170534979; 0.035226337; 0.00872428; 0.237695473; 'Cereales y tubérculos'; 'Arroz, avena, boniato, castaña, cereales de desayuno ricos en fibra, copos de maíz, harina, maíz, pan, pasta, patata, sémola de trigo, etc.')",
    "CALL InsertarGrupoYTipo(29; 1.119421488; 0.081446281; 0.011157025; 0.172933884; 'Legumbres'; 'Alubias, garbanzos, lentejas, etc.')",
    "CALL InsertarGrupoYTipo(36; 0.836939597; 0.167436242; 0.015221477; 0.005798658; 'Alimentos proteicos con muy poca grasa'; 'Pavo, pollo, ternera (entrecot y solomillo), jamón cocido, atún natural, pescado no graso, marisco, queso granulado, clara de huevo, etc.')",
    "CALL InsertarGrupoYTipo(37; 1.321273973; 0.205972603; 0.054369863; 0.001726027; 'Alimentos proteicos con poca grasa'; 'Lomo de cerdo, pollo sin piel, bistec de vaca/buey, jamón curado (sin grasa), pescados grasos (atún, sardina, trucha, boquerón...), vísceras, pato sin piel, codorniz, etc.')",
    "CALL InsertarGrupoYTipo(38; 1.897038168; 0.170259542; 0.125801527; 0.009282443; 'Alimentos proteicos semigrasos'; 'Chuletas de cerdo, cordero, anchoas, atún o sardinas en aceite, caballa, salmón, jamón curado con grasa, huevo, queso fresco, requesón, queso en porciones, etc.')"
]

# Extrae los datos de las llamadas y almacénalos en una lista de diccionarios
data = []
for call in calls:
    # Encuentra el índice del primer paréntesis y el último paréntesis
    start = call.find("(") + 1
    end = call.rfind(")")
    
    # Extrae la cadena entre los paréntesis y divide por los punto y coma
    parts = call[start:end].split("; ")
    
    # Verifica que parts tiene al menos 7 elementos
    if len(parts) < 7:
        print(f"La llamada {call} tiene menos de 7 argumentos")
        continue
    
    # Crea un diccionario con los datos y añádelo a la lista
    data.append({
        "ID_grupo": int(parts[0]),
        "Nombre_grupo": parts[5].strip("'"),
        "Ejemplos_grupo": parts[6].strip("'"),
        "Kcal_g": float(parts[1]),
        "Prot_g": float(parts[2]),
        "Lip_g": float(parts[3]),
        "Carb_g": float(parts[4]),
    })

# Crea un DataFrame a partir de la lista de diccionarios
df = pd.DataFrame(data)

# Escribe el DataFrame en un archivo .xlsx
df.to_excel("grupos_alimentos.xlsx", index=False)