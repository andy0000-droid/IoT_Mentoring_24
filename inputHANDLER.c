// option 1
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	for (int i = 0; i < 16; i++)
	{
		if ((GPIO_Pin >> i) == 0x0001)
		{
			input = i;
			INPUT_FLAG = True;
		}
	}
}

// option 2
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

}

// option 3
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
}