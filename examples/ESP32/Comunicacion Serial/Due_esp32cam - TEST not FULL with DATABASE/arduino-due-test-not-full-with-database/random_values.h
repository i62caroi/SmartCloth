#ifndef RANDOM_VALUES_H
#define RANDOM_VALUES_H



String generarValores_y_generarQuery(){
    // --- GENERAR VALORES ALEATORIOS ---
    // Generar valores aleatorios para guardar en la database
    float carb = random(5,30) + 0.5; // + 0.5 para ver que se muestran decimales, porque con .0 se trunca el valor
    float carb_R = round(carb/10);
    float lip = random(5,30) + 0.5;
    float lip_R = round(lip/10);
    float prot = random(5,30) + 0.5;
    float prot_R = round(prot/10);
    float kcal = (carb * 4) + (lip * 9) + (prot * 4);
    float peso = random(50, 150) + 0.5;

    // --- FORMAR CADENA A ENVIAR ---
    String comidaValues = "&carb=" + String(carb) + "&carb_R=" + String(carb_R)
                          + "&lip=" + String(lip) + "&lip_R=" + String(lip_R) 
                          + "&prot=" + String(prot) + "&prot_R=" + String(prot_R) 
                          + "&kcal=" + String(kcal) + "&peso=" + String(peso) + "";

    // No incluimos el tiempo del RTC porque ya se obtiene en el servidor
    String msg = "SAVE:" + comidaValues;

    return msg;

}




#endif