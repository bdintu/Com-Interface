/**
  ******************************************************************************
  * @file    et_stm32f_arm_kit_lcd.c
  * @author  MCD Application Team
  * @version V3.1.2
  * @date    09/28/2009
  * @brief   This file includes the LCD driver for AM-240320L8TNQW00H (LCD_ILI9320)
  *          Liquid Crystal Display Module of STM3210C-EVAL board.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
//#include "et_stm32f_arm_kit.h"
#include "stm32f1xx_hal.h"
#include "et_stm32f_arm_kit_lcd.h"
#include "fonts.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @defgroup STM3210C_EVAL_LCD 
  * @brief This file includes the LCD driver for AM-240320L8TNQW00H (LCD_ILI9320)
  *        Liquid Crystal Display Module of STM3210C-EVAL board.
  * @{
  */ 

/** @defgroup STM3210C_EVAL_LCD_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM3210C_EVAL_LCD_Private_Defines
  * @{
  */ 
#define START_BYTE      0x70
#define SET_INDEX       0x00
#define READ_STATUS     0x01
#define LCD_WRITE_REG   0x02
#define LCD_READ_REG    0x03

#define LCD_Timeout			200
/**
  * @}
  */ 


/** @defgroup STM3210C_EVAL_LCD_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM3210C_EVAL_LCD_Private_Variables
  * @{
  */ 
  /* Global variables to set the written text color */
static  __IO uint16_t TextColor = 0x0000, BackColor = 0xFFFF;
extern SPI_HandleTypeDef hspi3;
/**
  * @}
  */ 


/** @defgroup STM3210C_EVAL_LCD_Private_FunctionPrototypes
  * @{
  */ 
#ifndef USE_Delay
static void delay(__IO uint32_t nCount);
#endif /* USE_Delay*/
/**
  * @}
  */ 


/** @defgroup STM3210C_EVAL_LCD_Private_Functions
  * @{
  */ 


/**
  * @brief  Setups the LCD.
  * @param  None
  * @retval None
  */
void LCD_Setup(void)
{ 
/* Configure the LCD Control pins --------------------------------------------*/
  //LCD_GPIO_Config();
  //GPIO_WriteBit(LCD_BL_GPIO_PORT, LCD_BL_PIN, Bit_SET);
	HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_PIN, GPIO_PIN_SET);
  
/* Configure the LCD_SPI interface ----------------------------------------------*/
  //LCD_SPIConfig();
  //_delay_(5); /* Delay 50 ms */
	HAL_Delay(50);

  /* Start Initial Sequence ------------------------------------------------*/
  LCD_WriteReg(R229, 0x8000); /* Set the internal vcore voltage */
  LCD_WriteReg(R0,  0x0001); /* Start internal OSC. */
  LCD_WriteReg(R1,  0x0100); /* set SS and SM bit */
  LCD_WriteReg(R2,  0x0700); /* set 1 line inversion */
  LCD_WriteReg(R3,  0x1030); /* set GRAM write direction and BGR=1. */
  LCD_WriteReg(R4,  0x0000); /* Resize register */
  LCD_WriteReg(R8,  0x0202); /* set the back porch and front porch */
  LCD_WriteReg(R9,  0x0000); /* set non-display area refresh cycle ISC[3:0] */
  LCD_WriteReg(R10, 0x0000); /* FMARK function */
  LCD_WriteReg(R12, 0x0000); /* RGB interface setting */
  LCD_WriteReg(R13, 0x0000); /* Frame marker Position */
  LCD_WriteReg(R15, 0x0000); /* RGB interface polarity */

  /* Power On sequence -----------------------------------------------------*/
  LCD_WriteReg(R16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
  LCD_WriteReg(R17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
  LCD_WriteReg(R18, 0x0000); /* VREG1OUT voltage */
  LCD_WriteReg(R19, 0x0000); /* VDV[4:0] for VCOM amplitude */
  //_delay_(20);                 /* Dis-charge capacitor power voltage (200ms) */
	HAL_Delay(200);
	
  LCD_WriteReg(R16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
  LCD_WriteReg(R17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
  //_delay_(5);                  /* Delay 50 ms */
	HAL_Delay(50);
	
  LCD_WriteReg(R18, 0x0139); /* VREG1OUT voltage */
  //_delay_(5);                  /* Delay 50 ms */
	HAL_Delay(50);
	
  LCD_WriteReg(R19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
  LCD_WriteReg(R41, 0x0013); /* VCM[4:0] for VCOMH */
  //_delay_(5);                  /* Delay 50 ms */
	HAL_Delay(50);
	
  LCD_WriteReg(R32, 0x0000); /* GRAM horizontal Address */
  LCD_WriteReg(R33, 0x0000); /* GRAM Vertical Address */

  /* Adjust the Gamma Curve ------------------------------------------------*/
  LCD_WriteReg(R48, 0x0006);
  LCD_WriteReg(R49, 0x0101);
  LCD_WriteReg(R50, 0x0003);
  LCD_WriteReg(R53, 0x0106);
  LCD_WriteReg(R54, 0x0b02);
  LCD_WriteReg(R55, 0x0302);
  LCD_WriteReg(R56, 0x0707);
  LCD_WriteReg(R57, 0x0007);
  LCD_WriteReg(R60, 0x0600);
  LCD_WriteReg(R61, 0x020b);
  
  /* Set GRAM area ---------------------------------------------------------*/
  LCD_WriteReg(R80, 0x0000); /* Horizontal GRAM Start Address */
  LCD_WriteReg(R81, 0x00EF); /* (240-1) Horizontal GRAM End Address */
  LCD_WriteReg(R82, 0x0000); /* Vertical GRAM Start Address */
  LCD_WriteReg(R83, 0x013F); /* (320-1) Vertical GRAM End Address */

  /* Difference Config ET-TFT240320TP Rev.B & Rev.C */
  //LCD_WriteReg(R96,  0x2700); /* Gate Scan Line For ET-TFT240320TP3.2 RevB */
  LCD_WriteReg(R96,  0xA700); /* Gate Scan Line For ET-TFT240320TP3.2 RevC */

  LCD_WriteReg(R97,  0x0001); /* NDL,VLE, REV */
  LCD_WriteReg(R106, 0x0000); /* set scrolling line */

  /* Partial Display Control -----------------------------------------------*/
  LCD_WriteReg(R128, 0x0000);
  LCD_WriteReg(R129, 0x0000);
  LCD_WriteReg(R130, 0x0000);
  LCD_WriteReg(R131, 0x0000);
  LCD_WriteReg(R132, 0x0000);
  LCD_WriteReg(R133, 0x0000);

  /* Panel Control ---------------------------------------------------------*/
  LCD_WriteReg(R144, 0x0010);
  LCD_WriteReg(R146, 0x0000);
  LCD_WriteReg(R147, 0x0003);
  LCD_WriteReg(R149, 0x0110);
  LCD_WriteReg(R151, 0x0000);
  LCD_WriteReg(R152, 0x0000);

  /* Set GRAM write direction and BGR = 1 */
  /* I/D=01 (Horizontal : increment, Vertical : decrement) */
  /* AM=1 (address is updated in vertical writing direction) */
  LCD_WriteReg(R3, 0x1018);
  LCD_WriteReg(R7, 0x0173); /* 262K color and display ON */

  ///* Set GRAM write direction and BGR = 1
  //   I/D=10 (Horizontal : increment, Vertical : increment)
  //   AM=1 (address is updated in vertical writing direction)                  */
  //LCD_WriteReg(R3, 0x1038);
  //LCD_WriteReg(R7, 0x0173); /* 262K color and display ON */
}


/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval None
  */
//void ET_STM32_LCD_Init(void)
//{
//  /* Setups the LCD */
//  LCD_Setup();
//}


/**
  * @brief  Sets the Text color.
  * @param  Color: specifies the Text color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetTextColor(__IO uint16_t Color)
{
  TextColor = Color;
}


/**
  * @brief  Sets the Background color.
  * @param  Color: specifies the Background color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetBackColor(__IO uint16_t Color)
{
  BackColor = Color;
}


/**
  * @brief  Clears the selected line.
  * @param  Line: the Line to be cleared.
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @retval None
  */
//void LCD_ClearLine(uint8_t Line)
//{
//  LCD_DisplayStringLine(Line, (uint8_t *) "                    ");
//}


/**
  * @brief  Clears the hole LCD.
  * @param  Color: the color of the background.
  * @retval None
  */
void LCD_Clear(uint16_t Color)
{
  uint32_t index = 0;
  
  LCD_SetCursor(0x00, 0x013F); 
  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
  for(index = 0; index < 76800; index++)
  {
    LCD_WriteRAM(Color);
  }
  //GPIO_WriteBit(LCD_CS_GPIO_PORT, LCD_CS_PIN, Bit_SET); 
  HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET); 
}


/**
  * @brief  Sets the cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position. 
  * @retval None
  */
void LCD_SetCursor(uint8_t Xpos, uint16_t Ypos)
{
 LCD_WriteReg(R32, Xpos);
 LCD_WriteReg(R33, Ypos);
}


/**
  * @brief  Draws a character on LCD.
  * @param  Xpos: the Line where to display the character shape.
  * @param  Ypos: start column address.
  * @param  c: pointer to the character data.
  * @retval None
  */
void LCD_DrawChar(uint8_t Xpos, uint16_t Ypos, const uint16_t *c)
{
  uint32_t index = 0, i = 0;
  uint8_t Xaddress = 0;
   
  Xaddress = Xpos;
  
  LCD_SetCursor(Xaddress, Ypos);
  
  for(index = 0; index < 24; index++)
  {
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    for(i = 0; i < 16; i++)
    {
      if((c[index] & (1 << i)) == 0x00)
      {
        LCD_WriteRAM(BackColor);
      }
      else
      {
        LCD_WriteRAM(TextColor);
      }
    }   
    //GPIO_WriteBit(LCD_CS_GPIO_PORT, LCD_CS_PIN, Bit_SET); 
		HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET);
    Xaddress++;
    LCD_SetCursor(Xaddress, Ypos);
  }
}


/**
  * @brief  Displays one character (16dots width, 24dots height).
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  Column: start column address.
  * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
  * @retval None
  */
void LCD_DisplayChar(uint8_t Line, uint16_t Column, uint8_t Ascii)
{
  Ascii -= 32;
  LCD_DrawChar(Line, Column, &ASCII_Table[Ascii * 24]);
}


/**
  * @brief  Displays a maximum of 20 char on the LCD.
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  *ptr: pointer to string to display on LCD.
  * @retval None
  */
void LCD_DisplayStringLine(uint8_t Line, uint8_t *ptr)
{
  uint32_t i = 0;
  uint16_t refcolumn = 319;
  /* Send the string character by character on lCD */
  while ((*ptr != 0) & (i < 20))
  {
    /* Display one character on LCD */
    LCD_DisplayChar(Line, refcolumn, *ptr);
    /* Decrement the column position by 16 */
    refcolumn -= 16;
    /* Point on the next character */
    ptr++;
    /* Increment the character counter */
    i++;
  }
}


/**
  * @brief  Sets a display window
  * @param  Xpos: specifies the X buttom left position.
  * @param  Ypos: specifies the Y buttom left position.
  * @param  Height: display window height.
  * @param  Width: display window width.
  * @retval None
  */
void LCD_SetDisplayWindow(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{ 
  /* Horizontal GRAM Start Address */
  if(Xpos >= Height)
  {
    LCD_WriteReg(R80, (Xpos - Height + 1));
  }
  else
  {
    LCD_WriteReg(R80, 0);
  }
  /* Horizontal GRAM End Address */
  LCD_WriteReg(R81, Xpos);
  /* Vertical GRAM Start Address */
  if(Ypos >= Width)
  {
    LCD_WriteReg(R82, (Ypos - Width + 1));
  }  
  else
  {
    LCD_WriteReg(R82, 0);
  }
  /* Vertical GRAM End Address */
  LCD_WriteReg(R83, Ypos);
  LCD_SetCursor(Xpos, Ypos);
}


/**
  * @brief  Disables LCD Window mode.
  * @param  None
  * @retval None
  */
void LCD_WindowModeDisable(void)
{
  LCD_SetDisplayWindow(239, 0x13F, 240, 320);
  LCD_WriteReg(R3, 0x1018);    
}


/**
  * @brief  Displays a line.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Length: line length.
  * @param  Direction: line direction.
  *   This parameter can be one of the following values: Vertical or Horizontal.
  * @retval None
  */
void LCD_DrawLine(uint8_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction)
{
  uint32_t i = 0;
  
  LCD_SetCursor(Xpos, Ypos);
  if(Direction == Horizontal)
  { 
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    for(i = 0; i < Length; i++)
    {
      LCD_WriteRAM(TextColor);
    }
    //GPIO_WriteBit(LCD_CS_GPIO_PORT, LCD_CS_PIN, Bit_SET);
		HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET);
  }
  else
  {
    for(i = 0; i < Length; i++)
    {
      LCD_WriteRAMWord(TextColor);
      Xpos++;
      LCD_SetCursor(Xpos, Ypos);
    }
  }
}


/**
  * @brief  Displays a rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: display rectangle height.
  * @param  Width: display rectangle width.
  * @retval None
  */
void LCD_DrawRect(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
  LCD_DrawLine(Xpos, Ypos, Width, Horizontal);
  LCD_DrawLine((Xpos + Height), Ypos, Width, Horizontal);
  
  LCD_DrawLine(Xpos, Ypos, Height, Vertical);
  LCD_DrawLine(Xpos, (Ypos - Width + 1), Height, Vertical);
}


/**
  * @brief  Displays a circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @retval None
  */
void LCD_DrawCircle(uint8_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  D;/* Decision Variable */ 
  uint32_t  CurX;/* Current X Value */
  uint32_t  CurY;/* Current Y Value */ 
  
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;
  
  while (CurX <= CurY)
  {
    LCD_SetCursor(Xpos + CurX, Ypos + CurY);
    LCD_WriteRAMWord(TextColor);
    LCD_SetCursor(Xpos + CurX, Ypos - CurY);
    LCD_WriteRAMWord(TextColor);
    LCD_SetCursor(Xpos - CurX, Ypos + CurY);
    LCD_WriteRAMWord(TextColor);
    LCD_SetCursor(Xpos - CurX, Ypos - CurY);
    LCD_WriteRAMWord(TextColor);
    LCD_SetCursor(Xpos + CurY, Ypos + CurX);
    LCD_WriteRAMWord(TextColor);
    LCD_SetCursor(Xpos + CurY, Ypos - CurX);
    LCD_WriteRAMWord(TextColor);
    LCD_SetCursor(Xpos - CurY, Ypos + CurX);
    LCD_WriteRAMWord(TextColor);
    LCD_SetCursor(Xpos - CurY, Ypos - CurX);
    LCD_WriteRAMWord(TextColor);
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}

/**
  * @brief  Displays a monocolor picture.
  * @param  Pict: pointer to the picture array.
  * @retval None
  */
void LCD_DrawMonoPict(const uint32_t *Pict)
{
  uint32_t index = 0, i = 0;
  LCD_SetCursor(0, 319); 
  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
  for(index = 0; index < 2400; index++)
  {
    for(i = 0; i < 32; i++)
    {
      if((Pict[index] & (1 << i)) == 0x00)
      {
        LCD_WriteRAM(BackColor);
      }
      else
      {
        LCD_WriteRAM(TextColor);
      }
    }
  }
  //GPIO_WriteBit(LCD_CS_GPIO_PORT, LCD_CS_PIN, Bit_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

#ifdef USE_LCD_DrawBMP
/**
  * @brief  Displays a bitmap picture loaded in the SPI Flash.
  * @param  BmpAddress: Bmp picture address in the SPI Flash.
  * @retval None
  */
//void LCD_DrawBMP(uint32_t BmpAddress)
//{
//  uint32_t i = 0, size = 0;
//
//  /* Read bitmap size */
//  SPI_FLASH_BufferRead((uint8_t*)&size, BmpAddress + 2, 4);
//
//  /* get bitmap data address offset */
//  SPI_FLASH_BufferRead((uint8_t*)&i, BmpAddress + 10, 4);
//  
//  size = (size - i)/2;
//
//  SPI_FLASH_StartReadSequence(BmpAddress + i);
//
//  /* Disable SPI1  */
//  SPI_Cmd(SPI1, DISABLE);
//  /* SPI in 16-bit mode */
//  SPI_DataSizeConfig(SPI1, SPI_DataSize_16b);
//
//  /* Enable SPI1  */
//  SPI_Cmd(SPI1, ENABLE);
//  
//  /* Set GRAM write direction and BGR = 1 */
//  /* I/D=00 (Horizontal : decrement, Vertical : decrement) */
//  /* AM=1 (address is updated in vertical writing direction) */
//  LCD_WriteReg(R3, 0x1008);
//
//  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
//
//  /* Read bitmap data from SPI Flash and send them to LCD */
//  for(i = 0; i < size; i++)
//  {
//    LCD_WriteRAM(__REV_HalfWord(SPI_FLASH_SendHalfWord(0xA5A5)));
//  }
//
//  LCD_CtrlLinesWrite(LCD_NCS_GPIO_PORT, LCD_NCS_PIN, Bit_SET);
//
//  /* Deselect the FLASH: Chip Select high */
//  SPI_FLASH_CS_HIGH();
//
//  /* Disable SPI1  */
//  SPI_Cmd(SPI1, DISABLE);
//  /* SPI in 8-bit mode */
//  SPI_DataSizeConfig(SPI1, SPI_DataSize_8b);
//
//  /* Enable SPI1  */
//  SPI_Cmd(SPI1, ENABLE);
//
//  /* Set GRAM write direction and BGR = 1 */
//  /* I/D = 01 (Horizontal : increment, Vertical : decrement) */
//  /* AM = 1 (address is updated in vertical writing direction) */
//  LCD_WriteReg(R3, 0x1018);
//}


/**
  * @brief  Displays a bitmap picture loaded in the SPI Flash.
  * @param  BmpAddress: Bmp picture address in the SPI Flash.
  * @retval None
  */
void LCD_DrawBMP(const uint16_t *BmpAddress)
{
  uint32_t i = 0, size = 0;
  /* Read bitmap size */
  size = BmpAddress[1] | (BmpAddress[2] << 16);
  /* get bitmap data address offset */
  i = BmpAddress[5] | (BmpAddress[6] << 16);
  size = (size - i)/2;
  BmpAddress += i/2;
  /* Set GRAM write direction and BGR = 1 */
  /* I/D=00 (Horizontal : decrement, Vertical : decrement) */
  /* AM=1 (address is updated in vertical writing direction) */
  LCD_WriteReg(R3, 0x1008);
  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
  /* Read bitmap data from SPI Flash and send them to LCD */
  for(i = 0; i < size; i++)
  {
    LCD_WriteRAM(BmpAddress[i]);
  }
  //GPIO_Write(LCD_CS_GPIO_PORT, LCD_CS_PIN, Bit_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET);
  /* Set GRAM write direction and BGR = 1 */
  /* I/D = 01 (Horizontal : increment, Vertical : decrement) */
  /* AM = 1 (address is updated in vertical writing direction) */
  LCD_WriteReg(R3, 0x1018);
}
#endif

/**
  * @brief  Reset LCD control line(/CS) and Send Start-Byte
  * @param  Start_Byte: the Start-Byte to be sent
  * @retval None
  */
void LCD_CS_StartByte(uint8_t Start_Byte)
{
  //GPIO_WriteBit(LCD_CS_GPIO_PORT, LCD_CS_PIN, Bit_RESET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
	
//  SPI_I2S_SendData(LCD_SPI, Start_Byte);
//  while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) != RESET)
//  {
//  }
	HAL_SPI_Transmit(&hspi3, (uint8_t *) &Start_Byte, 1, LCD_Timeout);
	while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_TX)
  {
  }
}

/**
  * @brief  Writes index to select the LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @retval None
  */
void LCD_WriteRegIndex(uint8_t LCD_Reg)
{
  /* Reset LCD control line(/CS) and Send Start-Byte */
  LCD_CS_StartByte(START_BYTE | SET_INDEX);
  /* Write 16-bit Reg Index (High Byte is 0) */
//  SPI_I2S_SendData(LCD_SPI, 0x00);
//  while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) != RESET)
//  {
//  }
	SPI3->DR = 0x00;
	while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_TX)
  {
  }
	
//  SPI_I2S_SendData(LCD_SPI, LCD_Reg);
//  while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) != RESET)
//  {
//  }
	HAL_SPI_Transmit(&hspi3, (uint8_t *) &LCD_Reg, 1, LCD_Timeout);
	while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_TX)
  {
  }
	
//  GPIO_WriteBit(LCD_CS_GPIO_PORT, LCD_CS_PIN, Bit_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET);
	
}


/**
  * @brief  Reads the selected LCD Register.
  * @param  None
  * @retval LCD Register Value.
  */
uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
  uint16_t tmp = 0;
  uint8_t i = 0;
  
  /* LCD_SPI prescaler: 4 */
  LCD_SPI->CR1 &= 0xFFC7;
  LCD_SPI->CR1 |= 0x0008;
  /* Write 16-bit Index (then Read Reg) */
  LCD_WriteRegIndex(LCD_Reg);
  /* Read 16-bit Reg */
  /* Reset LCD control line(/CS) and Send Start-Byte */
  LCD_CS_StartByte(START_BYTE | LCD_READ_REG);
  
  for(i = 0; i < 5; i++)
  {
//    SPI_I2S_SendData(LCD_SPI, 0xFF);
//    while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) != RESET)
//    {
//    }
		
		HAL_SPI_Transmit(&hspi3, (uint8_t *) 0xFF, 1, LCD_Timeout);
		while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_TX)
		{
		}
    /* One byte of invalid dummy data read after the start byte */
//    while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_RXNE) == RESET)
//    {    
//    }
//    SPI_I2S_ReceiveData(LCD_SPI); 
		//Receive Upper Byte First
		while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_RX)
		{
		}
		HAL_SPI_Receive(&hspi3, (uint8_t *) (&tmp)+1, 1, LCD_Timeout);
		
		while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_RX)
		{
		}
		HAL_SPI_Receive(&hspi3, (uint8_t *) (&tmp)+0, 1, LCD_Timeout);
  }
//  SPI_I2S_SendData(LCD_SPI, 0xFF);
//  /* Read upper byte */
//  while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) != RESET)
//  {
//  }
		HAL_SPI_Transmit(&hspi3, (uint8_t *) 0xFF, 1, LCD_Timeout);
		while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_TX)
		{
		}	
	
  /* Read upper byte */
//  while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_RXNE) == RESET)
//  {
//  }
//  tmp = SPI_I2S_ReceiveData(LCD_SPI);
	
		while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_RX)
		{
		}
		HAL_SPI_Receive(&hspi3, (uint8_t *) (&tmp)+1, 1, LCD_Timeout);
  
  
//  SPI_I2S_SendData(LCD_SPI, 0xFF);
//  while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) != RESET)
//  {
//  }
		HAL_SPI_Transmit(&hspi3, (uint8_t *) 0xFF, 1, LCD_Timeout);
		while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_TX)
		{
		}	
  /* Read lower byte */
//  while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_RXNE) == RESET)
//  {
//  }
//  tmp = ((tmp & 0xFF) << 8) | SPI_I2S_ReceiveData(LCD_SPI);
	
		while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_RX)
		{
		}
		HAL_SPI_Receive(&hspi3, (uint8_t *) (&tmp)+0, 1, LCD_Timeout);
		
  //GPIO_WriteBit(LCD_CS_GPIO_PORT, LCD_CS_PIN, Bit_SET);
		HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET);
  /* LCD_SPI prescaler: 2 */
  LCD_SPI->CR1 &= 0xFFC7;
  return tmp;
}


/**
  * @brief  Prepare to write to the LCD RAM.
  * @param  None
  * @retval None
  */
void LCD_WriteRAM_Prepare(void)
{
  LCD_WriteRegIndex(R34); /* Select GRAM Reg */
  /* Reset LCD control line(/CS) and Send Start-Byte */
  LCD_CS_StartByte(START_BYTE | LCD_WRITE_REG);
}


/**
  * @brief  Writes 1 word to the LCD RAM.
  * @param  RGB_Code: the pixel color in RGB mode (5-6-5).
  * @retval None
  */
void LCD_WriteRAMWord(uint16_t RGB_Code)
{
  LCD_WriteRAM_Prepare();
  LCD_WriteRAM(RGB_Code);
  //GPIO_WriteBit(LCD_CS_GPIO_PORT, LCD_CS_PIN, Bit_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}


/**
  * @brief  Writes to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @param  LCD_RegValue: value to write to the selected register.
  * @retval None
  */
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  /* Write 16-bit Index (then Write Reg) */
  LCD_WriteRegIndex(LCD_Reg);
  /* Write 16-bit Reg */
  /* Reset LCD control line(/CS) and Send Start-Byte */
  LCD_CS_StartByte(START_BYTE | LCD_WRITE_REG);
  
//  SPI_I2S_SendData(LCD_SPI, LCD_RegValue>>8);
//  while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) != RESET)
//  {
//  }
// 	SPI_I2S_FLAG_BSY = ((uint16_t)0x0080)
//	SPI3->DR = LCD_RegValue>>8;
//	while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_TX)
//  {
//  }
	
	HAL_SPI_Transmit(&hspi3, (uint8_t *) (&LCD_RegValue)+1, 1, LCD_Timeout);
	while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_TX)
  {
  }
	
	
//	SPI_I2S_SendData(LCD_SPI, (LCD_RegValue & 0xFF));	  
//  while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) != RESET)
//  {
//  }
	HAL_SPI_Transmit(&hspi3, (uint8_t *) (&LCD_RegValue)+0, 1, LCD_Timeout);
	while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_TX)
  {
  }
	
//  GPIO_WriteBit(LCD_CS_GPIO_PORT, LCD_CS_PIN, Bit_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET);
	
}


/**
  * @brief  Writes to the LCD RAM.
  * @param  RGB_Code: the pixel color in RGB mode (5-6-5).
  * @retval None
  */
void LCD_WriteRAM(uint16_t RGB_Code)
{
//  SPI_I2S_SendData(LCD_SPI, RGB_Code >> 8);
//  while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) != RESET)
//  {
//  }
//Send Higher Byte
	HAL_SPI_Transmit(&hspi3, (uint8_t *) (&RGB_Code)+1, 1, LCD_Timeout);
	while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_TX)
  {
  }

	
//  SPI_I2S_SendData(LCD_SPI, RGB_Code & 0xFF);
//  while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) != RESET)
//  {
//  }
//Send Lower Byte
	HAL_SPI_Transmit(&hspi3, (uint8_t *) (&RGB_Code)+0, 1, LCD_Timeout);
	while(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY_TX)
  {
  }
}


/**
  * @brief  Power on the LCD.
  * @param  None
  * @retval None
  */
void LCD_PowerOn(void)
{ 
  /* Power On sequence ---------------------------------------------------------*/
  LCD_WriteReg(R16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
  LCD_WriteReg(R17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
  LCD_WriteReg(R18, 0x0000); /* VREG1OUT voltage */
  LCD_WriteReg(R19, 0x0000); /* VDV[4:0] for VCOM amplitude */
  //_delay_(20);                 /* Dis-charge capacitor power voltage (200ms) */
	HAL_Delay(200);
  LCD_WriteReg(R16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
  LCD_WriteReg(R17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
  //_delay_(5);                  /* Delay 50 ms */
	HAL_Delay(50);
  LCD_WriteReg(R18, 0x0139); /* VREG1OUT voltage */
  //_delay_(5);                  /* delay 50 ms */
	HAL_Delay(50);
  LCD_WriteReg(R19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
  LCD_WriteReg(R41, 0x0013); /* VCM[4:0] for VCOMH */
  //_delay_(5);                  /* delay 50 ms */
	HAL_Delay(50);
  LCD_WriteReg(R7, 0x0173);  /* 262K color and display ON */
}

/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void LCD_DisplayOn(void)
{
  /* Display On */
  LCD_WriteReg(R7, 0x0173); /* 262K color and display ON */
 
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void LCD_DisplayOff(void)
{
  /* Display Off */
  LCD_WriteReg(R7, 0x0);
}

///**
//  * @brief  Configures LCD control lines in Output Push-Pull mode.
//  * @param  None
//  * @retval None
//  */
//void LCD_GPIO_Config(void)
//{
//  GPIO_InitTypeDef GPIO_InitStructure;

//  /* Enable GPIO clock */
//  RCC_APB2PeriphClockCmd(LCD_CS_GPIO_CLK | LCD_BL_GPIO_CLK, ENABLE);

//  /* Configure NCS in Output Push-Pull mode */
//  GPIO_InitStructure.GPIO_Pin = LCD_CS_PIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Init(LCD_CS_GPIO_PORT, &GPIO_InitStructure);  

//  /* Configure BL in Output Push-Pull mode */
//  GPIO_InitStructure.GPIO_Pin = LCD_BL_PIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Init(LCD_BL_GPIO_PORT, &GPIO_InitStructure);  
//}

///**
//  * @brief  Configures the LCD_SPI interface.
//  * @param  None
//  * @retval None
//  */
//void LCD_SPIConfig(void)
//{
//  SPI_InitTypeDef    SPI_InitStructure;
//  GPIO_InitTypeDef   GPIO_InitStructure;

//  /* Enable GPIO clock */
//  RCC_APB2PeriphClockCmd(LCD_SPI_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);
//  GPIO_PinRemapConfig(GPIO_Remap_SPI3, ENABLE);

//  /* Enable SPI clock  */
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
//  
//  /* Configure SPI pins: SCK, MISO and MOSI */
//  GPIO_InitStructure.GPIO_Pin = LCD_SPI_SCK_PIN | LCD_SPI_MISO_PIN | LCD_SPI_MOSI_PIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_Init(LCD_SPI_GPIO_PORT, &GPIO_InitStructure);
//  
//  SPI_I2S_DeInit(LCD_SPI);
//  
//  /* SPI Config */
//  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
//  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
//  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
//  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
//  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//  SPI_Init(LCD_SPI, &SPI_InitStructure);

//  /* SPI enable */
//  SPI_Cmd(LCD_SPI, ENABLE);
//}

///**
//  * @brief  Configures the Touch Sensor interface.
//  * @param  None
//  * @retval None
//  */
//void ET_STM32_TCS_Config(void)
//{
//  GPIO_InitTypeDef GPIO_InitStructure;

//  /* Enable GPIO clock */
//  RCC_APB2PeriphClockCmd(TCS_GPIO_CLK, ENABLE);

//  /* Configure CS in Output Push-Pull mode */
//  GPIO_InitStructure.GPIO_Pin = TCS_CS_PIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Init(TCS_GPIO_PORT, &GPIO_InitStructure);  

//  /* Configure MOSI in Output Push-Pull mode */
//  GPIO_InitStructure.GPIO_Pin = TCS_MOSI_PIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Init(TCS_GPIO_PORT, &GPIO_InitStructure);

//  /* Configure SCK in Output Push-Pull mode */
//  GPIO_InitStructure.GPIO_Pin = TCS_SCK_PIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Init(TCS_GPIO_PORT, &GPIO_InitStructure);

//  /* Configure PEN as input floating */
//  GPIO_InitStructure.GPIO_Pin = TCS_PEN_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(TCS_GPIO_PORT, &GPIO_InitStructure);  

//  /* Configure MISO as input floating */
//  GPIO_InitStructure.GPIO_Pin = TCS_MISO_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(TCS_GPIO_PORT, &GPIO_InitStructure); 
//}

///**
//  * @brief  Writes to Touch Sensor register.
//  * @param  TCS_Data : address of the selected register.
//  * @retval None
//  */
//unsigned char TCS_SPI_Write_Byte(unsigned char DataByte)
//{
//  unsigned char Bit,result = 0 ;				 	   

//  for (Bit = 0; Bit < 8; Bit++)              // 8 Bit Write
//  {		
//    GPIO_WriteBit(TCS_GPIO_PORT, TCS_SCK_PIN, Bit_SET); // Start Clock  High

//		//--------- write Data ---------
//		if((DataByte & 0x80)== 0x80)
//		   GPIO_WriteBit(TCS_GPIO_PORT, TCS_MOSI_PIN, Bit_SET); // Set bit data = 1
//		else
//		   GPIO_WriteBit(TCS_GPIO_PORT, TCS_MOSI_PIN, Bit_RESET);  // Clear bit data = 0
//		   	
//		GPIO_WriteBit(TCS_GPIO_PORT, TCS_SCK_PIN, Bit_RESET);    // Clock low

//		DataByte <<= 1;	 // Shift Next Bit Data
//      
//	  	//---------- Read Data -------
//	    result <<= 1;	
//	 											  	  
//		if (GPIO_ReadInputDataBit(TCS_GPIO_PORT,TCS_MISO_PIN) == Bit_SET)
//        {
//		  result |= 0x01;
//		}

//	  }

//	  return (result);                    
//}

/**
  * @brief  Writes to Touch Sensor register.
  * @param  TCS_Data : address of the selected register.
  * @retval None
  */
unsigned char TCS_SPI_Write_Byte(unsigned char DataByte)
{
  unsigned char Bit,result = 0 ;				 	   

  for (Bit = 0; Bit < 8; Bit++)              // 8 Bit Write
  {		
    HAL_GPIO_WritePin(TCS_GPIO_PORT, TCS_SCK_PIN, GPIO_PIN_SET); // Start Clock  High

		//--------- write Data ---------
		if((DataByte & 0x80)== 0x80)
		   HAL_GPIO_WritePin(TCS_GPIO_PORT, TCS_MOSI_PIN, GPIO_PIN_SET); // Set bit data = 1
		else
		   HAL_GPIO_WritePin(TCS_GPIO_PORT, TCS_MOSI_PIN, GPIO_PIN_RESET);  // Clear bit data = 0
		   	
		HAL_GPIO_WritePin(TCS_GPIO_PORT, TCS_SCK_PIN, GPIO_PIN_RESET);    // Clock low

		DataByte <<= 1;	 // Shift Next Bit Data
      
	  	//---------- Read Data -------
	    result <<= 1;	
	 											  	  
		if (HAL_GPIO_ReadPin(TCS_GPIO_PORT,TCS_MISO_PIN) == GPIO_PIN_SET)
        {
		  result |= 0x01;
		}

	  }

	  return (result);                    
}

///**
//  * @brief  Return Touch Screen X position value
//  * @param  None
//  * @retval X position.
//  */
//static uint16_t IOE_TS_Read_X(void)
//{
//  int32_t x, xr;
// 
//  x = I2C_ReadDataBuffer(IOE_1_ADDR, IOE_REG_TSC_DATA_Y);
//  
//  /* first correction */
//  xr =  (x * 320) >> 12;
//
//  /* second correction */
//  xr = ((xr * 32)/29) - 17;
//  
//  if(xr <= 0)
//    xr = 0;
//  
//  return (uint16_t)(xr); 
//}

/**
  * @brief  Return Touch Screen X position value
  * @param  None
  * @retval X position.
  */
//uint16_t TCS_Read_X(void)
//{
//  int32_t x, xr;

//  long tcs_adc_X ,tcs_adc_Y;
//  unsigned char buf_data[4]={0,0,0,0};

//  if (GPIO_ReadInputDataBit(TCS_GPIO_PORT,TCS_PEN_PIN) == Bit_RESET)
//  {
//    GPIO_WriteBit(TCS_GPIO_PORT, TCS_CS_PIN, Bit_RESET);							// Enable Touch Interface
//	
//    buf_data[0] = TCS_SPI_Write_Byte(0xD0);											// Write Command Measure X-Position 
//	buf_data[0] = TCS_SPI_Write_Byte(0x00);											// Read ADC data X-Position (7-bit byte High) data: 0ddddddd	(bit) 
//	buf_data[1] = TCS_SPI_Write_Byte(0x90);											// Write Command Measure Y-Position ; Read ADC data X-Position (5-bit byte Low)  data:ddddd000(bit)
//	buf_data[2] = TCS_SPI_Write_Byte(0x00);											// Read ADC data Y-Position(7-bit byte High) data: 0ddddddd (bit)
//	buf_data[3] = TCS_SPI_Write_Byte(0x00);											// Read ADC data Y-Position(5-bit byte Low)  data: ddddd000 (bit)

//	tcs_adc_X  = buf_data[0];														// Mark ADC Data X-Position 12 bit
//	tcs_adc_X  = tcs_adc_X << 5;
//	tcs_adc_X |= buf_data[1] >> 3;
//	tcs_adc_X  = tcs_adc_X & 0x00000FFF;
//	 
//	tcs_adc_Y  = buf_data[2];   													// Mark ADC Data Y-Position 12 bit
//	tcs_adc_Y  = tcs_adc_Y << 5;   													// Shift 7 bit High
//	tcs_adc_Y |= buf_data[3] >> 3;   												// Shift 5 bit low
//	tcs_adc_Y  = tcs_adc_Y & 0x00000FFF;											// total ADC data 12 bit
//	
//	GPIO_WriteBit(TCS_GPIO_PORT, TCS_CS_PIN, Bit_SET);								// Disable Touch Interface			
//	
//    x = 4095-tcs_adc_Y;  															// ADC 12 bit :LCD Start Landscape	
//  }
//  else
//  {
//    x = 0;
//  }
//  
//  /* first correction */
//  xr =  (x * 320) >> 12;
// 
//  /* second correction */
//  xr = ((xr * 32)/29) - 17;
//  
//  if(xr <= 0)
//  {
//    xr = 0;
//  }

//  return (uint16_t)(xr); 
//}

/**
  * @brief  Return Touch Screen X position value
  * @param  None
  * @retval X position.
  */
uint16_t TCS_Read_X(void)
{
  int32_t x, xr;

  long tcs_adc_X ,tcs_adc_Y;
  unsigned char buf_data[4]={0,0,0,0};

  if (HAL_GPIO_ReadPin(TCS_GPIO_PORT,TCS_PEN_PIN) == GPIO_PIN_RESET)
  {
    HAL_GPIO_WritePin(TCS_GPIO_PORT, TCS_CS_PIN, GPIO_PIN_RESET);							// Enable Touch Interface
	
    buf_data[0] = TCS_SPI_Write_Byte(0xD0);											// Write Command Measure X-Position 
		buf_data[0] = TCS_SPI_Write_Byte(0x00);											// Read ADC data X-Position (7-bit byte High) data: 0ddddddd	(bit) 
		buf_data[1] = TCS_SPI_Write_Byte(0x90);											// Write Command Measure Y-Position ; Read ADC data X-Position (5-bit byte Low)  data:ddddd000(bit)
		buf_data[2] = TCS_SPI_Write_Byte(0x00);											// Read ADC data Y-Position(7-bit byte High) data: 0ddddddd (bit)
		buf_data[3] = TCS_SPI_Write_Byte(0x00);											// Read ADC data Y-Position(5-bit byte Low)  data: ddddd000 (bit)

		tcs_adc_X  = buf_data[0];														// Mark ADC Data X-Position 12 bit
		tcs_adc_X  = tcs_adc_X << 5;
		tcs_adc_X |= buf_data[1] >> 3;
		tcs_adc_X  = tcs_adc_X & 0x00000FFF;
	 
		tcs_adc_Y  = buf_data[2];   												// Mark ADC Data Y-Position 12 bit
		tcs_adc_Y  = tcs_adc_Y << 5;   											// Shift 7 bit High
		tcs_adc_Y |= buf_data[3] >> 3;   										// Shift 5 bit low
		tcs_adc_Y  = tcs_adc_Y & 0x00000FFF;								// total ADC data 12 bit
	
		HAL_GPIO_WritePin(TCS_GPIO_PORT, TCS_CS_PIN, GPIO_PIN_SET);								// Disable Touch Interface			
	
    x = 4095-tcs_adc_Y;  															// ADC 12 bit :LCD Start Landscape	
  }
  else
  {
    x = 0;
  }
  
  /* first correction */
  xr =  (x * 320) >> 12;
 
  /* second correction */
  xr = ((xr * 32)/29) - 17;
  
  if(xr <= 0)
  {
    xr = 0;
  }

  return (uint16_t)(xr); 
}

///**
//  * @brief  Return Touch Screen Y position value
//  * @param  None
//  * @retval Y position.
//  */
//static uint16_t IOE_TS_Read_Y(void)
//{
//  int32_t y, yr;
//  y= I2C_ReadDataBuffer(IOE_1_ADDR, IOE_REG_TSC_DATA_X);
//  
//  yr= (y * 240) >> 12;
//  yr = ((yr * 240) / 217) - 12;
//  
//  if(yr <= 0)
//    yr = 0;
//  
//  return (uint16_t)(yr); 
//}

///**
//  * @brief  Return Touch Screen Y position value
//  * @param  None
//  * @retval Y position.
//  */
//uint16_t TCS_Read_Y(void)
//{
//  int32_t y, yr;

//  //long dif_adc_X,dif_adc_Y;

//  long tcs_adc_X ,tcs_adc_Y;
//  unsigned char buf_data[4]={0,0,0,0};

//  if (GPIO_ReadInputDataBit(TCS_GPIO_PORT,TCS_PEN_PIN) == Bit_RESET)
//  {    
//    GPIO_WriteBit(TCS_GPIO_PORT, TCS_CS_PIN, Bit_RESET);									// Enable Touch Interface
//	
//    buf_data[0] = TCS_SPI_Write_Byte(0xD0);													// Write Command Measure X-Position 
//	buf_data[0] = TCS_SPI_Write_Byte(0x00);													// Read ADC data X-Position (7-bit byte High) data: 0ddddddd	(bit) 
//	buf_data[1] = TCS_SPI_Write_Byte(0x90);													// Write Command Measure Y-Position ; Read ADC data X-Position (5-bit byte Low)  data:ddddd000(bit)
//	buf_data[2] = TCS_SPI_Write_Byte(0x00);													// Read ADC data Y-Position(7-bit byte High) data: 0ddddddd (bit)
//	buf_data[3] = TCS_SPI_Write_Byte(0x00);													// Read ADC data Y-Position(5-bit byte Low)  data: ddddd000 (bit)

//	tcs_adc_X  = buf_data[0];																// Mark ADC Data X-Position 12 bit
//	tcs_adc_X  = tcs_adc_X << 5;
//	tcs_adc_X |= buf_data[1] >> 3;
//	tcs_adc_X  = tcs_adc_X & 0x00000FFF;
//	 
//	tcs_adc_Y  = buf_data[2];   															// Mark ADC Data Y-Position 12 bit
//	tcs_adc_Y  = tcs_adc_Y << 5;   															// Shift 7 bit High
//	tcs_adc_Y |= buf_data[3] >> 3;   														// Shift 5 bit low
//	tcs_adc_Y  = tcs_adc_Y & 0x00000FFF;													// total ADC data 12 bit
//	
//	GPIO_WriteBit(TCS_GPIO_PORT, TCS_CS_PIN, Bit_SET);										// Disable Touch Interface				

//    y = tcs_adc_X;	 																		// ADC 12 bit :LCD Start Landscape	
//  }
//  else
//  {
//    y = 0;
//  }
//  
//  yr= (y * 240) >> 12;
//  yr = ((yr * 240) / 217) - 12;
//  
//  if(yr <= 0)
//  {
//    yr = 0;
//  }

//  return (uint16_t)(yr); 
//}

/**
  * @brief  Return Touch Screen Y position value
  * @param  None
  * @retval Y position.
  */
uint16_t TCS_Read_Y(void)
{
  int32_t y, yr;

  //long dif_adc_X,dif_adc_Y;

  long tcs_adc_X ,tcs_adc_Y;
  unsigned char buf_data[4]={0,0,0,0};

  if (HAL_GPIO_ReadPin(TCS_GPIO_PORT,TCS_PEN_PIN) == GPIO_PIN_RESET)
  {    
    HAL_GPIO_WritePin(TCS_GPIO_PORT, TCS_CS_PIN, GPIO_PIN_RESET);									// Enable Touch Interface
	
    buf_data[0] = TCS_SPI_Write_Byte(0xD0);													// Write Command Measure X-Position 
		buf_data[0] = TCS_SPI_Write_Byte(0x00);													// Read ADC data X-Position (7-bit byte High) data: 0ddddddd	(bit) 
		buf_data[1] = TCS_SPI_Write_Byte(0x90);													// Write Command Measure Y-Position ; Read ADC data X-Position (5-bit byte Low)  data:ddddd000(bit)
		buf_data[2] = TCS_SPI_Write_Byte(0x00);													// Read ADC data Y-Position(7-bit byte High) data: 0ddddddd (bit)
		buf_data[3] = TCS_SPI_Write_Byte(0x00);													// Read ADC data Y-Position(5-bit byte Low)  data: ddddd000 (bit)

		tcs_adc_X  = buf_data[0];																// Mark ADC Data X-Position 12 bit
		tcs_adc_X  = tcs_adc_X << 5;
		tcs_adc_X |= buf_data[1] >> 3;
		tcs_adc_X  = tcs_adc_X & 0x00000FFF;
	 
		tcs_adc_Y  = buf_data[2];   														// Mark ADC Data Y-Position 12 bit
		tcs_adc_Y  = tcs_adc_Y << 5;   													// Shift 7 bit High
		tcs_adc_Y |= buf_data[3] >> 3;   												// Shift 5 bit low
		tcs_adc_Y  = tcs_adc_Y & 0x00000FFF;										// total ADC data 12 bit
	
	HAL_GPIO_WritePin(TCS_GPIO_PORT, TCS_CS_PIN, GPIO_PIN_SET);										// Disable Touch Interface				

    y = tcs_adc_X;	 																		// ADC 12 bit :LCD Start Landscape	
  }
  else
  {
    y = 0;
  }
  
  yr= (y * 240) >> 12;
  yr = ((yr * 240) / 217) - 12;
  
  if(yr <= 0)
  {
    yr = 0;
  }

  return (uint16_t)(yr); 
}

#ifndef USE_Delay
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = (100000 * nCount); index != 0; index--)
  {
  }
}
#endif /* USE_Delay*/
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */
  
/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
