#ifndef __OLED_H__
#define __OLED_H__

void OLED_Init(void);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);

void OLED_Color_Turn_Over(uint8_t flag);
void OLED_Route_180(uint8_t flag);
void OLED_Refresh(void);
void OLED_Clear(void);

void OLED_Draw_Point(uint8_t x, uint8_t y, uint8_t state);
void OLED_Show_Char(uint8_t x, uint8_t y, uint8_t character, uint8_t font_size, uint8_t flag);
void OLED_Show_String(uint8_t x, uint8_t y, uint8_t *str, uint8_t font_size, uint8_t flag);
void OLED_Show_Num(uint8_t x, uint8_t y, uint8_t num, uint8_t len, uint8_t font_size, uint8_t flag);


#endif
