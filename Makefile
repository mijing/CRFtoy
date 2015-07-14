CFLAGS=-pthread -std=c++11 

all: crf_classify crf_learn clean
.PHONY: all

learnobjs=crf_learn.o corpus.o crf_train.o example.o idmap.o labelmap.o \
 model.o template.o print.o string_lib.o
crf_learn:$(learnobjs)
	g++ -o crf_learn $(learnobjs) $(CFLAGS) -Wl,--no-as-needed

classobjs=crf_classify.o corpus.o crf_train.o example.o idmap.o labelmap.o \
 model.o template.o print.o string_lib.o crf_test.o
crf_classify: $(classobjs)
	g++ -o crf_classify $(classobjs) $(CFLAGS) -Wl,--no-as-needed

crf_classify.o: crf_test.hpp
	g++ -c crf_classify.cpp $(CFLAGS)

crf_learn.o: crf_train.hpp example.hpp lib/print.hpp \
 lib/string_lib.hpp model.hpp idmap.hpp template.hpp labelmap.hpp \
 corpus.hpp
	g++ -c crf_learn.cpp $(CFLAGS)

corpus.o: corpus.hpp lib/print.hpp labelmap.hpp idmap.hpp
	g++ -c corpus.cpp $(CFLAGS)

crf_train.o: example.hpp lib/print.hpp lib/string_lib.hpp \
 template.hpp corpus.hpp labelmap.hpp idmap.hpp crf_train.hpp model.hpp \
 matrix.hpp cube.hpp
	g++ -c crf_train.cpp $(CFLAGS)

cube.o: cube.hpp
	g++ -c cube.cpp $(CFLAGS)

example.o: example.hpp lib/print.hpp lib/string_lib.hpp
	g++ -c example.cpp $(CFLAGS)

idmap.o: idmap.hpp
	g++ -c idmap.cpp $(CFLAGS)
	
labelmap.o: labelmap.hpp
	g++ -c labelmap.cpp $(CFLAGS)

matrix.o: matrix.hpp
	g++ -c matrix.cpp $(CFLAGS)

model.o: model.hpp idmap.hpp template.hpp labelmap.hpp
	g++ -c model.cpp  $(CFLAGS)

template.o: template.hpp
	g++ -c template.cpp $(CFLAGS)

print.o: lib/print.hpp
	g++ -c lib/print.cpp $(CFLAGS)
	
string_lib.o: lib/string_lib.hpp
	g++ -c lib/string_lib.cpp $(CFLAGS)
	
crf_test.o: crf_test.hpp template.hpp model.hpp idmap.hpp \
 labelmap.hpp corpus.hpp lib/print.hpp example.hpp lib/string_lib.hpp \
 matrix.hpp cube.hpp crf_train.hpp
	g++ -c crf_test.cpp $(CFLAGS)
	
clean:
	rm *.o