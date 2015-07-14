CFLAGS=-std=c++11

all: crf_classify crf_learn clean
.PHONY: all

objects=crf_learn.o corpus.o crf_train.o example.o idmap.o labelmap.o \
 model.o template.o print.o string_lib.o
crf_learn:$(objects)
	g++ -o crf_learn $(objects)

objects=crf_classify.o corpus.o crf_train.o example.o idmap.o labelmap.o \
 model.o template.o print.o string_lib.o crf_test.o
crf_classify: $(objects)
	g++ -o crf_classify $(objects)

crf_classify.o: crf_test.hpp
	g++ $(CFLAGS) -c crf_classify.cpp

crf_learn.o: crf_train.hpp example.hpp lib/print.hpp \
 lib/string_lib.hpp model.hpp idmap.hpp template.hpp labelmap.hpp \
 corpus.hpp
	g++ $(CFLAGS) -c crf_learn.cpp

corpus.o: corpus.hpp lib/print.hpp labelmap.hpp idmap.hpp
	g++ $(CFLAGS) -c corpus.cpp

crf_train.o: example.hpp lib/print.hpp lib/string_lib.hpp \
 template.hpp corpus.hpp labelmap.hpp idmap.hpp crf_train.hpp model.hpp \
 matrix.hpp cube.hpp
	g++ $(CFLAGS) -c crf_train.cpp

cube.o: cube.hpp
	g++ $(CFLAGS) -c cube.cpp

example.o: example.hpp lib/print.hpp lib/string_lib.hpp
	g++ $(CFLAGS) -c example.cpp

idmap.o: idmap.hpp
	g++ $(CFLAGS) -c idmap.cpp
	
labelmap.o: labelmap.hpp
	g++ $(CFLAGS) -c labelmap.cpp

matrix.o: matrix.hpp
	g++ $(CFLAGS) -c matrix.cpp

model.o: model.hpp idmap.hpp template.hpp labelmap.hpp
	g++ $(CFLAGS) -c model.cpp 

template.o: template.hpp
	g++ $(CFLAGS) -c template.cpp

print.o: lib/print.hpp
	g++ $(CFLAGS) -c lib/print.cpp
	
string_lib.o: lib/string_lib.hpp
	g++ $(CFLAGS) -c lib/string_lib.cpp
	
crf_test.o: crf_test.hpp template.hpp model.hpp idmap.hpp \
 labelmap.hpp corpus.hpp lib/print.hpp example.hpp lib/string_lib.hpp \
 matrix.hpp cube.hpp crf_train.hpp
	g++ $(CFLAGS) -c crf_test.cpp
	
clean:
	rm *.o