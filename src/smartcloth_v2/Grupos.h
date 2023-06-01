#ifndef GRUPOS_H
#define GRUPOS_H

#define NUM_GRUPOS 26


typedef struct {
    int ID_grupo;
    String Nombre_grupo;
    String Ejemplos_grupo;
    float Kcal_g; //por 1 gramo
    float Prot_g;
    float Lip_g;
    float Carb_g;
}Grupo;


/* 
   Caracteres ASCII/ISO 8859-1 (Latin-1) en HEX ==> https://cs.stanford.edu/people/miles/iso8859.html

   También en Table 14-1 (pag 99) del datasheet de RA8876:
 
    á --> E1    Á --> C1
    é --> E9    É --> C9
    í --> ED    Í --> CD
    ó --> F3    Ó --> D3
    ú --> FA    Ú --> DA

    ñ --> F1    Ñ --> D1

    ¿ --> BF    ? --> 3F

*/


                                    // ID |     Nombre     |           Ejemplos                  |   Kcal   |  Proteinas | Lipidos | Carbohidratos
                                    
Grupo gruposAlimentos[NUM_GRUPOS] = { {1,"L\xE1""cteos enteros","Leche entera de vaca (pasteurizada o UHT), de oveja, de cabra,\n  yogurt natural entero, cuajada, etc.",0.684059925,0.038014981,0.037910112,0.047191011},
                                      {2,"L\xE1""cteos semidesnatados","Leche semidesnatada pasteurizada y UHT",0.465,0.033,0.016,0.046},
                                      {3,"L\xE1""cteos desnatados","Leche desnatada pasteurizada y UHT, natural, con frutas,\n  yogurt desnatado, yogurt desnatado de sabores, etc.",0.391466667,0.041822222,0.0016,0.052133333},
                                      {4,"L\xE1""cteos azucarados","Batidos l\xE1""cteos de cacao y otros sabores, leche entera\n  fermentada con frutas, yogures enteros de sabores y azucarados,\n  yogures l\xED""quidos de sabores y azucarados",0.841395349,0.032483721,0.02067907,0.131274419},
                                      {5,"Postres l\xE1""cteos","Arroz con leche, flan de huevo, flan de vainilla y natillas\x85""",1.130612245,0.036653061,0.026938776,0.184489796},
                                      {6,"Frutas frescas, desecadas y zumos","Albaricoque, ar\xE1""ndanos, cerezas, ciruelas, d\xE1""til seco, fresa,\n  granada, higos, kiwi, mandarina,manzana, melocot\xF3""n, mel\xF3""n,\n  naranja, pera, pi\xF1""a, pl\xE1""tano, sand\xED""a, uvas\x85""",0.455913978,0.00719086,0.001747312,0.102822581},
                                      {7,"Verduras y hortalizas","Acelgas, apio, alcachofa, berenjena, br\xF3""coli, calabac\xED""n,\n  calabaza, champi\xF1""ones, col, esp\xE1""rragos, espinacas, guisantes,\n  lechuga, jud\xED""as, pimientos, tomate, zanahoria, etc.",0.224860335,0.01673743,0.003128492,0.035195531},
                                      {8,"Cereales y tub\xE9""rculos","Arroz, avena, boniato, casta\xF1""a, cereales de desayuno ricos en\n  fibra, copos de ma\xED""z, harina, ma\xED""z, pan, pasta, patata,\n  s\xE9""mola de trigo, etc.",2.095873684,0.063073684,0.015621053,0.4256},
                                      {9,"Legumbres","Alubias, garbanzos, lentejas, etc.",3.01,0.219,0.03,0.465},
                                      {10,"Reposter\xED""a, pasteler\xED""a y otros","Bizcoho, bollo, croissant, ensaimada, galletas (de cualquier\n  tipo), magdalena, muesli, pan de pasas, tartas, pasteles,\n  boller\xED""a industrial, etc.",4.193949153,0.071186441,0.211423729,0.501864407},
                                      {11,"Alimentos ricos en grasas saludables","Aceites de cacahuete, de oliva y de h\xED""gado de bacalao,\n  aceitunas, aguacate, almendras, avellanas, cacahuetes, pistachos,\n  mayonesa de aceite de oliva, etc.",6.517826087,0.053478261,0.679565217,0.045652174},
                                      {12,"Alimentos ricos en grasas vegetales","Aceite de girasol, aceite de ma\xED""z, aceite de soja, mayonesa\n  light, nueces, pi\xF1""ones, etc.",0.87375,0.0110625,0.0879375,0.00975},
                                      {13,"Alimentos ricos en grasas saturadas","Coco fresco o seco, aceite de coco, mantequilla, nata l\xED""quida\n  para cocinar o montar, etc.",3.625396825,0.029365079,0.367460317,0.049206349},
                                      {14,"Alimentos muy grasos (mezclas)","Margarina light, margarina vegetal enriquecida, manteca y tocino de cerdo",6.08,0.0272,0.6624,0.0032},
                                      {15,"Az\xFA""cares y dulces","Az\xFA""car blanco y moreno, miel, leche condensada, cacao soluble\n  azucarado, confitura de fruta baja en calor\xED""as",3.491368421,0.021052632,0.018526316,0.810947368},
                                      {16,"Alimentos proteicos con muy poca grasa","Pavo, pollo, ternera (entrecot y solomillo), jam\xF3""n cocido,\n  at\xFA""n natural, pescado no graso, marisco, queso granulado,\n  clara de huevo, etc.",0.368903088,0.073801917,0.006709265,0.002555911},
                                      {17,"Alimentos proteicos con poca grasa","Lomo de cerdo, pollo sin piel, bistec de vaca\x2F""buey, jam\xF3""n\n  curado (sin grasa), pescados grasos (at\xFA""n, sardina, trucha,\n  boquer\xF3""n\x85""), v\xED""sceras, pato sin piel, codorniz, etc.",0.73488,0.11456,0.03024,0.00096},
                                      {18,"Alimentos proteicos semigrasos","Chuletas de cerdo, cordero, anchoas, at\xFA""n o sardinas en\n  aceite, caballa, salm\xF3""n, jam\xF3""n curado con grasa, huevo,\n  queso fresco, reques\xF3""n, queso en porciones, etc.",1.706813187,0.153186813,0.113186813,0.008351648},
                                      {19,"Alimentos proteicos grasos","Chuletas\x2F""costillas de cordero, chorizo, salchichas, fuet,\n  quesos (azul, babybel, camembert, cheddar, de cabra, emmental,\n  gouda, gruyer, manchego\x85""), etc.",3.601914894,0.222765957,0.299361702,0.004468085},
                                      {20,"Alimentos proteicos muy grasos","Carne picada sazonada, panceta de cerdo, morcilla, mortadela,\n  pat\xE9"", salami, salchich\xF3""n, etc.",3.419606061,0.145090909,0.308212121,0.016333333},
                                      {27,"Verduras y hortalizas","Acelgas, apio, alcachofa, berenjena, br\xF3""coli, calabac\xED""n,\n  calabaza, champi\xF1""ones, col, esp\xE1""rragos, espinacas, guisantes, \n  lechuga, jud\xED""as, pimientos, tomate, zanahoria, etc.",0.237068966,0.017646177,0.003298351,0.037106447},
                                      {28,"Cereales y tub\xE9""rculos","Arroz, avena, boniato, casta\xF1""a, cereales de desayuno ricos en\n  fibra, copos de ma\xED""z, harina, ma\xED""z, pan, pasta, patata,\n  s\xE9""mola de trigo, etc.",1.170534979,0.035226337,0.00872428,0.237695473},
                                      {29,"Legumbres","Alubias, garbanzos, lentejas, etc.",1.119421488,0.081446281,0.011157025,0.172933884},
                                      {36,"Alimentos proteicos con muy poca grasa","Pavo, pollo, ternera (entrecot y solomillo), jam\xF3""n cocido,\n  at\xFA""n natural, pescado no graso, marisco, queso granulado,\n  clara de huevo, etc.",0.836939597,0.167436242,0.015221477,0.005798658},
                                      {37,"Alimentos proteicos con poca grasa","Lomo de cerdo, pollo sin piel, bistec de vaca\x2F""buey, jam\xF3""n\n  curado (sin grasa),pescados grasos (at\xFA""n, sardina, trucha,\n  boquer\xF3""n\x85""), v\xED""sceras, pato sin piel, codorniz, etc.",1.321273973,0.205972603,0.054369863,0.001726027},
                                      {38,"Alimentos proteicos semigrasos","Chuletas de cerdo, cordero, anchoas, at\xFA""n o sardinas en\n  aceite, caballa, salm\xF3""n, jam\xF3""n curado \n  con grasa, huevo,\n  queso fresco, reques\xF3""n, queso en porciones, etc.",1.897038168,0.170259542,0.125801527,0.009282443}
                                    };

Grupo grupoEscogido;
Grupo grupoAnterior;


/*---------------------------------------------------------------------------------------------------------
   getGrupoAlimentos(): Obtener grupo de alimentos seleccionado (checkAllButtons() en BUTTONS.h)
----------------------------------------------------------------------------------------------------------*/
void setGrupoAlimentos(int id){       // 'id' pasado es buttonGrande 
    int posGrupo = 0;
    for(int i = 0; i < NUM_GRUPOS; i++){
        if(gruposAlimentos[i].ID_grupo == id){ 
            posGrupo = i;
            break;
        }
    }
    //Serial.print(F("\nPosicion en vector grupo: ")); Serial.println(posGrupo);
    grupoAnterior = grupoEscogido;
    grupoEscogido = gruposAlimentos[posGrupo]; 
}






#endif
