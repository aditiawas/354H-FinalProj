all: smoothing

smoothing-apple: catmull.o loop.o
	g++ -std=c++11 -w -g render.cpp -o render -framework GLUT -framework OpenGL
	g++ -std=c++11 -w -g smoothing.cpp -o smoothing catmull.o loop.o -lfltk

smoothing: catmull.o loop.o render.o 
	g++ -w -g -O2 -Wall -Wextra -Wpedantic render.cpp -o render -std=c++11 -lfltk -lGLEW -lglfw -lGL -lGLU -lglut -lm -lstdc++ -lpthread
	g++ -w -g -O2 -Wall -Wextra -Wpedantic smoothing.cpp -o smoothing catmull.o loop.o -std=c++11 -lfltk -lGLEW -lglfw -lGL -lGLU -lglut -lm -lstdc++ -lpthread

.c.o: 
	g++ -std=c++11 -w -g -c $<

clean: 
	rm -rf *.o *.out smoothing render *.dSYM assets/*/*.out