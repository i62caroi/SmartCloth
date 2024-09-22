/**
 * @file grupos.h
 * @brief Definiciones de grupos de alimentos
 *
 * @author Irene Casares Rodríguez
 * @date 06/06/24
 * @version 1.0
 *
 * Este archivo contiene la definición de los grupos de alimentos y sus características.
 * 
 * @see https://cs.stanford.edu/people/miles/iso8859.html Colección de caracteres ASCII/ISO 8859-1 (Latin-1) en HEX
 * @see https://www.raio.com.tw/data_raio/Datasheet/RA887677/RA8876_Brief_DS_V11_Eng.pdf Table 14-1 (pag 99) del datasheet de RA8876 para caracteres Latin-1
 * 
 */


#ifndef GRUPOS_H
#define GRUPOS_H

#include "COLORS.h" // Colores del texto de nombre de grupo y ejemplos
#include "debug.h"  // SM_DEBUG --> SerialPC

 
#define NUM_GRUPOS 28 // 27 nuestros (crudos y cocinados) y el de barcode
#define BARCODE_PRODUCT_INDEX 50 // ID del grupo de alimentos para el barcode



/*******************************************************************************
/*******************************************************************************
                          DECLARACIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/
void    setGrupoAlimentos(byte id);                             // Establece el grupo de alimentos seleccionado
void    updateGrupoActualFromBarcode(String &productInfo);    // Actualiza el grupo de alimentos seleccionado con la info del producto barcode leído

// --- CONVERSIÓN DE TEXTO ---
void    convertSpecialCharactersToHEX_ref(String &input);       // Convierte a HEX caracteres especiales en una cadena de texto pasada por referencia
String  convertSpecialCharactersToHEX(String input);            // Convierte a HEX caracteres especiales en una cadena de texto y devuelve la cadena modificada
/******************************************************************************/
/******************************************************************************/






/*******************************************************************************
/*******************************************************************************
                          GRUPOS DE ALIMENTOS
/******************************************************************************/
/******************************************************************************/

/*-----------------------------------------------------------------------------*/
/**
 * @struct Grupo
 * @brief Estructura que representa un grupo de alimentos.
 *
 * Esta estructura contiene información sobre un grupo de alimentos, como su ID, color, nombre,
 * ejemplos y contenido nutricional por gramo.
 */
/*-----------------------------------------------------------------------------*/
typedef struct {
    byte          ID_grupo;         /**< ID del grupo */
    uint16_t      color_grupo;      /**< Color del texto del grupo */
    String        Nombre_grupo;     /**< Nombre del grupo */
    String        Ejemplos_grupo;   /**< Ejemplos del grupo */
    float         Kcal_g;           /**< Calorías por gramo */
    float         Prot_g;           /**< Proteínas por gramo */
    float         Lip_g;            /**< Lípidos por gramo */
    float         Carb_g;           /**< Carbohidratos por gramo */
}Grupo;




//
// Si el grupo de alimentos seleccionado es de TIPO_A, necesita diferenciar entre crudo o cocinado porque sus valores
// nutricionales son diferentes según el caso. Para los de TIPO_B no importa si los cocinas o no. 
//
// Los IDs de los grupos van del 1 al 20 para los crudos y después van los de cocinado. Por ejemplo, el grupo 7 (Verduras y Hortalizas)
// es crudo, mientras que su correspondiente cocinado es el grupo 27. Por eso, si es un grupo de TIPO_A COCINADO se hace (buttonGrande+20)
// en checkAllButtons(), para acceder a los valores de ese grupo pero cocinado.
//
// Los grupos de TIPO_A son: 7 (27), 8 (28), 9 (29), 16 (36), 17 (37), 18 (38) y 19 (39)
// Los grupos de TIPO_B son: 1, 2, 3, 4, 5, 6, 10, 11, 12, 13, 14, 15 y 20
//
// El usuario puede escoger crudo o cocinado independientemente del grupo seleccionado y así se mostrará en pantalla, pero solo afectará
// a los valores si el grupo es de TIPO_A. Esto se hace porque el usuario no tiene por qué saber para qué grupos cambian los valores según
// si el alimento está crudo o cocinado.
//





                                    // ID | Color | Nombre | Ejemplos | Kcal | Proteinas | Lipidos | Carbohidratos
/*-----------------------------------------------------------------------------*/
/**
 * @var gruposAlimentos
 * @brief Arreglo de objetos Grupo que representa los grupos de alimentos.
 *
 * Este array almacena los distintos grupos de alimentos y sus características.
 */               
/*-----------------------------------------------------------------------------*/                    
Grupo gruposAlimentos[NUM_GRUPOS] = { {1,COLOR_G1,"Lácteos enteros","Leche entera de vaca (pasteurizada o UHT), de oveja, de cabra, yogurt\n   natural entero, cuajada, etc.",0.7105,0.0378,0.0414,0.0515},
                                      {2,COLOR_G2,"Lácteos semidesnatados","Leche semidesnatada pasteurizada y UHT",0.4729,0.0332,0.0174,0.0495},
                                      {3,COLOR_G3,"Lácteos desnatados","Leche desnatada pasteurizada y UHT, natural, con frutas, yogurt desnatado,\n   yogurt desnatado de sabores, etc.",0.3393,0.0338,0.0028,0.0478},
                                      {4,COLOR_G4,"Lácteos azucarados","Batidos lácteos de cacao y otros sabores, leche entera fermentada con\n   frutas, yogures enteros de sabores y azucarados, yogures líquidos de\n   sabores y azucarados",0.8598,0.0301,0.0257,0.1316},
                                      {5,COLOR_G5,"Postres lácteos","Arroz con leche, flan de huevo, flan de vainilla y natillas...",1.8484,0.0356,0.0806,0.2478},
                                      {6,COLOR_G6,"Frutas frescas, desecadas y zumos","Albaricoque, arándanos, cerezas, ciruelas, dátil seco, fresa, granada,\n   higos, kiwi, mandarina,manzana, melocotón, melón, naranja, pera, piña,\n   plátano, sandía, uvas...",0.4809,0.0082,0.0025,0.1119},
                                      {7,COLOR_G7,"Verduras y hortalizas","Acelgas, apio, alcachofa, berenjena, brócoli, calabacín, calabaza,\n   champiñones, col, espárragos, espinacas, guisantes, lechuga, judías,\n   pimientos, tomate, zanahoria, etc.",0.2454,0.0148,0.0037,0.0412},
                                      {8,COLOR_G8,"Cereales y tubérculos","Arroz, avena, boniato, castaña, cereales de desayuno ricos en fibra,\n   copos de maíz, harina, maíz, pan, pasta, patata, sémola de trigo, etc.",2.1053,0.0622,0.0142,0.4452},
                                      {9,COLOR_G9,"Legumbres","Alubias, garbanzos, lentejas, etc.",3.2236,0.2148,0.0327,0.5523},
                                      {10,COLOR_G10,"Repostería, pastelería y otros","Bizcocho, bollo, croissant, ensaimada, galletas (de cualquier tipo),\n   magdalena, muesli, pan de pasas, tartas, pasteles, bollería industrial...",3.8755,0.0707,0.1883,0.5055},
                                      {11,COLOR_G11,"Alimentos ricos en grasas saludables","Aceites de cacahuete, de oliva y de hígado de bacalao, aceitunas,\n   aguacate, almendras, avellanas, cacahuetes, pistachos, mayonesa de aceite\n   de oliva, etc.",7.2434,0.0283,0.7874,0.0275},
                                      {12,COLOR_G12,"Alimentos ricos en grasas vegetales","Aceite de girasol, aceite de maíz, aceite de soja, mayonesa light, nueces,\n   piñones, etc.",8.3561,0.0232,0.9312,0.0066},
                                      {13,COLOR_G13,"Alimentos ricos en grasas saturadas","Coco fresco o seco, aceite de coco, mantequilla, nata líquida para cocinar\n   o montar, etc.",5.6178,0.0126,0.6115,0.0178},
                                      {14,COLOR_G14,"Alimentos muy grasos (mezclas)","Margarina light, margarina vegetal enriquecida, manteca y tocino de cerdo",8.2686,0.0107,0.9136,0.0007},
                                      {15,COLOR_G15,"Azúcares y dulces","Azúcar blanco y moreno, miel, leche condensada, cacao soluble azucarado,\n   confitura de fruta baja en calorías",3.5162,0.0262,0.0247,0.8446},
                                      {16,COLOR_G16,"Alimentos proteicos con muy poca grasa","Pavo, pollo, ternera (entrecot y solomillo), jamón cocido, atún natural,\n   pescado no graso, marisco, queso granulado, clara de huevo, etc.",0.9947,0.1930,0.0176,0.0101},
                                      {17,COLOR_G17,"Alimentos proteicos con poca grasa","Lomo de cerdo, pollo sin piel, bistec de vaca/buey, jamón curado (sin\n   grasa), pescados grasos (atún, sardina, trucha, boquerón...), vísceras, pato\n   sin piel, codorniz, etc.",1.3225,0.1814,0.0505,0.0375},
                                      {18,COLOR_G18,"Alimentos proteicos semigrasos","Chuletas de cerdo, cordero, anchoas, atún o sardinas en aceite, caballa,\n   salmón, jamón curado con grasa, huevo, queso fresco, requesón, queso en\n   porciones, etc.",1.5525,0.1347,0.1126,0.0064},
                                      {19,COLOR_G19,"Alimentos proteicos grasos","Chuletas/costillas de cordero, chorizo, salchichas, fuet, quesos (azul,\n   babybel, camembert, cheddar, de cabra, emmental, gouda, gruyer, manchego...)",2.7904,0.2061,0.2160,0.0},
                                      {20,COLOR_G20,"Alimentos proteicos muy grasos","Carne picada sazonada, panceta de cerdo, morcilla, mortadela, paté,\n   salami, salchichón, etc.",3.1614,0.1316,0.2817,0.0364},
                                      {27,COLOR_G7,"Verduras y hortalizas","Acelgas, apio, alcachofa, berenjena, brócoli, calabacín, calabaza,\n   champiñones, col, espárragos, espinacas, guisantes, lechuga, judías,\n   pimientos, tomate, zanahoria, etc.",0.2282,0.0158,0.0098,0.0235},
                                      {28,COLOR_G8,"Cereales y tubérculos","Arroz, avena, boniato, castaña, cereales de desayuno ricos en fibra,\n   copos de maíz, harina, maíz, pan, pasta, patata, sémola de trigo, etc.",0.9617,0.0195,0.0051,0.2375},
                                      {29,COLOR_G9,"Legumbres","Alubias, garbanzos, lentejas, etc.",1.1762,0.0858,0.0146,0.1868},
                                      {36,COLOR_G16,"Alimentos proteicos con muy poca grasa","Pavo, pollo, ternera (entrecot y solomillo), jamón cocido, atún natural,\n   pescado no graso, marisco, queso granulado, clara de huevo, etc.",0.9450,0.1978,0.0131,0.0013},
                                      {37,COLOR_G17,"Alimentos proteicos con poca grasa","Lomo de cerdo, pollo sin piel, bistec de vaca/buey, jamón curado (sin\n   grasa), pescados grasos (atún, sardina, trucha, boquerón...), vísceras, pato\n   sin piel, codorniz, etc.",1.6939,0.2269,0.0728,0.0258},
                                      {38,COLOR_G18,"Alimentos proteicos semigrasos","Chuletas de cerdo, cordero, anchoas, atún o sardinas en aceite, caballa,\n   salmón, jamón curado con grasa, huevo, queso fresco, requesón, queso en\n   porciones, etc.",1.4769,0.1298,0.1067,0.0064},
                                      {39,COLOR_G19,"Alimentos proteicos grasos","Chuletas/costillas de cordero, chorizo, salchichas, fuet, quesos (azul,\n   babybel, camembert, cheddar, de cabra, emmental, gouda, gruyer, manchego...)",2.8156,0.1869,0.2260,0.0}
                                      //{BARCODE_PRODUCT_INDEX,COLOR_G50,"","",0.0,0.0,0.0,0.0} 
                                    };



Grupo grupoActual; // Grupo de alimentos seleccionado
Grupo grupoAnterior; // Grupo de alimentos seleccionado anteriormente, necesario para saber qué valores guardar al
                     // poner peso y luego escoger otro grupo, lo que confirma el peso puesto.


/******************************************************************************/
/******************************************************************************/






/*******************************************************************************
/*******************************************************************************
                           DEFINICIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/

/*-----------------------------------------------------------------------------*/
/**
 * @brief Establece el grupo de alimentos seleccionado.
 * @param id ID del grupo a seleccionar ('buttonGrande' o 50 si es el de barcode).
 */
/*-----------------------------------------------------------------------------*/
void setGrupoAlimentos(byte id)      
{
    // Obtener posición del grupo en el array
    byte posGrupo = 0;
    for(byte i = 0; i < NUM_GRUPOS; i++){
        if(gruposAlimentos[i].ID_grupo == id){ 
            posGrupo = i;
            break;
        }
    }
    grupoAnterior = grupoActual;
    grupoActual = gruposAlimentos[posGrupo]; 

    // Convertir caracteres especiales a HEX en grupoActual
    convertSpecialCharactersToHEX_ref(grupoActual.Nombre_grupo);
    convertSpecialCharactersToHEX_ref(grupoActual.Ejemplos_grupo);
    // No hace falta para el grupoAnterior porque no se muestra en pantalla, 
    // solo se usa para guardar los valores al poner peso y luego escoger otro grupo.

}




/*-----------------------------------------------------------------------------*/
/**
 * Actualiza la información del grupo de alimentos 'grupoEscodigo' basándose en el código de barras proporcionado.
 * 
 * Esta función toma una cadena de texto que representa la información del producto obtenida a través
 * de un código de barras. La información del producto debe seguir un formato específico donde los
 * diferentes campos están separados por punto y coma (;). El formato esperado es:
 * "PRODUCT:<barcode>;<nombre_producto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>"
 * 
 * Los campos extraídos son:
 * - barcode: El código de barras del producto.
 * - nombre_producto: El nombre del producto.
 * - carb_1g: La cantidad de carbohidratos por gramo.
 * - lip_1g: La cantidad de lípidos (grasas) por gramo.
 * - prot_1g: La cantidad de proteínas por gramo.
 * - kcal_1g: La cantidad de kilocalorías por gramo.
 * 
 * Estos valores se utilizan para actualizar la información del grupo de alimentos actualmente seleccionado.
 * 
 * @param productInfo Una cadena de texto que contiene la información del producto, incluyendo el nombre y los valores nutricionales.
 */
/*-----------------------------------------------------------------------------*/
void updateGrupoActualFromBarcode(String &productInfo)
{
    #ifdef SM_DEBUG
        SerialPC.println("Actualizando 'grupoActual' con la info del producto...");
    #endif

    // ----- ACTUALIZAR GRUPO ANTERIOR -----
    // Actualizar grupoAnterior con el valor que tiene grupoActual antes de modificarlo con los valores del barcode
    grupoAnterior = grupoActual;

    // ----- OBTENER INFO DEL PRODUCTO ------------------------
    String cad = productInfo.substring(8); // Elimina el prefijo "PRODUCT:"

    int idx_nombre = cad.indexOf(';');
    int idx_carb = cad.indexOf(';', idx_nombre + 1);
    int idx_lip = cad.indexOf(';', idx_carb + 1);
    int idx_prot = cad.indexOf(';', idx_lip + 1);
    int idx_kcal = cad.indexOf(';', idx_prot + 1);

    String barcode = cad.substring(0, idx_nombre);
    String nombre_producto = cad.substring(idx_nombre + 1, idx_carb);
    float carb_1g = cad.substring(idx_carb + 1, idx_lip).toFloat();
    float lip_1g = cad.substring(idx_lip + 1, idx_prot).toFloat();
    float prot_1g = cad.substring(idx_prot + 1, idx_kcal).toFloat();
    float kcal_1g = cad.substring(idx_kcal + 1).toFloat();
    // --------------------------------------------------------


    // ----- ACTUALIZAR GRUPO ACTUAL CON INFO DEL BARCODE -----
    // Obtener posición del grupo de barcode en el array por automatizarlo, pero podríamos poner directamente 27 porque es el último
    /*byte posGrupo = 0;
    for(byte i = 0; i < NUM_GRUPOS; i++){
        if(gruposAlimentos[i].ID_grupo == BARCODE_PRODUCT_INDEX){ 
            posGrupo = i; 
            break;
        }
    }

    // Actualizar grupoActual con la info por defecto del grupo barcode (nombre y ejemplos vacíos, y valores nutricionales a 0)
    grupoActual = gruposAlimentos[posGrupo]; // Grupo de barcode. Toma el ID (50) y el color, pero los datos los modificamos
    */

    //grupoActual = gruposAlimentos[gruposAlimentos.length() - 1];
    //grupoActual = gruposAlimentos[27];

    // Modificar datos con info del producto:
    grupoActual.ID_grupo = BARCODE_PRODUCT_INDEX; // 50
    grupoActual.color_grupo = COLOR_G50;
    grupoActual.Nombre_grupo = convertSpecialCharactersToHEX(nombre_producto); // Convertir caracteres especiales en el nombre a HEX
    grupoActual.Ejemplos_grupo = ""; // No hay ejemplos para el producto barcode
    grupoActual.Carb_g = carb_1g;
    grupoActual.Lip_g = lip_1g;
    grupoActual.Prot_g = prot_1g;
    grupoActual.Kcal_g = kcal_1g;

    #ifdef SM_DEBUG
        SerialPC.println("\nCodigo: " + barcode);
        SerialPC.println("ID grupo: " + String(grupoActual.ID_grupo));
        SerialPC.println("Nombre: " + grupoActual.Nombre_grupo);
        SerialPC.println("Carb_1g: " + String(grupoActual.Carb_g));
        SerialPC.println("Lip_1g: " + String(grupoActual.Lip_g));
        SerialPC.println("Prot_1g: " + String(grupoActual.Prot_g));
        SerialPC.println("Kcal_1g: " + String(grupoActual.Kcal_g));
    #endif
    // ---------------------------------------------------------

}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte los caracteres especiales en una cadena de texto.
 * 
 * Esta función reemplaza los caracteres especiales en una cadena de texto con sus correspondientes
 * códigos de caracteres en formato hexadecimal. Los caracteres especiales que se reemplazan son:
 * 
 * - Minúsculas: vocales acentuadas (á, é, í, ó, ú) y la letra 'ñ'.
 * - Mayúsculas: vocales acentuadas (Á, É, Í, Ó, Ú) y la letra 'Ñ'.
 * - Signos de interrogación (¿), exclamación (¡), escape (/) y puntos suspensivos (...).
 * 
 * Se utilizan los caracteres especiales en ISO 8859-1 (Latin-1) en HEX.
 * 
 * @param input La cadena de texto en la que se reemplazarán los caracteres especiales.
 * 
 * @see https://cs.stanford.edu/people/miles/iso8859.html Colección de caracteres ASCII/ISO 8859-1 (Latin-1) en HEX
 * @see https://www.raio.com.tw/data_raio/Datasheet/RA887677/RA8876_Brief_DS_V11_Eng.pdf Table 14-1 (pag 99) del datasheet de RA8876 para caracteres Latin-1
 */
/*-----------------------------------------------------------------------------*/
void convertSpecialCharactersToHEX_ref(String &input) 
{
    // Minúsculas: vocales y 'ñ'
    input.replace("á", "\xE1");
    input.replace("é", "\xE9");
    input.replace("í", "\xED");
    input.replace("ó", "\xF3");
    input.replace("ú", "\xFA");
    input.replace("ñ", "\xF1");

    // Mayúsculas: vocales y 'Ñ'
    input.replace("Á", "\xC1");
    input.replace("É", "\xC9");
    input.replace("Í", "\xCD");
    input.replace("Ó", "\xD3");
    input.replace("Ú", "\xDA");
    input.replace("Ñ", "\xD1");

    // Signos de interrogación y exclamación
    input.replace("¿", "\xBF");
    input.replace("¡", "\xA1");

    // Caracter especial '/'
    input.replace("/", "\x2F");

    // Puntos suspensivos
    input.replace("...", "\x85");
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte los caracteres especiales en una cadena de texto utilizando una función auxiliar.
 * 
 * Esta función llama a `convertSpecialCharactersToHEX_ref` para realizar la conversión de los caracteres
 * especiales en la cadena de texto. La función auxiliar modifica la cadena directamente a través de una
 * referencia.
 * 
 * @param input La cadena de texto en la que se van a reemplazar los caracteres especiales.
 * @return La cadena de texto con los caracteres especiales reemplazados.
 */
/*-----------------------------------------------------------------------------*/
String convertSpecialCharactersToHEX(String input) 
{
    convertSpecialCharactersToHEX_ref(input);
    return input;
}




/******************************************************************************/
/******************************************************************************/


#endif
