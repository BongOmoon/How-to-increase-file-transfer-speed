CFLAGS = -W -Wall -g
TARGET = cli0.3
obj1 = cli0.3
OBJECTS = $(obj1).o

all: $(TARGET)

$(TARGET): $(OBJECTS)
			 $(CC) -o $@ $^

clean:
		rm *.o $(TARGET)
