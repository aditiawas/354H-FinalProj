all: smoothing 

smoothing: smoothing.o subdivobjects.o
	gcc  -o smoothing smoothing.o subdivobjects.o -lfltk_images -lfltk -lstdc++

.c.o: 
	gcc -g -c $<

clean: 
	rm -rf *.o