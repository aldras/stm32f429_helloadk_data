#include "stm32f4xx.h"
#include "usb_task.h"

#include "usb_bsp.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_adk_core.h"

#include "tm_stm32f4_ili9341_ltdc.h"
#include "tm_stm32f4_fonts.h"

/* Private function prototypes */
void usb_receive(void) ;
void usb_send(void);

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE           USB_OTG_Core_dev __ALIGN_END ;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST                     USB_Host __ALIGN_END ;

void usb_init(void) {
  /* Init Host Library */
  USBH_Init(&USB_OTG_Core_dev,
            USB_OTG_HS_CORE_ID,
            &USB_Host,
            &USBH_ADK_cb,
            &USR_Callbacks);

  /* Init ADK Library */
  USBH_ADK_Init("ammlab.org", "HelloADK", "HelloADK for GR-SAKURA for STM32F4", "1.0", "https://play.google.com/store/apps/details?id=org.ammlab.android.helloadk",  "1234567");
}

void usb_task(void) {
    /* Host Task handler */
    USBH_Process(&USB_OTG_Core_dev , &USB_Host);

    /* Accessory Mode enabled */
    ADK_State state = USBH_ADK_getStatus();
    if( state == ADK_IDLE) {
    /* --------------------------------------------------------------------------- */
        // in
        usb_receive();

        // out
        usb_send();
    /* --------------------------------------------------------------------------- */
    }
}

void usb_receive(void) {
    uint8_t msg[2];
    uint16_t len;
    len = USBH_ADK_read(&USB_OTG_Core_dev, msg, sizeof(msg));
    if( len > 0 ){
        if( msg[0] == 0x1){
            if( msg[1] == 0x1) {
                STM_EVAL_LEDOn(LED2);
                TM_ILI9341_Puts(110, 100, "ON ", &TM_Font_11x18, ILI9341_COLOR_GREEN, ILI9341_COLOR_BLACK);
            }else{
                STM_EVAL_LEDOff(LED2);
                TM_ILI9341_Puts(110, 100, "OFF", &TM_Font_11x18, ILI9341_COLOR_GREEN, ILI9341_COLOR_BLACK);
            }
        }
    }
}

void usb_send(void) {
    uint8_t msg[2];
    if( STM_EVAL_PBGetState(BUTTON_USER) ){
        msg[0] = 1;
        msg[1] = 1;
        TM_ILI9341_Puts(45, 200, "USER BUTTON ON ", &TM_Font_11x18, ILI9341_COLOR_GREEN, ILI9341_COLOR_BLACK);
    }else{
        msg[0] = 1;
        msg[1] = 0;
        TM_ILI9341_Puts(45, 200, "USER BUTTON OFF", &TM_Font_11x18, ILI9341_COLOR_GREEN, ILI9341_COLOR_BLACK);
    }
    USBH_ADK_write(&USB_OTG_Core_dev, msg, sizeof(msg));
}
