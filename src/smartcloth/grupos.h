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
    á --> E1    Á --> C1
    é --> E9    É --> C9
    í --> ED    Í --> CD
    ó --> F3    Ó --> D3
    ú --> FA    Ú --> DA

    ñ --> F1    Ñ --> D1

*/


                                    // ID |     Nombre     |           Ejemplos                  |   Kcal   |  Proteinas | Lipidos | Carbohidratos
                                    
Grupo gruposAlimentos[NUM_GRUPOS] = { {1,"L\xE1""cteos Enteros","Leche, cuajada, yogur de leche entera",0.684059925,0.038014981,0.037910112,0.047191011},
                                      {2,"L\xE1""cteos Semidesnatados","Leche, yogur de leche semidesnatada",0.465,0.033,0.016,0.046},
                                      {3,"L\xE1""cteos Desnatados","Leche, yogur de leche desnatada",0.391466667,0.041822222,0.0016,0.052133333},
                                      {4,"L\xE1""cteos Azucarados","Batidos, yogures azucarados",0.841395349,0.032483721,0.02067907,0.131274419},
                                      {5,"Postres L\xE1""cteos","Arroz con leche, flan, natillas",1.130612245,0.036653061,0.026938776,0.184489796},
                                      {6,"Frutas Frescas, Desecadas y Zumos","Todo tipo de frutas (naranjas, manzanas, fresas, etc.)",0.455913978,0.00719086,0.001747312,0.102822581},
                                      {7,"Hortalizas y Verduras (Crudas)","Todo tipo de verduras (lechugas, tomates, setas, etc.)",0.224860335,0.01673743,0.003128492,0.035195531},
                                      {8,"Cereales y Tub\xE9""rculos (Crudos)","Arroz, pasta, avena, cereales de desayuno, pan",2.095873684,0.063073684,0.015621053,0.4256},
                                      {9,"Legumbres (Crudas)","Alubias, garbanzos, lentejas",3.01,0.219,0.03,0.465},
                                      {10,"Reposter\xED""a, Pasteler\xED""a y Otros","Boller\xED""a, galletas, magdalenas, muesli",4.193949153,0.071186441,0.211423729,0.501864407},
                                      {11,"Grasas Monoinsaturadas","Aceite de oliva, aceitunas, aguacates, almendras, avellanas, pistachos",6.517826087,0.053478261,0.679565217,0.045652174},
                                      {12,"Grasas Poliinsaturadas","Aceite de girasol, aceite de soja, nueces, etc.",0.87375,0.0110625,0.0879375,0.00975},
                                      {13,"Grasas Saturadas","Coco, mantequilla, nata",3.625396825,0.029365079,0.367460317,0.049206349},
                                      {14,"Alimentos muy grasos (mezclas)","Manteca de cerdo, margarina, tocino",6.08,0.0272,0.6624,0.0032},
                                      {15,"Az\xFA""cares y Dulces","Az\xFA""car blanco o moreno, cacao en polvo, caramelos, miel",3.491368421,0.021052632,0.018526316,0.810947368},
                                      {16,"Prote\xED""nas muy magras","Pavo, pollo, entrecot de ternera, at\xFA""n conserva, jam\xF3""n cocido, bacalao",0.368903088,0.073801917,0.006709265,0.002555911},
                                      {17,"Prote\xED""nas magras","Cerdo lomo, conejo, costilla ternera, bistec, jam\xF3""n curado",0.73488,0.11456,0.03024,0.00096},
                                      {18,"Prote\xED""nas semigrasas","Cerdo chuletas con grasa, conservas en aceite, jam\xF3""n curado con grasa, huevo",1.706813187,0.153186813,0.113186813,0.008351648},
                                      {19,"Prote\xED""nas con grasas","Chorizo, fuet, salchichas, queso graso",3.601914894,0.222765957,0.299361702,0.004468085},
                                      {20,"Prote\xED""nas con mucha grasa","Carne picada sazonada, panceta, morcilla, mortadela, pat\xE9"", salami",3.419606061,0.145090909,0.308212121,0.016333333},
                                      {27,"Hortalizas y Verduras (Cocidas)","Todo tipo de verduras (lechugas, tomates, setas, etc.)",0.237068966,0.017646177,0.003298351,0.037106447},
                                      {28,"Cereales y Tub\xE9""rculos (Cocidos)","Arroz, pasta, avena, cereales de desayuno, pan",1.170534979,0.035226337,0.00872428,0.237695473},
                                      {29,"Legumbres (Cocidas)","Alubias, garbanzos, lentejas",1.119421488,0.081446281,0.011157025,0.172933884},
                                      {36,"Prote\xED""nas muy magras","Pavo, pollo, entrecot de ternera, at\xFA""n conserva, jam\xF3""n cocido, bacalao",0.836939597,0.167436242,0.015221477,0.005798658},
                                      {37,"Prote\xED""nas magras","Cerdo lomo, conejo, costilla ternera, bistec, jam\xF3""n curado",1.321273973,0.205972603,0.054369863,0.001726027},
                                      {38,"Prote\xED""nas semigrasas","Cerdo chuletas con grasa, conservas en aceite, jam\xF3""n curado con grasa, huevo",1.897038168,0.170259542,0.125801527,0.009282443}
                                    };

Grupo grupoEscogido;
Grupo grupoAnterior;


/*---------------------------------------------------------------------------------------------------------
   getGrupoAlimentos(): Obtener grupo de alimentos seleccionado (checkAllButtons() en BUTTONS.h)
----------------------------------------------------------------------------------------------------------*/
void getGrupoAlimentos(int id){       // 'id' pasado es buttonGrande 
    int posGrupo;
    for(int i = 0; i < NUM_GRUPOS; i++){
        if(gruposAlimentos[i].ID_grupo == id){ 
            posGrupo = i;
            break;
        }
    }
    Serial.print(F("\nPosicion en vector grupo: ")); Serial.println(posGrupo);
    grupoAnterior = grupoEscogido;
    grupoEscogido = gruposAlimentos[posGrupo]; 
}






#endif
