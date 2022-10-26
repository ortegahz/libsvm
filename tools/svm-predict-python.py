from subprocess import *

path_model = '/media/manu/kingstop/workspace/libsvm/tools/hpe_train_pick.model'
data = [0.827586, -0.111111, 0.787184, 0.892857, 0.0188679, 0.64693, 0.932203, -0.0196078, 0.769894, 0.242424, -0.392857, -0.00681186, 0.896552, -0.163636, 1.04912, -0.260274, -0.428571, -0.110251, 0.194969, -0.142857, 0.498866, -0.607362, 0.135802, -0.529445, -0.0552147, 0.0638298, 0.412176, -0.210191, 0.486911, -0.81763, 0.396226, 0.575758, 0.681913, -0.391304, 0.590643, 0.211009, -0.0191083, 0.66474, 0.164127, -0.319149, 1, -0.958485, 0.28, 1, -0.862893, -0.287234, 0.934272, -0.924708, -0.387978, 1, -0.994235]

cmd = f'../svm-predict-python {path_model}'
for feat in data:
    cmd += f' {feat}'

f = Popen(cmd, shell=True, stdout=PIPE).stdout

line = f.readline()
label, pos_p, neg_p = map(float, line.split())
print((label, pos_p, neg_p))
