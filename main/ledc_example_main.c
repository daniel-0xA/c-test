/* Ledc fade example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"
#include "freertos/queue.h"
#include "driver/ledc.h"
#include "esp_attr.h"   
#include "esp_err.h"

/*
 * About this example
 * 1. init LEDC module:
 *    a. You need to set the timer of LEDC first, this decide the frequency and resolution of PWM.
 *    b. You need to set the LEDC channel you want to use, and bind with one of the timers.
 *
 * 2. You can install a default fade function, then you can use fade APIs.
 *
 * 3. You can also set a target duty directly without fading.
 *
 * 4. This example use GPIO18/19/4/5 as LEDC ouput, and it will change the duty repeatedly.
 *
 * 5. GPIO18/19 are from high speed channel group. GPIO4/5 are from low speed channel group.
 *
 */
#define LEDC_HS_TIMER          LEDC_TIMER_0
//#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (18)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO       (19)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1

#define LEDC_LS_TIMER          LEDC_TIMER_1
//#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_LS_CH2_GPIO       (4)
#define LEDC_LS_CH2_CHANNEL    LEDC_CHANNEL_2
#define LEDC_LS_CH3_GPIO       (5)
#define LEDC_LS_CH3_CHANNEL    LEDC_CHANNEL_3

#define LEDC_TEST_CH_NUM       (4)
typedef struct {
    int channel;
    int io;
    int mode;
    int timer_idx;
} ledc_info_t;

void app_main()
{
    int ch;
    ledc_info_t ledc_ch[LEDC_TEST_CH_NUM] = {
        {
            .channel   = 0,
            .io        = LEDC_HS_CH0_GPIO,
            .mode      = 0,
            .timer_idx = LEDC_HS_TIMER
        },
        {
            .channel   = 1,
            .io        = LEDC_HS_CH1_GPIO,
            .mode      = 0,
            .timer_idx = LEDC_HS_TIMER
        },
        {
            .channel   = 2,
            .io        = LEDC_LS_CH2_GPIO,
            .mode      = 1,
            .timer_idx = LEDC_LS_TIMER
        },
        {
            .channel   = 3,
            .io        = LEDC_LS_CH3_GPIO,
            .mode      = 1,
            .timer_idx = LEDC_LS_TIMER
        }
    };

    ledc_timer_config_t ledc_timer = {
        .bit_num = LEDC_TIMER_13_BIT, //set timer counter bit number
        .freq_hz = 3250,              //set frequency of pwm
        .speed_mode = 0,   //timer mode,
        .timer_num = LEDC_HS_TIMER    //timer index
    };
    //configure timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    //configure timer1 for low speed channels
    ledc_timer.speed_mode = 1;
    ledc_timer.timer_num = LEDC_LS_TIMER;
    ledc_timer_config(&ledc_timer);

    for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
        ledc_channel_config_t ledc_channel = {
            //set LEDC channel 0
            .channel = ledc_ch[ch].channel,
            //set the duty for initialization.(duty range is 0 ~ ((2**bit_num)-1)
            .duty = 0,
            //GPIO number
            .gpio_num = ledc_ch[ch].io,
            //GPIO INTR TYPE, as an example, we enable fade_end interrupt here.
            .intr_type = LEDC_INTR_FADE_END,
            //set LEDC mode, from ledc_mode_t
            .speed_mode = ledc_ch[ch].mode,
            //set LEDC timer source, if different channel use one timer,
            //the frequency and bit_num of these channels should be the same
            .timer_sel = ledc_ch[ch].timer_idx,
        };
        //set the configuration
        ledc_channel_config(&ledc_channel);
    }

    //initialize fade service.
    ledc_fade_func_install(0);
    while (1) {
        // printf("LEDC fade up\n");
        // for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            // ledc_set_fade_with_time(ledc_ch[ch].mode, ledc_ch[ch].channel, 4000, 2000);
            // ledc_fade_start(ledc_ch[ch].mode, ledc_ch[ch].channel, LEDC_FADE_NO_WAIT);
        // }
        // vTaskDelay(3000 / portTICK_PERIOD_MS);

        // printf("LEDC fade down\n");
        // for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            // ledc_set_fade_with_time(ledc_ch[ch].mode, ledc_ch[ch].channel, 0, 2000);
            // ledc_fade_start(ledc_ch[ch].mode, ledc_ch[ch].channel, LEDC_FADE_NO_WAIT);
        // }
        // vTaskDelay(3000 / portTICK_PERIOD_MS);

        printf("LEDC set duty without fade\n");
        for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            ledc_set_duty(ledc_ch[ch].mode, ledc_ch[ch].channel, 3250);
            ledc_update_duty(ledc_ch[ch].mode, ledc_ch[ch].channel);
        }

    }
}
