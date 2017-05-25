# CRFtoy
CRFtoy is another CRF tool writing in c++ which is optimized with L-BFGS and OWLQN<br>
Optimize L2 regularization with L-BFGS and L1 with OWLQN

## How to make
  $ ./configure<br>
  $ make<br>
## usage
training<br>
./crf_train [options] trainFile templateFile modelFile<br>
There are 2 major parameters to control the training condition<br>
-c --cost2<br>
    the cost parameter of L2 regularization (default 1.0)<br>
-d --cost1<br>
    the cost parameter of L1 regularization (default 0.0, meaning L1 is not used)<br>
testing<br>
    ./crf_test [options] testFile modelFile outputFile<br>
   
