#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "svm.h"

void predict(struct svm_model *model, struct svm_node *x, double *res)
{
	double predict_label;
	predict_label = svm_predict_probability(model, x, res + 1);
	res[0] = predict_label;
}

int main(int argc, char **argv)
{
	const char *PATH_MODEL = argv[1];
	const int DIM_FEAT = 51;

	struct svm_model *model;
	if ((model = svm_load_model(PATH_MODEL)) == 0)
	{
		fprintf(stderr, "can't open model file %s\n", PATH_MODEL);
		exit(1);
	}

	// double label = 1;
	// double data[DIM_FEAT] = {0.827586, -0.111111, 0.787184, 0.892857, 0.0188679, 0.64693, 0.932203, -0.0196078, 0.769894, 0.242424, -0.392857, -0.00681186, 0.896552, -0.163636, 1.04912, -0.260274, -0.428571, -0.110251, 0.194969, -0.142857, 0.498866, -0.607362, 0.135802, -0.529445, -0.0552147, 0.0638298, 0.412176, -0.210191, 0.486911, -0.81763, 0.396226, 0.575758, 0.681913, -0.391304, 0.590643, 0.211009, -0.0191083, 0.66474, 0.164127, -0.319149, 1, -0.958485, 0.28, 1, -0.862893, -0.287234, 0.934272, -0.924708, -0.387978, 1, -0.994235};

	double data[DIM_FEAT];
	char *endptr;
	for (int i = 0; i < DIM_FEAT; i++)
	{
		data[i] = strtod(argv[i+2], &endptr);
	}

	struct svm_node *x;
	int max_nr_attr = 64;
	x = (struct svm_node *)malloc(max_nr_attr * sizeof(struct svm_node));

	for (int i = 0; i < DIM_FEAT; i++)
	{
		x[i].index = i + 1;
		x[i].value = data[i];
	}
	x[DIM_FEAT].index = -1;

	int nr_class = svm_get_nr_class(model);
	double res[nr_class + 1];
	predict(model, x, res);
	printf("%g", res[0]);
	for (int j = 0; j < nr_class; j++)
		printf(" %g", res[j + 1]);
	printf("\n");

	svm_free_and_destroy_model(&model);
	free(x);

	return 0;
}
