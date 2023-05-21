
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



-------- MEMORIA ROM DE FUENTES EXTERNAS ----------------------------------------------------------------------------------

  	* Memoria ROM de fuentes externas GT30L16U2W de Genitop. Soporta las siguientes fuentes, según el datasheet de RA8876 [3]:
	      - 15x16 dost Unicode font
	      - 8x16 dots Special font
	      - 8x16 dots ASCII font
	      - 16 dots ASCII font (Arial)
	      - 16 dots ASCII font (Times New Roman)
	      - 8x16 dots Latin/Greek/Cyril font
	      - 16 dots Latin/Greek/Cyril/Arabia/Arabia-extendable font (Arial)

      La ROM GT20L24F6Y sí soporta 8x16 ISO8859-1 (Latin), que es la codificación que usamos generalmente.

---------------------------------------------------------------------------------------------------------------------------








[1] Pantalla ER-TFTM070-6: https://www.buydisplay.com/spi-7-inch-tft-lcd-dislay-module-1024x600-ra8876-optl-touch-screen-panel

[2] SDRAM W9812G6KH-6: https://www.mouser.es/datasheet/2/949/w9812g6kh_a05_20170222-1489666.pdf

[3] Chip RA8876: https://www.raio.com.tw/data_raio/Datasheet/RA887677/RA8876_Brief_DS_V11_Eng.pdf



