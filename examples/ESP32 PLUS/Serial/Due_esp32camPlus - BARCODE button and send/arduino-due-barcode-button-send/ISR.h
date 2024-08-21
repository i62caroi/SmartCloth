/**
 * @file ISR.h
 * @brief Definiciones de las Rutinas de Servicio de Interrupción (ISR)
 *
 * @author Irene Casares Rodríguez
 * @date 07/05/24
 * @version 1.0
 *
 */

#ifndef ISR_H
#define ISR_H

#define BARCODE_DEBOUNCE_TIME 500


const byte intPinBarcode = 53;
volatile bool pulsandoBarcode = false;

/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de barcode
 */
/*-----------------------------------------------------------------------------*/
void ISR_barcode(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > BARCODE_DEBOUNCE_TIME) pulsandoBarcode = true;
    last_interrupt_time = interrupt_time;
}






#endif
