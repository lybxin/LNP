main:
	gcc -D_REENTRANT -Wall -o rawtest.out testraw.c ../../common/common.c ../../common/rawops.c -lpthread
	gcc -Wall -o server.out tcpserver.c ../../common/common.c ../../common/rawops.c

nowarn:
	gcc -D_REENTRANT -o rawtest.out testraw.c ../../common/common.c ../../common/rawops.c -lpthread
	gcc -o server.out tcpserver.c ../../common/common.c ../../common/rawops.c
	
clean:
	rm *.out
	
