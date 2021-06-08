/******************************************************************************
*  Name: config.h
*  Description: project config file
*  Project: Wave2
*  Auther: 
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/

/**********************************************************************************************
* Global macros common component
**********************************************************************************************/
#define BUFFER_COM                      1
#define QUEUE_COM                       0
#define LIST_COM                        0
#define STACK_COM                       0
#define CRC_COM                         1

/**********************************************************************************************
* Global macros driver layer
**********************************************************************************************/
#define CLK_DRIVER                      1
#define SPI_DRIVER                      1
#define TC_DRIVER                       1
#define TCC_DRIVER                      1
#define IIC_SLAVE_DRIVER                1
#define IIC_MASTER_DRIVER               1
#define RTC_DRIVER                      1
#define EIC_DRIVER                      1
#define FLASH_DRIVER                    1
#define WDOG_DRIVER                     1
#define UART_DRIVER                     1
#define ADC_DRIVER                      1
/**********************************************************************************************
* Global macros middle layer
**********************************************************************************************/
#define CLOCK_MODULE                    1
#define POWER_MODULE                    1
#define FLASH_MODULE                    1
#define LED_MODULE                      1
#define LCD_MODULE                      1
#define WDOG_MODULE                     1
#define DESERIALIZER_MODULE             1
#define ADC_MODULE                      1
#define EEPROM_MODULE                   1
#define THERMAL_MODULE                  1
#define SOAK_MODULE                     1
#define SLEEP_MODULE                    1
#define POST_MODULE                     1   
#define FILTER_MODULE                   1
#define SCHEDULER_MODULE                1    
#define ETM_MODULE                      1
#define DIAGNOSTIC_MODULE               1
#define DMAC_MODULE                     1
#define EVSYS_MODULE                    1
#define PTC_MODULE                      1

#define SWITCHPACK_MODULE               0
#define SWITCHPACK_BACKLIGHT_MODULE     0
#define LOCALWAKEUP_STATUS              1
#define TOUCH_MODULE                    1
/**********************************************************************************************
* Global macros application layer
**********************************************************************************************/
#define CPU_LOAD_APP                    0
#define CPU_LOAD_PRINTF                 0

#define SCHEDULER_APP                   1
#define LIFE_APP                        1

#define BOOT_APP                        0

/**********************************************************************************************
* Global macros for unit test
**********************************************************************************************/

/**********************************************************************************************
* Global macros for while counter
**********************************************************************************************/

