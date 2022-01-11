CFLAGS=-I.. -g3 -pthread 

all: part2-mutex part3-semaphores

mc-circ-buffer: mc-circ-buffer.c ../mcmini.h
	gcc ${CFLAGS} -o $@ $<

part2-mutex: part2-philosophers-mutex.c 
	gcc ${CFLAGS} -o $@ $<

# mc-mutex-deadlock: mc-mutex-deadlock.cpp
# 	g++ ${CFLAGS} -o $@ $<

part3-semaphores: part3-philosophers-sem.c 
	gcc ${CFLAGS} -o $@ $<

check: ../mcmini all
	for file in mc-[a-bd-z]*.c; do \
	  echo ""; \
	  echo "***** $$file *****"; \
	  ../mcmini --max-frontier-length 3000 `basename $$file .c`; \
	done

clean:
	rm -f mc-circ-buffer mc-mutex-deadlock mc-philosophers-deadlock
	cd mc-aba && make clean
