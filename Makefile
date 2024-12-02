CXXFLAGS = -g3 -Wall -fmessage-length=0 `sdl2-config --cflags` -w

OBJS = wordshooter.o util.o

LIBS = -L/usr/X11R6/lib -lm -lpthread \
       -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib -L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib \
       -lglut -lGLU -lGL -lX11 -lfreeimage \
       `sdl2-config --libs` -lSDL2_mixer
       
TARGET = word-shooter	

$(TARGET):	$(OBJS) 
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)