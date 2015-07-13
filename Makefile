CFLAGS=-std=c++11

all: crf_classify crf_learn clean
.PHONY: all

objects=crf_learn.o corpus.o crf_train.o example.o idMap.o labelMap.o \
 model.o template.o print.o string_lib.o
crf_learn:$(objects)
	g++ -o crf_learn $(objects)

objects=crf_classify.o corpus.o crf_train.o example.o idMap.o labelMap.o \
 model.o template.o print.o string_lib.o crf_test.o
crf_classify: $(objects)
	g++ -o crf_classify $(objects)

crf_classify.o: crf_test.hpp
	g++ $(CFLAGS) -c crf_classify.cpp

crf_learn.o: crf_train.hpp example.hpp lib/print.hpp \
 lib/string_lib.hpp model.hpp idMap.hpp template.hpp labelMap.hpp \
 corpus.hpp
	g++ $(CFLAGS) -c crf_learn.cpp

corpus.o: corpus.hpp lib/print.hpp labelMap.hpp idMap.hpp
	g++ $(CFLAGS) -c corpus.cpp

crf_train.o: example.hpp lib/print.hpp lib/string_lib.hpp \
 template.hpp corpus.hpp labelMap.hpp idMap.hpp crf_train.hpp model.hpp \
 matrix.hpp cube.hpp
	g++ $(CFLAGS) -c crf_train.cpp

cube.o: cube.hpp
	g++ $(CFLAGS) -c cube.cpp

example.o: example.hpp lib/print.hpp lib/string_lib.hpp
	g++ $(CFLAGS) -c example.cpp

idMap.o: idMap.hpp
	g++ $(CFLAGS) -c idMap.cpp
	
labelMap.o: labelMap.hpp
	g++ $(CFLAGS) -c labelMap.cpp

matrix.o: matrix.hpp
	g++ $(CFLAGS) -c matrix.cpp

model.o: model.hpp idMap.hpp template.hpp labelMap.hpp
	g++ $(CFLAGS) -c model.cpp 

template.o: template.hpp
	g++ $(CFLAGS) -c template.cpp

print.o: lib/print.hpp
	g++ $(CFLAGS) -c lib/print.cpp
	
string_lib.o: lib/string_lib.hpp
	g++ $(CFLAGS) -c lib/string_lib.cpp
	
crf_test.o: crf_test.hpp template.hpp model.hpp idMap.hpp \
 labelMap.hpp corpus.hpp lib/print.hpp example.hpp lib/string_lib.hpp \
 matrix.hpp cube.hpp crf_train.hpp
	g++ $(CFLAGS) -c crf_test.cpp
	
clean:
	rm *.o