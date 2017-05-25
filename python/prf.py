import getopt
import sys


def preprocess(examples):
    labels = []
    for example in examples:
        example = example.strip()
        if example == "":
            labels.append("")
        else:
            labels.append(example.split()[-1].strip())
    return labels

def count_seq(labels):
    sum = 0
    i = 0
    while i < len(labels):
        while i < len(labels) and (labels[i] == "" or labels[i] == "O"):
            i += 1
        if i < len(labels) and (labels[i] == "B" or labels[i].startswith("B-")):
            i += 1
            sum += 1
            while i < len(labels) and (labels[i] == "I" or labels[i].startswith("I-")):
                i += 1
        elif i < len(labels):
            # print i, "error label", labels[i]
            i += 1
    return sum

from optparse import OptionParser

if __name__=="__main__":

    usage = "python prf.py [options].\n\
Labels should start with 'B', 'B-', 'I' or 'I-'; and the out label is 'O'."

    optParser = OptionParser(usage = usage)
    optParser.add_option("-e", "--example", dest = "testfile",
            help = "test file which should have labels at last column")
    optParser.add_option("-p", "--prediction", dest = "classfile",
            help = "predicted file which should have labels at last column")
    options, args = optParser.parse_args()

    testfile = options.testfile
    classfile = options.classfile
    
    if testfile == None or len(testfile) == 0 or \
            classfile == None or len(classfile) == 0:
        optParser.print_help()
        exit(0)

    try:
        fp = open(classfile)
    except:
        print "Failed to open file %s, please make sure it exist."%classfile

    predictions = fp.readlines()
    fp.close()

    fp = open(testfile)
    try:
        fp = open(testfile)
    except:
        print "Failed to open file %s, please make sure it exist."%testfile
    examples = fp.readlines()
    fp.close()

    # print len(examples), len(predictions)
    assert(len(examples)==len(predictions) or len(examples)+1==len(predictions))

    example_labels = preprocess(examples)
    prediction_labels = preprocess(predictions)
    assert(len(example_labels) == len(prediction_labels) or
            len(example_labels)+1 == len(prediction_labels))

    #print "##################### count examples ######################"
    example_sum = count_seq(example_labels)
    #print "################### count predictions #####################"
    prediction_sum = count_seq(prediction_labels)

    common = 0
    i = 0
    while i < len(example_labels):
        while i < len(example_labels) and (example_labels[i] == "" or
                example_labels[i] == "O"):
            i += 1
        flag = True
        if i < len(example_labels) and (example_labels[i] == "B"
                or example_labels[i].startswith("B-")):
            if example_labels[i] != prediction_labels[i]:
                flag = False
            i += 1
            while i < len(example_labels) and (example_labels[i] == "I"
                    or example_labels[i].startswith("I-")):
                if example_labels[i] != prediction_labels[i]:
                    flag = False
                i += 1
            if flag:
                common += 1
        elif i < len(example_labels):
            i += 1
            #print i, "error, example_label:", example_labels[i],\
            #        "predict_label:", prediction_labels[i]

    print common, example_sum, prediction_sum
    print "recall=%s"%(common * 1.0 / example_sum)
    print "precision=%s"%(common * 1.0 / prediction_sum)
    print "f1-score=%s"%(2.0*common/(example_sum + prediction_sum))

