
CXX = g++



CURDIR = $(shell pwd)
LIBDIR = ./lib
OBJDIR = ./obj
INCLUDEDIR = ./include
SRCDIR = ./src
LIBCSVDIR = libcsv-3.0.3
LIBCSV_NAME = libcsv.a

INCLUDE = -I $(INCLUDEDIR)
CXXFLAGS = -std=c++14 $(INCLUDE)

CSV_READER_OBJ = $(OBJDIR)/CSVReader.o
CSV_WRITER_OBJ = $(OBJDIR)/CSVWriter.o
CSV_TEST_OBJ   = $(OBJDIR)/testcsv.o

all: $(LIBDIR)/$(LIBCSV_NAME)

$(CSV_READER_OBJ): $(SRCDIR)/CSVReader.cpp $(INCLUDEDIR)/CSVReader.h
	$(CXX) $(SRCDIR)/CSVReader.cpp $(CXXFLAGS) -o $(CSV_READER_OBJ)

$(CSV_WRITER_OBJ): $(SRCDIR)/CSVWriter.cpp $(INCLUDEDIR)/CSVWriter.h
	$(CXX) $(SRCDIR)/CSVWriter.cpp $(CXXFLAGS) -o $(CSV_WRITER_OBJ)

$(LIBCSVDIR)/Makefile: 
	cd $(LIBCSVDIR); ./configure --prefix=$(CURDIR); cd ..

$(LIBCSVDIR)/libcsv.la: $(LIBCSVDIR)/Makefile
	cd $(LIBCSVDIR); make; cd ..

$(LIBDIR)/$(LIBCSV_NAME): $(LIBCSVDIR)/libcsv.la
	cd $(LIBCSVDIR); make install; cd ..

clean:
	rm -f lib/*
	rm -f include/csv.h
	cd $(LIBCSVDIR); make clean; cd ..
	rm -f $(LIBCSVDIR)/Makefile
