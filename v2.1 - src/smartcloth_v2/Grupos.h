/**
 * @file grupos.h
 * @brief Definiciones de grupos de alimentos
 *
 * @author Irene Casares Rodríguez
 * @date 06/06/23
 * @version 1.0
 *
 * Este archivo contiene la definición de los grupos de alimentos y sus características.
 *
 * @note Este archivo asume la existencia del siguiente archivo de encabezado:
 *       - "COLORS.h" para la definición de las macros de colores
 * 
 * @see COLORS.h
 * @see https://cs.stanford.edu/people/miles/iso8859.html Colección de caracteres ASCII/ISO 8859-1 (Latin-1) en HEX
 * @see https://www.raio.com.tw/data_raio/Datasheet/RA887677/RA8876_Brief_DS_V11_Eng.pdf Table 14-1 (pag 99) del datasheet de RA8876 para caracteres Latin-1
 * 
 */


#ifndef GRUPOS_H
#define GRUPOS_H

#define NUM_GRUPOS 26 
//#define NUM_GRUPOS 27 // 26 nuestros (crudos y cocinados) y el de barcode

#include "COLORS.h" // Colores del texto de nombre de grupo y ejemplos




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



/* 
    Caracteres especiales en ISO 8859-1 (Latin-1) en HEX:
        á --> E1    Á --> C1
        é --> E9    É --> C9
        í --> ED    Í --> CD
        ó --> F3    Ó --> D3
        ú --> FA    Ú --> DA

        ñ --> F1    Ñ --> D1

        ¿ --> BF    ? --> 3F
        ¡ --> A1    ! --> 21
*/


//
// Si el grupo de alimentos seleccionado es de TIPO_A, necesita diferenciar entre crudo o cocinado porque sus valores
// nutricionales son diferentes según el caso. Para los de TIPO_B no importa si los cocinas o no. 
//
// Los IDs de los grupos van del 1 al 20 para los crudos y después van los de cocinado. Por ejemplo, el grupo 7 (Verduras y Hortalizas)
// es crudo, mientras que su correspondiente cocinado es el grupo 27. Por eso, si es un grupo de TIPO_A COCINADO se hace (buttonGrande+20)
// en checkAllButtons(), para acceder a los valores de ese grupo pero cocinado.
//
// Los grupos de TIPO_A son: 7 (27), 8 (28), 9 (29), 16 (36), 17 (37) y 18 (38)
// Los grupos de TIPO_B son: 1, 2, 3, 4, 5, 6, 10, 11, 12, 13, 14, 15, 19 y 20
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
Grupo gruposAlimentos[NUM_GRUPOS] = { {1,COLOR_G1,"L\xE1""cteos enteros","Leche entera de vaca (pasteurizada o UHT), de oveja, de cabra, yogurt\n   natural entero, cuajada, etc.",0.684059925,0.038014981,0.037910112,0.047191011},
                                      {2,COLOR_G2,"L\xE1""cteos semidesnatados","Leche semidesnatada pasteurizada y UHT",0.465,0.033,0.016,0.046},
                                      {3,COLOR_G3,"L\xE1""cteos desnatados","Leche desnatada pasteurizada y UHT, natural, con frutas, yogurt desnatado,\n   yogurt desnatado de sabores, etc.",0.391466667,0.041822222,0.0016,0.052133333},
                                      {4,COLOR_G4,"L\xE1""cteos azucarados","Batidos l\xE1""cteos de cacao y otros sabores, leche entera fermentada con\n   frutas, yogures enteros de sabores y azucarados, yogures l\xED""quidos de\n   sabores y azucarados",0.841395349,0.032483721,0.02067907,0.131274419},
                                      {5,COLOR_G5,"Postres l\xE1""cteos","Arroz con leche, flan de huevo, flan de vainilla y natillas\x85""",1.130612245,0.036653061,0.026938776,0.184489796},
                                      {6,COLOR_G6,"Frutas frescas, desecadas y zumos","Albaricoque, ar\xE1""ndanos, cerezas, ciruelas, d\xE1""til seco, fresa, granada,\n   higos, kiwi, mandarina,manzana, melocot\xF3""n, mel\xF3""n, naranja, pera, pi\xF1""a,\n   pl\xE1""tano, sand\xED""a, uvas\x85""",0.455913978,0.00719086,0.001747312,0.102822581},
                                      {7,COLOR_G7,"Verduras y hortalizas","Acelgas, apio, alcachofa, berenjena, br\xF3""coli, calabac\xED""n, calabaza,\n   champi\xF1""ones, col, esp\xE1""rragos, espinacas, guisantes, lechuga, jud\xED""as,\n   pimientos, tomate, zanahoria, etc.",0.224860335,0.01673743,0.003128492,0.035195531},
                                      {8,COLOR_G8,"Cereales y tub\xE9""rculos","Arroz, avena, boniato, casta\xF1""a, cereales de desayuno ricos en fibra,\n   copos de ma\xED""z, harina, ma\xED""z, pan, pasta, patata, s\xE9""mola de trigo, etc.",2.095873684,0.063073684,0.015621053,0.4256},
                                      {9,COLOR_G9,"Legumbres","Alubias, garbanzos, lentejas, etc.",3.01,0.219,0.03,0.465},
                                      {10,COLOR_G10,"Reposter\xED""a, pasteler\xED""a y otros","Bizcocho, bollo, croissant, ensaimada, galletas (de cualquier tipo),\n   magdalena, muesli, pan de pasas, tartas, pasteles, boller\xED""a industrial\x85""",4.193949153,0.071186441,0.211423729,0.501864407},
                                      {11,COLOR_G11,"Alimentos ricos en grasas saludables","Aceites de cacahuete, de oliva y de h\xED""gado de bacalao, aceitunas,\n   aguacate, almendras, avellanas, cacahuetes, pistachos, mayonesa de aceite\n   de oliva, etc.",6.517826087,0.053478261,0.679565217,0.045652174},
                                      {12,COLOR_G12,"Alimentos ricos en grasas vegetales","Aceite de girasol, aceite de ma\xED""z, aceite de soja, mayonesa light, nueces,\n   pi\xF1""ones, etc.",0.87375,0.0110625,0.0879375,0.00975},
                                      {13,COLOR_G13,"Alimentos ricos en grasas saturadas","Coco fresco o seco, aceite de coco, mantequilla, nata l\xED""quida para cocinar\n   o montar, etc.",3.625396825,0.029365079,0.367460317,0.049206349},
                                      {14,COLOR_G14,"Alimentos muy grasos (mezclas)","Margarina light, margarina vegetal enriquecida, manteca y tocino de cerdo",6.08,0.0272,0.6624,0.0032},
                                      {15,COLOR_G15,"Az\xFA""cares y dulces","Az\xFA""car blanco y moreno, miel, leche condensada, cacao soluble azucarado,\n   confitura de fruta baja en calor\xED""as",3.491368421,0.021052632,0.018526316,0.810947368},
                                      {16,COLOR_G16,"Alimentos proteicos con muy poca grasa","Pavo, pollo, ternera (entrecot y solomillo), jam\xF3""n cocido, at\xFA""n natural,\n   pescado no graso, marisco, queso granulado, clara de huevo, etc.",0.368903088,0.073801917,0.006709265,0.002555911},
                                      {17,COLOR_G17,"Alimentos proteicos con poca grasa","Lomo de cerdo, pollo sin piel, bistec de vaca\x2F""buey, jam\xF3""n curado (sin\n   grasa), pescados grasos (at\xFA""n, sardina, trucha, boquer\xF3""n\x85""), v\xED""sceras, pato\n   sin piel, codorniz, etc.",0.73488,0.11456,0.03024,0.00096},
                                      {18,COLOR_G18,"Alimentos proteicos semigrasos","Chuletas de cerdo, cordero, anchoas, at\xFA""n o sardinas en aceite, caballa,\n   salm\xF3""n, jam\xF3""n curado con grasa, huevo, queso fresco, reques\xF3""n, queso en\n   porciones, etc.",1.706813187,0.153186813,0.113186813,0.008351648},
                                      {19,COLOR_G19,"Alimentos proteicos grasos","Chuletas\x2F""costillas de cordero, chorizo, salchichas, fuet, quesos (azul,\n   babybel, camembert, cheddar, de cabra, emmental, gouda, gruyer, manchego\x85"")",3.601914894,0.222765957,0.299361702,0.004468085},
                                      {20,COLOR_G20,"Alimentos proteicos muy grasos","Carne picada sazonada, panceta de cerdo, morcilla, mortadela, pat\xE9"",\n   salami, salchich\xF3""n, etc.",3.419606061,0.145090909,0.308212121,0.016333333},
                                      {27,COLOR_G7,"Verduras y hortalizas","Acelgas, apio, alcachofa, berenjena, br\xF3""coli, calabac\xED""n, calabaza,\n   champi\xF1""ones, col, esp\xE1""rragos, espinacas, guisantes, lechuga, jud\xED""as,\n   pimientos, tomate, zanahoria, etc.",0.237068966,0.017646177,0.003298351,0.037106447},
                                      {28,COLOR_G8,"Cereales y tub\xE9""rculos","Arroz, avena, boniato, casta\xF1""a, cereales de desayuno ricos en fibra,\n   copos de ma\xED""z, harina, ma\xED""z, pan, pasta, patata, s\xE9""mola de trigo, etc.",1.170534979,0.035226337,0.00872428,0.237695473},
                                      {29,COLOR_G9,"Legumbres","Alubias, garbanzos, lentejas, etc.",1.119421488,0.081446281,0.011157025,0.172933884},
                                      {36,COLOR_G16,"Alimentos proteicos con muy poca grasa","Pavo, pollo, ternera (entrecot y solomillo), jam\xF3""n cocido, at\xFA""n natural,\n   pescado no graso, marisco, queso granulado, clara de huevo, etc.",0.836939597,0.167436242,0.015221477,0.005798658},
                                      {37,COLOR_G17,"Alimentos proteicos con poca grasa","Lomo de cerdo, pollo sin piel, bistec de vaca\x2F""buey, jam\xF3""n curado (sin\n   grasa), pescados grasos (at\xFA""n, sardina, trucha, boquer\xF3""n\x85""), v\xED""sceras, pato\n   sin piel, codorniz, etc.",1.321273973,0.205972603,0.054369863,0.001726027},
                                      {38,COLOR_G18,"Alimentos proteicos semigrasos","Chuletas de cerdo, cordero, anchoas, at\xFA""n o sardinas en aceite, caballa,\n   salm\xF3""n, jam\xF3""n curado con grasa, huevo, queso fresco, reques\xF3""n, queso en\n   porciones, etc.",1.897038168,0.170259542,0.125801527,0.009282443}
                                    };
                                    // {50,COLOR_G50,"NOMBRE PRODUCTO","",0.0,0.0,0.0,0.0}




Grupo grupoEscogido; // Grupo de alimentos seleccionado
Grupo grupoAnterior; // Grupo de alimentos seleccionado anteriormente




/*-----------------------------------------------------------------------------*/
/**
 * @brief Establece el grupo de alimentos seleccionado.
 * @param id 'buttonGrande', ID del grupo a seleccionar.
 */
/*-----------------------------------------------------------------------------*/
void setGrupoAlimentos(byte id)      
{
    byte posGrupo = 0;
    for(byte i = 0; i < NUM_GRUPOS; i++){
        if(gruposAlimentos[i].ID_grupo == id){ 
            posGrupo = i;
            break;
        }
    }
    grupoAnterior = grupoEscogido;
    grupoEscogido = gruposAlimentos[posGrupo]; 
}






#endif
