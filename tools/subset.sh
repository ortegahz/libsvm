number=$1

echo $number

python subset.py -s 1 /home/manu/tmp/hpe_train $number /home/manu/tmp/hpe_train_pick
python subset.py -s 1 /home/manu/tmp/hpe_test $number /home/manu/tmp/hpe_test_pick