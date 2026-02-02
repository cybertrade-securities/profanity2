CC=g++
CDEFINES=
SOURCES=Dispatcher.cpp Mode.cpp precomp.cpp profanity.cpp SpeedSample.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=profanity2

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_S),Darwin)
	LDFLAGS=-framework OpenCL
	CFLAGS=-c -std=c++11 -Wall -O2
	ifeq ($(UNAME_M),x86_64)
		CFLAGS += -mmmx
	endif
else ifneq (,$(findstring MINGW,$(UNAME_S)))
	EXECUTABLE=profanity2.exe
	LDFLAGS=-s -lOpenCL -lws2_32 -mcmodel=large -static-libgcc -static-libstdc++
	CFLAGS=-c -std=c++11 -Wall -mmmx -O2 -mcmodel=large 
else ifneq (,$(findstring MSYS,$(UNAME_S)))
	EXECUTABLE=profanity2.exe
	LDFLAGS=-s -lOpenCL -lws2_32 -mcmodel=large -static-libgcc -static-libstdc++
	CFLAGS=-c -std=c++11 -Wall -mmmx -O2 -mcmodel=large 
else ifneq (,$(findstring CYGWIN,$(UNAME_S)))
	EXECUTABLE=profanity2.exe
	LDFLAGS=-s -lOpenCL -lws2_32 -mcmodel=large -static-libgcc -static-libstdc++
	CFLAGS=-c -std=c++11 -Wall -mmmx -O2 -mcmodel=large 
else
	LDFLAGS=-s -lOpenCL -mcmodel=large
	CFLAGS=-c -std=c++11 -Wall -mmmx -O2 -mcmodel=large 
endif

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(CDEFINES) $< -o $@

clean:
	rm -rf *.o cache-opencl.* $(EXECUTABLE)

