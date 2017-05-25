# CRFtoy
CRFtoy is another CRF tool writing in c++ which is optimized with L-BFGS and OWLQN<br>
Optimize L2 regularization with L-BFGS and L1 with OWLQN

## How to make
　　the version of g++ should be 4.8.x or higher<br>
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
# References
　　J. Lafferty, A. McCallum, and F. Pereira. Conditional random fields: Probabilistic models for segmenting and labeling sequence data, In Proc. of ICML, pp.282-289, 2001<br>
　　M Collins. Log-Linear Models, MEMMs, and CRFs. http://www.cs.columbia.edu/~mcollins/crf.pdf<br>
　　Galen Andrew and Gao Jianfeng. Orthant-Wise Limited-memory Quasi-Newton Optimizer for L1-regularized Objectives.<br>
　　libLBFGS. http://www.chokkan.org/software/liblbfgs/<br>
　　L-BFGS code by Jorge Nocedal. http://users.iems.northwestern.edu/~nocedal/lbfgs.html<br>
