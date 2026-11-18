CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3
TARGET = gerador_off
SRCS = main.cpp sculptor.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp sculptor.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) cenario_lobos.off cenario_lobos_completo.off

.PHONY: all run clean
