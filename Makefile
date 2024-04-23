all: smoothing 

smoothing: smoothing.o subdivobjects.o
	gcc  -o smoothing smoothing.o subdivobjects.o -lfltk_images -lfltk -lstdc++ -lGL -lglut -lGLEW

.c.o: 
	gcc -g -c $<

clean: 
	rm -rf *.o