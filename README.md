# CFAF80160Ax-0096Tx Demonstration Code
This is Arduino sample code for the CFAF80160Ax-0096Tx family of displays. These displays are 0.96" TFTs that can fit just about anywhere. They use the [Sitronix ST7735S](https://www.crystalfontz.com/controllers/Sitronix/ST7735S/) LCD controller and can operate in 3-wire or 4-wire SPI and support 65k colors. In order to interface with these displays, a ZIF to header breakout board or the [CFA10102 breakout board](https://www.crystalfontz.com/product/cfa10102-oled-breakout-board) will be required.

## Connection Guide
```
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
```
## Display information
Here are links to our active displays:\
[CFAF80160A0-0096TN](https://www.crystalfontz.com/product/cfaf80160a00096tn-80x160-small-color-tft-display)\
[CFAF80160A0-0096TW](https://www.crystalfontz.com/product/cfaf80160a00096tw-80x160-tiny-touchscreen-tft-display)

For more information about other TFT offerings, please see our full list [here](https://www.crystalfontz.com/c/tft-lcd-displays/25).
