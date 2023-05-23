
Pantalla modelo: ER-TFTM070-6 de BuyDisplay [1] (SPI 7"TFT LCD Dislay 1024x600 OPTL Capacitive Touch Screen)

	------ INFO BASE --------------------------
	* 1024x600 (7") TFT LCD
	* Pantalla táctil capacitiva
	* Comunicación SPI
	-------------------------------------------

	------ FLASH ------------------------------
    * No tiene memoria Flash externa!!!
    -------------------------------------------



------ CHIP GRÁFICO -------------------------------------------------------------------------------------------------------

  	* Chip gráfico RA8876L4N. Este es una verión más reciente y mejorada del controlador RA8876.
  	  Algunas de las mejoras incluyen:

  		1 - Mejor rendimiento: El RA8876L4N puede ofrecer una mayor velocidad de actualización de 
  			la pantalla y un rendimiento mejorado en términos de respuesta y procesamiento de gráficos.

		2 - Mayor resolución: El RA8876L4N es compatible con una resolución de pantalla más alta, 
			lo que permite una mayor densidad de píxeles y una calidad de imagen mejorada.

		3 - Funcionalidad adicional: El RA8876L4N puede ofrecer nuevas características y funcionalidades, 
			como la capacidad de soportar más colores, una mayor cantidad de capas gráficas superpuestas y 
			mejoras en la administración de energía.

---------------------------------------------------------------------------------------------------------------------------



-------- MEMORIA SDRAM ----------------------------------------------------------------------------------------------------

  	* Memoria SDRAM tipo W9812G6KH-6 [2]:
	  		- 2 M words x 4 banks x 16 bits (128Mbits)
	  		- 166 MHz de frecuencia de reloj
	  		- modo auto-refresh
	  		- 64 ms de tiempo de refresh
	  		- CAS Latency: 2 y 3
	  		- Velocidad de acceso CL3 (3 ciclos de reloj)
	  		- Row address: A0-A11
	  		- Column address: A0-A8

  	  Esto se corresponde con los siguientes valores de Datasheet 8.1.2 SDRAM Connection del chip RA8876:
        Density: 128Mb (4 banks) 
        Addressing => row: [A0-A11]
                  => column: [A0-A8]  
  
      En la ra8876SdramInitial() de RA8876_Lite no aparece esta SDRAM como opción, pero sí la W9812G6JH. La principal 
      diferencia entre la W9812G6JH y la W9812G6KH-6 es la velocidad de acceso y el tiempo de ciclo de reloj, siendo 
      la W9812G6JH más rápida en términos de acceso y tiempo de ciclo de reloj.

---------------------------------------------------------------------------------------------------------------------------


-------------- FUENTES INTERNAS -----------------------------------------------------------------
  * El chip RA8876 tiene una fuente interna de ancho fijo disponible 
    en tres tamaños:
      RA8876_FONT_SIZE_16 => 8x16
      RA8876_FONT_SIZE_24 => 12x24
      RA8876_FONT_SIZE_32 => 16x32
    
    Estos tamaños se pueden variar escalando hasta 4 veces la anchura (W) y la altura (H),
    independientemente (p.ej. W x2 y H x3):
      Escala x1 (sin escala) => RA8876_TEXT_W_SCALE_X1 | RA8876_TEXT_H_SCALE_X1
      Escala x2              => RA8876_TEXT_W_SCALE_X2 | RA8876_TEXT_H_SCALE_X2
      Escala x3              => RA8876_TEXT_W_SCALE_X3 | RA8876_TEXT_H_SCALE_X3
      Escala x4              => RA8876_TEXT_W_SCALE_X4 | RA8876_TEXT_H_SCALE_X4
------------------------------------------------------------------------------------------------


-------- MEMORIA ROM DE FUENTES EXTERNAS ----------------------------------------------------------------------------------

  	* Memoria ROM de fuentes externas GT30L16U2W de Genitop. El número de modelo 
    sigue el siguiente formato: GTppLhhcba

    	· pp => tipo de empaquetado (20 = SOT23-6, 21 = SOP-8, 30 = SOP-8-B)
		· L  => bajo voltaje (3.3V)
		· hh => altura de la fuente más alta incluida 
		· c  => set de caracteres más prominente (S = GB2312, M = GB18030, U = Unicode, 
					T = BIG5, F = Foreign Unicode and ISO-8859)
		· b  => número de bloques de librerías 
		· a  => character arrangement

	  Por tanto, la GT30L16U2W trabaja a 3.3V, permite una altura máxima de 16 pixeles y 
	  el set de caracteres más prominente es UNICODE, entre otras características.

      --- FUENTES QUE EL RA8876 PUEDE USAR DESDE ESTE CHIP -----

      Nota: la columna "Range" solo proporciona el carácter mínimo y máximo admitido. 
      No todos los caracteres dentro del rango indicado estarán necesariamente disponibles.

      -----------------------------------------------------------------------
	  |	 Note    |  Family	|  Width   |  Height |	Encoding  | Range (hex) |
	  -----------------------------------------------------------------------
	  |	   -     |  FIXED	|    8	   |	16	 |	 ASCII	  |  20-7F 		|
	  |	   -	 |  FIXED	|   15	   |    16	 |	UNICODE	  |   ?			|
	  |	Latin	 |  FIXED	|    8	   |    16	 |	UNICODE	  | 00A0-0217	|
	  |	Greek	 |  FIXED	|    8	   |    16	 |	UNICODE	  | 0370-03CF	|
	  |	Cyrillic |  FIXED	|    8	   |    16	 |	UNICODE	  | 0400-04F9	|
	  |	   -	 |  ARIAL	|  varía   |	16	 |	ASCII	  | 20-7F		|
	  |	Latin	 |  ARIAL	|  varía   |    16	 |	UNICODE	  | 00A0-0217	|
	  |	Greek	 |  ARIAL	|  varía   |    16	 |	UNICODE	  | 0370-03CF	|
	  |	Cyrillic |  ARIAL	|  varía   |    16	 |	UNICODE	  | 0400-04F9	|
	  |	Arabian	 |  ARIAL	|  varía   |    16	 |	UNICODE	  | 0600-06F9	|
	  |	Arabian  |  ARIAL	|  varía   |    16	 |	  ?		  | B000-B1F1	|
	  |    -     |  TIMES	|  varía   |    16	 |	ASCII	  | 20-7F		|
	  |	Symbol   |    ?	    |   8	   |	16	 |	GB2312	  | ACA1-ACDF	|
	  -----------------------------------------------------------------------

	  Info obtenida de:
	  - https://github.com/xlatb/ra8876/wiki/Font-ROM-GT30L16U2W
	  - [Datasheet RA8876 14.2.2 GT30L16U2W] https://www.raio.com.tw/data_raio/Datasheet/RA887677/RA8876_Brief_DS_V11_Eng.pdf


	  ------------- CONCLUSIÓN USO DE FUENTES INTERNAS/EXTERNAS --------------------------------------
    	>> Interna -> Texto de 8x16/12x24/16x32, escalándolo hasta 4 veces.
    	>> Externa -> Texto de 8x16/15x16. Habrá que ver si se escala también.
  	  ------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------------------








[1] Pantalla ER-TFTM070-6: https://www.buydisplay.com/spi-7-inch-tft-lcd-dislay-module-1024x600-ra8876-optl-touch-screen-panel

[2] SDRAM W9812G6KH-6: https://www.mouser.es/datasheet/2/949/w9812g6kh_a05_20170222-1489666.pdf

[3] Chip RA8876: https://www.raio.com.tw/data_raio/Datasheet/RA887677/RA8876_Brief_DS_V11_Eng.pdf



