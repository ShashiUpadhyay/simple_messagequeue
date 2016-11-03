.PHONY: a.out
a.out:
	gcc -c -pthread MQ.c
	g++ -pthread -o sender sender.cpp MQ.o
	g++ -pthread -o receiver receiver.cpp MQ.o
