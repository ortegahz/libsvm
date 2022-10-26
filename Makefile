CXX ?= g++
# CFLAGS = -Wall -Wconversion -O3 -fPIC
CFLAGS = -Wall -Wconversion -g -fPIC
SHVER = 3
OS = $(shell uname)

all: svm-train svm-predict svm-scale svm-predict-python

lib: svm.o
	if [ "$(OS)" = "Darwin" ]; then \
		SHARED_LIB_FLAG="-dynamiclib -Wl,-install_name,libsvm.so.$(SHVER)"; \
	else \
		SHARED_LIB_FLAG="-shared -Wl,-soname,libsvm.so.$(SHVER)"; \
	fi; \
	$(CXX) $${SHARED_LIB_FLAG} svm.o -o libsvm.so.$(SHVER)

svm-predict-python: svm-predict-python.c svm.o
	$(CXX) $(CFLAGS) svm-predict-python.c svm.o -o svm-predict-python -lm
svm-predict: svm-predict.c svm.o
	$(CXX) $(CFLAGS) svm-predict.c svm.o -o svm-predict -lm
svm-train: svm-train.c svm.o
	$(CXX) $(CFLAGS) svm-train.c svm.o -o svm-train -lm
svm-scale: svm-scale.c
	$(CXX) $(CFLAGS) svm-scale.c -o svm-scale
svm.o: svm.cpp svm.h
	$(CXX) $(CFLAGS) -c svm.cpp
clean:
	rm -f *~ svm.o svm-train svm-predict svm-predict-python svm-scale libsvm.so.$(SHVER)
