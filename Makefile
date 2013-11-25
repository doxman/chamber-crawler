cc3k: main.o floor.o object.o display.o
	g++ main.o floor.o object.o display.o -o cc3k

main.o: main.cc floor.h
	g++ -c main.cc

floor.o: floor.h floor.cc display.h object.h
	g++ -c floor.cc

object.o: object.cc object.h
	g++ -c object.cc
	
display.o: display.h display.cc
<<<<<<< HEAD
	g++ -c display.cc
=======
	g++ -c display.cc
>>>>>>> ea3d8adced5d4200f7b6240001b608e6233c4fd7
