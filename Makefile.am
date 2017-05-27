UTOMAKE_OPTIONS=foreign
ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = no-dependencies
bin_PROGRAMS=crf_train crf_test
crf_train_SOURCES=corpus.hpp  crf_test.hpp  crf_train.hpp  cube.hpp  example.hpp  idmap.hpp \
				  labelmap.hpp  matrix.hpp  model.hpp  param.hpp  template.hpp \
				  corpus.cpp  crf_test.cpp  crf_train.cpp param.cpp \
				  crf_train_main.cpp  cube.cpp  example.cpp  idmap.cpp  labelmap.cpp  \
				  matrix.cpp  model.cpp  template.cpp \
				  lib/print.cpp  lib/print.hpp  lib/string_lib.cpp  lib/string_lib.hpp \
				  lib/common.h lbfgs/arithmetic_ansi.h  lbfgs/arithmetic_sse_double.h \
				  lbfgs/arithmetic_sse_float.h  lbfgs/lbfgs.h lbfgs/lbfgs.c

crf_test_SOURCES=corpus.hpp  crf_test.hpp  crf_train.hpp  cube.hpp  example.hpp  idmap.hpp \
                 labelmap.hpp  matrix.hpp  model.hpp  param.hpp  template.hpp \
				 corpus.cpp  crf_test.cpp  crf_train.cpp  param.cpp \
				 crf_test_main.cpp  cube.cpp  example.cpp  idmap.cpp  labelmap.cpp \
                 matrix.cpp  model.cpp  template.cpp \
				 lib/print.cpp  lib/print.hpp  lib/string_lib.cpp  lib/string_lib.hpp \
				 lib/common.h lbfgs/arithmetic_ansi.h  lbfgs/arithmetic_sse_double.h \
				 lbfgs/arithmetic_sse_float.h  lbfgs/lbfgs.h  lbfgs/lbfgs.c
