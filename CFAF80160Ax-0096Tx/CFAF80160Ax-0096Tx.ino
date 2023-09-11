//==============================================================================
//
//  CRYSTALFONTZ
//
//  The controller is a Sitronix ST7735S
//    https://www.crystalfontz.com/controllers/Sitronix/ST7735S/
//
//  Seeeduino, an open-source 3.3v capable Arduino clone.
//    https://www.crystalfontz.com/product/cfapn15062-seeeduino-arduino-clone-microprocessor
//    https://github.com/SeeedDocument/SeeeduinoV4/raw/master/resources/Seeeduino_v4.2_sch.pdf
//
//==============================================================================
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>
//
// Connection guide:
// LCD 4-wire SPI pins and control lines on Seeeduino:
// ARD       | Port  | Display pin      | Function                                |
//-----------+-------+------------------+-----------------------------------------+
// 3.3V      |       |                  | POWER 3.3V                              |
// 3.3V      |       | 03               | Interface selection           (SPI4W)   |
// GND       |       |                  | GROUND                                  |
//-----------+-------+------------------+-----------------------------------------+
// D8        | PORTB | 10               | Data/command                    (D/C)   |
// D9        | PORTB | 07               | Reset                           (RST)   |
// D10       | PORTB | 06               | Chip select                     (CS)    |
// D11       | PORTB | 08               | Serial data                     (SDA)   |
// D13       | PORTB | 09               | Serial clock                    (SCK)   |
//-----------+-------+------------------+-----------------------------------------+
//=================================================================================
// Capacitive touch screen connection:
// ARD       | Port  | Touchscreen pin  | Function                                |
//-----------+-------+------------------+-----------------------------------------+
// 3.3V      |       |                  | POWER 3.3V                              |
// GND       |       |                  | GROUND                                  |
//-----------+-------+------------------+-----------------------------------------+
// D2        | PORTD | RESET            | Reset                                   |
// D17       | PORTB | INT              | Interrupt                               |
// D18       | PORTB | SDA              | I2C serial data                         |
// D19       | PORTB | SCL              | I2C clock                               |
//-----------+-------+------------------+-----------------------------------------+
// Interface Selection
// SPI4W | Interface mode  |
//-------+-----------------+
// 0     | 3-wire SPI      |
// 1     | 4-wire SPI      |
//-------+-----------------+
//=================================================================================
// SD card connection (using CFA10112)
//  ARD      | Port  | Adapter pin |  Function                                    |
//-----------+-------+-------------+----------------------------------------------+
// 3.3V      |       |  3.3V       |  POWER 3.3V                                  |
// GND       |       |  GND        |  GROUND                                      |
//-----------+-------+-------------+----------------------------------------------+
// D7        | PORTD |  CS         |  Chip select                         (CS)    |
// D11       | PORTB |  DI         |  Serial data in                      (DI)    |
// D12       | PORTB |  DO         |  Serial data out                     (DO)    |
// D13       | PORTB |  SCK        |  Serial clock                        (SCK)   |
//-----------+-------+-------------+----------------------------------------------+

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

// define the Arduino pin locations per the table above
#define MOSI (11)
#define SCL (13)
#define DI (8)
#define RST (9)
#define CS (10)

// define the capacitive touch pin locations
#define TCH_RES (2)
#define INTERRUPT (17)
#define I2C_SDA (18)
#define I2C_SCL (19)

// define clear and set commands for each of the pins
#define MOSI_MASK (0x08)
#define CLR_MOSI (PORTB &= ~(MOSI_MASK))
#define SET_MOSI (PORTB |= (MOSI_MASK))

#define SCL_MASK (0x20)
#define CLR_SCL (PORTB &= ~(SCL_MASK))
#define SET_SCL (PORTB |= (SCL_MASK))

#define DI_MASK (0x01)
#define CLR_DI (PORTB &= ~(DI_MASK))
#define SET_DI (PORTB |= (DI_MASK))

#define RST_MASK (0x02)
#define CLR_RST (PORTB &= ~(RST_MASK))
#define SET_RST (PORTB |= (RST_MASK))

#define CS_MASK (0x04)
#define CLR_CS (PORTB &= ~(CS_MASK))
#define SET_CS (PORTB |= (CS_MASK))

//=================================================================================

// ST7735S defines

#define ST7735_SLPOUT (0x11)
#define ST7735S_DISPINVON (0x21)
#define ST7735_DISPON (0x29)
#define ST7735_CASET (0x2A)
#define ST7735_RASET (0x2B)
#define ST7735_RAMWR (0x2C)
#define ST7735_MADCTL (0x36)
#define ST7735_COLMOD (0x3A)
#define ST7735_FRMCTR1 (0xB1)
#define ST7735_FRMCTR2 (0xB2)
#define ST7735_FRMCTR3 (0xB3)
#define ST7735_INVCTR (0xB4)
#define ST7735_PWCTR1 (0xC0)
#define ST7735_PWCTR2 (0xC1)
#define ST7735_PWCTR3 (0xC2)
#define ST7735_PWCTR4 (0xC3)
#define ST7735_PWCTR5 (0xC4)
#define ST7735_VMCTR1 (0xC5)
#define ST7735_GMCTRP1 (0xE0)
#define ST7735_GMCTRN1 (0xE1)

// touch screen controller defines
#define FT_NOP_MODE (0x00)
#define FT_REG_NUM_FINGER (0x02)
#define FT_TP1_REG (0X03)
#define FT_TP2_REG (0X09)
#define FT_ID_G_LIB_VERSION (0xA1)
#define FT_ID_G_MODE (0xA4)
#define FT_ID_G_TRANSMISSION (0x80)
#define FT_ID_G_PERIODACTIVE (0x88)
#define NVM_ADDR_DATA_WR (0xD0)

//=================================================================================

// SPI send command and data functions
void SPI_sendCommand(uint8_t command)
{
  CLR_DI; // command mode

  CLR_CS; // falling edge enables serial interface and start of transmission

  SPI.transfer(command);

  SET_CS; // end transmission
}

void SPI_sendData(uint8_t data)
{
  SET_DI; // data mode

  CLR_CS; // start transmission

  SPI.transfer(data);

  SET_CS; // end transmission
}

//=================================================================================

// LCD initialization sequence
void Initialize_LCD(void)
{
  // reset the LCD controller
  CLR_RST;
  delay(1);
  SET_RST;
  delay(150);

  // sleep out command
  SPI_sendCommand(ST7735_SLPOUT);
  delay(120);

  // frame rate control (in normal mode/ Full colors)
  SPI_sendCommand(ST7735_FRMCTR1);
  SPI_sendData(0X05);
  SPI_sendData(0X3C);
  SPI_sendData(0X3C);

  // frame rate control (in idle mode/ 8-colors)
  SPI_sendCommand(ST7735_FRMCTR2);
  SPI_sendData(0X05);
  SPI_sendData(0X3C);
  SPI_sendData(0X3C);

  // frame rate control (in partial mode/ full colors)
  SPI_sendCommand(ST7735_FRMCTR3);
  SPI_sendData(0X05);
  SPI_sendData(0X3C);
  SPI_sendData(0X3C);
  SPI_sendData(0X05);
  SPI_sendData(0X3C);
  SPI_sendData(0X3C);

  // display inversion control
  SPI_sendCommand(ST7735_INVCTR);
  SPI_sendData(0x07);

  // power control 1
  SPI_sendCommand(ST7735_PWCTR1);
  SPI_sendData(0xE9);

  SPI_sendData(0x09);

  // power control 2
  SPI_sendData(0x04);
  SPI_sendCommand(ST7735_PWCTR2);
  SPI_sendData(0xC5);

  // power control 3
  SPI_sendCommand(ST7735_PWCTR3);
  SPI_sendData(0x0D);
  SPI_sendData(0x00);

  // power control 4
  SPI_sendCommand(ST7735_PWCTR4);
  SPI_sendData(0x8D);

  SPI_sendData(0x6A);

  // power control 5
  SPI_sendCommand(ST7735_PWCTR5);
  SPI_sendData(0x8D);
  SPI_sendData(0xEE);

  // VCOM control 1
  SPI_sendCommand(ST7735_VMCTR1);
  SPI_sendData(0x15);

  // positive gamma control
  SPI_sendCommand(ST7735_GMCTRP1);
  SPI_sendData(0X07);
  SPI_sendData(0X0E);
  SPI_sendData(0X08);
  SPI_sendData(0X07);
  SPI_sendData(0X10);
  SPI_sendData(0X07);
  SPI_sendData(0X02);
  SPI_sendData(0X07);
  SPI_sendData(0X09);
  SPI_sendData(0X0F);
  SPI_sendData(0X25);
  SPI_sendData(0X36);
  SPI_sendData(0X00);
  SPI_sendData(0X08);
  SPI_sendData(0X04);
  SPI_sendData(0X10);

  // negative gamma control
  SPI_sendCommand(ST7735_GMCTRN1);
  SPI_sendData(0X0A);
  SPI_sendData(0X0D);
  SPI_sendData(0X08);
  SPI_sendData(0X07);
  SPI_sendData(0X0F);
  SPI_sendData(0X07);
  SPI_sendData(0X02);
  SPI_sendData(0X07);
  SPI_sendData(0X09);
  SPI_sendData(0X0F);
  SPI_sendData(0X25);
  SPI_sendData(0X35);
  SPI_sendData(0X00);
  SPI_sendData(0X09);
  SPI_sendData(0X04);
  SPI_sendData(0X10);

  // interface pixel format (16-bit)
  SPI_sendCommand(ST7735_COLMOD);
  SPI_sendData(0x05);

  // display on command
  SPI_sendCommand(ST7735_DISPON); // Display On
  delay(1);

  // memory data access control (B-G-R mode)
  SPI_sendCommand(ST7735_MADCTL);
  SPI_sendData(0xC0);

  // display inversion on
  SPI_sendCommand(ST7735S_DISPINVON);
}

//=================================================================================

// function to set write position on display
void setWrite_position(uint8_t x, uint8_t y)
{
  SPI_sendCommand(ST7735_CASET); // column address set function
  SPI_sendData(0x00);
  SPI_sendData(0x1A + x); // the x axis will increment according to the variable passed through
  SPI_sendData(0x00);
  SPI_sendData(0x69);

  SPI_sendCommand(ST7735_RASET); // row address set function
  SPI_sendData(0x00);
  SPI_sendData(0x01 + y); // the y axis will increment according to the variable passed through
  SPI_sendData(0x00);
  SPI_sendData(0xA0);

  SPI_sendCommand(ST7735_RAMWR);
}

// fill LCD function (one color)
void Fill_LCD(uint8_t R, uint8_t G, uint8_t B)
{
  register int i;
  uint8_t x1, x2;
  setWrite_position(0, 0); // set origin

  x1 = (R & 0xF8) | (G >> 5); // 5-6-5 conversion
  x2 = ((G & 0x1C) << 3) | (B >> 3);

  for (i = 0; i < (160 * 80); i++)
  {
    SPI_sendData(x1);
    SPI_sendData(x2);
  }
}

// draw colored bars on the display
void ColorBars()
{
  uint16_t number = 4;               // 4 bars
  uint16_t barHeight = 160 / number; // divide the colors into equal heights
  int i;
  int j;
  uint8_t R, G, B, x1, x2;

  // blue bar
  setWrite_position(0, 0);
  R = 0xFF;
  G = 0x00;
  B = 0x00;
  x1 = (R & 0xF8) | (G >> 5); // 5-6-5 conversion
  x2 = ((G & 0x1C) << 3) | (B >> 3);
  for (i = 0; i < barHeight; i++) // set the vertical limit as the height of one bar
  {
    for (j = 0; j < 80; j++)
    {
      SPI_sendData(x1);
      SPI_sendData(x2);
    }
  }

  // green bar
  setWrite_position(0, barHeight); // update the position to start from the end of the first bar
  R = 0x00;
  G = 0xFF;
  B = 0x00;
  x1 = (R & 0xF8) | (G >> 5); 
  x2 = ((G & 0x1C) << 3) | (B >> 3);
  for (i = 0; i < barHeight; i++)
  {
    for (j = 0; j < 80; j++)
    {
      SPI_sendData(x1);
      SPI_sendData(x2);
    }
  }

  // red bar
  setWrite_position(0, (barHeight * 2)); // update the position to start from the end of the second bar
  R = 0x00;
  G = 0x00;
  B = 0xFF;
  x1 = (R & 0xF8) | (G >> 5); 
  x2 = ((G & 0x1C) << 3) | (B >> 3);
  for (i = 0; i < barHeight; i++)
  {
    for (j = 0; j < 80; j++)
    {
      SPI_sendData(x1);
      SPI_sendData(x2);
    }
  }

  // purple bar
  setWrite_position(0, (barHeight * 3)); // update the position to start from the end of the third bar
  R = 0xFF;
  G = 0x00;
  B = 0xFF;
  x1 = (R & 0xF8) | (G >> 5); 
  x2 = ((G & 0x1C) << 3) | (B >> 3);
  for (i = 0; i < barHeight; i++)
  {
    for (j = 0; j < 80; j++)
    {
      SPI_sendData(x1);
      SPI_sendData(x2);
    }
  }
}

// function to show bmps in sd card
void show_BMPs_in_root(void)
{
  // these are the colors pulled from the uSD card
  uint8_t R;
  uint8_t G;
  uint8_t B;

  File root_dir;
  root_dir = SD.open("/");
  if (0 == root_dir)
  {
    Serial.println("show_BMPs_in_root: Can't open \"root\"");
    return;
  }
  File bmp_file;

  while (1)
  {

    bmp_file = root_dir.openNextFile();
    if (0 == bmp_file)
    {
      // no more files, break out of while()
      // root_dir will be closed below
      break;
    }
    // skip directories (what about volume name?)
    if (0 == bmp_file.isDirectory())
    {
      // the file name must include ".BMP"
      if (0 != strstr(bmp_file.name(), ".BMP"))
      {
        // the BMP must be exactly 38454 long
        //(this is correct for 80x160, 24-bit)
        Serial.println("size=");
        Serial.print(bmp_file.size());

        if (38454 == bmp_file.size())
        {
          // jump over BMP header. BMP must be 80x160 24-bit
          bmp_file.seek(54);

          static uint8_t read_a_line[80 * 3];         // 80*3 (R, G, B)
          for (uint16_t line = 0; line < 160; line++) // traverse the entire display
          {
            setWrite_position(0, (159 - line));
            bmp_file.read(read_a_line, 80 * 3);   // read a line
            SPI_send_pixels(80 * 3, read_a_line); // transfer the line to the display
          }
        }
      }
    }
    // release the BMP file handle
    bmp_file.close();
    delay(1000);
  }
  // release the root directory file handle
  root_dir.close();
}

// function to send read pixels to the display
void SPI_send_pixels(uint16_t byte_count, uint8_t *data_ptr)
{
  uint8_t subpixel;

  // select the LCD data register
  SET_DI;
  // select the LCD controller
  CLR_CS;

  uint8_t R;
  uint8_t G;
  uint8_t B;

  // load the first byte
  while (byte_count / 3)
  {
    // extract the R, G and B data
    R = *data_ptr;
    data_ptr++;

    G = *data_ptr;
    data_ptr++;

    B = *data_ptr;
    data_ptr++;

    // carry out the 5-6-5 conversion (16-bit)
    subpixel = (R & 0xF8) | (G >> 5);
    SPI.transfer(subpixel);
    subpixel = ((G & 0x1C) << 3) | (B >> 3);
    SPI.transfer(subpixel);

    // count this byte
    byte_count -= 3;
  }
  // deselect the LCD controller
  SET_CS;
}

//=================================================================================

// send command function for I2C
void sendCommand(uint8_t reg, uint8_t data)
{
  // start trasmission by sending address 0x38
  Wire.beginTransmission(0x38);
  Wire.write(reg);
  Wire.write(data);
  // send the data over
  Serial.println("return value: "); // a return of 0 means success
  Serial.println(Wire.endTransmission());
}

// setup function for touch controller
void tchSetup()
{
  Wire.begin();
  Wire.setClock(400000);

  digitalWrite(TCH_RES, LOW);
  delay(500);
  digitalWrite(TCH_RES, HIGH);
  delay(200);

  // send device mode
  sendCommand(FT_NOP_MODE, 0x00);

  // ID_G_MODE set interrupt
  sendCommand(FT_ID_G_MODE, 0x00);
  //  0x00 - Polling Mode
  //  0x01 - Trigger Mode

  // touch threshold
  sendCommand(FT_ID_G_TRANSMISSION, 20);

  // reporting rate
  sendCommand(FT_ID_G_PERIODACTIVE, 12);
}

void touchDemo()
{
  // set the interrupt threshold
  int THRESHOLD = (0.75 * 1023) / 3.3; // 0.75 is the threshold in this case
  Serial.println("TOP OF TOUCH DEMO");
  // draw a little X box
  for (int i = 0; i < 20; i++)
  {
    Fast_Horizontal_Line(1, i, 20, 0x00, 0x00, 0xFF);
  }
  drawLine(4, 2, 17, 18, 0xFF, 0xFF, 0xFF);
  drawLine(4, 18, 17, 2, 0xFF, 0xFF, 0xFF);

  // set the variables used in the demo
  uint8_t
      press1,
      release1, contact1, touchID1, w1, m1,
      press2, release2, contact2, touchID2, w2, m2;
  uint16_t
      x1,
      y1,
      x2, y2;

  // start touch demo
  while (1)
  {
    // check to see if the interrupt is toggled. since it's in polling mode, it will remain LOW
    // whenever there is an active touch. if it is in trigger mode, it will pulse
    if (analogRead(INTERRUPT) < THRESHOLD)
    {
      Serial.println("got a touch!");
      // start trasmission by sending address 0x38
      Wire.beginTransmission(0x38);
      // start reading at register 0x02
      Wire.write(0x02);
      // send the data over
      Wire.endTransmission();

      // read 16 bytes from address 0x38
      Wire.requestFrom(0x38, 16);

      // register 2 contains the number of points touched
      uint8_t
          pointsTouched;
      pointsTouched = Wire.read() & 0x0F;
      Serial.print("num of points touched:");
      Serial.println(pointsTouched);
      if (0 < pointsTouched)
      {

        // read the data
        ((uint8_t *)(&x1))[1] = Wire.read();
        ((uint8_t *)(&x1))[0] = Wire.read();
        ((uint8_t *)(&y1))[1] = Wire.read();
        ((uint8_t *)(&y1))[0] = Wire.read();
        w1 = Wire.read();
        m1 = Wire.read();

        // crunch the data
        press1 = ((x1 & 0xC000) == 0x0000) ? 1 : 0;
        release1 = ((x1 & 0xC000) == 0x4000) ? 1 : 0;
        contact1 = ((x1 & 0xC000) == 0x8000) ? 1 : 0;
        touchID1 = y1 & 0xE0000;
        x1 &= 0x0FFF;
        y1 &= 0x0FFF;

        // check for two points touched
        if (1 < pointsTouched)
        {
          ((uint8_t *)(&x2))[1] = Wire.read();
          ((uint8_t *)(&x2))[0] = Wire.read();
          ((uint8_t *)(&y2))[1] = Wire.read();
          ((uint8_t *)(&y2))[0] = Wire.read();
          w2 = Wire.read();
          m2 = Wire.read();

          press2 = ((x2 & 0xC000) == 0x0000) ? 1 : 0;
          release2 = ((x2 & 0xC000) == 0x4000) ? 1 : 0;
          contact2 = ((x2 & 0xC000) == 0x8000) ? 1 : 0;
          touchID2 = y2 & 0xE0000;
          x2 &= 0x0FFF;
          y2 &= 0x0FFF;
          drawPixel(x2, y2, 0x00, 0xFF, 0x00);
        }
        drawPixel(x1, y1, 0xFF, 0xFF, 0xFF);

        // break out of the loop if the little X box is clicked
        if ((x1 < 20 && y1 < 20) || (x2 < 20 && y2 < 20))
          break;
      }
    }
  }
}

// function to draw a pixel on the screen
void drawPixel(uint16_t x, uint16_t y, uint8_t R, uint8_t G, uint8_t B)
{
  uint8_t x1, x2;

  setWrite_position(x, y);
  x1 = (R & 0xF8) | (G >> 5);
  x2 = ((G & 0x1C) << 3) | (B >> 3);
  SPI_sendData(x1);
  SPI_sendData(x2);
}

// draws a line of a specified color from point [x0, y0] to [x1, y1]
// from: http://rosettacode.org/wiki/Bitmap/Bresenham's_line_algorithm#C
void drawLine(uint16_t x0, uint16_t y0,
              uint16_t x1, uint16_t y1, uint8_t R, uint8_t G, uint8_t B)
{
  // general case
  if (y0 != y1)
  {
    // find the deltas and slopes
    int16_t dx = abs((int16_t)x1 - (int16_t)x0);
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t dy = abs((int16_t)y1 - (int16_t)y0);
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = (dx > dy ? dx : -dy) / 2;

    for (;;)
    {
      drawPixel(x0, y0, R, G, B);
      if ((x0 == x1) && (y0 == y1))
      {
        break;
      }
      int16_t e2 = err;
      if (e2 > -dx)
      {
        err -= dy;
        x0 = (uint16_t)((int16_t)x0 + sx);
      }
      if (e2 < dy)
      {
        err += dx;
        y0 = (uint16_t)((int16_t)y0 + sy);
      }
    }
  }
  else
  {
    // optimized for this display
    Fast_Horizontal_Line(x0, y0, x1, R, G, B);
  }
}

void Fast_Horizontal_Line(uint16_t x0, uint16_t y, uint16_t x1, uint8_t R, uint8_t G, uint8_t B)
{
  if (x0 < x1)
    for (int i = x0; i < x1; i++)
    {
      drawPixel(i, y, R, G, B);
    }
  else
    for (int i = x1; i < x0; i++)
    {
      drawPixel(i, y, R, G, B);
    }
}

//=================================================================================

// setup and loop functions
void setup()
{
  DDRD = 0x80;
  DDRB = 0x3F;
  pinMode(INTERRUPT, INPUT);
  pinMode(TCH_RES, OUTPUT);
  SD.begin(7);

  digitalWrite(TCH_RES, LOW);
  CLR_MOSI;
  CLR_SCL;
  CLR_DI;
  SET_CS;

  Serial.begin(9600);
  Serial.println("start of setup");

  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  Initialize_LCD(); // call the LCD initialization function
  Serial.println("end of setup");
}

// set the demos to 1 to try it out
#define touch_demo 0
#define colorbars 0
#define bmp_demo 1
#define showcolor_demo 0

void loop()
{
#if touch_demo
  Fill_LCD(0x00, 0x00, 0x00);
  tchSetup();
  touchDemo();
#endif

#if bmp_demo
  show_BMPs_in_root();
  delay(10000);
#endif

#if colorbars
  ColorBars();
  delay(10000);
#endif

#if showcolor_demo
  Fill_LCD(0xFF, 0x00, 0xFF); // purple
  delay(1000);
  Fill_LCD(0x00, 0xFF, 0xFF); // yellow
  delay(1000);
  Fill_LCD(0xFF, 0xFF, 0x00); // aqua
  delay(1000);
#endif

while (1) ;
}
