/** 
 * @file Files.h
 * @brief Contiene los nombres de los ficheros guardados en la SD y utilizados para mostrar imágenes o guardar información.
 *
 * @author Irene Casares Rodríguez
 * @date 17/07/23
 * @version 1.0
 *  
 */

#ifndef FILES_H
#define FILES_H


/*  ------ SD ------ */
// LIBRERÍA SD LIMITA LA LONGITUD DEL NOMBRE DEL FICHERO A 8 CARACTERES Y 3 PARA EXTENSION
// Los 8 caracteres no incluyen el path hasta el fichero

// --- FICHERO GUARDAR INFO LOCAL ---
char    fileCSV[30] = "data/data-sc.csv"; ///< Nombre del archivo para guardar las comidas realizadas
//char    fileCSV[30] = "data/daTest.csv"; // Probando el borrado del csv


// --- FICHERO GUARDAR INFO ESP32 ---
char    fileTXT[30] = "data/data-esp.txt"; ///< Nombre del archivo para guardar las comidas realizadas con pares <grupo>,<peso>


// --- IMAGENES RELOJ ARENA ---
char    fileReloj1[30]    = "bin/carga/reloj1.bin";
char    fileReloj2[30]    = "bin/carga/reloj2.bin";
char    fileReloj3[30]    = "bin/carga/reloj3.bin";
char    fileReloj4[30]    = "bin/carga/reloj4.bin";
char    fileRelGir1[30]   = "bin/carga/rel_gir1.bin";
char    fileRelGir2[30]   = "bin/carga/rel_gir2.bin";
char    fileRelGir3[30]   = "bin/carga/rel_gir3.bin";
char    fileRelGir4[30]   = "bin/carga/rel_gir4.bin";
char    fileRelGir5[30]   = "bin/carga/rel_gir5.bin";
char    fileRelGir6[30]   = "bin/carga/rel_gir6.bin";

// --- IMAGENES LETRAS LOGO ARRANQUE ---
char    fileS[30]     = "bin/arranque/s.bin";
char    fileM[30]     = "bin/arranque/m.bin";
char    fileA[30]     = "bin/arranque/a.bin";
char    fileR[30]     = "bin/arranque/r.bin";
char    fileT[30]     = "bin/arranque/t.bin";
char    fileC[30]     = "bin/arranque/c.bin";
char    fileL[30]     = "bin/arranque/l.bin";
char    fileO[30]     = "bin/arranque/o.bin";
char    fileH[30]     = "bin/arranque/h.bin";
char    fileLogo[30]  = "bin/arranque/log.bin";

// --- IMAGENES PANTALLA INICIAL ---
char    fileBrain1[30]      = "bin/inicial/brain1.bin";
char    fileBrain2Green[30] = "bin/inicial/brain2_g.bin";

// --- IMAGENES MANOS ---
char    fileManoWhiteIcon[30]   = "bin/manos/manowppt.bin";
char    fileManoGreenIcon[30]   = "bin/manos/manogppt.bin";

// --- IMAGENES GRUPOS ---
char    fileGrupo1[30]      = "bin/grupo/grupo1.bin";
char    fileGrupo2[30]      = "bin/grupo/grupo2.bin";
char    fileGrupo3[30]      = "bin/grupo/grupo3.bin";
char    fileGrupo4[30]      = "bin/grupo/grupo4.bin";

// --- IMAGENES BOTONES ---
char    fileBotonAnadir[30]     = "bin/botones/anadir.bin";
char    fileBotonEliminar[30]   = "bin/botones/borrar.bin";
char    fileBotonGuardar[30]    = "bin/botones/guardar.bin";
char    fileCocinadoGrande[30]  = "bin/botones/coci_gra.bin";
char    fileCrudoGrande[30]     = "bin/botones/cru_gra.bin";

// --- IMAGEN BASCULA ---
char   fileScale[30]  = "bin/alimento/scale_g.bin";


// --- IMAGENES DASHBOARD ---
char    fileCocinadoPeq[30]     = "bin/dash/coci_peq.bin";
char    fileCrudoPeq[30]        = "bin/dash/cru_peq.bin";
char    fileKcal[30]            = "bin/dash/kcal.bin"; // kcal_20

// --- IMAGENES ERROR ---
char    fileCruz[30]          = "bin/error/cruz.bin";

// --- IMAGENES AVISO ---
char    fileAvisoYellow[30]   = "bin/aviso/aviso_y.bin";



#endif
