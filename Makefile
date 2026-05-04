SDSL_ROOT := sdsl-lite/build
INCLUDEPATHS := -I$(SDSL_ROOT)/include -I$(SDSL_ROOT)/external/libdivsufsort/include
LIBPATHS    := -L$(SDSL_ROOT)/lib -L$(SDSL_ROOT)/external/libdivsufsort/lib
LIBS        := -lsdsl -ldivsufsort -ldivsufsort64

CXX      := g++
CXXFLAGS := -O2 -std=c++17 -Wall -Wno-sign-compare

TARGET := fm_index_demo
SRCS   := main.cpp FM_index.cpp

$(TARGET): $(SRCS) FM_index.hh
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBPATHS) $(INCLUDEPATHS) $(LIBS)

clean:
	rm -f $(TARGET)
