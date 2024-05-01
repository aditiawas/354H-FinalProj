all: smoothing

smoothing-apple: catmull.o loop.o
	g++ -std=c++11 -w -g render.cpp -o render -framework GLUT -framework OpenGL
	g++ -std=c++11 -w -g smoothing.cpp -o smoothing catmull.o loop.o -lfltk

smoothing: catmull.o loop.o
	g++ -std=c++11 -w -g render.cpp -o render -lfltk_images -lfltk -lstdc++ -lGL -lglut -lGLEW -lm -lGLU
	g++ -std=c++11 -w -g smoothing.cpp -o smoothing catmull.o loop.o -lfltk_images -lfltk -lstdc++ -lGL -lglut -lGLEW -lGLU -lm

.c.o: 
	g++ -std=c++11 -w -g -c $<

clean: 
	rm -rf *.o *.out smoothing render *.dSYM assets/*/*.out