CFLAGS = -W -Wall -g
TARGET = ser0.3
obj1 = ser0.3
OBJECTS = $(obj1).o

all: $(TARGET)

$(TARGET): $(OBJECTS)
			 $(CC) -o $@ $^ -lpthread

clean:
		rm *.o $(TARGET)

