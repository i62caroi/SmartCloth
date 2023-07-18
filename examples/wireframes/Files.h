/** 
 * @file Files.h
 * @brief Contiene los nombres de los ficheros guardados en la SD y utilizados para mostrar imágenes o guardar información.
 *
 * @author Irene Casares Rodríguez
 * @date 21/06/23
 * @version 1.0
 *  
 */

#ifndef FILES_H
#define FILES_H


/*  ------ SD ------ */
// LIBRERÍA SD LIMITA LA LONGITUD DEL NOMBRE DEL FICHERO A 8 CARACTERES Y 3 PARA EXTENSION
// Los 8 caracteres no incluyen el path hasta el fichero

// --- FICHERO GUARDAR INFO ---
char    fileCSV[30] = "data/data-SC.csv"; ///< Nombre del archivo para guardar las comidas realizadas

// --- IMAGENES RELOJ ARENA ---
char    fileReloj1[30]    = "bin/carga/reloj1.bin";
char    fileReloj2[30]    = "bin/carga/reloj2.bin";
char    fileReloj3[30]    = "bin/carga/reloj3.bin";
char    fileReloj4[30]    = "bin/carga/reloj4.bin";
char    fileRelGir1[30]   = "bin/carga/relGir1.bin";
char    fileRelGir2[30]   = "bin/carga/relGir2.bin";
char    fileRelGir3[30]   = "bin/carga/relGir3.bin";
char    fileRelGir4[30]   = "bin/carga/relGir4.bin";
char    fileRelGir5[30]   = "bin/carga/relGir5.bin";
char    fileRelGir6[30]   = "bin/carga/relGir6.bin";

// --- IMAGENES LETRAS LOGO ARRANQUE ---
char    fileS[30]     = "bin/arranque/S.bin";
char    fileM[30]     = "bin/arranque/M.bin";
char    fileA[30]     = "bin/arranque/A.bin";
char    fileR[30]     = "bin/arranque/R.bin";
char    fileT[30]     = "bin/arranque/T.bin";
char    fileC[30]     = "bin/arranque/C.bin";
char    fileL[30]     = "bin/arranque/L.bin";
char    fileO[30]     = "bin/arranque/O.bin";
char    fileH[30]     = "bin/arranque/H.bin";
char    fileLogo[30]  = "bin/arranque/Log.bin";

// --- IMAGENES PANTALLA INICIAL ---
char    fileBrain1[30]      = "bin/inicial/brain1.bin";
char    fileBrain2Green[30] = "bin/inicial/brain2G.bin";

// --- IMAGENES MANOS ---
//char    fileManoWhite[30]       = "bin/manos/manoW.bin";
char    fileManoWhiteIcon[30]   = "bin/manos/manoWppt.bin";
char    fileManoGreenIcon[30]   = "bin/manos/manoGppt.bin";

// --- IMAGENES GRUPOS Y MANO ---
char    fileGrupo1[30]      = "bin/grupo/grupo1.bin";
char    fileGrupo2[30]      = "bin/grupo/grupo2.bin";
char    fileGrupo3[30]      = "bin/grupo/grupo3.bin";
char    fileGrupo4[30]      = "bin/grupo/grupo4.bin";
char    fileManoGreen[30]   = "bin/grupo/manoG.bin";


// --- IMAGENES BOTONES ---
char    fileBotonAnadir[30]     = "bin/botones/anadir.bin";
char    fileBotonEliminar[30]   = "bin/botones/borrar.bin";
char    fileBotonGuardar[30]    = "bin/botones/guardar.bin";
char    fileCocinadoGrande[30]  = "bin/botones/cociGra.bin";
char    fileCrudoGrande[30]     = "bin/botones/crudoGra.bin";
//char    fileManoYellow[30]      = "bin/botones/manoY.bin"; // Usamos la manoW


// --- IMAGEN BASCULA ---
char   fileScale[30]  = "bin/alimento/scaleG.bin";


// --- IMAGENES DASHBOARD ---
char    fileCocinadoPeq[30]     = "bin/dash/cociPeq.bin";
char    fileCrudoPeq[30]        = "bin/dash/crudoPeq.bin";
char    fileKcal[30]          = "bin/dash/kcal.bin"; // kcal_20

// --- IMAGENES ERROR ---
char    fileCruz[30]          = "bin/error/cruz.bin";

// --- IMAGENES AVISO ---
char    fileAvisoYellow[30]   = "bin/aviso/avisoY.bin";
//char    fileAvisoDarkOrange[30]   = "bin/aviso/avisoO.bin";



#endif
