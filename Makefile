all: smoothing

smoothing: catmull.o loop.o
	g++ -std=c++11 -w -g render.cpp -o render -framework GLUT -framework OpenGL
	g++ -std=c++11 -w -g smoothing.cpp -o smoothing catmull.o loop.o -lfltk
#ADITI works for me even with fewer flags this way but just for reference, original one is below
#g++ -std=c++11 -w -g -o smoothing smoothing.o catmull.o loop.o -lfltk_images -lfltk -lstdc++ -lGL -lglut -lGLEW -lm -framework GLUT -framework OpenGL

.c.o: 
	g++ -std=c++11 -w -g -c $<

clean: 
	rm -rf *.o *.out smoothing *.dSYM assets/*/*.out