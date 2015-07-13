import getopt
import sys
def usage():
    print "Usage: python prf.py -e TestFile -p ClassificationFile"

if __name__=="__main__":
    testfile=sys.argv[1]
    classfile=sys.argv[2]
            
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

