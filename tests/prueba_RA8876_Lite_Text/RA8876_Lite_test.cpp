//**************************************************************//
/*
File Name : Ra8876_Lite.cpp                                   
Author    : RAiO Application Team                             
Edit Date : 12/29/2015
Version   : v1.0
*/
//**************************************************************//
#include <SPI.h>
#include "RA8876_Lite_test.h"

#define mydebug

/* ************************************************************ */
SdramInfo defaultSdramInfo =
{
  120, // 120 MHz
  3,   // CAS latency 3
  4,   // 4 banks
  12,  // 12-bit row addresses
  9,   // 9-bit column addresses
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

//**************************************************************//
 Ra8876_Lite::Ra8876_Lite(int xnscs,int xnreset)
{
  _xnscs = xnscs;
  _xnreset = xnreset;

  m_width  = 0;
  m_height = 0;
  m_depth  = 0;

  m_oscClock = 10000;  // 10000kHz or 10MHz

  m_sdramInfo = &defaultSdramInfo;

  m_displayInfo = &defaultDisplayInfo;
}



/* *************************************************************
    Given a target frequency in kHz, finds PLL parameters k and n to reach as
    close as possible to the target frequency without exceeding it.
    The k parameter will be constrained to the range 1..kMax.
    Returns true iff PLL params were found, even if not an exact match.
   ************************************************************* */
bool Ra8876_Lite::calcPllParams(uint32_t targetFreq, int kMax, PllParams *pll)
{
  bool found = false;
  int foundk, foundn;
  uint32_t foundFreq;
  uint32_t foundError;  // Amount lower than requested frequency
  
  // k of 0 (i.e. 2 ** 0 = 1) is possible, but not sure if it's a good idea.
  for (int testk = 1; testk <= kMax; testk++)
  {
    if (m_oscClock % (1 << testk))
      continue;  // Step size with this k would be fractional

    int testn = (targetFreq / (m_oscClock / (1 << testk))) - 1;
    if ((testn < 1) || (testn > 63))
      continue;  // Param n out of range for this k
      
    // Fvco constraint found in data sheet section 6.1.2
    uint32_t fvco = m_oscClock * (testn + 1);
    if ((fvco < 100000) && (fvco > 600000))
      continue;  // Fvco out of range
      
    // Found some usable params, but perhaps at a lower frequency than requested.
    uint32_t freq = (m_oscClock * (testn + 1)) / (1 << testk);
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
bool Ra8876_Lite::calcClocks(void)
{
  // Data sheet section 5.2 gives max clocks:
  //  memClock : 166 MHz
  //  coreClock: 120 MHz (133MHz if not using internal font)
  //  scanClock: 100 MHz

  // Mem clock target is the same as SDRAM speed, but capped at 166 MHz
  uint32_t memClock = m_sdramInfo->speed * 1000;
  if (memClock > 166000)
    memClock = 166000;

  if (!calcPllParams(memClock, 3, &m_memPll)){
    Serial.println("Fallo 1");
    return false;
  }

  // Core clock target will be the same as the mem clock, but capped to
  //  120 MHz, because that is the max frequency if we want to use the
  //  internal font.
  uint32_t coreClock = m_memPll.freq;
  if (coreClock > 120000)
    coreClock = 120000;

  if (!calcPllParams(coreClock, 3, &m_corePll)){
    Serial.println("Fallo 2");
    return false;
  }

  // Scan clock target will be the display's dot clock, but capped at 100 MHz
  uint32_t scanClock = m_displayInfo->dotClock;
  if (scanClock > 100000)
    scanClock = 100000;

  if (!calcPllParams(scanClock, 7, &m_scanPll)){
    Serial.println("Fallo 3");
    return false;
  }

  dumpClocks();

  // Data sheet section 6.1.1 rules:
  // 1. Core clock must be less than or equal to mem clock
  if (m_corePll.freq > m_memPll.freq){
    Serial.println("Fallo 4");
    return false;
  }

  // 2. Core clock must be greater than half mem clock
  if ((m_corePll.freq * 2) <= m_memPll.freq){
    Serial.println("Fallo 5");
    return false;
  }

  // 3. Core clock must be greater than (scan clock * 1.5)
  if (m_corePll.freq <= (m_scanPll.freq + (m_scanPll.freq >> 1))){
    Serial.println("Fallo 6");
    return false;
  }
      
  return true;
}

/* *************************************************************
    Dump clock info to serial monitor.
   ************************************************************* */
void Ra8876_Lite::dumpClocks(void)
{
  #if defined(RA8876_DEBUG)
  Serial.println("\nMem\n---");
  Serial.print("Requested kHz: "); Serial.println(m_sdramInfo->speed * 1000);
  Serial.print("Actual kHz   : "); Serial.println(m_memPll.freq);
  Serial.print("PLL k        : "); Serial.println(m_memPll.k);
  Serial.print("PLL n        : "); Serial.println(m_memPll.n);

  Serial.println("\nCore\n----");
  Serial.print("kHz          : "); Serial.println(m_corePll.freq);
  Serial.print("PLL k        : "); Serial.println(m_corePll.k);
  Serial.print("PLL n        : "); Serial.println(m_corePll.n);
  
  Serial.println("\nScan\n----");
  Serial.print("Requested kHz: "); Serial.println(m_displayInfo->dotClock);
  Serial.print("Actual kHz   : "); Serial.println(m_scanPll.freq);
  Serial.print("PLL k        : "); Serial.println(m_scanPll.k);
  Serial.print("PLL n        : "); Serial.println(m_scanPll.n);
  #endif // RA8876_DEBUG

  // TODO: Frame rate?

  return;
}

/* *************************************************************
   ************************************************************* */
bool Ra8876_Lite::initPLL(void)
{
  #if defined(RA8876_DEBUG)
  Serial.println("init PLL");
  #endif // RA8876_DEBUG

  SPI.beginTransaction(m_spiSettings);

  //Serial.print("DRAM_FREQ "); Serial.println(m_memPll.freq);
  //Serial.print("7: "); Serial.println(m_memPll.k << 1);
  //Serial.print("8: "); Serial.println(m_memPll.n);
  lcdRegDataWrite(RA8876_MPLLC1, m_memPll.k << 1);
  lcdRegDataWrite(RA8876_MPLLC2, m_memPll.n);

  //Serial.print("CORE_FREQ "); Serial.println(m_corePll.freq);
  //Serial.print("9: "); Serial.println(m_corePll.k << 1);
  //Serial.print("A: "); Serial.println(m_corePll.n);
  lcdRegDataWrite(RA8876_SPLLC1, m_corePll.k << 1);
  lcdRegDataWrite(RA8876_SPLLC2, m_corePll.n);

  // Per the data sheet, there are two divider fields for the scan clock, but the math seems
  //  to work out if we treat k as a single 3-bit number in bits 3..1.
  //Serial.print("SCAN_FREQ "); Serial.println(m_scanPll.freq);
  //Serial.print("5: "); Serial.println(m_scanPll.k << 1);
  //Serial.print("6: "); Serial.println(m_scanPll.n);
  lcdRegDataWrite(RA8876_PPLLC1, m_scanPll.k << 1);
  lcdRegDataWrite(RA8876_PPLLC2, m_scanPll.n);

  // Toggle bit 7 of the CCR register to trigger a reconfiguration of the PLLs
  lcdRegDataWrite(RA8876_CCR0, 0x00);
  delay(2);
  lcdRegDataWrite(RA8876_CCR0, 0x80);
  delay(2);

  uint8_t ccr = lcdRegDataRead(RA8876_CCR);

  SPI.endTransaction();

  return (ccr & 0x80) ? true : false;
}

/* *************************************************************
    Initialize SDRAM interface.
   ************************************************************* */
bool Ra8876_Lite::initMemory(SdramInfo *info)
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

  SPI.beginTransaction(m_spiSettings);

  #if defined(RA8876_DEBUG)
  Serial.print("SDRAR: "); Serial.println(sdrar);  // Expected: 0x29 (41 decimal)
  #endif // RA8876_DEBUG
  lcdRegDataWrite(RA8876_SDRAR, sdrar);

  #if defined(RA8876_DEBUG)
  Serial.print("SDRMD: "); Serial.println(sdrmd);
  #endif // RA8876_DEBUG
  lcdRegDataWrite(RA8876_SDRMD, sdrmd);
  
  #if defined(RA8876_DEBUG)
  Serial.print("sdramRefreshRate: "); Serial.println(sdramRefreshRate);
  #endif // RA8876_DEBUG
  lcdRegDataWrite(RA8876_SDR_REF_ITVL0, sdramRefreshRate & 0xFF);
  lcdRegDataWrite(RA8876_SDR_REF_ITVL1, sdramRefreshRate >> 8);

  // Trigger SDRAM initialization
  lcdRegDataWrite(RA8876_SDRCR, 0x01);

  // Wait for SDRAM to be ready
  uint8_t status;
  for (int i = 0; i < 250; i++)
  {
    delay(1);
    
    if ((status = lcdStatusRead()) & 0x40)
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
bool Ra8876_Lite::initDisplay()
{
  SPI.beginTransaction(m_spiSettings);
  
  // Set chip config register
  uint8_t ccr = lcdRegDataRead(RA8876_CCR);
  ccr &= 0xE7;  // 24-bit LCD output
  ccr &= 0xFE;  // 8-bit host data bus
  lcdRegDataWrite(RA8876_CCR, ccr);

  lcdRegDataWrite(RA8876_MACR, 0x00);  // Direct write, left-to-right-top-to-bottom memory

  lcdRegDataWrite(RA8876_ICR, 0x00);  // Graphics mode, memory is SDRAM

  // TFT timing configure
  uint8_t dpcr = lcdRegDataRead(RA8876_DPCR);
  dpcr &= 0xFB;  // Vertical scan top to bottom
  dpcr &= 0xF8;  // Colour order RGB
  dpcr |= 0x80;  // Panel fetches PDAT at PCLK falling edge
  lcdRegDataWrite(RA8876_DPCR, dpcr);

  uint8_t pcsr = lcdRegDataRead(RA8876_PCSR);
  pcsr |= 0x80;  // XHSYNC polarity high
  pcsr |= 0x40;  // XVSYNC polarity high
  pcsr &= 0xDF;  // XDE polarity high
  lcdRegDataWrite(RA8876_PCSR, pcsr);

  /* --- Esta parte es lcdHorizontalWidthVerticalHeight() en RA8876_Lite --- */
  // Set display width
  lcdRegDataWrite(RA8876_HDWR, (m_displayInfo->width / 8) - 1);
  lcdRegDataWrite(RA8876_HDWFTR, (m_displayInfo->width % 8));
  // Set display height
  lcdRegDataWrite(RA8876_VDHR0, (m_displayInfo->height - 1) & 0xFF);
  lcdRegDataWrite(RA8876_VDHR1, (m_displayInfo->height - 1) >> 8);
  /* ----------------------------------------------------------------------- */

  /* --- Esta parte es lcdHorizontalNonDisplay() en RA8876_Lite ------------ */
  // Set horizontal non-display (back porch)
  lcdRegDataWrite(RA8876_HNDR, (m_displayInfo->hBackPorch / 8) - 1);
  lcdRegDataWrite(RA8876_HNDFTR, (m_displayInfo->hBackPorch % 8));
  /* ----------------------------------------------------------------------- */

  /* --- Esta parte es lcdHsyncStartPosition() en RA8876_Lite -------------- */ 
  // Set horizontal start position (front porch)
  lcdRegDataWrite(RA8876_HSTR, ((m_displayInfo->hFrontPorch + 4) / 8) - 1);
  /* ----------------------------------------------------------------------- */

  /* --- Esta parte es lcdHsyncPulseWidth() en RA8876_Lite ----------------- */ 
  // Set HSYNC pulse width
  lcdRegDataWrite(RA8876_HPWR, ((m_displayInfo->hPulseWidth + 4) / 8) - 1);
  /* ----------------------------------------------------------------------- */

  /* --- Esta parte es lcdVerticalNonDisplay() en RA8876_Lite ----------------- */ 
  // Set vertical non-display (back porch)
  lcdRegDataWrite(RA8876_VNDR0, (m_displayInfo->vBackPorch - 1) & 0xFF);
  lcdRegDataWrite(RA8876_VNDR1, (m_displayInfo->vBackPorch - 1) >> 8);
  /* ----------------------------------------------------------------------- */

  /* --- Esta parte es lcdVsyncStartPosition() en RA8876_Lite ----------------- */ 
  // Set vertical start position (front porch)
  lcdRegDataWrite(RA8876_VSTR, m_displayInfo->vFrontPorch - 1);
  /* ----------------------------------------------------------------------- */

  /* --- Esta parte es lcdVsyncPulseWidth() en RA8876_Lite ----------------- */ 
  // Set VSYNC pulse width
  lcdRegDataWrite(RA8876_VPWR, m_displayInfo->vPulseWidth - 1);
  /* ----------------------------------------------------------------------- */


  /* Image buffer configure */
  // Set main window to 16 bits per pixel
  lcdRegDataWrite(RA8876_MPWCTR, 0x04);  // PIP windows disabled, 16-bpp, enable sync signals

  /* --- Esta parte es displayImageStartAddress() en RA8876_Lite ---------- */ 
  // Set main window start address to 0
  lcdRegDataWrite(RA8876_MISA0, 0);
  lcdRegDataWrite(RA8876_MISA1, 0);
  lcdRegDataWrite(RA8876_MISA2, 0);
  lcdRegDataWrite(RA8876_MISA3, 0);
  /* ----------------------------------------------------------------------- */

  /* --- Esta parte es displayImageWidth() en RA8876_Lite ----------------- */ 
  // Set main window image width
  lcdRegDataWrite(RA8876_MIW0, m_width & 0xFF);
  lcdRegDataWrite(RA8876_MIW1, m_width >> 8);
  /* ----------------------------------------------------------------------- */

  /* --- Esta parte es displayWindowStartXY() en RA8876_Lite --------------- */ 
  // Set main window start coordinates
  lcdRegDataWrite(RA8876_MWULX0, 0);
  lcdRegDataWrite(RA8876_MWULX1, 0);
  lcdRegDataWrite(RA8876_MWULY0, 0);
  lcdRegDataWrite(RA8876_MWULY1, 0);
  /* ----------------------------------------------------------------------- */

  /* --- Esta parte es canvasImageStartAddress() en RA8876_Lite ------------ */ 
  // Set canvas start address
  lcdRegDataWrite(RA8876_CVSSA0, 0);
  lcdRegDataWrite(RA8876_CVSSA1, 0);
  lcdRegDataWrite(RA8876_CVSSA2, 0);
  lcdRegDataWrite(RA8876_CVSSA3, 0);
  /* ----------------------------------------------------------------------- */

  /* --- Esta parte es canvasImageWidth() en RA8876_Lite ------------------- */ 
  // Set canvas width
  lcdRegDataWrite(RA8876_CVS_IMWTH0, m_width & 0xFF);
  lcdRegDataWrite(RA8876_CVS_IMWTH1, m_width >> 8);
  /* ----------------------------------------------------------------------- */

  /* --- Esta parte es activeWindowXY() en RA8876_Lite --------------------- */ 
  // Set active window start coordinates
  /*lcdRegDataWrite(RA8876_AWUL_X0, 0);
  lcdRegDataWrite(RA8876_AWUL_X1, 0);
  lcdRegDataWrite(RA8876_AWUL_Y0, 0);
  lcdRegDataWrite(RA8876_AWUL_Y1, 0);*/
  activeWindowXY(0,0);
  /* ----------------------------------------------------------------------- */

  /* --- Esta parte es activeWindowWH() en RA8876_Lite --------------------- */ 
  // Set active window dimensions
  /*lcdRegDataWrite(RA8876_AW_WTH0, m_width & 0xFF);
  lcdRegDataWrite(RA8876_AW_WTH1, m_width >> 8);
  lcdRegDataWrite(RA8876_AW_HT0, m_height & 0xFF);
  lcdRegDataWrite(RA8876_AW_HT1, m_height >> 8);*/
  activeWindowWH(m_width,m_height);
  /* ----------------------------------------------------------------------- */

  // Set canvas addressing mode/colour depth
  uint8_t aw_color = 0x00;  // 2d addressing mode
  if (m_depth == 16)
    aw_color |= 0x01;
  else if (m_depth == 24)
    aw_color |= 0x02;
  lcdRegDataWrite(RA8876_AW_COLOR, aw_color);
  
  // Turn on display
  dpcr = lcdRegDataRead(RA8876_DPCR);
  dpcr |= 0x40;  // Display on
  lcdRegDataWrite(RA8876_DPCR, dpcr);

  // TODO: Track backlight pin and turn on backlight ==> se hace manualmente

  SPI.endTransaction();

  return true;
}

//**************************************************************//
//**************************************************************//
void Ra8876_Lite::displayImageStartAddress(ru32 addr)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_MISA0,addr);//20h
  lcdRegDataWrite(RA8876_MISA1,addr>>8);//21h 
  lcdRegDataWrite(RA8876_MISA2,addr>>16);//22h  
  lcdRegDataWrite(RA8876_MISA3,addr>>24);//23h 
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::displayImageWidth(ru16 width)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_MIW0,width); //24h
  lcdRegDataWrite(RA8876_MIW1,width>>8); //25h 
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::displayWindowStartXY(ru16 x0,ru16 y0)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_MWULX0,x0);//26h
  lcdRegDataWrite(RA8876_MWULX1,x0>>8);//27h
  lcdRegDataWrite(RA8876_MWULY0,y0);//28h
  lcdRegDataWrite(RA8876_MWULY1,y0>>8);//29h
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::canvasImageStartAddress(ru32 addr)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_CVSSA0,addr);//50h
  lcdRegDataWrite(RA8876_CVSSA1,addr>>8);//51h
  lcdRegDataWrite(RA8876_CVSSA2,addr>>16);//52h
  lcdRegDataWrite(RA8876_CVSSA3,addr>>24);//53h  
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::canvasImageWidth(ru16 width)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_CVS_IMWTH0,width);//54h
  lcdRegDataWrite(RA8876_CVS_IMWTH1,width>>8); //55h
  SPI.endTransaction();
}




/* *************************************************************
   ************************************************************* */
bool Ra8876_Lite::init(void)
{
  m_width  = m_displayInfo->width;
  m_height = m_displayInfo->height;
  m_depth  = 16;

  // Set up chip select pin
  pinMode(_xnscs, OUTPUT);
  digitalWrite(_xnscs, HIGH);

  // Set up reset pin, if provided
  if (_xnreset >= 0)
  {
    pinMode(_xnreset, OUTPUT);
    digitalWrite(_xnreset, HIGH);
    delay(5);
    digitalWrite(_xnreset, LOW);
    delay(5);
    digitalWrite(_xnreset, HIGH);
    delay(5);
  }

  if (!calcClocks())
  {
    Serial.println("calcClocks failed");
    return false;
  }

  SPI.begin();

  m_spiSettings = SPISettings(RA8876_SPI_SPEED, MSBFIRST, SPI_MODE3); 

  // SPI is now up, so we can do a soft reset if no hard reset was possible earlier
  if (_xnreset < 0)
    softReset();

  if (!initPLL())
  {
    Serial.println("initPLL failed");
    return false;
  }

  if (!initMemory(m_sdramInfo))
  {
    Serial.println("initMemory failed");
    return false;
  }

  if (!initDisplay())
  {
    Serial.println("initDisplay failed");
    return false;
  }

  return true;
}

/* *************************************************************
    Trigger a soft reset. Note that the data sheet section 19.2 
    says that this only resets the "internal state machine", 
    not any configuration registers.
   ************************************************************* */
void Ra8876_Lite::softReset(void)
{
  SPI.beginTransaction(m_spiSettings);

  // Trigger soft reset
  lcdRegDataWrite(RA8876_SRR, 0x01);
  delay(5);

  /* --- Esta parte es checkIcReady() en RA8876_Lite ------ */
  // Wait for status register to show "normal operation".
  uint8_t status;
  for (int i = 0; i < 250; i++)
  {
    delay(1);
    
    if (((status = lcdStatusRead()) & 0x02) == 0)
      break;
  }

  SPI.endTransaction();

  return;
}


//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdRegWrite(ru8 reg) 
{
  digitalWrite(_xnscs, LOW);
  SPI.transfer(RA8876_SPI_CMDWRITE);
  SPI.transfer(reg);
  digitalWrite(_xnscs, HIGH);
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdDataWrite(ru8 data) 
{
  digitalWrite(_xnscs, LOW);
  SPI.transfer(RA8876_SPI_DATAWRITE);
  SPI.transfer(data);
  digitalWrite(_xnscs, HIGH);
}
//**************************************************************//
//**************************************************************//
ru8 Ra8876_Lite::lcdDataRead(void) 
{
  digitalWrite(_xnscs, LOW);
  SPI.transfer(RA8876_SPI_DATAREAD);
  ru8 data = SPI.transfer(0);
  digitalWrite(_xnscs, HIGH);
  return data;
}
//**************************************************************//
//**************************************************************//
ru8 Ra8876_Lite::lcdStatusRead(void) 
{
  digitalWrite(_xnscs, LOW);
  SPI.transfer(RA8876_SPI_STATUSREAD);
  ru8 data = SPI.transfer(0);
  digitalWrite(_xnscs, HIGH);
  return data;
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdRegDataWrite(ru8 reg,ru8 data)
{
  lcdRegWrite(reg);
  lcdDataWrite(data);
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdRegDataWrite16(ru8 reg,ru16 data)
{
  lcdRegWrite(reg);
  lcdDataWrite(data & 0xFF);
  lcdRegWrite(reg + 1);
  lcdDataWrite(data>>8);
}
//**************************************************************//
//**************************************************************//
ru8 Ra8876_Lite::lcdRegDataRead(ru8 reg)
{
  lcdRegWrite(reg);
  return lcdDataRead();
}

//**************************************************************//
// support SPI interface to write 16bpp data after Regwrite 04h
//**************************************************************//
void Ra8876_Lite::lcdDataWrite16bbp(ru16 data) 
{
  digitalWrite(_xnscs, LOW);
  SPI.transfer(RA8876_SPI_DATAWRITE);
  SPI.transfer(data);
  SPI.transfer(data>>8);
  digitalWrite(_xnscs, HIGH);
}

//**************************************************************//
//RA8876 register 
//**************************************************************//
/*[Status Register] bit7  Host Memory Write FIFO full
0: Memory Write FIFO is not full.
1: Memory Write FIFO is full.
Only when Memory Write FIFO is not full, MPU may write another one pixel.*/ 
//**************************************************************//
void Ra8876_Lite::checkWriteFifoNotFull(void)
{  ru16 i;  
    SPI.beginTransaction(m_spiSettings);
   for(i=0;i<10000;i++) //Please according to your usage to modify i value.
   {
    if( (lcdStatusRead()&0x80)==0 ){break;}
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
void Ra8876_Lite::checkWriteFifoEmpty(void)
{ ru16 i;
    SPI.beginTransaction(m_spiSettings);
   for(i=0;i<10000;i++)   //Please according to your usage to modify i value.
   {
    if( (lcdStatusRead()&0x40)==0x40 ){break;}
   }
   SPI.endTransaction();
}

//**************************************************************//
/*[Status Register] bit5  Host Memory Read FIFO full
0: Memory Read FIFO is not full.
1: Memory Read FIFO is full.
When Memory Read FIFO is full, MPU may read 8bpp data 32
pixels, or 16bpp data 16 pixels, 24bpp data 8 pixels directly.*/
//**************************************************************//
void Ra8876_Lite::checkReadFifoNotFull(void)
{ ru16 i;
  SPI.beginTransaction(m_spiSettings);
  for(i=0;i<10000;i++)  //Please according to your usage to modify i value.
  {if( (lcdStatusRead()&0x20)==0x00 ){break;}}
  SPI.endTransaction();
}

//**************************************************************//
/*[Status Register] bit4   Host Memory Read FIFO empty
0: Memory Read FIFO is not empty.
1: Memory Read FIFO is empty.*/
//**************************************************************//
void Ra8876_Lite::checkReadFifoNotEmpty(void)
{ ru16 i;
  SPI.beginTransaction(m_spiSettings);
  for(i=0;i<10000;i++)// //Please according to your usage to modify i value. 
  {if( (lcdStatusRead()&0x10)==0x00 ){break;}}
  SPI.endTransaction();
}

//**************************************************************//
/*[Status Register] bit3   Core task is busy
Following task is running:
BTE, Geometry engine, Serial flash DMA, Text write or Graphic write
0: task is done or idle.   1: task is busy*/
//**************************************************************//
void Ra8876_Lite::check2dBusy(void)  
{  ru32 i; 
  SPI.beginTransaction(m_spiSettings);
   for(i=0;i<10000;i++)   //Please according to your usage to modify i value.
   { 
    if( (lcdStatusRead()&0x08)==0x00 )
    return;
   }
   SPI.endTransaction();
}  


//**************************************************************//
/*[Status Register] bit2   SDRAM ready for access
0: SDRAM is not ready for access   1: SDRAM is ready for access*/	
//**************************************************************//
boolean Ra8876_Lite::checkSdramReady(void)
{ru32 i;
  SPI.beginTransaction(m_spiSettings);
 for(i=0;i<1000000;i++) //Please according to your usage to modify i value.
 { 
   delayMicroseconds(1);
   if( (lcdStatusRead()&0x04)==0x04 )
    {return true;}
    
 }
 SPI.endTransaction();
 return false;
}

//**************************************************************//
/*[Status Register] bit1  Operation mode status
0: Normal operation state  1: Inhibit operation state
Inhibit operation state means internal reset event keep running or
initial display still running or chip enter power saving state.	*/
//**************************************************************//
boolean Ra8876_Lite::checkIcReady(void)
{ru32 i;
  SPI.beginTransaction(m_spiSettings);
  for(i=0;i<1000000;i++)  //Please according to your usage to modify i value.
   {
     delayMicroseconds(1);
     if( (lcdStatusRead()&0x02)==0x00 )
     {return true;}     
   }
   SPI.endTransaction();
   return false;
}
//**************************************************************//
//**************************************************************//
//[05h] [06h] [07h] [08h] [09h] [0Ah]
//------------------------------------//----------------------------------*/


//**************************************************************//
//**************************************************************//
void Ra8876_Lite::activeWindowXY(ru16 x0,ru16 y0)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_AWUL_X0,x0);//56h
  lcdRegDataWrite(RA8876_AWUL_X1,x0>>8);//57h 
  lcdRegDataWrite(RA8876_AWUL_Y0,y0);//58h
  lcdRegDataWrite(RA8876_AWUL_Y1,y0>>8);//59h 
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::activeWindowWH(ru16 width,ru16 height)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_AW_WTH0,width);//5ah
  lcdRegDataWrite(RA8876_AW_WTH1,width>>8);//5bh
  lcdRegDataWrite(RA8876_AW_HT0,height);//5ch
  lcdRegDataWrite(RA8876_AW_HT1,height>>8);//5dh  
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite:: setPixelCursor(ru16 x,ru16 y)
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_CURH0,x); //5fh
  lcdRegDataWrite(RA8876_CURH1,x>>8);//60h
  lcdRegDataWrite(RA8876_CURV0,y);//61h
  lcdRegDataWrite(RA8876_CURV1,y>>8);//62h
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_Source0_MemoryStartAddr(ru32 addr)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_S0_STR0,addr);//93h
  lcdRegDataWrite(RA8876_S0_STR1,addr>>8);//94h
  lcdRegDataWrite(RA8876_S0_STR2,addr>>16);//95h
  lcdRegDataWrite(RA8876_S0_STR3,addr>>24);////96h
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_Source0_ImageWidth(ru16 width)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_S0_WTH0,width);//97h
  lcdRegDataWrite(RA8876_S0_WTH1,width>>8);//98h
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_Source0_WindowStartXY(ru16 x0,ru16 y0)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_S0_X0,x0);//99h
  lcdRegDataWrite(RA8876_S0_X1,x0>>8);//9ah
  lcdRegDataWrite(RA8876_S0_Y0,y0);//9bh
  lcdRegDataWrite(RA8876_S0_Y1,y0>>8);//9ch
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_Source1_MemoryStartAddr(ru32 addr)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_S1_STR0,addr);//9dh
  lcdRegDataWrite(RA8876_S1_STR1,addr>>8);//9eh
  lcdRegDataWrite(RA8876_S1_STR2,addr>>16);//9fh
  lcdRegDataWrite(RA8876_S1_STR3,addr>>24);//a0h
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_Source1_ImageWidth(ru16 width)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_S1_WTH0,width);//a1h
  lcdRegDataWrite(RA8876_S1_WTH1,width>>8);//a2h
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_Source1_WindowStartXY(ru16 x0,ru16 y0)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_S1_X0,x0);//a3h
  lcdRegDataWrite(RA8876_S1_X1,x0>>8);//a4h
  lcdRegDataWrite(RA8876_S1_Y0,y0);//a5h
  lcdRegDataWrite(RA8876_S1_Y1,y0>>8);//a6h
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void  Ra8876_Lite::bte_DestinationMemoryStartAddr(ru32 addr)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_DT_STR0,addr);//a7h
  lcdRegDataWrite(RA8876_DT_STR1,addr>>8);//a8h
  lcdRegDataWrite(RA8876_DT_STR3,addr>>16);//a9h
  lcdRegDataWrite(RA8876_DT_STR3,addr>>24);//aah
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void  Ra8876_Lite::bte_DestinationImageWidth(ru16 width)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_DT_WTH0,width);//abh
  lcdRegDataWrite(RA8876_DT_WTH1,width>>8);//ach
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void  Ra8876_Lite::bte_DestinationWindowStartXY(ru16 x0,ru16 y0)	
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_DT_X0,x0);//adh
  lcdRegDataWrite(RA8876_DT_X1,x0>>8);//aeh
  lcdRegDataWrite(RA8876_DT_Y0,y0);//afh
  lcdRegDataWrite(RA8876_DT_Y1,y0>>8);//b0h
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void  Ra8876_Lite::bte_WindowSize(ru16 width, ru16 height)
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_BTE_WTH0,width);//b1h
  lcdRegDataWrite(RA8876_BTE_WTH1,width>>8);//b2h
  lcdRegDataWrite(RA8876_BTE_HIG0,height);//b3h
  lcdRegDataWrite(RA8876_BTE_HIG1,height>>8);//b4h
  SPI.endTransaction();
}

//**************************************************************//
/*These 8 bits determine prescaler value for Timer 0 and 1.*/
/*Time base is “Core_Freq / (Prescaler + 1)”*/
//**************************************************************//
void Ra8876_Lite::pwm_Prescaler(ru8 prescaler)
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_PSCLR,prescaler);//84h
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::pwm_ClockMuxReg(ru8 pwm1_clk_div, ru8 pwm0_clk_div, ru8 xpwm1_ctrl, ru8 xpwm0_ctrl)
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_PMUXR,pwm1_clk_div<<6|pwm0_clk_div<<4|xpwm1_ctrl<<2|xpwm0_ctrl);//85h
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::pwm_Configuration(ru8 pwm1_inverter,ru8 pwm1_auto_reload,ru8 pwm1_start,ru8 
                       pwm0_dead_zone, ru8 pwm0_inverter, ru8 pwm0_auto_reload,ru8 pwm0_start)
 {
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_PCFGR,pwm1_inverter<<6|pwm1_auto_reload<<5|pwm1_start<<4|pwm0_dead_zone<<3|
                  pwm0_inverter<<2|pwm0_auto_reload<<1|pwm0_start);//86h                
  SPI.endTransaction();
 }   
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::pwm0_Duty(ru16 duty)
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_TCMPB0L,duty);//88h 
  lcdRegDataWrite(RA8876_TCMPB0H,duty>>8);//89h 
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::pwm0_ClocksPerPeriod(ru16 clocks_per_period)
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_TCNTB0L,clocks_per_period);//8ah
  lcdRegDataWrite(RA8876_TCNTB0H,clocks_per_period>>8);//8bh
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::pwm1_Duty(ru16 duty)
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_TCMPB1L,duty);//8ch 
  lcdRegDataWrite(RA8876_TCMPB1H,duty>>8);//8dh
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::pwm1_ClocksPerPeriod(ru16 clocks_per_period)
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_TCNTB1L,clocks_per_period);//8eh
  lcdRegDataWrite(RA8876_TCNTB1F,clocks_per_period>>8);//8fh
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite:: ramAccessPrepare(void)
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegWrite(RA8876_MRWDP); //04h
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::foreGroundColor16bpp(ru16 color)
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_FGCR,color>>8);//d2h
  lcdRegDataWrite(RA8876_FGCG,color>>3);//d3h
  lcdRegDataWrite(RA8876_FGCB,color<<3);//d4h
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::backGroundColor16bpp(ru16 color)
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_BGCR,color>>8);//d5h
  lcdRegDataWrite(RA8876_BGCG,color>>3);//d6h
  lcdRegDataWrite(RA8876_BGCB,color<<3);//d7h
  SPI.endTransaction();
}

 /*graphic function*/
//**************************************************************//
/* If use ra8876 graphic mode function turns on */
//**************************************************************//
 void Ra8876_Lite::graphicMode(boolean on)
 {
    SPI.beginTransaction(m_spiSettings);
    if(on){
      lcdRegDataWrite(RA8876_ICR,RA8877_LVDS_FORMAT<<3|RA8876_GRAPHIC_MODE<<2|RA8876_MEMORY_SELECT_IMAGE);//03h  //switch to graphic mode
    }
    else{
      lcdRegDataWrite(RA8876_ICR,RA8877_LVDS_FORMAT<<3|RA8876_TEXT_MODE<<2|RA8876_MEMORY_SELECT_IMAGE);//03h  //switch back to text mode
    }
    SPI.endTransaction();
 }
//**************************************************************//
/*support serial IF to write 16bpp pixel*/
//**************************************************************//
void Ra8876_Lite:: putPixel_16bpp(ru16 x,ru16 y,ru16 color)
{
  SPI.beginTransaction(m_spiSettings);
  setPixelCursor(x,y);
  ramAccessPrepare();
  //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
  //lcdDataWrite(color);
  //lcdDataWrite(color>>8);
  lcdDataWrite16bbp(color);
  //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
  SPI.endTransaction();
}
//**************************************************************//
/*support ra8876 serial IF to write 16bpp(RGB565) picture data for user operation */
//**************************************************************//
void Ra8876_Lite:: putPicture_16bpp(ru16 x,ru16 y,ru16 width, ru16 height)
{ru16 i,j;
  SPI.beginTransaction(m_spiSettings);
  activeWindowXY(x,y);
  activeWindowWH(width,height);
  setPixelCursor(x,y);
  ramAccessPrepare();
  SPI.endTransaction();
}
//**************************************************************//
/*support serial IF to write 16bpp(RGB565) picture data byte format from data pointer*/
//**************************************************************//
void Ra8876_Lite:: putPicture_16bpp(ru16 x,ru16 y,ru16 width, ru16 height, const unsigned char *data)
{ru16 i,j;
  SPI.beginTransaction(m_spiSettings);
  activeWindowXY(x,y);
  activeWindowWH(width,height);
  setPixelCursor(x,y);
  ramAccessPrepare();
  for(j=0;j<height;j++)
  {
    for(i=0;i<width;i++)
    {
    //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
    lcdDataWrite(*data);
    data++;
    //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
    lcdDataWrite(*data);
    data++;
    }
  } 
  checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
  activeWindowXY(0,0);
  activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT);
  SPI.endTransaction();
}
//**************************************************************//
/*support serial IF to write 16bpp(RGB565) picture data word format from data pointer*/
//**************************************************************//
void Ra8876_Lite:: putPicture_16bpp(ru16 x,ru16 y,ru16 width, ru16 height, const unsigned short *data)
{ru16 i,j;
  SPI.beginTransaction(m_spiSettings);
  activeWindowXY(x,y);
  activeWindowWH(width,height);
  setPixelCursor(x,y);
  ramAccessPrepare();
  for(j=0;j<height;j++)
  {
    for(i=0;i<width;i++)
    {
    //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
    lcdDataWrite16bbp(*data);
    data++;
    //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
    }
  } 
  checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
  activeWindowXY(0,0);
  activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT);
  SPI.endTransaction();
}

/*text function*/
//**************************************************************//
/* If use ra8876 text mode function turns on */
//**************************************************************//
void Ra8876_Lite::textMode(boolean on)
 {
   SPI.beginTransaction(m_spiSettings);
    if(on){
      lcdRegDataWrite(RA8876_ICR,RA8877_LVDS_FORMAT<<3|RA8876_TEXT_MODE<<2|RA8876_MEMORY_SELECT_IMAGE);//03h  //switch to text mode
    }
    else{
      lcdRegDataWrite(RA8876_ICR,RA8877_LVDS_FORMAT<<3|RA8876_GRAPHIC_MODE<<2|RA8876_MEMORY_SELECT_IMAGE);//03h  //switch back to graphic mode
    }
  SPI.endTransaction();
}
 //**************************************************************//
 //**************************************************************//
void Ra8876_Lite::textColor(ru16 foreground_color,ru16 background_color)
 {
   foreGroundColor16bpp(foreground_color);
   backGroundColor16bpp(background_color);
 }
 //**************************************************************//
 //**************************************************************//
void Ra8876_Lite:: setTextCursor(ru16 x,ru16 y)
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_F_CURX0,x); //63h
  lcdRegDataWrite(RA8876_F_CURX1,x>>8);//64h
  lcdRegDataWrite(RA8876_F_CURY0,y);//65h
  lcdRegDataWrite(RA8876_F_CURY1,y>>8);//66h
  SPI.endTransaction();
}
 //**************************************************************//
 /* source_select = 0 : internal CGROM,  source_select = 1: external CGROM, source_select = 2: user-define*/
 /* size_select = 0 : 8*16/16*16, size_select = 1 : 12*24/24*24, size_select = 2 : 16*32/32*32  */
 /* iso_select = 0 : iso8859-1, iso_select = 1 : iso8859-2, iso_select = 2 : iso8859-4, iso_select = 3 : iso8859-5*/
 //**************************************************************//
 void Ra8876_Lite::setTextParameter1(ru8 source_select,ru8 size_select,ru8 iso_select)//cch
 {
   SPI.beginTransaction(m_spiSettings);
   lcdRegDataWrite(RA8876_CCR0,source_select<<6|size_select<<4|iso_select);//cch
   SPI.endTransaction();
 }
 //**************************************************************//
 /*align = 0 : full alignment disable, align = 1 : full alignment enable*/
 /*chroma_key = 0 : text with chroma key disable, chroma_key = 1 : text with chroma key enable*/
 /* width_enlarge and height_enlarge can be set 0~3, (00b: X1) (01b : X2)  (10b : X3)  (11b : X4)*/
 //**************************************************************//
void Ra8876_Lite::setTextParameter2(ru8 align, ru8 chroma_key, ru8 width_enlarge, ru8 height_enlarge)
{
  SPI.beginTransaction(m_spiSettings);
  lcdRegDataWrite(RA8876_CCR1,align<<7|chroma_key<<6|width_enlarge<<2|height_enlarge);//cdh
  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::genitopCharacterRomParameter(ru8 scs_select, ru8 clk_div, ru8 rom_select, ru8 character_select, ru8 gt_width)
{ 
  SPI.beginTransaction(m_spiSettings);
  if(scs_select==0)
  lcdRegDataWrite(RA8876_SFL_CTRL,RA8876_SERIAL_FLASH_SELECT0<<7|RA8876_SERIAL_FLASH_FONT_MODE<<6|RA8876_SERIAL_FLASH_ADDR_24BIT<<5|RA8876_FOLLOW_RA8876_MODE<<4|RA8876_SPI_FAST_READ_8DUMMY);//b7h
  if(scs_select==1)
  lcdRegDataWrite(RA8876_SFL_CTRL,RA8876_SERIAL_FLASH_SELECT1<<7|RA8876_SERIAL_FLASH_FONT_MODE<<6|RA8876_SERIAL_FLASH_ADDR_24BIT<<5|RA8876_FOLLOW_RA8876_MODE<<4|RA8876_SPI_FAST_READ_8DUMMY);//b7h
  
  lcdRegDataWrite(RA8876_SPI_DIVSOR,clk_div);//bbh 
  
  lcdRegDataWrite(RA8876_GTFNT_SEL,rom_select<<5);//ceh
  lcdRegDataWrite(RA8876_GTFNT_CR,character_select<<3|gt_width);//cfh
  SPI.endTransaction();
}
//**************************************************************//
//support ra8876 internal font and external string font code write from data pointer
//**************************************************************//
void Ra8876_Lite:: putString(ru16 x0,ru16 y0, char *str)
{
  SPI.beginTransaction(m_spiSettings);
  textMode(true);
  setTextCursor(x0,y0);
  ramAccessPrepare();
  while(*str != '\0')
  {
  checkWriteFifoNotFull();  
  lcdDataWrite(*str);
  ++str; 
  } 
  check2dBusy();
  textMode(false);
  SPI.endTransaction();
}
/*put value,base on sprintf*/
//**************************************************************//
//vaule: -2147483648(-2^31) ~ 2147483647(2^31-1)
//len: 1~11 minimum output length
/*
  [flag] 
    n:靠右　
    -:靠左
    +:輸出正負號
    (space):當不輸出正負號時，就輸出空白
    0:在開頭處(左側) 補 0，而非補空白。 
 */
//**************************************************************//
void Ra8876_Lite:: putDec(ru16 x0,ru16 y0,rs32 vaule,ru8 len,const char *flag)
{
  SPI.beginTransaction(m_spiSettings);

  char char_buffer[12];
  switch(len)
  {
    case 1:
           if(flag=="n")
           {sprintf(char_buffer ,"%1d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="-")
           {sprintf(char_buffer ,"%-1d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+1d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%01d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 2:
           if(flag=="n")
           {sprintf(char_buffer ,"%2d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="-")
           { sprintf(char_buffer ,"%-2d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           { sprintf(char_buffer ,"%+2d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           { sprintf(char_buffer ,"%02d", vaule); putString(x0,y0,char_buffer);}
           break; 
    case 3: 
           if(flag=="n")
           {sprintf(char_buffer ,"%3d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="-")
           {sprintf(char_buffer ,"%-3d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+3d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%03d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 4: 
           if(flag=="n")
           {sprintf(char_buffer ,"%4d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="-")
           { sprintf(char_buffer ,"%-4d", vaule);  putString(x0,y0,char_buffer);}
           else if(flag=="+")
           { sprintf(char_buffer ,"%+4d", vaule);  putString(x0,y0,char_buffer);}
           else if(flag=="0")
           { sprintf(char_buffer ,"%04d", vaule);  putString(x0,y0,char_buffer);}
           break;
    case 5: 
           if(flag=="n")
           {sprintf(char_buffer ,"%5d", vaule); putString(x0,y0,char_buffer);}    
           else if(flag=="-")
           {sprintf(char_buffer ,"%-5d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+5d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%05d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 6: 
           if(flag=="n")
           {sprintf(char_buffer ,"%6d", vaule); putString(x0,y0,char_buffer);}    
           else if(flag=="-")
           {sprintf(char_buffer ,"%-6d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+6d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%06d", vaule); putString(x0,y0,char_buffer);}
           break; 
    case 7: 
           if(flag=="n")
           {sprintf(char_buffer ,"%7d", vaule); putString(x0,y0,char_buffer);} 
           else if(flag=="-")
           {sprintf(char_buffer ,"%-7d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+7d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%07d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 8: 
           if(flag=="n")
           {sprintf(char_buffer ,"%8d", vaule); putString(x0,y0,char_buffer);}     
           else if(flag=="-")
           {sprintf(char_buffer ,"%-8d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+8d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%08d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 9:
           if(flag=="n")
           {sprintf(char_buffer ,"%9d", vaule); putString(x0,y0,char_buffer);}  
           else if(flag=="-")
           {sprintf(char_buffer ,"%-9d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+9d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%09d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 10:
           if(flag=="n")
           {sprintf(char_buffer ,"%10d", vaule); putString(x0,y0,char_buffer);}  
           else if(flag=="-")
           {sprintf(char_buffer ,"%-10d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+10d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%010d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 11: 
           if(flag=="n")
           {sprintf(char_buffer ,"%11d", vaule); putString(x0,y0,char_buffer);} 
           else if(flag=="-")
           {sprintf(char_buffer ,"%-11d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+11d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%011d", vaule); putString(x0,y0,char_buffer);}
           break;       
    default:   
           break;    
  }
  SPI.endTransaction();
}

//**************************************************************//
//vaule: (3.4E-38) ~ (3.4E38)
//len: 1~11 minimum output length
//precision: right side of point numbers 1~4 
/*
  [flag] 
    n:靠右　
    -:靠左
    +:輸出正負號
    (space):當不輸出正負號時，就輸出空白
    #: 強制輸出小數點
    0:在開頭處(左側) 補 0，而非補空白。
    
 */
 //arduino Floats have only 6-7 decimal digits of precision. That means the total number of digits, not the number to the right of the decimal point. 
 //Unlike other platforms, where you can get more precision by using a double (e.g. up to 15 digits), on the Arduino, double is the same size as float.
//**************************************************************//
void Ra8876_Lite:: putFloat(ru16 x0,ru16 y0,double vaule,ru8 len,ru8 precision, const char *flag)
{
  SPI.beginTransaction(m_spiSettings);

  char char_buffer[20];
  switch(len)
  {
    case 1:
           if(flag=="n")
           {
             if(precision==1)
             sprintf(char_buffer ,"%1.1f", vaule); putString(x0,y0,char_buffer);
             if(precision==2)
             sprintf(char_buffer ,"%1.2f", vaule); putString(x0,y0,char_buffer);
             if(precision==3)
             sprintf(char_buffer ,"%1.3f", vaule); putString(x0,y0,char_buffer);
             if(precision==4)
             sprintf(char_buffer ,"%1.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-1.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-1.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-1.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-1.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+1.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+1.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+1.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+1.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%01.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%01.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%01.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%01.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 2:
           if(flag=="n")
           {
           if(precision==1)  
           sprintf(char_buffer ,"%2.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2)  
           sprintf(char_buffer ,"%2.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3)  
           sprintf(char_buffer ,"%2.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4)  
           sprintf(char_buffer ,"%2.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="-")
           { 
           if(precision==1) 
           sprintf(char_buffer ,"%-2.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2) 
           sprintf(char_buffer ,"%-2.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3) 
           sprintf(char_buffer ,"%-2.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4) 
           sprintf(char_buffer ,"%-2.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           { 
           if(precision==1)
           sprintf(char_buffer ,"%+2.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2)
           sprintf(char_buffer ,"%+2.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3)
           sprintf(char_buffer ,"%+2.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4)
           sprintf(char_buffer ,"%+2.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           { 
           if(precision==1)
           sprintf(char_buffer ,"%02.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2)
           sprintf(char_buffer ,"%02.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3)
           sprintf(char_buffer ,"%02.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4)
           sprintf(char_buffer ,"%02.4f", vaule); putString(x0,y0,char_buffer);
           }
           break; 
    case 3: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%3.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%3.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%3.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%3.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="-")
           {
           if(precision==1)
           sprintf(char_buffer ,"%-3.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2)
           sprintf(char_buffer ,"%-3.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3)
           sprintf(char_buffer ,"%-3.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4)
           sprintf(char_buffer ,"%-3.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
           if(precision==1)
           sprintf(char_buffer ,"%+3.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2)
           sprintf(char_buffer ,"%+3.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3)
           sprintf(char_buffer ,"%+3.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4)
           sprintf(char_buffer ,"%+3.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
           if(precision==1)
           sprintf(char_buffer ,"%03.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2)
           sprintf(char_buffer ,"%03.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3)
           sprintf(char_buffer ,"%03.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4)
           sprintf(char_buffer ,"%03.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 4: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%4.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%4.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%4.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%4.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="-")
           { 
            if(precision==1)
            sprintf(char_buffer ,"%-4.1f", vaule);  putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-4.2f", vaule);  putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-4.3f", vaule);  putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-4.4f", vaule);  putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {   
            if(precision==1)
            sprintf(char_buffer ,"%+4.1f", vaule);  putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+4.2f", vaule);  putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+4.3f", vaule);  putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+4.4f", vaule);  putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           { 
            if(precision==1)
            sprintf(char_buffer ,"%04.1f", vaule);  putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%04.2f", vaule);  putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%04.3f", vaule);  putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%04.4f", vaule);  putString(x0,y0,char_buffer);
           }
           break;
    case 5: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%5.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%5.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%5.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%5.4f", vaule); putString(x0,y0,char_buffer);
           }    
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-5.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-5.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-5.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-5.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+5.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+5.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+5.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+5.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%05.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%05.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%05.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%05.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 6: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%6.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%6.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%6.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%6.4f", vaule); putString(x0,y0,char_buffer);
           }    
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-6.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-6.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-6.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-6.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+6.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+6.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+6.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+6.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%06.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%06.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%06.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%06.4f", vaule); putString(x0,y0,char_buffer);
           }
           break; 
    case 7: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%7.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%7.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%7.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%7.4f", vaule); putString(x0,y0,char_buffer);
           } 
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-7.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-7.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-7.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-7.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+7.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+7.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+7.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+7.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%07.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%07.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%07.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%07.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 8: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%8.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%8.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%8.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%8.4f", vaule); putString(x0,y0,char_buffer);
           }     
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-8.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-8.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-8.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-8.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+8.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+8.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+8.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+8.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%08.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%08.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%08.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%08.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 9:
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%9.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%9.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%9.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%9.4f", vaule); putString(x0,y0,char_buffer);
           }  
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-9.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-9.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-9.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-9.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+9.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+9.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+9.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+9.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%09.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%09.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%09.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%09.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 10:
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%10.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%10.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%10.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%10.4f", vaule); putString(x0,y0,char_buffer);
           }  
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-10.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-10.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-10.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-10.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+10.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+10.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+10.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+10.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%010.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%010.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%010.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%010.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 11: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%11.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%11.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%11.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%11.4f", vaule); putString(x0,y0,char_buffer);
           } 
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-11.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-11.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-11.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-11.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+11.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+11.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+11.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+11.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%011.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%011.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%011.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%011.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;       
    default:   
           break;    
  }
  SPI.endTransaction();
}
//**************************************************************//
//vaule: 0x00000000 ~ 0xffffffff
//len: 1~11 minimum output length
/*
  [flag] 
    n:靠右,補空白　
    #:強制輸出 0x 作為開頭.
    0:在開頭處(左側) 補 0，而非補空白。
    x:強制輸出 0x 作為開頭，補 0。 
 */
//**************************************************************//
void Ra8876_Lite:: putHex(ru16 x0,ru16 y0,ru32 vaule,ru8 len,const char *flag)
{
  SPI.beginTransaction(m_spiSettings);

  char char_buffer[12];
  switch(len)
  {
    case 1:
           if(flag=="n")
           {sprintf(char_buffer ,"%1x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%01x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#1x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#01x", vaule); putString(x0,y0,char_buffer);}
           break;
    case 2:
           if(flag=="n")
           {sprintf(char_buffer ,"%2x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           { sprintf(char_buffer ,"%02x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           { sprintf(char_buffer ,"%#2x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           { sprintf(char_buffer ,"%#02x", vaule); putString(x0,y0,char_buffer);}
           break; 
    case 3: 
           if(flag=="n")
           {sprintf(char_buffer ,"%3x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%03x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#3x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#03x", vaule); putString(x0,y0,char_buffer);}
           break;
    case 4: 
           if(flag=="n")
           {sprintf(char_buffer ,"%4x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           { sprintf(char_buffer ,"%04x", vaule);  putString(x0,y0,char_buffer);}
           else if(flag=="#")
           { sprintf(char_buffer ,"%#4x", vaule);  putString(x0,y0,char_buffer);}
           else if(flag=="x")
           { sprintf(char_buffer ,"%#04x", vaule);  putString(x0,y0,char_buffer);}
           break;
    case 5: 
           if(flag=="n")
           {sprintf(char_buffer ,"%5x", vaule); putString(x0,y0,char_buffer);}    
           else if(flag=="0")
           {sprintf(char_buffer ,"%05x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#5x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#05x", vaule); putString(x0,y0,char_buffer);}
           break;
    case 6: 
           if(flag=="n")
           {sprintf(char_buffer ,"%6x", vaule); putString(x0,y0,char_buffer);}    
           else if(flag=="0")
           {sprintf(char_buffer ,"%06x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#6x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#06x", vaule); putString(x0,y0,char_buffer);}
           break; 
    case 7: 
           if(flag=="n")
           {sprintf(char_buffer ,"%7x", vaule); putString(x0,y0,char_buffer);} 
           else if(flag=="0")
           {sprintf(char_buffer ,"%07x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#7x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#07x", vaule); putString(x0,y0,char_buffer);}
           break;
    case 8: 
           if(flag=="n")
           {sprintf(char_buffer ,"%8x", vaule); putString(x0,y0,char_buffer);}     
           else if(flag=="0")
           {sprintf(char_buffer ,"%08x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#8x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#08x", vaule); putString(x0,y0,char_buffer);}
           break;
    case 9: 
           if(flag=="n")
           {sprintf(char_buffer ,"%9x", vaule); putString(x0,y0,char_buffer);} 
           else if(flag=="0")
           {sprintf(char_buffer ,"%09x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#9x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#09x", vaule); putString(x0,y0,char_buffer);}
           break;
    case 10: 
           if(flag=="n")
           {sprintf(char_buffer ,"%10x", vaule); putString(x0,y0,char_buffer);}     
           else if(flag=="0")
           {sprintf(char_buffer ,"%010x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#10x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#010x", vaule); putString(x0,y0,char_buffer);}
           break;
      
    default:   
           break;    
  }
  SPI.endTransaction();
}
 /*draw function*/
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawLine(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 color)
{
  SPI.beginTransaction(m_spiSettings);

  //Color
  foreGroundColor16bpp(color);

  //First point
  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh

  //Second point
  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh        

  //Draw
  lcdRegDataWrite(RA8876_DCR0,RA8876_DRAW_LINE);//67h,0x80

  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawSquare(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 color)
{
  SPI.beginTransaction(m_spiSettings);

  //Color
  foreGroundColor16bpp(color);

  //First point
  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh

  //Second point
  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh        

  //Draw
  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_SQUARE);//76h,0xa0

  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawSquareFill(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 color)
{
  SPI.beginTransaction(m_spiSettings);

  //Color
  foreGroundColor16bpp(color);

  //First point
  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh

  //Second point
  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh        

  //Draw
  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_SQUARE_FILL);//76h,0xe0

  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawCircleSquare(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 xr, ru16 yr, ru16 color)
{
  SPI.beginTransaction(m_spiSettings);

  //Color
  foreGroundColor16bpp(color);

  //First point
  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh

  //Second point
  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh    

  //Radius
  lcdRegDataWrite(RA8876_ELL_A0,xr);//77h    
  lcdRegDataWrite(RA8876_ELL_A1,xr>>8);//79h 
  lcdRegDataWrite(RA8876_ELL_B0,yr);//7ah    
  lcdRegDataWrite(RA8876_ELL_B1,yr>>8);//7bh

  //Draw
  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_CIRCLE_SQUARE);//76h,0xb0

  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawCircleSquareFill(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 xr, ru16 yr, ru16 color)
{
  SPI.beginTransaction(m_spiSettings);

  //Color
  foreGroundColor16bpp(color);

  //First point
  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh

  //Second point
  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh    

  //Radius
  lcdRegDataWrite(RA8876_ELL_A0,xr);//77h    
  lcdRegDataWrite(RA8876_ELL_A1,xr>>8);//78h 
  lcdRegDataWrite(RA8876_ELL_B0,yr);//79h    
  lcdRegDataWrite(RA8876_ELL_B1,yr>>8);//7ah

  //Draw
  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_CIRCLE_SQUARE_FILL);//76h,0xf0

  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawTriangle(ru16 x0,ru16 y0,ru16 x1,ru16 y1,ru16 x2,ru16 y2,ru16 color)
{
  SPI.beginTransaction(m_spiSettings);

  //Color
  foreGroundColor16bpp(color);

  //First point
  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh

  //Second point
  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh  

  //Third point
  lcdRegDataWrite(RA8876_DTPH0,x2);//70h
  lcdRegDataWrite(RA8876_DTPH1,x2>>8);//71h
  lcdRegDataWrite(RA8876_DTPV0,y2);//72h
  lcdRegDataWrite(RA8876_DTPV1,y2>>8);//73h  

  //Draw
  lcdRegDataWrite(RA8876_DCR0,RA8876_DRAW_TRIANGLE);//67h,0x82

  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawTriangleFill(ru16 x0,ru16 y0,ru16 x1,ru16 y1,ru16 x2,ru16 y2,ru16 color)
{
  SPI.beginTransaction(m_spiSettings);

  //Color
  foreGroundColor16bpp(color);

  //First point
  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh

  //Second point
  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh  

  //Third point
  lcdRegDataWrite(RA8876_DTPH0,x2);//70h
  lcdRegDataWrite(RA8876_DTPH1,x2>>8);//71h
  lcdRegDataWrite(RA8876_DTPV0,y2);//72h
  lcdRegDataWrite(RA8876_DTPV1,y2>>8);//73h  

  //Draw
  lcdRegDataWrite(RA8876_DCR0,RA8876_DRAW_TRIANGLE_FILL);//67h,0xa2

  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawCircle(ru16 x0,ru16 y0,ru16 r,ru16 color)
{
  SPI.beginTransaction(m_spiSettings);

  //Color
  foreGroundColor16bpp(color);

  //Central point
  lcdRegDataWrite(RA8876_DEHR0,x0);//7bh
  lcdRegDataWrite(RA8876_DEHR1,x0>>8);//7ch
  lcdRegDataWrite(RA8876_DEVR0,y0);//7dh
  lcdRegDataWrite(RA8876_DEVR1,y0>>8);//7eh

  //Radius
  lcdRegDataWrite(RA8876_ELL_A0,r);//77h    
  lcdRegDataWrite(RA8876_ELL_A1,r>>8);//78h 
  lcdRegDataWrite(RA8876_ELL_B0,r);//79h    
  lcdRegDataWrite(RA8876_ELL_B1,r>>8);//7ah

  //Draw
  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_CIRCLE);//76h,0x80

  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawCircleFill(ru16 x0,ru16 y0,ru16 r,ru16 color)
{
  SPI.beginTransaction(m_spiSettings);

  //Color
  foreGroundColor16bpp(color);

  //Central point
  lcdRegDataWrite(RA8876_DEHR0,x0);//7bh
  lcdRegDataWrite(RA8876_DEHR1,x0>>8);//7ch
  lcdRegDataWrite(RA8876_DEVR0,y0);//7dh
  lcdRegDataWrite(RA8876_DEVR1,y0>>8);//7eh

  //Radius
  lcdRegDataWrite(RA8876_ELL_A0,r);//77h    
  lcdRegDataWrite(RA8876_ELL_A1,r>>8);//78h 
  lcdRegDataWrite(RA8876_ELL_B0,r);//79h    
  lcdRegDataWrite(RA8876_ELL_B1,r>>8);//7ah

  //Draw
  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_CIRCLE_FILL);//76h,0xc0

  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawEllipse(ru16 x0,ru16 y0,ru16 xr,ru16 yr,ru16 color)
{
  SPI.beginTransaction(m_spiSettings);

  //Color
  foreGroundColor16bpp(color);

  //Central point
  lcdRegDataWrite(RA8876_DEHR0,x0);//7bh
  lcdRegDataWrite(RA8876_DEHR1,x0>>8);//7ch
  lcdRegDataWrite(RA8876_DEVR0,y0);//7dh
  lcdRegDataWrite(RA8876_DEVR1,y0>>8);//7eh

  //Radius
  lcdRegDataWrite(RA8876_ELL_A0,xr);//77h    
  lcdRegDataWrite(RA8876_ELL_A1,xr>>8);//78h 
  lcdRegDataWrite(RA8876_ELL_B0,yr);//79h    
  lcdRegDataWrite(RA8876_ELL_B1,yr>>8);//7ah

  //Draw
  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_ELLIPSE);//76h,0x80

  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawEllipseFill(ru16 x0,ru16 y0,ru16 xr,ru16 yr,ru16 color)
{
  SPI.beginTransaction(m_spiSettings);

  //Color
  foreGroundColor16bpp(color);

  //Central point
  lcdRegDataWrite(RA8876_DEHR0,x0);//7bh
  lcdRegDataWrite(RA8876_DEHR1,x0>>8);//7ch
  lcdRegDataWrite(RA8876_DEVR0,y0);//7dh
  lcdRegDataWrite(RA8876_DEVR1,y0>>8);//7eh

  //Radius
  lcdRegDataWrite(RA8876_ELL_A0,xr);//77h    
  lcdRegDataWrite(RA8876_ELL_A1,xr>>8);//78h 
  lcdRegDataWrite(RA8876_ELL_B0,yr);//79h    
  lcdRegDataWrite(RA8876_ELL_B1,yr>>8);//7ah

  //Draw
  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_ELLIPSE_FILL);//76h,0xc0

  check2dBusy();

  SPI.endTransaction();
}

/*BTE function*/
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bteMemoryCopy(ru32 s0_addr,ru16 s0_image_width,ru16 s0_x,ru16 s0_y,ru32 des_addr,ru16 des_image_width, 
                                ru16 des_x,ru16 des_y,ru16 copy_width,ru16 copy_height)
{
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

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_CODE_12<<4|RA8876_BTE_MEMORY_COPY_WITH_ROP);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h

  check2dBusy();

  SPI.endTransaction();
} 
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bteMemoryCopyWithROP(ru32 s0_addr,ru16 s0_image_width,ru16 s0_x,ru16 s0_y,ru32 s1_addr,ru16 s1_image_width,ru16 s1_x,ru16 s1_y,
                                       ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 copy_width,ru16 copy_height,ru8 rop_code)
{
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

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,rop_code<<4|RA8876_BTE_MEMORY_COPY_WITH_ROP);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h

  check2dBusy();

  SPI.endTransaction();
} 
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bteMemoryCopyWithChromaKey(ru32 s0_addr,ru16 s0_image_width,ru16 s0_x,ru16 s0_y,
ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 copy_width,ru16 copy_height, ru16 chromakey_color)
{
  bte_Source0_MemoryStartAddr(s0_addr);
  bte_Source0_ImageWidth(s0_image_width);
  bte_Source0_WindowStartXY(s0_x,s0_y);
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(copy_width,copy_height);
  backGroundColor16bpp(chromakey_color);

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_MEMORY_COPY_WITH_CHROMA);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
  
  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bteMpuWriteWithROP(ru32 s1_addr,ru16 s1_image_width,ru16 s1_x,ru16 s1_y,ru32 des_addr,ru16 des_image_width,
ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru8 rop_code,const unsigned char *data)
{
   ru16 i,j;
  bte_Source1_MemoryStartAddr(s1_addr);
  bte_Source1_ImageWidth(s1_image_width);
  bte_Source1_WindowStartXY(s1_x,s1_y);
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,rop_code<<4|RA8876_BTE_MPU_WRITE_WITH_ROP);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
  
  for(i=0;i< height;i++)
  {	
   for(j=0;j< (width*2);j++)
   {
    checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    }
   }
  checkWriteFifoEmpty();

  SPI.endTransaction();
}

//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bteMpuWriteWithROP(ru32 s1_addr,ru16 s1_image_width,ru16 s1_x,ru16 s1_y,ru32 des_addr,ru16 des_image_width,
ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru8 rop_code,const unsigned short *data)
{
   ru16 i,j;
  bte_Source1_MemoryStartAddr(s1_addr);
  bte_Source1_ImageWidth(s1_image_width);
  bte_Source1_WindowStartXY(s1_x,s1_y);
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,rop_code<<4|RA8876_BTE_MPU_WRITE_WITH_ROP);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
  
 for(j=0;j<height;j++)
 {
  for(i=0;i<width;i++)
  {
   checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
   lcdDataWrite16bbp(*data);
   data++;
   //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
  }
 } 
  checkWriteFifoEmpty();

  SPI.endTransaction();
}
//**************************************************************//
//write data after setting
//**************************************************************//
void Ra8876_Lite::bteMpuWriteWithROP(ru32 s1_addr,ru16 s1_image_width,ru16 s1_x,ru16 s1_y,ru32 des_addr,ru16 des_image_width,
ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru8 rop_code)
{
   ru16 i,j;
  bte_Source1_MemoryStartAddr(s1_addr);
  bte_Source1_ImageWidth(s1_image_width);
  bte_Source1_WindowStartXY(s1_x,s1_y);
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,rop_code<<4|RA8876_BTE_MPU_WRITE_WITH_ROP);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
  ramAccessPrepare();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bteMpuWriteWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru16 chromakey_color,const unsigned char *data)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);
  backGroundColor16bpp(chromakey_color);

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_MPU_WRITE_WITH_CHROMA);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
  
  for(i=0;i< height;i++)
  {	
   for(j=0;j< (width*2);j++)
   {
    checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    }
   }
  checkWriteFifoEmpty();
  
  SPI.endTransaction();
}

//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bteMpuWriteWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru16 chromakey_color,const unsigned short *data)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);
  backGroundColor16bpp(chromakey_color);

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_MPU_WRITE_WITH_CHROMA);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
  
 for(j=0;j<height;j++)
 {
  for(i=0;i<width;i++)
  {
   checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
   lcdDataWrite16bbp(*data);
   data++;
   //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
  }
 } 
  checkWriteFifoEmpty();

  SPI.endTransaction();
}
//**************************************************************//
//write data after setting
//**************************************************************//
void Ra8876_Lite::bteMpuWriteWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru16 chromakey_color)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);
  backGroundColor16bpp(chromakey_color);

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_MPU_WRITE_WITH_CHROMA);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
  ramAccessPrepare();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bteMpuWriteColorExpansion(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru16 foreground_color,ru16 background_color,const unsigned char *data)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);
  foreGroundColor16bpp(foreground_color);
  backGroundColor16bpp(background_color);

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_BUS_WIDTH8<<4|RA8876_BTE_MPU_WRITE_COLOR_EXPANSION);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
  
  for(i=0;i< height;i++)
  {	
   for(j=0;j< (width/8);j++)
   {
    checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    }
   }
  checkWriteFifoEmpty();
  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
//write data after setting
//**************************************************************//
void Ra8876_Lite::bteMpuWriteColorExpansion(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru16 foreground_color,ru16 background_color)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height); 
  foreGroundColor16bpp(foreground_color);
  backGroundColor16bpp(background_color);

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_BUS_WIDTH8<<4|RA8876_BTE_MPU_WRITE_COLOR_EXPANSION);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
  ramAccessPrepare();

  SPI.endTransaction();
}
//**************************************************************//
/*background_color do not set the same as foreground_color*/
//**************************************************************//
void Ra8876_Lite::bteMpuWriteColorExpansionWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru16 foreground_color,ru16 background_color, const unsigned char *data)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);
  foreGroundColor16bpp(foreground_color);
  backGroundColor16bpp(background_color);

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_BUS_WIDTH8<<4|RA8876_BTE_MPU_WRITE_COLOR_EXPANSION_WITH_CHROMA);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
  
  for(i=0;i< height;i++)
  {	
   for(j=0;j< (width/8);j++)
   {
    checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    }
   }
  checkWriteFifoEmpty();
  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
/*background_color do not set the same as foreground_color*/
//write data after setting
//**************************************************************//
void Ra8876_Lite::bteMpuWriteColorExpansionWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru16 foreground_color,ru16 background_color)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);
  foreGroundColor16bpp(foreground_color);
  backGroundColor16bpp(background_color);

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_BUS_WIDTH8<<4|RA8876_BTE_MPU_WRITE_COLOR_EXPANSION_WITH_CHROMA);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
  ramAccessPrepare();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite:: btePatternFill(ru8 p8x8or16x16, ru32 s0_addr,ru16 s0_image_width,ru16 s0_x,ru16 s0_y,
                                 ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height)
{ 
  bte_Source0_MemoryStartAddr(s0_addr);
  bte_Source0_ImageWidth(s0_image_width);
  bte_Source0_WindowStartXY(s0_x,s0_y);
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height); 

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_CODE_12<<4|RA8876_BTE_PATTERN_FILL_WITH_ROP);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  
  if(p8x8or16x16 == 0)
   lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4|RA8876_PATTERN_FORMAT8X8);//90h
  else
   lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4|RA8876_PATTERN_FORMAT16X16);//90h
   
  check2dBusy();

  SPI.endTransaction();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite:: btePatternFillWithChromaKey(ru8 p8x8or16x16, ru32 s0_addr,ru16 s0_image_width,ru16 s0_x,ru16 s0_y,
                                 ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru16 chromakey_color)
{
  bte_Source0_MemoryStartAddr(s0_addr);
  bte_Source0_ImageWidth(s0_image_width);
  bte_Source0_WindowStartXY(s0_x,s0_y);
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);
  backGroundColor16bpp(chromakey_color); 

  SPI.beginTransaction(m_spiSettings);

  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_CODE_12<<4|RA8876_BTE_PATTERN_FILL_WITH_CHROMA);//91h
  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
  if(p8x8or16x16 == 0)
   lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4|RA8876_PATTERN_FORMAT8X8);//90h
  else
   lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4|RA8876_PATTERN_FORMAT16X16);//90h
  check2dBusy();

  SPI.endTransaction();
}

 /*DMA Function*/
 //**************************************************************//
 /*If used 32bit address serial flash through ra8876, must be set command to serial flash to enter 4bytes mode first.
 only needs set one times after power on */
 //**************************************************************//
 void Ra8876_Lite:: setSerialFlash4BytesMode(ru8 scs_select)
 {
    SPI.beginTransaction(m_spiSettings);

    if(scs_select==0)
    {
    lcdRegDataWrite( RA8876_SPIMCR2, RA8876_SPIM_NSS_SELECT_0<<5|RA8876_SPIM_MODE0);//b9h
    lcdRegDataWrite( RA8876_SPIMCR2, RA8876_SPIM_NSS_SELECT_0<<5|RA8876_SPIM_NSS_ACTIVE<<4|RA8876_SPIM_MODE0);//b9h 
    lcdRegWrite( RA8876_SPIDR);//b8h
    delay(1);
    lcdDataWrite(0xB7);//
    delay(1);
    lcdRegDataWrite( RA8876_SPIMCR2, RA8876_SPIM_NSS_SELECT_0<<5|RA8876_SPIM_NSS_INACTIVE<<4|RA8876_SPIM_MODE0);//b9h 
    }
    if(scs_select==1)
    {
    lcdRegDataWrite( RA8876_SPIMCR2 ,RA8876_SPIM_NSS_SELECT_1<<5|RA8876_SPIM_MODE0);//b9h
    lcdRegDataWrite( RA8876_SPIMCR2, RA8876_SPIM_NSS_SELECT_1<<5|RA8876_SPIM_NSS_ACTIVE<<4|RA8876_SPIM_MODE0);//b9h
    lcdRegWrite( RA8876_SPIDR);//b8h
    delay(1);
    lcdDataWrite(0xB7);//
    delay(1);
    lcdRegDataWrite( RA8876_SPIMCR2, RA8876_SPIM_NSS_SELECT_1<<5|RA8876_SPIM_NSS_INACTIVE<<4|RA8876_SPIM_MODE0);//b9h 
    } 

    SPI.endTransaction();
 }
//**************************************************************//
/* scs = 0 : select scs0, scs = 1 : select scs1, */
//**************************************************************//
 void Ra8876_Lite:: dma_24bitAddressBlockMode(ru8 scs_select,ru8 clk_div,ru16 x0,ru16 y0,ru16 width,ru16 height,ru16 picture_width,ru32 addr)
 {
    SPI.beginTransaction(m_spiSettings);

    if(scs_select==0)
      lcdRegDataWrite(RA8876_SFL_CTRL,RA8876_SERIAL_FLASH_SELECT0<<7|RA8876_SERIAL_FLASH_DMA_MODE<<6|RA8876_SERIAL_FLASH_ADDR_24BIT<<5|RA8876_FOLLOW_RA8876_MODE<<4|RA8876_SPI_FAST_READ_8DUMMY);//b7h
    if(scs_select==1)
      lcdRegDataWrite(RA8876_SFL_CTRL,RA8876_SERIAL_FLASH_SELECT1<<7|RA8876_SERIAL_FLASH_DMA_MODE<<6|RA8876_SERIAL_FLASH_ADDR_24BIT<<5|RA8876_FOLLOW_RA8876_MODE<<4|RA8876_SPI_FAST_READ_8DUMMY);//b7h
  
    lcdRegDataWrite(RA8876_SPI_DIVSOR,clk_div);//bbh  
    lcdRegDataWrite(RA8876_DMA_DX0,x0);//c0h
    lcdRegDataWrite(RA8876_DMA_DX1,x0>>8);//c1h
    lcdRegDataWrite(RA8876_DMA_DY0,y0);//c2h
    lcdRegDataWrite(RA8876_DMA_DY1,y0>>8);//c3h 
    lcdRegDataWrite(RA8876_DMAW_WTH0,width);//c6h
    lcdRegDataWrite(RA8876_DMAW_WTH1,width>>8);//c7h
    lcdRegDataWrite(RA8876_DMAW_HIGH0,height);//c8h
    lcdRegDataWrite(RA8876_DMAW_HIGH1,height>>8);//c9h 
    lcdRegDataWrite(RA8876_DMA_SWTH0,picture_width);//cah
    lcdRegDataWrite(RA8876_DMA_SWTH1,picture_width>>8);//cbh 
    lcdRegDataWrite(RA8876_DMA_SSTR0,addr);//bch
    lcdRegDataWrite(RA8876_DMA_SSTR1,addr>>8);//bdh
    lcdRegDataWrite(RA8876_DMA_SSTR2,addr>>16);//beh
    lcdRegDataWrite(RA8876_DMA_SSTR3,addr>>24);//bfh 
    
    lcdRegDataWrite(RA8876_DMA_CTRL,RA8876_DMA_START);//b6h 
    check2dBusy(); 

    SPI.endTransaction();
 }
 //**************************************************************//
/* scs = 0 : select scs0, scs = 1 : select scs1, */
//**************************************************************//
 void Ra8876_Lite:: dma_32bitAddressBlockMode(ru8 scs_select,ru8 clk_div,ru16 x0,ru16 y0,ru16 width,ru16 height,ru16 picture_width,ru32 addr)
 {
    SPI.beginTransaction(m_spiSettings);

    if(scs_select==0)
      lcdRegDataWrite(RA8876_SFL_CTRL,RA8876_SERIAL_FLASH_SELECT0<<7|RA8876_SERIAL_FLASH_DMA_MODE<<6|RA8876_SERIAL_FLASH_ADDR_32BIT<<5|RA8876_FOLLOW_RA8876_MODE<<4|RA8876_SPI_FAST_READ_8DUMMY);//b7h
    if(scs_select==1)
      lcdRegDataWrite(RA8876_SFL_CTRL,RA8876_SERIAL_FLASH_SELECT1<<7|RA8876_SERIAL_FLASH_DMA_MODE<<6|RA8876_SERIAL_FLASH_ADDR_32BIT<<5|RA8876_FOLLOW_RA8876_MODE<<4|RA8876_SPI_FAST_READ_8DUMMY);//b7h
  
    lcdRegDataWrite(RA8876_SPI_DIVSOR,clk_div);//bbh 
    
    lcdRegDataWrite(RA8876_DMA_DX0,x0);//c0h
    lcdRegDataWrite(RA8876_DMA_DX1,x0>>8);//c1h
    lcdRegDataWrite(RA8876_DMA_DY0,y0);//c2h
    lcdRegDataWrite(RA8876_DMA_DY1,y0>>8);//c3h 
    lcdRegDataWrite(RA8876_DMAW_WTH0,width);//c6h
    lcdRegDataWrite(RA8876_DMAW_WTH1,width>>8);//c7h
    lcdRegDataWrite(RA8876_DMAW_HIGH0,height);//c8h
    lcdRegDataWrite(RA8876_DMAW_HIGH1,height>>8);//c9h 
    lcdRegDataWrite(RA8876_DMA_SWTH0,picture_width);//cah
    lcdRegDataWrite(RA8876_DMA_SWTH1,picture_width>>8);//cbh 
    lcdRegDataWrite(RA8876_DMA_SSTR0,addr);//bch
    lcdRegDataWrite(RA8876_DMA_SSTR1,addr>>8);//bdh
    lcdRegDataWrite(RA8876_DMA_SSTR2,addr>>16);//beh
    lcdRegDataWrite(RA8876_DMA_SSTR3,addr>>24);//bfh  
    
    lcdRegDataWrite(RA8876_DMA_CTRL,RA8876_DMA_START);//b6h 
    check2dBusy(); 

    SPI.endTransaction();
 }
 

