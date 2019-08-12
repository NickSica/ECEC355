SOURCE	:= Main.c Parser.c Registers.c Core.c
CC	:= gcc
TARGET	:= RVSim

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) -g -o $(TARGET) $(SOURCE)

matrix: $(TARGET)
	./RVSim cpu_traces/uncommented_matrix

example: $(TARGET)
	./RVSim cpu_traces/example_cpu_trace

clean:
	rm $(TARGET)
