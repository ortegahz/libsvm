/*!
    \file    main.c
    \brief   USART printf demo

    \version 2023-06-28, V1.1.3, demo for GD32W51x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32w51x.h"
#include "systick.h"
#include <stdio.h>
#include "gd32w515p_eval.h"

void test_status_led_init(void);
void flash_led(int times);
void test_key_init(void);

#include "svm.h"
#include "data.h"

extern size_t strlen(const char *);

int print_null(const char *s, ...) { return 0; }

static int (*info)(const char *fmt, ...) = &printf;

struct svm_node *x;
struct svm_node *x_space_new;
double *sv_coef_new;
int max_nr_attr = 32;

struct svm_model *model;
int predict_probability = 0;

static char *line = NULL;
static int max_line_len;

const int a = 0;

void predict_hard_code()
{
    int correct = 0;
    int total = 0;
    double error = 0;
    double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

    int svm_type = 0;
    int nr_class = 2;
    int j;

    int i = 0;
    double target_label, predict_label;
    char *idx, *val, *label, *endptr;
    int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0

    printf("start ... \n");
    predict_label = svm_predict_hard_code(model, x);
    printf("end ... \n");

    // double *prob_estimates = NULL;
    // predict_label = svm_predict_probability_hard_code(model, x, prob_estimates);
    // if (predict_probability)
    // 	free(prob_estimates);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* ----------------------------------------------------------------------------
    TIMER2 Configuration:
    TIMER2CLK = SystemCoreClock/9000 = 20KHz.
    TIMER2 configuration is timing mode, and the timing is 0.2s(4000/20000 = 0.2s).
    ---------------------------------------------------------------------------- */
    timer_parameter_struct timer_initpara;

    /* enable the peripherals clock */
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    rcu_periph_clock_enable(RCU_TIMER2);

    /* deinit a TIMER */
    timer_deinit(TIMER2);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER2 configuration */
    timer_initpara.prescaler = 8999;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 65535;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_init(TIMER2, &timer_initpara);

    /* clear interrupt bit */
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
    /* enable the TIMER interrupt */
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);
    /* enable a TIMER */
    timer_enable(TIMER2);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    systick_config();
    /* initialize test status LED */
    test_status_led_init();
    /* flash LED for test */
    flash_led(2);
    /* test key initialize */
    test_key_init();
    /* USART initialize */
    gd_eval_com_init(EVAL_COM0);

    /* configure the TIMER peripheral */
    timer_config();

    /* the software must wait until TC=1. the TC flag remains cleared during all data
    transfers and it is set by hardware at the last frame end of transmission */
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TC))
        ;

    printf("\n\r SystemCoreClock --> %d \n\r ", SystemCoreClock);

    printf("addr predict_probability -> %04x \n", &predict_probability);

    x_space_new = (struct svm_node *)x_space_hex;
    // for (int i = 0; i < TOTAL_SV_FEAT; i++)
    // {
    //   printf("x_space_new[%d] --> %d, %lf \n", i, (x_space_new + i)->index, (x_space_new + i)->value);
    // }

    sv_coef_new = (double *)sv_coef_hex;
    for (int i = 0; i < TOTAL_SV; i++)
    {
        printf("sv_coef_new[%d] --> %lf \n", i, sv_coef_new[i]);
    }

    model = svm_load_model_hard_code();
    printf("svm_load_model_hard_code() done ... \n");

    // x = (struct svm_node *)malloc(max_nr_attr * sizeof(struct svm_node));
    // if (x == NULL)
    //   printf("x Malloc error !!! \n");
    x = (struct svm_node *)x_hex;
    printf("addr x -> %04x \n", x);

    for (int i = 0; i < (NUM_OF_FEATS + 1); i++)
    {
        printf("x[%d] --> %d, %lf \n", i, (x + i)->index, (x + i)->value);
    }

    while (1)
    {
        timer_counter_value_config(TIMER2, 0);
        // delay_1ms(50);
        predict_hard_code();
        uint32_t _cnt = timer_counter_read(TIMER2);
        float elapse = (float)_cnt * 1. / 20000.;
        printf("\n\r _cnt --> %d \n\r", _cnt);
        printf("\n\r elapse --> %f ms \n\r", elapse * 1000.);
    }

    // free(x);
    // free(line);
    return 0;
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE))
        ;
    return ch;
}

/*!
    \brief      test status LED initialize
    \param[in]  none
    \param[out] none
    \retval     none
*/
void test_status_led_init(void)
{
    /* LEDs initialize */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
}

/*!
    \brief      LED blink function
    \param[in]  times: the blink times of LEDs
    \param[out] none
    \retval     none
*/
void flash_led(int times)
{
    int i;

    for (i = 0; i < times; i++)
    {
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn on LEDs */
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED3);

        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn off LEDs */
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED3);
    }
}

/*!
    \brief      test key init
    \param[in]  none
    \param[out] none
    \retval     none
*/
void test_key_init(void)
{
    gd_eval_key_init(KEY_TAMPER_WAKEUP, KEY_MODE_GPIO);
}
