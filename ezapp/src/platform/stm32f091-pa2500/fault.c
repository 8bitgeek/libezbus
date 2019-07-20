/**
  ******************************************************************************
  * @file    fault.c
  * @author  Mike Sharkey
  ******************************************************************************
  */
#include "board.h"

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
extern void Error_Handler(void)
{
	/* User may add here some code to deal with this error */
	for(;;);
}

extern void _fault(void)
{
  for(;;);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	for(;;);
}
#endif