/** 
 * @file Files.h
 * @brief Contiene los nombres de los ficheros guardados en la SD y utilizados para mostrar imágenes o guardar información.
 *
 * @author Irene Casares Rodríguez
 * @date 25/07/24
 * @version 1.0
 *  
 */

#ifndef FILES_H
#define FILES_H


/*  ------ SD ------ */
// LIBRERÍA SD LIMITA LA LONGITUD DEL NOMBRE DEL FICHERO A 8 CARACTERES Y 3 PARA EXTENSION
// Los 8 caracteres no incluyen el path hasta el fichero

// --- FICHERO GUARDAR INFO LOCAL ---
char    fileCSV[30] = "data/data-sc.csv";       // Archivo CSV para guardar las comidas realizadas (el acumulado se obtiene de aquí)


// --- FICHERO GUARDAR INFO ESP32 ---
char    fileTXT[30] = "data/data-esp.txt";      // Fichero TXT para guardar las comidas realizadas y que están pendientes de subir a la database
char    auxFileTXT[20] = "data/aux_file.txt";    // Fichero TXT auxiliar para guardar las comidas no subidas a la database durante la sincronización

// --- IMAGENES RELOJ ARENA ---
char    fileReloj1[30]    = "bin/carga/reloj1.bin";             // 65x103
char    fileReloj2[30]    = "bin/carga/reloj2.bin";             // 65x103
char    fileReloj3[30]    = "bin/carga/reloj3.bin";             // 65x103
char    fileReloj4[30]    = "bin/carga/reloj4.bin";             // 65x103
char    fileRelGir1[30]   = "bin/carga/rel_gir1.bin";           // 95x115
char    fileRelGir2[30]   = "bin/carga/rel_gir2.bin";           // 112x112
char    fileRelGir3[30]   = "bin/carga/rel_gir3.bin";           // 113x94
char    fileRelGir4[30]   = "bin/carga/rel_gir4.bin";           // 100x65
char    fileRelGir5[30]   = "bin/carga/rel_gir5.bin";           // 113x94
char    fileRelGir6[30]   = "bin/carga/rel_gir6.bin";           // 99x113

// --- IMAGENES LETRAS LOGO ARRANQUE ---
char    fileS[30]     = "bin/arranque/s.bin";                   // 95x159
char    fileM[30]     = "bin/arranque/m.bin";                   // 104x154
char    fileA[30]     = "bin/arranque/a.bin";                   // 104x159
char    fileR[30]     = "bin/arranque/r.bin";                   // 85x159
char    fileT[30]     = "bin/arranque/t.bin";                   // 104x159
char    fileC[30]     = "bin/arranque/c.bin";                   // 85x159
char    fileL[30]     = "bin/arranque/l.bin";                   // 85x159
char    fileO[30]     = "bin/arranque/o.bin";                   // 85x159
char    fileH[30]     = "bin/arranque/h.bin";                   // 85x159
char    fileLogo[30]  = "bin/arranque/log.bin";                 // 162x169

// --- IMAGENES PANTALLA INICIAL ---
char    fileBrain1[30]      = "bin/inicial/brain1.bin";         // 120x108
char    fileBrain2Green[30] = "bin/inicial/brain2_g.bin";       // 120x108

// --- IMAGENES MANOS ---
char    fileManoWhiteIcon[30]   = "bin/manos/manowppt.bin";     // 120x128
char    fileManoGreenIcon[30]   = "bin/manos/manogppt.bin";     // 120x128

// --- IMAGENES GRUPOS ---
char    fileGrupo1[30]      = "bin/grupo/grupo1.bin";           // 130x125
char    fileGrupo2[30]      = "bin/grupo/grupo2.bin";           // 130x125
char    fileGrupo3[30]      = "bin/grupo/grupo3.bin";           // 130x125
char    fileGrupo4[30]      = "bin/grupo/grupo4.bin";           // 130x125

// --- IMAGENES BOTONES ---
char    fileBotonAnadir[30]     = "bin/botones/anadir.bin";     // 172x130
char    fileBotonEliminar[30]   = "bin/botones/borrar.bin";     // 172x130
char    fileBotonGuardar[30]    = "bin/botones/guardar.bin";    // 172x130
char    fileCocinadoGrande[30]  = "bin/botones/coci_gra.bin";   // 177x160
char    fileCrudoGrande[30]     = "bin/botones/cru_gra.bin";    // 177x160

// --- IMAGEN BASCULA ---
char   fileScale[30]  = "bin/alimento/scale_g.bin";             // 150x150


// --- IMAGENES DASHBOARD ---
char    fileCocinadoPeq[30]     = "bin/dash/coci_peq.bin";      // 47x42
char    fileCrudoPeq[30]        = "bin/dash/cru_peq.bin";       // 47x42
char    fileKcal[30]            = "bin/dash/kcal.bin";          // 60x65

// --- IMAGENES ERROR ---
char    fileCruz[30]          = "bin/error/cruz.bin";           // 114x127

// --- IMAGENES AVISO ---
char    fileAvisoYellow[30]   = "bin/aviso/aviso_y.bin";        // 135x113
char    fileAvisoNew[30]      = "bin/aviso/aviso.bin";          // 143x126. Usado para "PRODUCTO NO ENCONTRADO"



// --- IMAGENES SINCRONIZACION ---
char    fileSincronizando[40] = "bin/sync/sync.bin";            // 188x123
char    fileSMSincronizado[40]  = "bin/sync/sync_ok.bin";       // 220x136



// --- IMAGENES BARCODE ------------
char    fileScanBarcode[30]    = "bin/barcode/scan.bin";        // 171x128
char    fileSearchProduct[30]  = "bin/barcode/lupa.bin";        // 82x130
char    fileProductFound[30]   = "bin/barcode/pr_found.bin";    // 297x104
// "PRODUCTO NO ENCONTRADO" se hace con imagen de aviso



// --- IMAGENES GUARDAR COMIDA ----
char    fileGuardando[30] = "bin/guardar/disquete.bin";         // 91x98
char    fileGuardandoWifi[30] = "bin/guardar/conexion.bin";     // 42x67
char    fileGuardandoNoWifi[30] = "bin/guardar/no_conex.bin";   // 54x85







#endif
