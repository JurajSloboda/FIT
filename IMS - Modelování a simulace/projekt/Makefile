CC              = g++
CFLAGS         = -g 
OBJ_FILES      = main.o cellMatrix.o cell.o

all: ims

run: experiment1 experiment2 experiment3 experiment4 experiment5 experiment6 experiment7 experiment8 experiment9

ims:	$(OBJ_FILES)
		$(CC) $(OBJ_FILES) -o $@ $(CFLAGS)


main.o: main.cpp cellMatrix.h cell.h
	$(CC) -o $@ -c $<  $(CFLAGS)

cellMatrix.o: cellMatrix.cpp cellMatrix.h
	$(CC) -o $@ -c $<  $(CFLAGS)

cell.o: cell.cpp cell.h
	$(CC) -o $@ -c $<  $(CFLAGS)

clean:
	rm -f *.o ims 

.PHONY: clean

experiment1:
	./ims -t 50 -s 50 -d 3 -r 100 -c 14 -C 21 >Experiment_results/experiment1

experiment2:
	./ims -t 50 -s 50 -d 30 -r 50 -c 14 -C 21 >Experiment_results/experiment2

experiment3:
	./ims -t 50 -s 50 -d 3 -r 100 -c 14 -C 21 -i 2 -I 14 -p 30 >Experiment_results/experiment3

experiment4:
	./ims -t 50 -s 50 -d 3 -r 100 -c 14 -C 21 -i 2 -I 14 -p 70 -m 10 -l 10 >Experiment_results/experiment4

experiment5:
	./ims -t 50 -s 50 -d 3 -r 100 -c 14 -C 21 -i 2 -I 14 -p 70 -m 25 -l 25 >Experiment_results/experiment5

experiment6:
	./ims -t 50 -s 50 -d 3 -r 100 -c 14 -C 21 -i 2 -I 14 -p 70 -m 50 -l 50 >Experiment_results/experiment6

experiment7:
	./ims -t 50 -s 50 -d 3 -r 100 -c 14 -C 21 -i 2 -I 14 -p 70 -m 100 -l 50 >Experiment_results/experiment7

experiment8:	
	./ims -t 50 -s 50 -d 3 -r 100 -c 14 -C 21 -i 2 -I 14 -p 70 -m 100 -l 50 -q 1 >Experiment_results/experiment8

experiment9:	
	./ims -t 50 -s 50 -d 3 -r 100 -c 14 -C 21 -i 2 -I 14 -p 70 -m 100 -l 50 -q 2 >Experiment_results/experiment9