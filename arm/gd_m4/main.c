/*!
    \file  main.c
    \brief USART printf
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/
#include <stdio.h>

#include "gd32f30x.h"
#include "gd32f30x_eval.h"
#include "systick.h"

#include "svm.h"

extern size_t strlen(const char *);

int print_null(const char *s, ...) { return 0; }

static int (*info)(const char *fmt, ...) = &printf;

struct svm_node *x;
int max_nr_attr = 32;

struct svm_model *model;
int predict_probability = 0;

static char *line = NULL;
static int max_line_len;

// const struct svm_node *modelSVBuffer[248] __attribute__((section(".text"))) = {0};
const struct svm_node x_space[6121] = {0};

const int a = 0;

void predict_hard_code()
{
    int correct = 0;
    int total = 0;
    double error = 0;
    double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

    int svm_type = 0;
    int nr_class = 2;
    double *prob_estimates = NULL;
    int j;

    int i = 0;
    double target_label, predict_label;
    char *idx, *val, *label, *endptr;
    int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0

    for (i = 0; i < 24; i++)
    {
        x[i].index = i;
        x[i].value = 7.04817e-05;
        // printf("x[%d] index -> %d && value -> %lf \n", i, x[i].index, x[i].value);
    }
    x[i].index = -1;
    // printf("x[%d] index -> %d && value -> %lf \n", i, x[i].index, x[i].value);

    // i = 0;
    // while (x[i].index != -1)
    // {
    //     printf("x[%d].index -> %d \n", i, x[i++].index);
    // }

	printf("start ... \n");
    predict_label = svm_predict_hard_code(model, x);
	printf("end ... \n");
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    // sys init
    gd_eval_com_init(EVAL_COM1);
    printf("system init done ... \n");

    // for (int i = 0; i < 2; i++)
    // {
    //     printf("x_space[%d].index -> %d \n", i, x_space[i].index);
    //     printf("x_space[%d].value -> %lf \n", i, x_space[i].value);
    // }

    printf("addr predict_probability -> %04x \n", &predict_probability);

    model = svm_load_model_hard_code();
    printf("svm_load_model_hard_code() done ... \n");

    x = (struct svm_node *)malloc(max_nr_attr * sizeof(struct svm_node));
    if (x == NULL)
        printf("x Malloc error !!! \n");

    while (1)
    {
        printf("predict_hard_code() start ... \n");
        predict_hard_code();
        printf("predict_hard_code() end ... \n");
    }
    
    // printf("predict_hard_code() start ... \n");
    // predict_hard_code();
    // printf("predict_hard_code() end ... \n");
    // svm_free_and_destroy_model(&model);
    free(x);
    free(line);
    return 0;
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while (RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE))
        ;
    return ch;
}
