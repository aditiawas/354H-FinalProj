all: smoothing 

smoothing: smoothing.o subdivobjects.o catmull.o loop.o
	gcc -g -o smoothing smoothing.o subdivobjects.o catmull.o loop.o -lfltk_images -lfltk -lstdc++ -lGL -lglut -lGLEW -lm

.c.o: 
	gcc -g -c $<

clean: 
	rm -rf *.o