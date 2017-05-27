chmod +x crf_train crf_test

time ./crf_train data/train.data data/template model
echo
./crf_test data/test.data model output
echo
python python/prf.py -e data/test.data -p output

