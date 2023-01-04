/* Copyright (c) 2021 ALIF SEMICONDUCTOR

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ALIF SEMICONDUCTOR nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
#include <stdio.h>
#include "Driver_GPIO.h"
#include <RTE_Components.h>
#include CMSIS_device_header

#define GPIO1_PIN14              (14) /*< ds1 led connected to this gpio pin >*/
#define GPIO1_PIN15              (15) /*< ds2 led connected to this gpio pin >*/
#define GPIO1                    1 /* Use LEDS in the GPIO1 port */

extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(GPIO1);
ARM_DRIVER_GPIO *ptrDrv = &ARM_Driver_GPIO_(GPIO1);

void alif_evaluation_led_setup(uint8_t led_ds1)
{
  /*
   * gpio1 pin14 is connected active high led ds1.
   * gpio1 pin15 is connected active high led ds2.
   */

//   uint8_t led_ds1 = GPIO1_PIN14;
//   uint8_t led_ds2 = GPIO1_PIN15;

   ptrDrv->Initialize(led_ds1, NULL);
   ptrDrv->PowerControl(led_ds1, ARM_POWER_FULL);
   ptrDrv->SetDirection(led_ds1, GPIO_PIN_DIRECTION_OUTPUT);
   ptrDrv->SetValue(led_ds1, GPIO_PIN_OUTPUT_STATE_HIGH);
}

void alif_evaluation_board_led_toggle(uint8_t led)
{
  ptrDrv->SetValue(led, GPIO_PIN_OUTPUT_STATE_TOGGLE);
}

void alif_evaluation_board_led_enable(uint8_t led)
{
  ptrDrv->SetValue(led, GPIO_PIN_OUTPUT_STATE_HIGH);
}

void alif_evaluation_board_led_disable(uint8_t led)
{
  ptrDrv->SetValue(led, GPIO_PIN_OUTPUT_STATE_LOW);
}
