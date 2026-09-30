MPICXX = mpic++
CFLAGS = 
all: main

%.o: %.cpp 
	$(MPICXX) -o $@ -c $< 

main: main.o fonctions.o mnt.o
	$(MPICXX) -o $@ $^ 

clean:
	@rm -rf *.o

cleanall: clean
	@rm -rf main
