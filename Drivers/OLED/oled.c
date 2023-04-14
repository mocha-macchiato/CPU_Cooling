#include "main.h"
#include "oled.h"
#include "i2c.h"
#include "oled_font.h"

#define OLED_ADDRESS 0x78

#define OLED_LINE_NUM 64
#define OLED_COLUMN_NUM 128
uint8_t OLED_GRAM[OLED_LINE_NUM / 8][OLED_COLUMN_NUM] = {0};    // Display matrix.


/**
  * @brief  This function write a byte size of cmd in OLED.
  * @param  byte_data: A byte size of data.
  * @return None
  * @retval None
  */
static HAL_StatusTypeDef OLED_Write_Cmd(uint8_t byte_data) {
    return HAL_I2C_Mem_Write(&hi2c1, OLED_ADDRESS, 0x00, I2C_MEMADD_SIZE_8BIT, &byte_data, 1, 0xff);
}


/**
  * @brief  This function write data in OLED.
  * @param  data: Data to be writen in OLED.
  * @param  size: The size of data.
  * @return None
  * @retval None
  */
static HAL_StatusTypeDef OLED_Write_Data(uint8_t *data, size_t size) {
    return HAL_I2C_Mem_Write(&hi2c1, OLED_ADDRESS, 0x40, I2C_MEMADD_SIZE_8BIT, data, size, 0xffff);
}


/**
  * @brief  This function refresh the content of OLED display.
  * @retval None
  */
void OLED_Refresh(void) {
    for (uint8_t i = 0; i < OLED_LINE_NUM / 8; i++) {
        OLED_Write_Cmd(0xb0 + i);   // Set start line address.
        OLED_Write_Cmd(0x00);       // Set low column address.
        OLED_Write_Cmd(0x10);       // Set high column address.
        OLED_Write_Data(OLED_GRAM[i], OLED_COLUMN_NUM);  // Refresh 8 line data.    TODO
    }
}


/**
  * @brief  This function clear the content of OLED display.
  * @retval None
  */
void OLED_Clear(void) {
    for (uint8_t i = 0; i < OLED_LINE_NUM / 8; ++i) {
        for (int j = 0; j < OLED_COLUMN_NUM; ++j) {
            OLED_GRAM[i][j] = 0;
        }
    }
    OLED_Refresh();
}


/**
  * @brief  This function initialize OLED.
  * @retval None
  */
void OLED_Init(void) {
    OLED_Write_Cmd(0xAE);   // Turn off oled panel TODO
    OLED_Write_Cmd(0x00);   // Set low column address
    OLED_Write_Cmd(0x10);//---set high column address
    OLED_Write_Cmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_Write_Cmd(0x81);//--set contrast control register
    OLED_Write_Cmd(0xCF);// Set SEG Output Current Brightness
    OLED_Write_Cmd(0xA1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_Write_Cmd(0xC8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_Write_Cmd(0xA6);//--set normal display
    OLED_Write_Cmd(0xA8);//--set multiplex ratio(1 to 64)
    OLED_Write_Cmd(0x3f);//--1/64 duty
    OLED_Write_Cmd(0xD3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_Write_Cmd(0x00);//-not offset
    OLED_Write_Cmd(0xd5);//--set display clock divide ratio/oscillator frequency
    OLED_Write_Cmd(0x80);//--set divide ratio,  Set Clock as 100 Frames/Sec
    OLED_Write_Cmd(0xD9);//--set pre-charge period
    OLED_Write_Cmd(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_Write_Cmd(0xDA);//--set com pins hardware configuration
    OLED_Write_Cmd(0x12);
    OLED_Write_Cmd(0xDB);//--set vcomh
    OLED_Write_Cmd(0x30);//Set VCOM Deselect Level
    OLED_Write_Cmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_Write_Cmd(0x02);//
    OLED_Write_Cmd(0x8D);   // Set charge pump enable.
    OLED_Write_Cmd(0x14);   // Start the charge pump.
    OLED_Clear();
    OLED_Write_Cmd(0xAF);   // Turn on the oled display.
}


/**
  * @brief  This function turn on the OLED display.
  * @retval None
  */
void OLED_DisPlay_On(void) {
    OLED_Write_Cmd(0x8D);    // Set charge pump enable.
    OLED_Write_Cmd(0x14);    // Start the charge pump.
    OLED_Write_Cmd(0xAF);    // Turn on the oled display. TODO
}


/**
  * @brief  This function turn off the OLED display.
  * @retval None
  */
void OLED_DisPlay_Off(void) {
    OLED_Write_Cmd(0x8D);    // Set charge pump disable.
    OLED_Write_Cmd(0x10);    // Stop the charge pump.
    OLED_Write_Cmd(0xAE);    // Turn off the oled display. TODO
}


/**
  * @brief  This function turn over the OLED color display.
  * @param  flag: 0: Normal display;
  *               1: Turn over color display.
  * @return None
  * @retval None
  */
void OLED_Color_Turn_Over(uint8_t flag) {
    if (!flag) OLED_Write_Cmd(0xA6);    // TODO
    else OLED_Write_Cmd(0xA7);
}


/**
  * @brief  This function route the OLED display 180°.
  * @param  flag: 0: Normal display;
  *               1: Route 180° display.
  * @return None
  * @retval None
  */
void OLED_Route_180(uint8_t flag) {
    if (!flag) {
        OLED_Write_Cmd(0xC8);   // TODO
        OLED_Write_Cmd(0xA1);
    } else {
        OLED_Write_Cmd(0xC0);
        OLED_Write_Cmd(0xA0);
    }
}


/**
  * @brief  This function draw a point on display.
  * @param  x, y: Location.
  * @param  flag: 1: Filling;
  *                0: Clean.
  * @retval None
  */
void OLED_Draw_Point(uint8_t x, uint8_t y, uint8_t flag) {
    uint8_t y_oct = y / 8, y_dig = y % 8;
    if (flag) OLED_GRAM[y_oct][x] |= (1 << y_dig);
    else OLED_GRAM[y_oct][x] &= ~(1 << y_dig);
}


/**
  * @brief  This function show a char on display.
  * @param  x, y: Location.
  * @param  character: A character to display.
  * @param  font_size: Font size: 12, 16, 24
  * @param  flag: 1: Normal display;
  *               0: Turn over color display.
  * @retval None
  */
void OLED_Show_Char(uint8_t x, uint8_t y, uint8_t character, uint8_t font_size, uint8_t flag) {
    uint8_t char_index = 0, buffer = 0;
    uint8_t x_start = x, y_start = y;
    uint8_t const *font_lib = NULL;
    uint8_t display_size = 0;

    char_index = character - ' ';   // Get the index of the character in the font matrix.
    switch (font_size) {            // Match ascii font matrix via font size.
        case 16:
            font_lib = asc2_1608[char_index];
            display_size = 16;
            break;
        case 24:
            font_lib = asc2_2412[char_index];
            display_size = 36;
            break;
        default:
            font_lib = asc2_1206[char_index];
            display_size = 12;
            break;
    }

    for (uint8_t i = 0; i < display_size; ++i) {
        buffer = font_lib[i];
        for (uint8_t j = 0; j < 8; ++j) {       // Draw a column.
            if (buffer & 0x01) OLED_Draw_Point(x, y, flag);
            else OLED_Draw_Point(x, y, !flag);
            buffer >>= 1;
            y++;
        }
        x++;
        if ((i + 1) % (font_size / 2) == 0) {
            x = x_start;
            y_start += 8;
        }
        y = y_start;
    }
}


/**
  * @brief  This function show a string on display.
  * @param  x, y: Location.
  * @param  str: A string to display.
  * @param  font_size: Font size: 12, 16, 24
  * @param  flag: 1: Normal display;
  *               0: Turn over color display.
  * @retval None
  */
void OLED_Show_String(uint8_t x, uint8_t y, uint8_t *str, uint8_t font_size, uint8_t flag) {
    while ((*str >= ' ') && (*str <= '~')) {                // Check if the char in the ascii.
        OLED_Show_Char(x, y, *str, font_size, flag);
        x += font_size / 2;
        if (x + font_size / 2 >= OLED_COLUMN_NUM) {         // Change a new line.
            x = 0;
            y += font_size;
        }
        str++;
    }
}


/**
  * @brief  This function show numbers on display.
  * @param  x, y: Location.
  * @param  num: Numbers to display.
  * @param  len: Length of the numbers.
  * @param  font_size: Font size: 12, 16, 24
  * @param  flag: 1: Normal display;
  *               0: Turn over color display.
  * @retval None
  */
#define MAX_SHOW_NUM 21
void OLED_Show_Num(uint8_t x, uint8_t y, uint8_t num, uint8_t len, uint8_t font_size, uint8_t flag) {
    uint8_t buffer[MAX_SHOW_NUM] = {0};
    for (uint8_t i = len; i > 0; i--) {
        buffer[i-1] = num % 10 + '0';
        num /= 10;
    }
    OLED_Show_String(x, y, buffer, font_size, flag);
}




