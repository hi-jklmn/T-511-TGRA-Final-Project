target:
	g++ main.cpp models.cpp shader.cpp geometry.cpp -o gltest -std=c++11 -L/usr/lib -lglfw -lGLEW -lGLU -lGL -lassimp
