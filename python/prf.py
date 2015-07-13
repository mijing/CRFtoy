
classfile = "C:/Users/b-yiwwan/Downloads/CRF++-0.58/CRF++-0.58/classification"
testfile = "C:/Users/b-yiwwan/Downloads/CRF++-0.58/CRF++-0.58/example/chunking/train.data"

classfile = "data/classification"
testfile = "data/train.data"



fp = open(classfile)
predictions = fp.readlines()
fp.close()

fp = open(testfile)
examples = fp.readlines()
fp.close()
print len(examples), len(predictions)
assert(len(examples)==len(predictions) or len(examples)+1==len(predictions))

common = 0
sum = 0
for i in range(len(examples)):
    example = examples[i].strip()
    prediction = predictions[i].strip()
    if example=="":
        assert prediction==""
        continue
    sum+=1
    if example.split()[-1]==prediction.split()[-1]:
        common+=1;
    else:
        print  example, prediction, i


print 1.0*common/sum
