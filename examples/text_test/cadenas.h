#ifndef CADENAS_H
#define CADENAS_H

#include <string>

String ejemplos[8] = {
                    "Lácteos LÁCTEOS", 
                    "Tubérculos TUBÉRCULOS", 
                    "Líquidos LÍQUIDOS", 
                    "Jamón JAMÓN", 
                    "Atún ATÚN", 
                    "Ñoquis ÑOQUIS", 
                    "¿Cómo estás?", 
                    "¡Hola!"};

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
 * @brief Convierte los caracteres especiales en una cadena de texto.
 * 
 * Esta función reemplaza los caracteres especiales en una cadena de texto con sus correspondientes
 * códigos de caracteres en formato hexadecimal. Los caracteres especiales que se reemplazan son:
 * 
 * - Minúsculas: vocales acentuadas (á, é, í, ó, ú) y la letra 'ñ'.
 * - Mayúsculas: vocales acentuadas (Á, É, Í, Ó, Ú) y la letra 'Ñ'.
 * - Signos de interrogación (¿), exclamación (¡), escape (/) y puntos suspensivos (...).
 * 
 * @param input La cadena de texto en la que se van a reemplazar los caracteres especiales.
 * @return La cadena de texto con los caracteres especiales reemplazados.
 */
/*-----------------------------------------------------------------------------*/
/*String convertSpecialCharacters_copy(String input) 
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

    return input;
}*/

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


#endif