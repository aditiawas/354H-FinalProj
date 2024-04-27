all: smoothing

smoothing: smoothing.o subdivobjects.o catmull.o loop.o
	g++ -std=c++11 -w -g -o smoothing smoothing.o subdivobjects.o catmull.o loop.o -lfltk_images -lfltk -lstdc++ -lGL -lglut -lGLEW -lm

.c.o: 
	g++ -std=c++11 -w -g -c $<

clean: 
	rm -rf *.o