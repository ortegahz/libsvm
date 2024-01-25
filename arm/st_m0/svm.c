#include "svm.h"

extern struct svm_node *x_space_new;
extern double *sv_coef_new;

#define Malloc(type, n) (type *)malloc((n) * sizeof(type))

void read_model_header_hard_code(struct svm_model *model)
{
	struct svm_parameter *param = &model->param;
	// parameters for training only won't be assigned, but arrays are assigned as NULL for safety
	param->nr_weight = 0;
	param->weight_label = NULL;
	param->weight = NULL;

	param->svm_type = 0;
	param->kernel_type = 2;
	param->degree = 0;
	param->gamma = 0.5;
	param->coef0 = 0.0;
	model->nr_class = 2;
	printf("model->nr_class -> %d \n", model->nr_class);
	model->l = 248;
	model->rho = Malloc(double, 1);
	if (model->rho == NULL)
		printf("model->rho Malloc error !!! \n");
	model->rho[0] = 45.508316488952168;
	model->label = Malloc(int, 2);
	if (model->label == NULL)
		printf("model->label Malloc error !!! \n");
	model->label[0] = 1;
	model->label[1] = -1;
	model->probA = Malloc(double, 1);
	if (model->probA == NULL)
		printf("model->probA Malloc error !!! \n");
	model->probA[0] = -0.26574469547452156;
	model->probB = Malloc(double, 1);
	model->probB[0] = -0.1356215583183184;
	// prob_density_marks
	model->nSV = Malloc(int, 2);
	if (model->nSV == NULL)
		printf("model->nSV Malloc error !!! \n");
	model->nSV[0] = 122;
	model->nSV[1] = 126;
	printf("model->nSV[1] -> %d \n", model->nSV[1]);
}

struct svm_model *svm_load_model_hard_code()
{
	printf("step 0 ... \n");
	struct svm_model *model = Malloc(struct svm_model, 1);
	if (model == NULL)
		printf("model Malloc error !!! \n");
	printf("addr model -> %04x \n", model);
	model->rho = NULL;
	model->probA = NULL;
	model->probB = NULL;
	model->prob_density_marks = NULL;
	model->sv_indices = NULL;
	model->label = NULL;
	model->nSV = NULL;

	printf("step 1 ... \n");

	// set header
	read_model_header_hard_code(model);

	printf("step 2 ... \n");

	// set sv_coef and SV
	int elements = 6121;
	int m = model->nr_class - 1;
	int l = model->l;
	model->sv_coef = Malloc(double *, m);
	if (model->sv_coef == NULL)
		printf("model->sv_coef Malloc error !!! \n");
	int i;
	for (i = 0; i < m; i++)
	{
		// model->sv_coef[i] = Malloc(double, l);
		model->sv_coef[i] = sv_coef_new;
		printf("addr model->sv_coef[i] -> %04x \n", model->sv_coef[i]);
	}

	model->SV = Malloc(struct svm_node *, l);
	if (model->SV == NULL)
		printf("model->SV Malloc error !!! \n");
	// model->SV = modelSVBuffer;

	// struct svm_node *x_space = NULL;
	// x_space = Malloc(struct svm_node, elements);
	// if (x_space== NULL) printf("x_space Malloc error !!! \n");

	printf("addr model->SV -> %04x \n", model->SV);
	// printf("addr x_space -> %04x \n", x_space);

	printf("step 3 ... \n");
	// int j = 0;
	for (int j = 0, i = 0; i < l; i++)
	{
		model->SV[i] = &x_space_new[j];
		while (x_space_new[j++].index != -1);

		// model->sv_coef[0][i] = 2048;
		// for (int k = 0; k < 24; k++)
		// {
		// 	double val = 6.58069e-05;
		// 	x_space[j].index = k + 1;
		// 	x_space[j].value = val;
		// 	// struct svm_node *p = x_space + j;
		// 	// p->index = k + 1;
		// 	// p->value = val;
		// 	// model->SV[i][j].index = k + 1;
		// 	// model->SV[i][j].value = val;
		// 	// printf("x_space[%d] index -> %d && value -> %lf \n", j, x_space[j].index, x_space[j].value);
		// 	// printf("model->SV[%d][%d] index -> %d && value -> %lf \n", i, j, model->SV[i][j].index, model->SV[i][j].value);
		// 	// printf("p index -> %d && value -> %lf \n", p->index, p->value);
		// 	++j;
		// }
		// x_space[j++].index = -1;
		// printf("x_space[%d] index -> %d && value -> %lf \n", j, x_space[j].index, x_space[j].value);
	}

	// for (int i = 0; i < j; i++)
	// {
	// 	printf("x_space[%i] index -> %d && value -> %lf \n", i, x_space[i].index, x_space[i].value);
	// }

	// for (int i = 0; i < l; i++)
	// {
	// 	int k = 0;
	// 	while (model->SV[i][k++].index != -1)
	// 	{
	// 		printf("model->SV[%d][%d] index -> %d and value -> %lf \n", i, k - 1, model->SV[i][k - 1].index, model->SV[i][k - 1].value);
	// 	}
	// }

	printf("step end ... \n");

	model->free_sv = 1; // XXX
	return model;
}

double k_function_hard_code(const struct svm_node *x, const struct svm_node *y,
							const struct svm_parameter *param)
{
	double sum = 0;

	// while (x->index != -1 && y->index != -1)
	while (x->index != -1)
	// for (int i = 0; i < 25; i++)
	{
		double d = x->value - y->value;
		// double d = x->value - 0.56;
		sum += d * d;
		++x;
		++y;
	}

	while (x->index != -1)
	{
		// printf("x->index -> %d \n", x->index);
		sum += x->value * x->value;
		++x;
	}

	while (y->index != -1)
	{
		// printf("y->index -> %d \n", y->index);
		sum += y->value * y->value;
		++y;
	}

	return exp(-param->gamma * sum);
}

double svm_predict_values_hard_code(const struct svm_model *model, const struct svm_node *x, double *dec_values)
{
	printf("svm_predict_values_hard_code step 0 \n");
	int i;
	int nr_class = model->nr_class;
	int l = model->l;

	printf("svm_predict_values_hard_code step 1 \n");
	double *kvalue = Malloc(double, l);
	for (i = 0; i < l; i++)
		kvalue[i] = k_function_hard_code(x, model->SV[i], &model->param);

	printf("svm_predict_values_hard_code step 2 \n");

	int *start = Malloc(int, nr_class);
	start[0] = 0;
	for (i = 1; i < nr_class; i++)
		start[i] = start[i - 1] + model->nSV[i - 1];

	int *vote = Malloc(int, nr_class);
	for (i = 0; i < nr_class; i++)
		vote[i] = 0;

	int p = 0;
	for (i = 0; i < nr_class; i++)
		for (int j = i + 1; j < nr_class; j++)
		{
			double sum = 0;
			int si = start[i];
			int sj = start[j];
			int ci = model->nSV[i];
			int cj = model->nSV[j];

			int k;
			double *coef1 = model->sv_coef[j - 1];
			double *coef2 = model->sv_coef[i];
			for (k = 0; k < ci; k++)
				sum += coef1[si + k] * kvalue[si + k];
			for (k = 0; k < cj; k++)
				sum += coef2[sj + k] * kvalue[sj + k];
			sum -= model->rho[p];
			dec_values[p] = sum;

			printf("dec_values[p] -> %lf\n", dec_values[p]);
			if (dec_values[p] > 0)
				++vote[i];
			else
				++vote[j];
			p++;
		}

	int vote_max_idx = 0;
	printf("vote[%d] -> %d\n", 0, vote[0]);
	for (i = 1; i < nr_class; i++)
	{
		printf("vote[%d] -> %d\n", i, vote[i]);
		if (vote[i] > vote[vote_max_idx])
			vote_max_idx = i;
	}

	printf("svm_predict_values_hard_code step end \n");

	free(kvalue);
	free(start);
	free(vote);
	return model->label[vote_max_idx];
}

double svm_predict_hard_code(const struct svm_model *model, const struct svm_node *x)
{
	int nr_class = model->nr_class;
	double *dec_values;
	dec_values = Malloc(double, nr_class *(nr_class - 1) / 2);

	double pred_result = svm_predict_values_hard_code(model, x, dec_values);

	free(dec_values);
	return pred_result;
}