

void actState_UPLOAD_DATA(){ 
    // En este caso sí podemos usar delay (espera bloqueante) porque no atenderemos a las acciones del usuario,
    // por lo que no hay que estar pendiente de interrupciones. 

    bool showingScreen = false; // Se muestra pantalla en caso de que haya algo que subir y se pueda subir (WiFi)

    if(!doneState){
        // -----  INFORMACIÓN MOSTRADA  ------------------------
        #if defined(SM_DEBUG)
        SerialPC.println(F("\nDATA EN EL TXT")); 
        #endif

        // SI DESCOMENTO ESTA LÍNEA, SIEMPRE QUE ENCIENDAN SM Y TENGAN ALGO QUE SUBIR, SALDRÁ ESTA PANTALLA.
        // SI ESTÁN 2 SEMANAS SIN QUE VAYA ALGUIEN A PONERLE EL WIFI, SIEMPRE LES SALDRÁ LA PANTALLA.
        // CREO QUE ES MEJOR HACERLO EN UN SEGUNDO PLANO Y SOLO MOSTRAR INFO DE LA SINCRONIZACIÓN SI OCURRE,
        // OSEA SI HAY ALGO QUE SUBIR, HAY WIFI Y SE SUBE.

        //showDataToUpload(DATA_TO_UPLOAD);  // Mostrar aviso de data en TXT para sincronizar con web
        //delay(3000); // 3 segundos para leer mensaje

        // Podemos hacer espera bloqueante porque, en este estado, no queremos atender a interrupciones del usuario
        // ----- FIN INFORMACIÓN MOSTRADA -----------------------

        if (!checkWifiConnection()) { // No hay WiFi (preguntado al esp32 y responde NO-WIFI)
            // -----  INFORMACIÓN MOSTRADA  ------------------------
            #if defined(SM_DEBUG)
            SerialPC.println(F("\nPaso a Init porque no hay WiFi..."));
            #endif

            // CREO QUE ES MEJOR HACERLO EN UN SEGUNDO PLANO Y SOLO MOSTRAR INFO DE LA SINCRONIZACIÓN SI OCURRE,
            // OSEA SI HAY ALGO QUE SUBIR, HAY WIFI Y SE SUBE.

            //showDataToUpload(NO_INTERNET_CONECTION); // No hay conexión, no se puede sincronizar SM con web

            // El delay y el GO_TO_INIT se hace al final para todos los casos
            // ----- FIN INFORMACIÓN MOSTRADA -----------------------
        }
        else{ // Hay WiFi 
            // -----  INFORMACIÓN MOSTRADA  -------------------------
            showDataToUpload(UPLOADING_DATA); // Sincronizando data del SM con web
            //delay(2000);
            // ----- FIN INFORMACIÓN MOSTRADA -----------------------

            // -----  ENVIAR INFORMACION AL ESP32  ------------------
            if(prepareSaving()){ // Indicar al ESP32 que se le va a enviar info (SAVE) y esperar su respuesta (WAITING-FOR-DATA)
                sendFileToESP32(); // Enviar el fichero TXT línea a línea al ESP32 y terminar con FIN-TRANSMISION            
                handleResponseFromESP32(SHOW_SCREEN_UPLOAD_DATA); // Actuar según respuesta y mostrar mensaje acorde
                // En este caso, ignoramos el valor devuelto de handleResponseFromESP32() porque no lo necesitamos
            }
            else{ // El ESP32 no respondió en 30 segundos. Actuamos como si no hubiera WiFi
                showDataToUpload(NO_INTERNET_CONECTION); // No hay conexión, no se puede sincronizar SM con web
                // El delay y el GO_TO_INIT se hace al final para todos los casos
            }
            showingScreen = true;
            // ------------------------------------------------------
        }

        // Pasar a Init para todos los casos:
        // - No había WiFi (NO_INTERNET_CONECTION)
        // - Había WiFi y se subió OK (UPLOADED_DATA)
        // - Había WiFi pero falló la subida (HTTP_ERROR). Esto se mira en handleResponseFromESP32()
        if(showingScreen) delay(3000);
        addEventToBuffer(GO_TO_INIT);
        flagEvent = true;
        

        doneState = true;               // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                        // cada vez que se entre a esta función debido al loop de Arduino.
    }
    
}

