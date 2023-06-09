/*
File Name : RA8876_v2.cpp                                   
Author    : Irene Casares Rodríguez                          
Edit Date : 23/05/2023
Version   : v2.0

    Version v1.0 obtenida de la librería RA8876 de https://github.com/xlatb/ra8876/tree/master/src

    Se ha completado con funciones de RA8876_Lite de Raio, adaptándolas con las
    funciones de la librería base RA8876.

    Guía de RA8876_Lite: https://www.raio.com.tw/data_raio/RA887677/Arduino/RA8876_Lite_UserGuide_v1.0_Eng.pdf

    Raio RA8876 Technical documents: https://www.raio.com.tw/en/download_center.html 
*/

/* ************************************************************ */

/* ************************************************************ */
#pragma GCC diagnostic warning "-Wall"
#include "RA8876_v2.h"

/* ************************************************************ */
/* Datasheet 8.1.2 SDRAM Connection: 
   Nuestra pantalla tiene integrada una SDRAM tipo W9812G6KH-6:
	  		- 2 M words x 4 banks x 16 bits (128Mbits)
	  		- 166 MHz de frecuencia de reloj
	  		- modo auto-refresh
	  		- 64 ms de tiempo de refresh
	  		- CAS Latency: 2 y 3
	  		- Velocidad de acceso CL3 (3 ciclos de reloj)
	  		- Row address: A0-A11
	  		- Column address: A0-A8
*/
SdramInfo defaultSdramInfo =
{
    166, // MHz (Prev => 120 MHz)
    3,   // CAS latency 3
    4,   // 4 banks
    12,  // 12-bit row addresses (A0-A11)
    9,   // 9-bit column addresses (A0-A8)
    64   // 64 millisecond refresh time
};
/* ************************************************************ */
DisplayInfo defaultDisplayInfo =
{
    1024,   // Display width
    600,    // Display height
    50000,  // Pixel clock in kHz

    160,    // Horizontal front porch
    160,    // Horizontal back porch
    70,     // HSYNC pulse width

    12,     // Vertical front porch
    23,     // Vertical back porch
    10      // VSYNC pulse width
};


//***********************+*******************************************************************************
//-------------------------------------------------------------------------------------------------------
//----------------------------------- PRIVATE FUNCTIONS -------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//***********************+*******************************************************************************


/* *************************************************************
    Trigger a hardware reset.
   ************************************************************* */
void RA8876::_hardReset(void)
{
    delay(5);
    digitalWrite(_resetPin, LOW);
    delay(5);
    digitalWrite(_resetPin, HIGH);
    delay(5);

    return;
}

/* *************************************************************
    Trigger a soft reset. Note that the data sheet section 19.2 
    says that this only resets the "internal state machine", 
    not any configuration registers.
   ************************************************************* */
void RA8876::_softReset(void)
{
    SPI.beginTransaction(_spiSettings);

    // Trigger soft reset
    _writeReg(RA8876_REG_SRR, 0x01);
    delay(5);

    /* --- Esta parte es checkIcReady() en RA8876_Lite ------ */
    // Wait for status register to show "normal operation".
    uint8_t status;
    for (int i = 0; i < 250; i++)
    {
      delay(1);
      
      if (((status = _readStatus()) & 0x02) == 0)
        break;
    }

    SPI.endTransaction();

    return;
}

/* *************************************************************
    lcdRegWrite() en RA8876_Lite
   ************************************************************* */
void RA8876::_writeCmd(uint8_t reg)
{
    digitalWrite(_csPin, LOW);
    SPI.transfer(RA8876_CMD_WRITE);
    SPI.transfer(reg);
    digitalWrite(_csPin, HIGH);
}

/* *************************************************************
    lcdDataWrite() en RA8876_Lite
   ************************************************************* */
void RA8876::_writeData(uint8_t data)
{
    digitalWrite(_csPin, LOW);
    SPI.transfer(RA8876_DATA_WRITE);
    SPI.transfer(data);
    digitalWrite(_csPin, HIGH);
}

/* *************************************************************
    Support SPI interface to write 16bpp data after ramAccessPrepare().
    
    Parecido a writeData(), pero para 16bbp.

    lcdDataWrite16bbp() en RA8876_Lite
   ************************************************************* */
void RA8876::_writeData16bits(uint16_t data) 
{
    // Parecido a _writeData() de RA8876_v2, pero para 16bbp
    digitalWrite(_csPin, LOW);
    SPI.transfer(RA8876_DATA_WRITE);
    SPI.transfer(data);
    SPI.transfer(data>>8);
    digitalWrite(_csPin, HIGH);
}

/* *************************************************************
    Support SPI interface to write 64bits data after ramAccessPrepare().
   ************************************************************* */
void RA8876::_writeData64bits(uint64_t data) 
{
    digitalWrite(_csPin, LOW);
    SPI.transfer(RA8876_DATA_WRITE);
    SPI.transfer(&data, 8);
    digitalWrite(_csPin, HIGH);
}


/* *************************************************************
    Support SPI interface to write 256bits data after ramAccessPrepare().
   ************************************************************* */
void RA8876::_writeData256bits(uint64_t *data) 
{
    digitalWrite(_csPin, LOW);
    SPI.transfer(RA8876_DATA_WRITE);
    SPI.transfer(&data[0], 8);
    SPI.transfer(&data[1], 8);
    SPI.transfer(&data[2], 8);
    SPI.transfer(&data[3], 8);
    digitalWrite(_csPin, HIGH);
}


/* *************************************************************
    lcdDataRead()  en RA8876_Lite
   ************************************************************* */
uint8_t RA8876::_readData(void)
{
    digitalWrite(_csPin, LOW);
    SPI.transfer(RA8876_DATA_READ);
    uint8_t x = SPI.transfer(0);
    digitalWrite(_csPin, HIGH);
    return x;
}

/* *************************************************************
    Reads the special status register.
    This register uses a special cycle type instead of having an 
    address like other registers.
    See data sheet section 19.1.

    lcdStatusRead() en RA8876_Lite
   ************************************************************* */
uint8_t RA8876::_readStatus(void)
{
    digitalWrite(_csPin, LOW);
    SPI.transfer(RA8876_STATUS_READ);
    uint8_t x = SPI.transfer(0);
    digitalWrite(_csPin, HIGH);
    return x;
}

/* *************************************************************
    lcdRegDataWrite() en RA8876_Lite
   ************************************************************* */
void RA8876::_writeReg(uint8_t reg, uint8_t data)
{
    _writeCmd(reg);
    _writeData(data);
}

/* *************************************************************
    Like writeReg(), but does two successive register writes 
    of a 16-bit value, low byte first.
   ************************************************************* */
void RA8876::_writeReg16(uint8_t reg, uint16_t data)
{
    _writeCmd(reg);
    _writeData(data & 0xFF);
    _writeCmd(reg + 1);
    _writeData(data >> 8);
}

/* *************************************************************
    Like writeReg(), but does four successive register writes 
    of a 32-bit value, low byte first.
   ************************************************************* */
void RA8876::_writeReg32(uint8_t reg, uint32_t data)
{
    _writeCmd(reg);
    _writeData(data & 0xFF);
    _writeCmd(reg + 1);
    _writeData((data >> 8) & 0xFF);
    _writeCmd(reg + 2);
    _writeData((data >> 16) & 0xFF);
    _writeCmd(reg + 3);
    _writeData(data >> 24);
}

/* *************************************************************
    lcdRegDataRead() en RA8876_Lite
   ************************************************************* */
uint8_t RA8876::_readReg(uint8_t reg)
{
    _writeCmd(reg);
    return _readData();
}

/* *************************************************************
    Like _readReg(), but does two successive register reads of 
    a 16-bit value, low byte first.
   ************************************************************* */
uint16_t RA8876::_readReg16(uint8_t reg)
{
    uint16_t v;

    _writeCmd(reg);
    v = _readData();
    _writeCmd(reg + 1);
    v |= _readData() << 8;

    return v;
}


//**************************************************************//
/*[Status Register] bit7  Host Memory Write FIFO full
0: Memory Write FIFO is not full.
1: Memory Write FIFO is full.
Only when Memory Write FIFO is not full, MPU may write another one pixel.*/ 
//**************************************************************//
void RA8876::_checkWriteFifoNotFull(void)
{  
    SPI.beginTransaction(_spiSettings);  
    for(uint16_t i=0;i<10000;i++) //Please according to your usage to modify i value.
    {
      if( (_readStatus()&0x80)==0 ){break;} //lcdStatusRead() en RA8876_Lite
    }
    SPI.endTransaction();
}

//**************************************************************//
/*[Status Register] bit6  Host Memory Write FIFO empty
0: Memory Write FIFO is not empty.
1: Memory Write FIFO is empty.
When Memory Write FIFO is empty, MPU may write 8bpp data 64
pixels, or 16bpp data 32 pixels, 24bpp data 16 pixels directly.*/
//**************************************************************//
void RA8876::_checkWriteFifoEmpty(void)
{
    SPI.beginTransaction(_spiSettings);
    for(uint16_t i=0;i<10000;i++)   //Please according to your usage to modify i value.
    {
      if( (_readStatus()&0x40)==0x40 ){break;} //lcdStatusRead() en RA8876_Lite
    }
    SPI.endTransaction();
}


/* *************************************************************
    Given a target frequency in kHz, finds PLL parameters k and n to reach as
    close as possible to the target frequency without exceeding it.
    The k parameter will be constrained to the range 1..kMax.
    Returns true iff PLL params were found, even if not an exact match.
   ************************************************************* */
bool RA8876::_calcPllParams(uint32_t targetFreq, int kMax, PllParams *pll)
{
    bool found = false;
    int foundk = 0, foundn = 0;
    uint32_t foundFreq = 0;
    uint32_t foundError = 0;  // Amount lower than requested frequency
    
    // k of 0 (i.e. 2 ** 0 = 1) is possible, but not sure if it's a good idea.
    for (int testk = 1; testk <= kMax; testk++)
    {
      if (_oscClock % (1 << testk))
        continue;  // Step size with this k would be fractional

      int testn = (targetFreq / (_oscClock / (1 << testk))) - 1;
      if ((testn < 1) || (testn > 63))
        continue;  // Param n out of range for this k
        
      // Fvco constraint found in data sheet section 6.1.2
      uint32_t fvco = _oscClock * (testn + 1);
      if ((fvco < 100000) && (fvco > 600000))
        continue;  // Fvco out of range
      
      // Found some usable params, but perhaps at a lower frequency than requested.
      uint32_t freq = (_oscClock * (testn + 1)) / (1 << testk);
      uint32_t error = targetFreq - freq;
      if ((!found) || (found && (foundError > error)))
      {
        found = true;
        foundk = testk;
        foundn = testn;
        foundFreq = freq;
        foundError = error;

        // No need to keep searching if the frequency match was exact
        if (foundError == 0)
          break;
      }
    }

    if (found)
    {
      pll->freq = foundFreq;
      pll->k    = foundk;
      pll->n    = foundn;
    }

    return found;
}

/* *************************************************************
    Calculates the clock frequencies and their PLL parameters.
   ************************************************************* */
bool RA8876::_calcClocks(void)
{
    // Data sheet section 5.2 gives max clocks:
    //  memClock : 166 MHz
    //  coreClock: 120 MHz (133MHz if not using internal font)
    //  scanClock: 100 MHz

    // Mem clock target is the same as SDRAM speed, but capped at 166 MHz
    uint32_t memClock = _sdramInfo->speed * 1000;
    if (memClock > 166000)
      memClock = 166000;

    if (!_calcPllParams(memClock, 3, &_memPll))
      return false;

    // Core clock target will be the same as the mem clock, but capped to
    //  120 MHz, because that is the max frequency if we want to use the
    //  internal font.
    uint32_t coreClock = _memPll.freq;
    if (coreClock > 120000)
      coreClock = 120000;

    if (!_calcPllParams(coreClock, 3, &_corePll))
      return false;

    // Scan clock target will be the display's dot clock, but capped at 100 MHz
    uint32_t scanClock = _displayInfo->dotClock;
    if (scanClock > 100000)
      scanClock = 100000;

    if (!_calcPllParams(scanClock, 7, &_scanPll))
      return false;

    _dumpClocks();

    // Data sheet section 6.1.1 rules:
    // 1. Core clock must be less than or equal to mem clock
    if (_corePll.freq > _memPll.freq)
      return false;

    // 2. Core clock must be greater than half mem clock
    if ((_corePll.freq * 2) <= _memPll.freq)
      return false;

    // 3. Core clock must be greater than (scan clock * 1.5)
    if (_corePll.freq <= (_scanPll.freq + (_scanPll.freq >> 1)))
      return false;
        
    return true;
}

/* *************************************************************
    Dump clock info to serial monitor.
   ************************************************************* */
void RA8876::_dumpClocks(void)
{
    #if defined(RA8876_DEBUG)
    Serial.println("\nMem\n---");
    Serial.print("Requested kHz: "); Serial.println(_sdramInfo->speed * 1000);
    Serial.print("Actual kHz   : "); Serial.println(_memPll.freq);
    Serial.print("PLL k        : "); Serial.println(_memPll.k);
    Serial.print("PLL n        : "); Serial.println(_memPll.n);

    Serial.println("\nCore\n----");
    Serial.print("kHz          : "); Serial.println(_corePll.freq);
    Serial.print("PLL k        : "); Serial.println(_corePll.k);
    Serial.print("PLL n        : "); Serial.println(_corePll.n);
    
    Serial.println("\nScan\n----");
    Serial.print("Requested kHz: "); Serial.println(_displayInfo->dotClock);
    Serial.print("Actual kHz   : "); Serial.println(_scanPll.freq);
    Serial.print("PLL k        : "); Serial.println(_scanPll.k);
    Serial.print("PLL n        : "); Serial.println(_scanPll.n);
    #endif // RA8876_DEBUG

    // TODO: Frame rate?

    return;
}

/* *************************************************************
   ************************************************************* */
bool RA8876::_initPLL(void)
{
    #if defined(RA8876_DEBUG)
    Serial.println("init PLL");
    #endif // RA8876_DEBUG

    SPI.beginTransaction(_spiSettings);

    //Serial.print("DRAM_FREQ "); Serial.println(_memPll.freq);
    //Serial.print("7: "); Serial.println(_memPll.k << 1);
    //Serial.print("8: "); Serial.println(_memPll.n);
    _writeReg(RA8876_REG_MPLLC1, _memPll.k << 1);
    _writeReg(RA8876_REG_MPLLC2, _memPll.n);

    //Serial.print("CORE_FREQ "); Serial.println(_corePll.freq);
    //Serial.print("9: "); Serial.println(_corePll.k << 1);
    //Serial.print("A: "); Serial.println(_corePll.n);
    _writeReg(RA8876_REG_SPLLC1, _corePll.k << 1);
    _writeReg(RA8876_REG_SPLLC2, _corePll.n);

    // Per the data sheet, there are two divider fields for the scan clock, but the math seems
    //  to work out if we treat k as a single 3-bit number in bits 3..1.
    //Serial.print("SCAN_FREQ "); Serial.println(_scanPll.freq);
    //Serial.print("5: "); Serial.println(_scanPll.k << 1);
    //Serial.print("6: "); Serial.println(_scanPll.n);
    _writeReg(RA8876_REG_PPLLC1, _scanPll.k << 1);
    _writeReg(RA8876_REG_PPLLC2, _scanPll.n);

    // Toggle bit 7 of the CCR register to trigger a reconfiguration of the PLLs
    _writeReg(RA8876_REG_CCR, 0x00);
    delay(2);
    _writeReg(RA8876_REG_CCR, 0x80);
    delay(2);

    uint8_t ccr = _readReg(RA8876_REG_CCR);

    SPI.endTransaction();

    return (ccr & 0x80) ? true : false;
}

/* *************************************************************
    Initialize SDRAM interface.
   ************************************************************* */
bool RA8876::_initMemory(SdramInfo *info)
{ 
    #if defined(RA8876_DEBUG)
    Serial.println("init memory");    
    #endif // RA8876_DEBUG

    uint32_t sdramRefreshRate;
    uint8_t sdrar = 0x00;
    uint8_t sdrmd = 0x00;

    // Refresh rate
    sdramRefreshRate = ((uint32_t) info->refresh * info->speed * 1000) >> info->rowBits;

    // Number of banks
    if (info->banks == 2)
      ;  // NOP
    else if (info->banks == 4)
      sdrar |= 0x20;
    else
      return false;  // Unsupported number of banks

    // Number of row bits
    if ((info->rowBits < 11) || (info->rowBits > 13))
      return false;  // Unsupported number of row bits
    else 
      sdrar |= ((info->rowBits - 11) & 0x03) << 3;

    // Number of column bits
    if ((info->colBits < 8) || (info->colBits > 12))
      return false;  // Unsupported number of column bits
    else
      sdrar |= info->colBits & 0x03;

    // CAS latency
    if ((info->casLatency < 2) || (info->casLatency > 3))
      return false;  // Unsupported CAS latency
    else
      sdrmd |= info->casLatency & 0x03;

    SPI.beginTransaction(_spiSettings);

    #if defined(RA8876_DEBUG)
    Serial.print("SDRAR: "); Serial.println(sdrar);  // Expected: 0x29 (41 decimal)
    #endif // RA8876_DEBUG
    _writeReg(RA8876_REG_SDRAR, sdrar);

    #if defined(RA8876_DEBUG)
    Serial.print("SDRMD: "); Serial.println(sdrmd);
    #endif // RA8876_DEBUG
    _writeReg(RA8876_REG_SDRMD, sdrmd);
  
    #if defined(RA8876_DEBUG)
    Serial.print("sdramRefreshRate: "); Serial.println(sdramRefreshRate);
    #endif // RA8876_DEBUG
    _writeReg(RA8876_REG_SDR_REF_ITVL0, sdramRefreshRate & 0xFF);
    _writeReg(RA8876_REG_SDR_REF_ITVL1, sdramRefreshRate >> 8);

    // Trigger SDRAM initialization
    _writeReg(RA8876_REG_SDRCR, 0x01);

    // Wait for SDRAM to be ready
    uint8_t status;
    for (int i = 0; i < 250; i++)
    {
      delay(1);
      
      if ((status = _readStatus()) & 0x40)
        break;
    }

    SPI.endTransaction();

    #if defined(RA8876_DEBUG)
    Serial.println(status);
    #endif // RA8876_DEBUG
    
    return (status & 0x40) ? true : false;
}

/* *************************************************************
   ************************************************************* */
bool RA8876::_initDisplay()
{
    SPI.beginTransaction(_spiSettings);
    
    // Set chip config register
    uint8_t ccr = _readReg(RA8876_REG_CCR);
    ccr &= 0xE7;  // 24-bit LCD output
    ccr &= 0xFE;  // 8-bit host data bus
    _writeReg(RA8876_REG_CCR, ccr);

    _writeReg(RA8876_REG_MACR, 0x00);  // Direct write, left-to-right-top-to-bottom memory

    _writeReg(RA8876_REG_ICR, 0x00);  // Graphics mode, memory is SDRAM

    // TFT timing configure
    uint8_t dpcr = _readReg(RA8876_REG_DPCR);
    dpcr &= 0xFB;  // Vertical scan top to bottom
    dpcr &= 0xF8;  // Colour order RGB
    dpcr |= 0x80;  // Panel fetches PDAT at PCLK falling edge
    _writeReg(RA8876_REG_DPCR, dpcr);

    uint8_t pcsr = _readReg(RA8876_REG_PCSR);
    pcsr |= 0x80;  // XHSYNC polarity high
    pcsr |= 0x40;  // XVSYNC polarity high
    pcsr &= 0xDF;  // XDE polarity high
    _writeReg(RA8876_REG_PCSR, pcsr);

    /* --- Esta parte es lcdHorizontalWidthVerticalHeight() en RA8876_Lite --- */
    // Set display width
    _writeReg(RA8876_REG_HDWR, (_displayInfo->width / 8) - 1);
    _writeReg(RA8876_REG_HDWFTR, (_displayInfo->width % 8));
    // Set display height
    _writeReg(RA8876_REG_VDHR0, (_displayInfo->height - 1) & 0xFF);
    _writeReg(RA8876_REG_VDHR1, (_displayInfo->height - 1) >> 8);
    /* ----------------------------------------------------------------------- */

    /* --- Esta parte es lcdHorizontalNonDisplay() en RA8876_Lite ------------ */
    // Set horizontal non-display (back porch)
    _writeReg(RA8876_REG_HNDR, (_displayInfo->hBackPorch / 8) - 1);
    _writeReg(RA8876_REG_HNDFTR, (_displayInfo->hBackPorch % 8));
    /* ----------------------------------------------------------------------- */

    /* --- Esta parte es lcdHsyncStartPosition() en RA8876_Lite -------------- */ 
    // Set horizontal start position (front porch)
    _writeReg(RA8876_REG_HSTR, ((_displayInfo->hFrontPorch + 4) / 8) - 1);
    /* ----------------------------------------------------------------------- */

    /* --- Esta parte es lcdHsyncPulseWidth() en RA8876_Lite ----------------- */ 
    // Set HSYNC pulse width
    _writeReg(RA8876_REG_HPWR, ((_displayInfo->hPulseWidth + 4) / 8) - 1);
    /* ----------------------------------------------------------------------- */

    /* --- Esta parte es lcdVerticalNonDisplay() en RA8876_Lite ----------------- */ 
    // Set vertical non-display (back porch)
    _writeReg(RA8876_REG_VNDR0, (_displayInfo->vBackPorch - 1) & 0xFF);
    _writeReg(RA8876_REG_VNDR1, (_displayInfo->vBackPorch - 1) >> 8);
    /* ----------------------------------------------------------------------- */

    /* --- Esta parte es lcdVsyncStartPosition() en RA8876_Lite ----------------- */ 
    // Set vertical start position (front porch)
    _writeReg(RA8876_REG_VSTR, _displayInfo->vFrontPorch - 1);
    /* ----------------------------------------------------------------------- */

    /* --- Esta parte es lcdVsyncPulseWidth() en RA8876_Lite ----------------- */ 
    // Set VSYNC pulse width
    _writeReg(RA8876_REG_VPWR, _displayInfo->vPulseWidth - 1);
    /* ----------------------------------------------------------------------- */


    /* Image buffer configure */
    // Set main window to 16 bits per pixel
    _writeReg(RA8876_REG_MPWCTR, 0x04);  // PIP windows disabled, 16-bpp, enable sync signals

    // Set main window start address to 0
    displayImageStartAddress(PAGE1_START_ADDR); // displayImageStartAddress() en RA8876_Lite

    // Set main window image width
    displayImageWidth(_width);                  // displayImageWidth() en RA8876_Lite

    // Set main window start coordinates
    displayWindowStartXY(0,0);                  // displayWindowStartXY() en RA8876_Lite

    // Set canvas start address
    canvasImageStartAddress(PAGE1_START_ADDR);  // canvasImageStartAddress() en RA8876_Lite

    // Set canvas width
    canvasImageWidth(_width);                   // canvasImageWidth() en RA8876_Lite

    setCanvasWindow(0,0,_width,_height);        // // activeWindowXY() y activeWindowWH() en RA8876_Lite juntos


    // Set canvas addressing mode/colour depth
    uint8_t aw_color = 0x00;  // 2d addressing mode
    if (_depth == 16)
      aw_color |= 0x01;
    else if (_depth == 24)
      aw_color |= 0x02;
    _writeReg(RA8876_REG_AW_COLOR, aw_color);
    
    // Turn on display
    dpcr = _readReg(RA8876_REG_DPCR);
    dpcr |= 0x40;  // Display on
    _writeReg(RA8876_REG_DPCR, dpcr);

    // TODO: Track backlight pin and turn on backlight ==> se hace manualmente

    SPI.endTransaction();

    return true;
}


/* *************************************************************
    Given a font encoding value, returns the corresponding bit pattern for
    use by internal fonts.
   ************************************************************* */
uint8_t RA8876::_internalFontEncoding(enum FontEncoding enc)
{
    uint8_t e;
    switch (enc)
    {
    case RA8876_FONT_ENCODING_8859_2:
      e = 0x01;
      break;
    case RA8876_FONT_ENCODING_8859_4:
      e = 0x02;
      break;
    case RA8876_FONT_ENCODING_8859_5:
      e = 0x03;
      break;
    default:
      e = 0x00;  // ISO-8859-1
      break;
    }

    return e;
}

/* *************************************************************
   ************************************************************* */
void RA8876::_setTextMode(void)
{
    // Restore text colour
    setTextForegroundColor(_textForeColor);
    setTextBackgroundColor(_textBackColor);
    setTextBackgroundTrans(_textBackTrans);

    _waitTaskBusy();

    // Enable text mode
    uint8_t icr = _readReg(RA8876_REG_ICR);
    _writeReg(RA8876_REG_ICR, icr | 0x04);
}

/* *************************************************************
   ************************************************************* */
void RA8876::_setGraphicsMode(void)
{
    _waitTaskBusy();

    // Disable text mode
    uint8_t icr = _readReg(RA8876_REG_ICR);
    _writeReg(RA8876_REG_ICR, icr & ~0x04);

}


/* *************************************************************
   ************************************************************* */
void RA8876::_drawTwoPointShape(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint8_t reg, uint8_t cmd)
{
    //Serial.println("_drawTwoPointShape");

    SPI.beginTransaction(_spiSettings);

    // First point
    _writeReg16(RA8876_REG_DLHSR0, x1);
    _writeReg16(RA8876_REG_DLVSR0, y1);

    // Second point
    _writeReg16(RA8876_REG_DLHER0, x2);
    _writeReg16(RA8876_REG_DLVER0, y2);

    // Colour
    setTextForegroundColor(color);

    // Draw
    _writeReg(reg, cmd);  // Start drawing

    // Wait for completion
    uint8_t status = _readStatus();
    int iter = 0;
    while (status & 0x08)
    {
      status = _readStatus();
      iter++;
    }

    //Serial.print(iter); Serial.println(" iterations");

    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::_drawThreePointShape(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color, uint8_t cmd)
{
    //Serial.println("_drawThreePointShape");

    SPI.beginTransaction(_spiSettings);

    // First point
    _writeReg16(RA8876_REG_DLHSR0, x1);
    _writeReg16(RA8876_REG_DLVSR0, y1);

    // Second point
    _writeReg16(RA8876_REG_DLHER0, x2);
    _writeReg16(RA8876_REG_DLVER0, y2);

    // Third point
    _writeReg16(RA8876_REG_DTPH0, x3);
    _writeReg16(RA8876_REG_DTPV0, y3);

    // Colour
    setTextForegroundColor(color);

    // Draw
    _writeReg(RA8876_REG_DCR0, cmd);  // Start drawing

    // Wait for completion
    uint8_t status = _readStatus();
    int iter = 0;
    while (status & 0x08)
    {
      status = _readStatus();
      iter++;
    }

    //Serial.print(iter); Serial.println(" iterations");

    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::_drawEllipseShape(uint16_t x, uint16_t y, uint16_t xrad, uint16_t yrad, uint16_t color, uint8_t cmd)
{
    //Serial.println("_drawEllipseShape");

    SPI.beginTransaction(_spiSettings);

    // First point
    _writeReg16(RA8876_REG_DEHR0, x);
    _writeReg16(RA8876_REG_DEVR0, y);

    // Radius
    _writeReg16(RA8876_REG_ELL_A0, xrad);
    _writeReg16(RA8876_REG_ELL_B0, yrad);

    // Colour
    setTextForegroundColor(color);

    // Draw
    _writeReg(RA8876_REG_DCR1, cmd);  // Start drawing

    // Wait for completion
    uint8_t status = _readStatus();
    int iter = 0;
    while (status & 0x08)
    {
      status = _readStatus();
      iter++;
    }

    //Serial.print(iter); Serial.println(" iterations");

    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::_drawRoundRectShape(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t radius, uint16_t color, uint8_t cmd)
{
    if(radius == 0) _drawTwoPointShape(x1, y1, x2, y2, color, RA8876_REG_DCR1, RA8876_DRAW_SQUARE);
    else{
        //uint16_t w = x2 - x1;
        //uint16_t h = y2 - y1;
        //if (w < h && (radius * 2) >= w) radius = (w / 2) - 1;
        //if (w > h && (radius * 2) >= h) radius = (h / 2) - 1;
        //if (radius == w || radius == h) drawRect(x1,y1,x2,y2,color);
        //else{
            SPI.beginTransaction(_spiSettings);

            // First point
            _writeReg16(RA8876_REG_DLHSR0, x1);
            _writeReg16(RA8876_REG_DLVSR0, y1);

            // Second point
            _writeReg16(RA8876_REG_DLHER0, x2);
            _writeReg16(RA8876_REG_DLVER0, y2);

            // Radius
            _writeReg16(RA8876_REG_ELL_A0, radius);
            _writeReg16(RA8876_REG_ELL_B0, radius);

            // Colour
            setTextForegroundColor(color);

            // Draw
            _writeReg(RA8876_REG_DCR1, cmd);  // Start drawing

            // Wait for completion
            uint8_t status = _readStatus();
            int iter = 0;
            while (status & 0x08)
            {
              status = _readStatus();
              iter++;
            }

            SPI.endTransaction();
        //}
    }
}


/* *************************************************************
    Leer tipos de 16bits de un fichero de la SD. BMP se guarda
    little-endian, igual que Arduino.
   ************************************************************* */

uint16_t RA8876::_read16(File f) {
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read(); // MSB
    return result;
}

/* *************************************************************
    Leer tipos de 32bits de un fichero de la SD. BMP se guarda
    little-endian, igual que Arduino.
   ************************************************************* */
uint32_t RA8876::_read32(File f) {
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read(); // MSB
    return result;
}


//***********************+*******************************************************************************
//-------------------------------------------------------------------------------------------------------
//----------------------------------- PUBLIC FUNCTIONS --------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//***********************+*******************************************************************************


/* *************************************************************
   ************************************************************* */
RA8876::RA8876(int csPin, int resetPin)
{
    _csPin    = csPin;
    _resetPin = resetPin;

    _width  = 0;
    _height = 0;
    _depth  = 0;

    _oscClock = 10000;  // 10000kHz or 10MHz

    _sdramInfo = &defaultSdramInfo;

    _displayInfo = &defaultDisplayInfo;

    _textForeColor = 0xFFFF; // White
    _textBackTrans = true;
    _align = 0;

    _fontRomInfo.present = false;  // No external font ROM chip
}

/* *************************************************************
   ************************************************************* */
bool RA8876::init(void)
{
    _width  = _displayInfo->width;
    _height = _displayInfo->height;
    _depth  = 16;

    // Set up chip select pin
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);

    // Set up reset pin, if provided
    if (_resetPin >= 0)
    {
      pinMode(_resetPin, OUTPUT);
      digitalWrite(_resetPin, HIGH);

      _hardReset();
    }

    if (!_calcClocks())
    {
      Serial.println("_calcClocks failed");
      return false;
    }

    SPI.begin();

    _spiSettings = SPISettings(RA8876_SPI_SPEED, MSBFIRST, SPI_MODE3); 

    // SPI is now up, so we can do a soft reset if no hard reset was possible earlier
    if (_resetPin < 0)
      _softReset();

    if (!_initPLL())
    {
      Serial.println("_initPLL failed");
      return false;
    }

    if (!_initMemory(_sdramInfo))
    {
      Serial.println("_initMemory failed");
      return false;
    }

    if (!_initDisplay())
    {
      Serial.println("_initDisplay failed");
      return false;
    }

    // Set default font
    selectInternalFont(RA8876_FONT_SIZE_16);
    setTextScale(RA8876_TEXT_W_SCALE_X1,RA8876_TEXT_H_SCALE_X1); // setTextScale(0);

    return true;
}

/* *************************************************************
   ************************************************************* */
void RA8876::initExternalFontRom(int spiIf, enum ExternalFontRom chip)
{
    // See data sheet figure 16-10
    // TODO: GT30L24T3Y supports FAST_READ command (0x0B) and runs at 20MHz. Are the other font chips the same?

    // Find a clock divisor. Values are in the range 2..512 in steps of 2.
    int divisor;
    uint32_t speed = 20000;  // 20MHz target speed
    for (divisor = 2; divisor <= 512; divisor += 2)
    {
      if (_corePll.freq / divisor <= speed)
        break;
    }

    _fontRomInfo.present = true;
    _fontRomInfo.spiInterface = spiIf;
    _fontRomInfo.spiClockDivisor = divisor;
    _fontRomInfo.chip = chip;

    #if defined(RA8876_DEBUG)
    Serial.print("External font SPI divisor: "); Serial.println(divisor);
    #endif // RA8876_DEBUG

    SPI.beginTransaction(_spiSettings);

    // Ensure SPI is enabled in chip config register
    uint8_t ccr = _readReg(RA8876_REG_CCR);
    if (!(ccr & 0x02))
      _writeReg(RA8876_REG_CCR, ccr | 0x02);

    #if defined(RA8876_DEBUG)
    Serial.print("SFL_CTRL: "); Serial.println(((spiIf & 1) << 7) | 0x14, HEX);
    #endif // RA8876_DEBUG
    _writeReg(RA8876_REG_SFL_CTRL, ((spiIf & 1) << 7) | 0x14);  // Font mode, 24-bit address, standard timing, supports FAST_READ
    #if defined(RA8876_DEBUG)
    Serial.print("SPI_DIVSOR: "); Serial.println((divisor >> 1) - 1, HEX);
    #endif // RA8876_DEBUG
    _writeReg(RA8876_REG_SPI_DIVSOR, (divisor >> 1) - 1);
    #if defined(RA8876_DEBUG)
    Serial.print("GTFNT_SEL: "); Serial.println((chip & 0x07) << 5, HEX);
    #endif // RA8876_DEBUG
    _writeReg(RA8876_REG_GTFNT_SEL, (chip & 0x07) << 5);

    SPI.endTransaction();
}


/* *************************************************************
    Parecido a canvasImageStartAddress() en RA8876_Lite
   ************************************************************* */
bool RA8876::setCanvasRegion(uint32_t address, uint16_t width)
{
    if (address & 0x3)
      return false;  // Address must me multiple of 4
    else if ((width & 0x03) || (width > 0x1FFF))
      return false;  // Width must be multiple of 4 and fit in 13 bits

    SPI.beginTransaction(_spiSettings);

    // Set canvas start address
    _writeReg32(RA8876_REG_CVSSA0, address);
    
    uint8_t aw_color = _readReg(RA8876_REG_AW_COLOR);

    if (width)
    {
      aw_color &= 0xFB;  // Block mode
      _writeReg16(RA8876_REG_CVS_IMWTH0, width);
    }
    else
    {
      aw_color |= 0x04;  // Linear mode
    }

    _writeReg(RA8876_REG_AW_COLOR, aw_color);

    SPI.endTransaction();

    return true;
}

/* *************************************************************
    activeWindowXY() y activeWindowWH() en RA8876_Lite juntos
   ************************************************************* */
bool RA8876::setCanvasWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    if (x + width > 8188)
      return false;
    else if (y + height > 8191)
      return false;

    SPI.beginTransaction(_spiSettings);
      
    // Set active window offset
    _writeReg16(RA8876_REG_AWUL_X0, x);
    _writeReg16(RA8876_REG_AWUL_Y0, y);

    // Set active window dimensions
    _writeReg16(RA8876_REG_AW_WTH0, width);
    _writeReg16(RA8876_REG_AW_HT0, height);

    SPI.endTransaction();

    return true;
}

/* *************************************************************
   ************************************************************* */
void RA8876::displayImageStartAddress(uint32_t addr)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg(RA8876_REG_MISA0,addr);//20h
    _writeReg(RA8876_REG_MISA1,addr>>8);//21h 
    _writeReg(RA8876_REG_MISA2,addr>>16);//22h  
    _writeReg(RA8876_REG_MISA3,addr>>24);//23h 
    SPI.endTransaction();
}

/* *************************************************************
************************************************************* */
void RA8876::displayImageWidth(uint16_t width)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg(RA8876_REG_MIW0,width); //24h
    _writeReg(RA8876_REG_MIW1,width>>8); //25h 
    SPI.endTransaction();
}

/* *************************************************************
************************************************************* */
void RA8876::displayWindowStartXY(uint16_t x0,uint16_t y0)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg(RA8876_REG_MWULX0,x0);//26h
    _writeReg(RA8876_REG_MWULX1,x0>>8);//27h
    _writeReg(RA8876_REG_MWULY0,y0);//28h
    _writeReg(RA8876_REG_MWULY1,y0>>8);//29h
    SPI.endTransaction();
}

/* *************************************************************
************************************************************* */
void RA8876::canvasImageStartAddress(uint32_t addr)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg(RA8876_REG_CVSSA0,addr);//50h
    _writeReg(RA8876_REG_CVSSA1,addr>>8);//51h
    _writeReg(RA8876_REG_CVSSA2,addr>>16);//52h
    _writeReg(RA8876_REG_CVSSA3,addr>>24);//53h  
    SPI.endTransaction();
}

/* *************************************************************
************************************************************* */
void RA8876::canvasImageWidth(uint16_t width)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg(RA8876_REG_CVS_IMWTH0,width);//54h
    _writeReg(RA8876_REG_CVS_IMWTH1,width>>8); //55h
    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
bool RA8876::setDisplayRegion(uint32_t address, uint16_t width)
{
    if (address & 0x3)
      return false;  // Address must me multiple of 4
    else if ((width & 0x03) || (width > 8188))
      return false;  // Width must be multiple of 4 and max 8188

    SPI.beginTransaction(_spiSettings);
    
    // Set main window start address
    _writeReg32(RA8876_REG_MISA0, address);

    // Set main window image width
    _writeReg16(RA8876_REG_MIW0, width);

    SPI.endTransaction();

    return true;
}

/* *************************************************************
   ************************************************************* */
bool RA8876::setDisplayOffset(uint16_t x, uint16_t y)
{
    if (x > 8188)
      return false;
    else if (y > 8191)
      return false;

    SPI.beginTransaction(_spiSettings);

    // Set main window offset
    _writeReg16(RA8876_REG_MWULX0, x & 0xFFFC);  // Low two bits must be zero
    _writeReg16(RA8876_REG_MWULY0, y);

    SPI.endTransaction();

    return true;
}


/* *************************************************************
    Show colour bars of 8 colours in repeating horizontal bars.
    This does not alter video memory, but rather instructs the video controller to display
    the pattern rather than the contents of memory.
   ************************************************************* */
void RA8876::colorBarTest(bool enabled)
{
    SPI.beginTransaction(_spiSettings);

    uint8_t dpcr = _readReg(RA8876_REG_DPCR);

    if (enabled)
      dpcr = dpcr | 0x20;
    else
      dpcr = dpcr & ~0x20;

    _writeReg(RA8876_REG_DPCR, dpcr);

    SPI.endTransaction();
}


/* *************************************************************
   ************************************************************* */
void RA8876::ramAccessPrepare(void)
{
    SPI.beginTransaction(_spiSettings);
    _writeCmd(RA8876_REG_MRWDP); //lcdRegWrite()  en RA8876_Lite
    SPI.endTransaction();
}


/* *************************************************************
   ************************************************************* */
void RA8876::selectInternalFont(enum FontSize size, enum FontEncoding enc)
{
    _fontSource = RA8876_FONT_SOURCE_INTERNAL;
    _fontSize   = size;
    _fontFlags  = 0;

    SPI.beginTransaction(_spiSettings);

    _writeReg(RA8876_REG_CCR0, 0x00 | ((size & 0x03) << 4) | _internalFontEncoding(enc));

    uint8_t ccr1 = _readReg(RA8876_REG_CCR1);
    ccr1 |= 0x40;  // Transparent background
    _writeReg(RA8876_REG_CCR1, ccr1);

    SPI.endTransaction();
}


/* *************************************************************
   ************************************************************* */
void RA8876::selectExternalFont(enum ExternalFontFamily family, enum FontSize size, enum FontEncoding enc, FontFlags flags)
{
    _fontSource = RA8876_FONT_SOURCE_EXT_ROM;
    _fontSize   = size;
    _fontFlags  = flags;

    SPI.beginTransaction(_spiSettings);

    #if defined(RA8876_DEBUG)
    Serial.print("CCR0: "); Serial.println(0x40 | ((size & 0x03) << 4), HEX);
    #endif // RA8876_DEBUG

    _writeReg(RA8876_REG_CCR0, 0x40 | ((size & 0x03) << 4));  // Select external font ROM and size

    uint8_t ccr1 = _readReg(RA8876_REG_CCR1);
    ccr1 |= 0x40;  // Transparent background

    #if defined(RA8876_DEBUG)
    Serial.print("CCR1: "); Serial.println(ccr1, HEX);
    #endif // RA8876_DEBUG

    _writeReg(RA8876_REG_CCR1, ccr1);

    #if defined(RA8876_DEBUG)
    Serial.print("GTFNT_CR: "); Serial.println((enc << 3) | (family & 0x03), HEX);
    #endif // RA8876_DEBUG
    
    _writeReg(RA8876_REG_GTFNT_CR, (enc << 3) | (family & 0x03));  // Character encoding and family

    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::setTextScale(uint8_t X_scale, uint8_t Y_scale)
{
    if((X_scale != _textScaleX) || (Y_scale != _textScaleY)){
        _textScaleX = X_scale;
        _textScaleY = Y_scale;
        SPI.beginTransaction(_spiSettings);
        _writeReg(RA8876_REG_CCR1,X_scale<<2|Y_scale);//cdh
        SPI.endTransaction();
    }
}


/* *************************************************************
   ************************************************************* */
void RA8876::setCursor(uint16_t x, uint16_t y)
{
    if(x < 0) x = 0; if(x >= _width) x = _width-1;
    if(y < 0) y = 0; if(y >= _height) y = _height-1;

    if((x != _cursorX) || (y != _cursorY)){
        _cursorX = x;
        _cursorY = y;
        SPI.beginTransaction(_spiSettings);
        _writeReg16(RA8876_REG_F_CURX0, x);  // Text cursor X-coordinate register 0
        _writeReg16(RA8876_REG_F_CURY0, y);  // Text cursor Y-coordinate register 0
        SPI.endTransaction();
    }
}


/* *************************************************************
   ************************************************************* */
void RA8876::setTextForegroundColor(uint16_t color)
{
    _textForeColor = color;

    SPI.beginTransaction(_spiSettings);

    _writeReg(RA8876_REG_FGCR, color >> 11 << 3);
    _writeReg(RA8876_REG_FGCG, ((color >> 5) & 0x3F) << 2);
    _writeReg(RA8876_REG_FGCB, (color & 0x1F) << 3);

    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::setTextBackgroundColor(uint16_t color)
{
    _textBackColor = color;

    SPI.beginTransaction(_spiSettings);

    _writeReg(RA8876_REG_BGCR, color >> 11 << 3);
    _writeReg(RA8876_REG_BGCG, ((color >> 5) & 0x3F) << 2);
    _writeReg(RA8876_REG_BGCB, (color & 0x1F) << 3);

    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::setTextBackgroundTrans(uint8_t trans)
{
    //if(trans != _textBackTrans){
        _textBackTrans = trans; // ON or OFF
        SPI.beginTransaction(_spiSettings);
        _writeReg(RA8876_REG_CCR1,_align<<7|trans<<6|_textScaleX<<2|_textScaleY);//cdh
        SPI.endTransaction();
    //}
}


/* *************************************************************
   ************************************************************* */
void RA8876::setTextColor(uint16_t f_color, uint16_t b_color, uint8_t trans)
{
    /*if(f_color != _textForeColor) setTextForegroundColor(f_color);
    if(trans == 0){ // Transparency OFF => mostrar b_color
      if(b_color != _textBackColor) setTextBackgroundColor(b_color);
    } //Si se quiere hacer transparente (trans = 1), no hace falta modificar background.
    if(trans != _textBackTrans) setTextBackgroundTrans(trans);*/

    setTextForegroundColor(f_color);
    setTextBackgroundColor(b_color);
    setTextBackgroundTrans(trans);
  
}



/* *************************************************************
    Similar to write(), but does no special handling of control characters.
   ************************************************************* */
void RA8876::putChars(const char *buffer, size_t size)
{
    SPI.beginTransaction(_spiSettings);

    _setTextMode();

    // Write characters
    _writeCmd(RA8876_REG_MRWDP);
    for (unsigned int i = 0; i < size; i++)
    {
      _waitWriteFifo();
      _writeData(buffer[i]);
    }

    _setGraphicsMode();

    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::putChars16(const uint16_t *buffer, unsigned int count)
{
    SPI.beginTransaction(_spiSettings);

    _setTextMode();

    // Write characters
    _writeCmd(RA8876_REG_MRWDP);
    for (unsigned int i = 0; i < count; i++)
    {
      _waitWriteFifo();
      _writeData(buffer[i] >> 8);

      _waitWriteFifo();
      _writeData(buffer[i] & 0xFF);
    }

    _setGraphicsMode();

    SPI.endTransaction();
}

//**************************************************************//
//support ra8876 internal font and external string font code write from data pointer
//**************************************************************//
void RA8876:: putString(uint16_t x0,uint16_t y0, char *str)
{
    SPI.beginTransaction(_spiSettings);

    _setTextMode();
    setCursor(x0,y0);
    ramAccessPrepare();
    while(*str != '\0')
    {
    _checkWriteFifoNotFull();  
    _writeData(*str);
    ++str; 
    } 
    //_check2dBusy();
    // Wait for completion
    uint8_t status = _readStatus();
    int iter = 0;
    while (status & 0x08)
    {
      status = _readStatus();
      iter++;
    }

    _setGraphicsMode();

    SPI.endTransaction();
}


/* *************************************************************
   ************************************************************* */
size_t RA8876::write(const uint8_t *buffer, size_t size)
{
    SPI.beginTransaction(_spiSettings);

    _setTextMode();

    _writeCmd(RA8876_REG_MRWDP);  // Set current register for writing to memory
    for (unsigned int i = 0; i < size; i++)
    {
      char c = buffer[i];

      if (c == '\r')
        ;  // Ignored
      else if (c == '\n')
      {
        setCursor(0, getCursorY() + getTextSizeY()+20);
        _writeCmd(RA8876_REG_MRWDP);  // Reset current register for writing to memory
      }
      else if ((_fontFlags & RA8876_FONT_FLAG_XLAT_FULLWIDTH) && ((c >= 0x21) || (c <= 0x7F)))
      {
        // Translate ASCII to Unicode fullwidth form (for Chinese fonts that lack ASCII)
        uint16_t fwc = c - 0x21 + 0xFF01;

        _waitWriteFifo();
        _writeData(fwc >> 8);

        _waitWriteFifo();
        _writeData(fwc & 0xFF);
      }
      else
      {
        _waitWriteFifo();
        _writeData(c);
      }
    }

    _setGraphicsMode();

    _cursorX = _readReg16(RA8876_REG_F_CURX0);
    _cursorY = _readReg16(RA8876_REG_F_CURY0);

    SPI.endTransaction();
  
    return size;
}



/* *************************************************************
   ************************************************************* */
void RA8876::setPixelCursor(uint16_t x,uint16_t y)
{
    SPI.beginTransaction(_spiSettings);
    
    _writeReg16(RA8876_REG_CURH0, x);  // Graphic read/write horizontal position 0
    _writeReg16(RA8876_REG_CURV0, y);  // Graphic read/write vertical position 0
    
    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    //Serial.println("drawPixel");
    //Serial.println(_readStatus());
    
    SPI.beginTransaction(_spiSettings);

    setPixelCursor(x, y);
    
    _writeReg(RA8876_REG_MRWDP, color & 0xFF);
    _writeReg(RA8876_REG_MRWDP, color >> 8);
    
    SPI.endTransaction();
}


/* *************************************************************
   ************************************************************* */
void RA8876::bte_Source0_MemoryStartAddr(uint32_t addr)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg32(RA8876_S0_STR0,addr);
    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::bte_Source0_ImageWidth(uint16_t width)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg16(RA8876_S0_WTH0,width);
    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::bte_Source0_WindowStartXY(uint16_t x0,uint16_t y0)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg16(RA8876_S0_X0,x0);
    _writeReg16(RA8876_S0_Y0,y0);
    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::bte_Source1_MemoryStartAddr(uint32_t addr)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg32(RA8876_S1_STR0,addr);
    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::bte_Source1_ImageWidth(uint16_t width)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg16(RA8876_S1_WTH0,width);
    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::bte_Source1_WindowStartXY(uint16_t x0,uint16_t y0)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg16(RA8876_S1_X0,x0);
    _writeReg16(RA8876_S1_Y0,y0);
    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void  RA8876::bte_DestinationMemoryStartAddr(uint32_t addr)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg32(RA8876_DT_STR0,addr);
    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void  RA8876::bte_DestinationImageWidth(uint16_t width)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg16(RA8876_DT_WTH0,width);
    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void  RA8876::bte_DestinationWindowStartXY(uint16_t x0,uint16_t y0)	
{
    SPI.beginTransaction(_spiSettings);
    _writeReg16(RA8876_DT_X0,x0);
    _writeReg16(RA8876_DT_Y0,y0);
    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void  RA8876::bte_WindowSize(uint16_t width, uint16_t height)
{
    SPI.beginTransaction(_spiSettings);
    _writeReg16(RA8876_BTE_WTH0,width);
    _writeReg16(RA8876_BTE_HIG0,height);
    SPI.endTransaction();
}

/* *************************************************************
   ************************************************************* */
void RA8876::bteMemoryCopy(uint32_t s0_addr,uint16_t s0_image_width,uint16_t s0_x,uint16_t s0_y,uint32_t des_addr,uint16_t des_image_width, 
                    uint16_t des_x,uint16_t des_y,uint16_t copy_width,uint16_t copy_height)
{
    SPI.beginTransaction(_spiSettings);

    bte_Source0_MemoryStartAddr(s0_addr);
    bte_Source0_ImageWidth(s0_image_width);
    bte_Source0_WindowStartXY(s0_x,s0_y);

    //bte_Source1_MemoryStartAddr(des_addr);
    //bte_Source1_ImageWidth(des_image_width);
    //bte_Source1_WindowStartXY(des_x,des_y);

    bte_DestinationMemoryStartAddr(des_addr);
    bte_DestinationImageWidth(des_image_width);
    bte_DestinationWindowStartXY(des_x,des_y);
    
    bte_WindowSize(copy_width,copy_height); 

    _writeReg(RA8876_BTE_CTRL1,RA8876_BTE_ROP_CODE_12<<4|RA8876_BTE_MEMORY_COPY_WITH_ROP);//91h
    _writeReg(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
    _writeReg(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
    
    // Wait for completion
    uint8_t status = _readStatus();
    int iter = 0;
    while (status & 0x08)
    {
      status = _readStatus();
      iter++;
    }

    SPI.endTransaction();
}
 

/* *************************************************************
   ************************************************************* */
void RA8876::bteMemoryCopyWithROP(uint32_t s0_addr,uint16_t s0_image_width,uint16_t s0_x,uint16_t s0_y,uint32_t s1_addr,uint16_t s1_image_width,uint16_t s1_x,uint16_t s1_y,
                            uint32_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t copy_width,uint16_t copy_height,uint8_t rop_code)
{
    SPI.beginTransaction(_spiSettings);

    bte_Source0_MemoryStartAddr(s0_addr);
    bte_Source0_ImageWidth(s0_image_width);
    bte_Source0_WindowStartXY(s0_x,s0_y);

    bte_Source1_MemoryStartAddr(s1_addr);
    bte_Source1_ImageWidth(s1_image_width);
    bte_Source1_WindowStartXY(s1_x,s1_y);

    bte_DestinationMemoryStartAddr(des_addr);
    bte_DestinationImageWidth(des_image_width);
    bte_DestinationWindowStartXY(des_x,des_y);

    bte_WindowSize(copy_width,copy_height);

    _writeReg(RA8876_BTE_CTRL1,rop_code<<4|RA8876_BTE_MEMORY_COPY_WITH_ROP);//91h
    _writeReg(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
    _writeReg(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
    
    //_check2dBusy();
    // Wait for completion
    uint8_t status = _readStatus();
    int iter = 0;
    while (status & 0x08)
    {
      status = _readStatus();
      iter++;
    }

    SPI.endTransaction();
}

/* *************************************************************
************************************************************* */
void RA8876::bteMemoryCopyWithChromaKey(uint32_t s0_addr,uint16_t s0_image_width,uint16_t s0_x,uint16_t s0_y,
                                uint32_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t copy_width,uint16_t copy_height,uint16_t chromakey_color)
{
    SPI.beginTransaction(_spiSettings);

    bte_Source0_MemoryStartAddr(s0_addr);
    bte_Source0_ImageWidth(s0_image_width);
    bte_Source0_WindowStartXY(s0_x,s0_y);

    bte_DestinationMemoryStartAddr(des_addr);
    bte_DestinationImageWidth(des_image_width);
    bte_DestinationWindowStartXY(des_x,des_y);

    bte_WindowSize(copy_width,copy_height);

    setTextBackgroundColor(chromakey_color);

    _writeReg(RA8876_BTE_CTRL1,RA8876_BTE_MEMORY_COPY_WITH_CHROMA);//91h
    _writeReg(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
    _writeReg(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
    
    //_check2dBusy();
    // Wait for completion
    uint8_t status = _readStatus();
    int iter = 0;
    while (status & 0x08)
    {
      status = _readStatus();
      iter++;
    }

    SPI.endTransaction();
}







/* *************************************************************
   -------------------- IMÁGENES -------------------------------
   ************************************************************* */


/* *************************************************************
    Mostrar imagen de 16bpp (RGB 5:6:5) en formato Word guardada en 
    un fichero.h en la Flash de arduino (véase RA8876_v2_Word_image.ino).

    Conversor online de JPG -> Word 16bpp: https://javl.github.io/image2cpp/ 
   ************************************************************* */
void RA8876::putPicture_16bpp(uint16_t x,uint16_t y,uint16_t width, uint16_t height, const unsigned short *data)
{
    _spiSettings = SPISettings(RA8876_SPI_SPEED_IMG, MSBFIRST, SPI_MODE3); //Incremento velocidad SPI para imagen => 50MHz

  setCanvasWindow(x,y,width,height); // activeWindowXY() y activeWindowWH() de RA8876_Lite
  setPixelCursor(x,y);
  ramAccessPrepare();
  for(uint16_t j=0;j<height;j++)
  {
    for(uint16_t i=0;i<width;i++)
    {
    _writeData16bits(*data);
    data++;
    }
  } 
  _checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
  setCanvasWindow(0,0,_width,_height);

  _spiSettings = SPISettings(RA8876_SPI_SPEED, MSBFIRST, SPI_MODE3); //Decremento velocidad SPI para texto => 3MHz

}


/* *************************************************************
    Mostrar imagen de 16bpp (RGB 5:6:5) en formato BIN guardada en el
    fichero 'filename' de la SD.

    Conversor online de JPG -> BIN: https://javl.github.io/image2cpp/

    También la herramienta de "RAiO Image Tool"
   ************************************************************* */
void RA8876::sdCardDraw16bppBIN8bits(uint16_t x,uint16_t y,uint16_t width, uint16_t height,char *filename)
{
    _spiSettings = SPISettings(RA8876_SPI_SPEED_IMG, MSBFIRST, SPI_MODE3); //Incremento velocidad SPI para imagen => 50MHz

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open(filename);
    // if the file is available, read it and write to ra8876:
    if (dataFile) {  
        //tft.putPicture_16bpp(x,y,width,height);    
        setCanvasWindow(x,y,width,height); // activeWindowXY() y activeWindowWH() de RA8876_Lite
        setPixelCursor(x,y);
        ramAccessPrepare();
      while (dataFile.available()) 
      {
          //Serial.write(dataFile.read());
          //_checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
          _writeData(dataFile.read());
          _writeData(dataFile.read());
      }
      dataFile.close();
    }   
    else Serial.println(F("Fichero no encontrado"));
    setCanvasWindow(0,0,_width,_height);

    _spiSettings = SPISettings(RA8876_SPI_SPEED, MSBFIRST, SPI_MODE3); //Decremento velocidad SPI para texto => 3MHz
}


/* *************************************************************
    Mostrar imagen de 16bpp (RGB 5:6:5) en formato BIN guardada en el
    fichero 'filename' de la SD. 
    Se leen 64 bits y se envían por SPI a la pantalla.

    Es 2.65 veces más rápido que sdCardDraw16bppBIN8bits().

    Conversor online de JPG -> BIN: https://javl.github.io/image2cpp/

    También la herramienta de "RAiO Image Tool"
   ************************************************************* */
void RA8876::sdCardDraw16bppBIN64bits(uint16_t x,uint16_t y,uint16_t width, uint16_t height,char *filename)
{
    _spiSettings = SPISettings(RA8876_SPI_SPEED_IMG, MSBFIRST, SPI_MODE3); //Incremento velocidad SPI para imagen => 50MHz

    uint64_t data;

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open(filename);
    // if the file is available, read it and write to ra8876:
    if (dataFile) {  
        //tft.putPicture_16bpp(x,y,width,height);    
        setCanvasWindow(x,y,width,height); // activeWindowXY() y activeWindowWH() de RA8876_Lite
        setPixelCursor(x,y);
        ramAccessPrepare();
      while (dataFile.available()) 
      {
          // => 2.65 veces más rápido que sdCardDraw16bppBIN8bits()
          //----
          dataFile.read(&data, sizeof(data));
          _writeData64bits(data); //64
          //----
      }
      dataFile.close();
    }   
    else Serial.println(F("Fichero no encontrado"));
    setCanvasWindow(0,0,_width,_height);

    _spiSettings = SPISettings(RA8876_SPI_SPEED, MSBFIRST, SPI_MODE3); //Decremento velocidad SPI para texto => 3MHz
}



/* *************************************************************
    Mostrar imagen de 16bpp (RGB 5:6:5) en formato BIN guardada en el
    fichero 'filename' de la SD. 
    Se leen 256 bits en 4 paquetes de 64 bits y se envían por SPI a la pantalla.

    Es 2.96 veces más rápido que sdCardDraw16bppBIN8bits().

    Conversor online de JPG -> BIN: https://javl.github.io/image2cpp/

    También la herramienta de "RAiO Image Tool"
   ************************************************************* */
void RA8876::sdCardDraw16bppBIN256bits(uint16_t x,uint16_t y,uint16_t width, uint16_t height,char *filename)
{
    _spiSettings = SPISettings(RA8876_SPI_SPEED_IMG, MSBFIRST, SPI_MODE3); //Incremento velocidad SPI para imagen => 50MHz

    uint64_t data[4];

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open(filename);
    // if the file is available, read it and write to ra8876:
    if (dataFile) {  
        //tft.putPicture_16bpp(x,y,width,height);    
        setCanvasWindow(x,y,width,height); // activeWindowXY() y activeWindowWH() de RA8876_Lite
        setPixelCursor(x,y);
        ramAccessPrepare();
      while (dataFile.available()) 
      {
          // => 2.96 veces más rápido que sdCardDraw16bppBIN8bits() 
          //----
          dataFile.read(&data[0], sizeof(data[0]));
          dataFile.read(&data[1], sizeof(data[1]));
          dataFile.read(&data[2], sizeof(data[2]));
          dataFile.read(&data[3], sizeof(data[3]));
          _writeData256bits(data); // 64 | 128 | 192 | 256
          //----
      }
      dataFile.close();
    }   
    else Serial.println(F("Fichero no encontrado"));
    setCanvasWindow(0,0,_width,_height);

    _spiSettings = SPISettings(RA8876_SPI_SPEED, MSBFIRST, SPI_MODE3); //Decremento velocidad SPI para texto => 3MHz
}

