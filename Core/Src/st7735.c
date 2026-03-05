/*
 * st7735.c
 *
 *  Created on: 16 ���. 2019 �.
 *      Author: Andriy Honcharenko
 *        Blog: https://stm32withoutfear.blogspot.com
 */

#include "st7735.h"
#include "stdlib.h"



void TFT_Init(void)
{
	    HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_SET);
	    delay_us(100);

	    HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_RESET);
	    delay_us(10); // زيادة بسيطة لضمان استجابة الشاشة

	    HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_SET);
	    delay_us(100); // الـ Reset يحتاج وقت أطول قليلاً ليكتمل داخلياً

	    HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_RESET);
	    delay_us(100);

	    HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_SET);
	    delay_us(120);

	    /* 2. SLEEP OUT MODE */
	    TFT_Write_Command(0x11); // Sleep Out
	    delay_ms_custom(10);    // ضروري جداً الانتظار 120ms بعد هذا الأمر قبل إرسال التالي

	    /* 3. COLOR MODE (Interface Pixel Format) */
	    TFT_Write_Command(0x3A);
	    TFT_Write_Data(0x05);    // ضبط الألوان على 16-bit (RGB565) [cite: 2026-01-27]

	    /* 4. Memory Data Access Control (Rotation & Color Order) */
	    TFT_Write_Command(0x36);
	    // القيمة 0xD6 التي كتبتها (0b11010110) تضبط الاتجاه وترتيب الألوان
	    TFT_Write_Data(0xD6);

	    /* 5. DISPLAY ON */
	    TFT_Write_Command(0x29); // Display ON
	    delay_ms_custom(20);
}

void TFT_Write_Data(uint8_t data)
{
	HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET); // تفعيل
	HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
	HAL_SPI_Transmit(&hspi1, &data, 1, 100);
	HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_SET); // تفعيل

}

void TFT_Write_Command(uint8_t cmd)
{
	HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET); // تفعيل
	HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);
	HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_SET); // تفعيل

}
void TFT_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
    uint32_t i, j, b;

    // تحديد مساحة الرسم للحرف الواحد
    TFT_SetDrawingArea(x, y, x + font.width - 1, y + font.height - 1);

    // البداية من صفوف الفونت
    for(i = 0; i < font.height; i++)
    {
        // جلب بيانات السطر الحالي للحرف من مصفوفة الفونت
        // الحرف ناقص 32 لأن جدول ASCII يبدأ بالمسافة عند 32
        b = font.data[(ch - 32) * font.height + i];

        for(j = 0; j < font.width; j++)
        {
            // فحص البت الحالي (هل هو بكسل لون أم بكسل خلفية؟)
            // ملاحظة: تأكد إذا كان الفونت عندك 16-bit أو 8-bit لتعديل الإزاحة (0x8000 أو 0x80)
            if((b << j) & 0x8000)
            {
                // إرسال لون الحرف (16-bit)
                TFT_Write_Data(color >> 8);   // البايت العالي (High Byte)
                TFT_Write_Data(color & 0xFF); // البايت المنخفض (Low Byte)
            }
            else
            {
                // إرسال لون الخلفية (16-bit)
                TFT_Write_Data(bgcolor >> 8);
                TFT_Write_Data(bgcolor & 0xFF);
            }
        }
    }
}
void TFT_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor)
{
    // التأكد من أن النص ليس فارغاً
    while(*str != '\0')
    {
        // التحقق من تجاوز عرض الشاشة (Auto Line Wrap)
        if(x + font.width >= ST7735_WIDTH)
        {
            x = 0;
            y += font.height;

            // التحقق من تجاوز طول الشاشة
            if(y + font.height >= ST7735_HEIGHT)
            {
                break;
            }

            // تخطي المسافات في بداية السطر الجديد لتوفير المساحة
            if(*str == ' ')
            {
                str++;
                continue;
            }
        }

        // استدعاء دالة رسم الحرف (يجب أن تكون معرفة لديك وتستخدم HAL_SPI_Transmit)
        TFT_WriteChar(x, y, *str, font, color, bgcolor);

        // الانتقال للإحداثي الأفقي التالي
        x += font.width;
        str++;
    }
}

void TFT_SetDrawingArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    // تحديد نطاق الأعمدة (Column Address Set)
    TFT_Write_Command(0x2A); // بدلاً من TFT_CASET [cite: 2026-01-27]
    TFT_Write_Data(x0 >> 8);
    TFT_Write_Data(x0 & 0xFF);
    TFT_Write_Data(x1 >> 8);
    TFT_Write_Data(x1 & 0xFF);

    // تحديد نطاق الصفوف (Row Address Set)
    TFT_Write_Command(0x2B); // بدلاً من TFT_RASET [cite: 2026-01-27]
    TFT_Write_Data(y0 >> 8);
    TFT_Write_Data(y0 & 0xFF);
    TFT_Write_Data(y1 >> 8);
    TFT_Write_Data(y1 & 0xFF);

    // أمر بدء كتابة البيانات في الذاكرة
    TFT_Write_Command(0x2C); // بدلاً من TFT_RAMWR [cite: 2026-01-27]
}

void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    // التحقق من حدود الشاشة لتجنب الكتابة في أماكن خاطئة
    if((x >= 128) || (y >= 160)) // استبدل الأرقام بـ ST7735_WIDTH و HEIGHT [cite: 2026-01-27]
    {
        return;
    }

    // فتح مساحة رسم لبكسل واحد فقط (من x إلى x ومن y إلى y)
    TFT_SetDrawingArea(x, y, x, y);

    // إرسال اللون (16-bit) مقسماً لبايتين
    TFT_Write_Data(color >> 8);   // High Byte
    TFT_Write_Data(color & 0xFF); // Low Byte
}

void TFT_FillDisplay(uint16_t color)
{
    uint32_t i; // استخدمنا uint32_t لأن العدد الإجمالي للبكسلات كبير
    uint8_t HIGH = color >> 8;
    uint8_t LOW  = color & 0xFF;

    // 1. تحديد نطاق الأعمدة (X coordinate) من 0 إلى 127
    TFT_Write_Command(0x2A);
    TFT_Write_Data(0x00);
    TFT_Write_Data(0x00); // البداية X=0
    TFT_Write_Data(0x00);
    TFT_Write_Data(127);  // النهاية X=127 [cite: 2026-01-27]

    // 2. تحديد نطاق الصفوف (Y coordinate) من 0 إلى 159
    TFT_Write_Command(0x2B);
    TFT_Write_Data(0x00);
    TFT_Write_Data(0x00); // البداية Y=0
    TFT_Write_Data(0x00);
    TFT_Write_Data(159);  // النهاية Y=159 [cite: 2026-01-27]

    // 3. أمر بدء كتابة البيانات في الذاكرة (RAM Write)
    TFT_Write_Command(0x2C);

    /* حساب عدد البكسلات الإجمالي: 128 (عرض) * 160 (طول) = 20,480 بكسل.
       كل بكسل يحتاج 2 بايت (High & Low).
    */
    for(i = 0; i < 20480; i++)
    {
        TFT_Write_Data(HIGH);
        TFT_Write_Data(LOW);
    }
}

void TFT_SWReset(void)
{

}

