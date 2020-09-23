AS=as
CC=gcc
CPP=cpp
CXX=g++
LD=ld
OBJCOPY = objcopy
OBJDUMP = objdump
STRIP = strip

LIB_DIR = ./lib
INC_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
LIBCSVDIR = ./libcsv-3.0.3
BIN_DIR = ./bin
TESTBIN_DIR = ./testbin
CXXFLAGS = -std=c++14 $(INCLUDE)

UNAME := $(shell uname)

PKGS = 

#DEBUG_MODE=TRUE
ifdef DEBUG_MODE
DEFINES  += -DDEBUG
CFLAGS   += -g -ggdb -D_GLIBCXX_DEBUG
else 
CFLAGS   += -O3
endif

ifeq ($(UNAME), Darwin)
# add paths if developing on mac with macports
INCLUDE     += -I '/opt/local/include'
TESTLDFLAGS += -L '/opt/local/lib'
endif

INCLUDE         += -I $(INC_DIR)
CFLAGS          +=  -Wall
LDFLAGS         += 
TESTLDFLAGS     += -lgtest -lgtest_main  -lpthread
LIBCSV_LIB = $(LIB_DIR)/libcsv.a
CPPFLAGS        += -std=c++14 -lexpat
PROJ_NAME       = proj4
TESTCSV_NAME   = testrouter

MAIN_OBJ        = $(OBJ_DIR)/speedtest.o

CSVTEST_OBJ    = $(OBJ_DIR)/testrouter.o


PROJ_OBJS       = $(OBJ_DIR)/CSVReader.o         \
                  $(OBJ_DIR)/CSVWriter.o		 \
				  $(OBJ_DIR)/MapRouter.o		 \
				  $(OBJ_DIR)/StringUtils.o 	 	 
		 

all: $(LIBCSV_LIB) directories test $(BIN_DIR)/$(PROJ_NAME)

./obj/XMLReader.o: $(SRC_DIR)/XMLReader.cpp $(INC_DIR)/XMLReader.h $(INC_DIR)/XMLEntity.h $(INC_DIR)/StringUtils.h
	$(CXX) $(SRC_DIR)/XMLReader.cpp -c $(CXXFLAGS) -o $(OBJ_DIR)/XMLReader.o

./obj/XMLWriter.o: $(SRC_DIR)/XMLWriter.cpp $(INC_DIR)/XMLWriter.h $(INC_DIR)/XMLEntity.h $(INC_DIR)/StringUtils.h
	$(CXX) $(SRC_DIR)/XMLWriter.cpp -c $(CXXFLAGS) -o $(OBJ_DIR)/XMLWriter.o

$(LIBCSVDIR)/Makefile: 
	cd $(LIBCSVDIR); chmod 755 configure; ./configure --prefix=$(CURDIR); cd ..

$(LIBCSVDIR)/libcsv.la: $(LIBCSVDIR)/Makefile
	cd $(LIBCSVDIR); make; cd ..

$(LIBCSV_LIB): $(LIBCSVDIR)/libcsv.la
	cd $(LIBCSVDIR); make install; cd ..


test: $(TESTBIN_DIR)/$(TESTCSV_NAME)
	$(TESTBIN_DIR)/$(TESTCSV_NAME)

#main
$(BIN_DIR)/$(PROJ_NAME): $(PROJ_OBJS) ./obj/XMLReader.o ./obj/XMLWriter.o $(MAIN_OBJ) $(LIBCSV_LIB)
	$(CXX) $(MAIN_OBJ) $(PROJ_OBJS) ./obj/XMLReader.o ./obj/XMLWriter.o -o $(BIN_DIR)/$(PROJ_NAME) $(CFLAGS) $(CPPFLAGS) $(DEFINES) $(LDFLAGS) $(LIBCSV_LIB)

#TestCases
$(TESTBIN_DIR)/$(TESTCSV_NAME): $(PROJ_OBJS) ./obj/XMLReader.o ./obj/XMLWriter.o $(CSVTEST_OBJ) $(LIBCSV_LIB)
	$(CXX) $(PROJ_OBJS) ./obj/XMLReader.o ./obj/XMLWriter.o $(CSVTEST_OBJ) -o $(TESTBIN_DIR)/$(TESTCSV_NAME) $(CFLAGS) $(CPPFLAGS) $(DEFINES) $(TESTLDFLAGS) $(LIBCSV_LIB)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@
	
.PHONY: directories
directories:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(TESTBIN_DIR)
	
clean:
	#rm -f lib/*
	rm -f include/csv.h
	cd $(LIBCSVDIR); make clean; cd ..
	rm -f $(LIBCSVDIR)/Makefile
	-f -rm $(OBJ_DIR)/*.o $(INC_DIR)/*.*~ $(SRC_DIR)/*.*~
	
.PHONY: clean