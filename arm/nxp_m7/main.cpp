/*
 * Copyright 2020-2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "board_init.h"
#include "demo_config.h"
#include "demo_info.h"
#include "fsl_debug_console.h"
#include "image.h"
#include "image_utils.h"
#include "model.h"
#include "output_postproc.h"
#include "timer.h"

#include "svm.h"
#include "input_data.h"

int print_null(const char *s, ...) { return 0; }

static int (*info)(const char *fmt, ...) = &printf;

struct svm_node *x;
int max_nr_attr = 64;

struct svm_model *model;
int predict_probability = 0;

static char *line = NULL;
static int max_line_len;

static char *readline(FILE *input)
{
	int len;

	if (fgets(line, max_line_len, input) == NULL)
		return NULL;

	while (strrchr(line, '\n') == NULL)
	{
		max_line_len *= 2;
		line = (char *)realloc(line, max_line_len);
		len = (int)strlen(line);
		if (fgets(line + len, max_line_len - len, input) == NULL)
			break;
	}
	return line;
}

void exit_input_error(int line_num)
{
	fprintf(stderr, "Wrong input format at line %d\n", line_num);
	exit(1);
}

void predict_buff(FILE *input)
{
	int correct = 0;
	int total = 0;
	double error = 0;
	double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

	int svm_type = svm_get_svm_type(model);
	int nr_class = svm_get_nr_class(model);
	double *prob_estimates = NULL;
	int j;

	if (predict_probability)
	{
		if (svm_type == NU_SVR || svm_type == EPSILON_SVR)
			info("Prob. model for test data: target value = predicted value + z,\nz: Laplace distribution e^(-|z|/sigma)/(2sigma),sigma=%g\n", svm_get_svr_probability(model));
		else if (svm_type == ONE_CLASS)
		{
			// nr_class = 2 for ONE_CLASS
			prob_estimates = (double *)malloc(nr_class * sizeof(double));
			printf("label normal outlier\n");
		}
		else
		{
			int *labels = (int *)malloc(nr_class * sizeof(int));
			svm_get_labels(model, labels);
			prob_estimates = (double *)malloc(nr_class * sizeof(double));
			printf("labels");
			for (j = 0; j < nr_class; j++)
				printf(" %d", labels[j]);
			printf("\n");
			free(labels);
		}
	}

	max_line_len = 1024;
	line = (char *)malloc(max_line_len * sizeof(char));
	while (readline(input) != NULL)
	{
		int i = 0;
		double target_label, predict_label;
		char *idx, *val, *label, *endptr;
		int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0

		label = strtok(line, " \t\n");
		if (label == NULL) // empty line
			exit_input_error(total + 1);

		target_label = strtod(label, &endptr);
		if (endptr == label || *endptr != '\0')
			exit_input_error(total + 1);

		while (1)
		{
			if (i >= max_nr_attr - 1) // need one more for index = -1
			{
				max_nr_attr *= 2;
				x = (struct svm_node *)realloc(x, max_nr_attr * sizeof(struct svm_node));
			}

			idx = strtok(NULL, ":");
			val = strtok(NULL, " \t");

			if (val == NULL)
				break;
			errno = 0;
			x[i].index = (int)strtol(idx, &endptr, 10);
			if (endptr == idx || errno != 0 || *endptr != '\0' || x[i].index <= inst_max_index)
				exit_input_error(total + 1);
			else
				inst_max_index = x[i].index;

			errno = 0;
			x[i].value = strtod(val, &endptr);
			if (endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				exit_input_error(total + 1);

			++i;
		}
		x[i].index = -1;

		if (predict_probability && (svm_type == C_SVC || svm_type == NU_SVC || svm_type == ONE_CLASS))
		{
			predict_label = svm_predict_probability(model, x, prob_estimates);
			printf("%g", predict_label);
			for (j = 0; j < nr_class; j++)
				printf(" %g", prob_estimates[j]);
			printf("\n");
		}
		else
		{
			predict_label = svm_predict(model, x);
			printf("%.17g\n", predict_label);
		}

		if (predict_label == target_label)
			++correct;
		error += (predict_label - target_label) * (predict_label - target_label);
		sump += predict_label;
		sumt += target_label;
		sumpp += predict_label * predict_label;
		sumtt += target_label * target_label;
		sumpt += predict_label * target_label;
		++total;
	}
	if (svm_type == NU_SVR || svm_type == EPSILON_SVR)
	{
		info("Mean squared error = %g (regression)\n", error / total);
		info("Squared correlation coefficient = %g (regression)\n",
			 ((total * sumpt - sump * sumt) * (total * sumpt - sump * sumt)) /
				 ((total * sumpp - sump * sump) * (total * sumtt - sumt * sumt)));
	}
	else
		info("Accuracy = %g%% (%d/%d) (classification)\n",
			 (double)correct / total * 100, correct, total);
	if (predict_probability)
		free(prob_estimates);
}

int main(void)
{
    BOARD_Init();
    TIMER_Init();

    // DEMO_PrintInfo();

    // if (MODEL_Init() != kStatus_Success)
    // {
    //     PRINTF("Failed initializing model" EOL);
    //     for (;;)
    //     {
    //     }
    // }

    // tensor_dims_t inputDims;
    // tensor_type_t inputType;
    // uint8_t *inputData = MODEL_GetInputTensorData(&inputDims, &inputType);

    // tensor_dims_t outputDims;
    // tensor_type_t outputType;
    // uint8_t *outputData = MODEL_GetOutputTensorData(&outputDims, &outputType);
    FILE *input;
	input = fmemopen(__tools_smartsd_scale, __tools_smartsd_scale_len, "r");
	if (input == NULL)
	{
		fprintf(stderr, "can't open input file \n");
		// fprintf(stderr, "can't open input file %s\n", argv[i]);
		// exit(1);
	}

	// if ((model = svm_load_model(argv[i + 1])) == 0)
	if ((model = svm_load_model_buff()) == 0)
	{
		fprintf(stderr, "can't open model file \n");
		// fprintf(stderr, "can't open model file %s\n", argv[i + 1]);
		// exit(1);
	}

	x = (struct svm_node *)malloc(max_nr_attr * sizeof(struct svm_node));
	if (predict_probability)
	{
		if (svm_check_probability_model(model) == 0)
		{
			fprintf(stderr, "Model does not support probabiliy estimates\n");
			exit(1);
		}
	}
	else
	{
		if (svm_check_probability_model(model) != 0)
			info("Model supports probability estimates, but disabled in prediction.\n");
	}

	// predict(input, output);
	predict_buff(input);
	svm_free_and_destroy_model(&model);
	free(x);
	free(line);
	fclose(input);
	// fclose(output);
	return 0;

    // while (1)
    // {
    //     // /* Expected tensor dimensions: [batches, height, width, channels] */
    //     // if (IMAGE_GetImage(inputData, inputDims.data[2], inputDims.data[1], inputDims.data[3]) != kStatus_Success)
    //     // {
    //     //     PRINTF("Failed retrieving input image" EOL);
    //     //     for (;;)
    //     //     {
    //     //     }
    //     // }

    //     // MODEL_ConvertInput(inputData, &inputDims, inputType);

    //     auto startTime = TIMER_GetTimeInUS();
    //     // MODEL_RunInference();
    //     auto endTime = TIMER_GetTimeInUS();
    //     PRINTF("----------------------------------------" EOL);
    //     PRINTF("     Inference time: %d ms" EOL, (endTime - startTime) / 1000);
    //     PRINTF("----------------------------------------" EOL);

    //     // MODEL_ProcessOutput(outputData, &outputDims, outputType, endTime - startTime);
    // }
}
