CC = g++
CFLAGS = -g -O3 -std=c++17 

SRCS = floorplanner.cpp file_parser.cpp seq_pair.cpp

OBJS = $(SRCS:.cpp=.o)

TARGET = floorplanner

.PHONY: clean

all: $(TARGET) clean

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(OBJS)
