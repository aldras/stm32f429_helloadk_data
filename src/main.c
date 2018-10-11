/**
  ******************************************************************************
  * @file    main.c
  * @author  Yuuichi Akagawa
  * @version V1.0.0
  * @date    2012/03/05
  * @brief   Android Open Accessory implementation
  ******************************************************************************
  * @attention
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *      http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  * <h2><center>&copy; COPYRIGHT (C)2012 Yuuichi Akagawa</center></h2>
  *
  ******************************************************************************
  */

/* Includes */
#include "stm32f4xx.h"
#include "debug.h"
#include "usart1.h"

#include "debug.h"

#include "calculate_clocks.h"

#include "tm_stm32f4_ili9341_ltdc.h"
#include "tm_stm32f4_fonts.h"

#include "stm32f4_discovery.h"
#include "usb_bsp.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_adk_core.h"

#include "usb_task.h"

char lcd_buffer00[50];

/* Private macro */
/* Private variables */
__IO uint32_t TimingDelay;

/* Private function prototypes */
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
/** @defgroup USBH_USR_MAIN_Private_Variables
* @{
*/
extern USB_OTG_Core_dev;
extern USBH_HOST USB_Host;

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
  int i = 0;
  RCC_ClocksTypeDef RCC_Clocks;

  /* Initialize LEDs and User_Button on STM32F4-Discovery --------------------*/
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);

  /* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

  #ifdef DEBUG
  /* Init Debug out setting(UART1) */
  init_USART1(230400);
  log_info("Init Debug out setting done USART1!\r\n");
#endif

  //calculate_clocks(0,0);

  //Initialize ILI9341 with LTDC
  //=====================================================================================================
  //By default layer 1 is used
  TM_ILI9341_Init();
  //Rotate LCD for 90 degrees
  TM_ILI9341_Rotate(TM_ILI9341_Orientation_Portrait_2);

  TM_ILI9341_SetLayer1();
  /* Fill data on layer 1 */
  TM_ILI9341_Fill(ILI9341_COLOR_BLACK);
  for(uint8_t i = 0; i < 16; i++) {
    TM_ILI9341_Puts(0, 20 * i, lcd_buffer00, &TM_Font_11x18, ILI9341_COLOR_GREEN, ILI9341_COLOR_BLACK);
    lcd_buffer00[0]++;
    if(lcd_buffer00[0] > 0x39) {
      lcd_buffer00[0] = 0x30;
    }
  }

  TM_ILI9341_SetLayer1();
  TM_ILI9341_Fill(ILI9341_COLOR_BLACK);
  TM_ILI9341_Puts(5, 5, "Android HelloADK test", &TM_Font_11x18, ILI9341_COLOR_BLUE, ILI9341_COLOR_BLACK);
  TM_ILI9341_Puts(110, 100, "OFF", &TM_Font_11x18, ILI9341_COLOR_GREEN, ILI9341_COLOR_BLACK);
  TM_ILI9341_Puts(45, 200, "USER BUTTON OFF", &TM_Font_11x18, ILI9341_COLOR_GREEN, ILI9341_COLOR_BLACK);
  //=====================================================================================================

  //init usb
  usb_init();

  while (1)
  {
    //usb task
    usb_task();

    Delay(1);

    if (i++ == 100)
    {
      STM_EVAL_LEDToggle(LED1);
      i = 0;
    }
  }
}


#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line
  number,ex: printf("Wrong parameters value: file %s on line %d\r\n",
  file, line) */

  /* Infinite loop */
  while (1)
  {}
}

#endif

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 1 ms.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}
