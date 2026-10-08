#include "board_io.h"
#include <stdbool.h>
#include <stdint.h>
#include "main.h"
/* CubeMX mapping: PB0/TIM3_CH3 drives light PWM; PB1/TIM3_CH4 drives fan PWM. */
extern TIM_HandleTypeDef htim3;
void board_io_init(void) { HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3); HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4); board_light_set(false,0u); board_fan_set(false,0u); }
void board_light_set(bool on,uint8_t level) {
    uint32_t compare=on?((uint32_t)level*htim3.Init.Period)/254u:0u;
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,compare);
    HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,on?GPIO_PIN_SET:GPIO_PIN_RESET);
}
void board_fan_set(bool on,uint8_t percent) {
    if(percent>100u)percent=100u;
    uint32_t compare=on?((uint32_t)percent*htim3.Init.Period)/100u:0u;
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,compare);
}
