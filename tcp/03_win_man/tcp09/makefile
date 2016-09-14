main:
	gcc -D_REENTRANT -Wall -o rawtest.out testraw.c ../../common/common.c ../../common/rawops.c -lpthread
	gcc -D_GNU_SOURCE -Wall -o server.out tcpserver.c ../../common/common.c ../../common/rawops.c ../../common/tcp_private.c

nowarn:
	gcc -D_REENTRANT -o rawtest.out testraw.c ../../common/common.c ../../common/rawops.c -lpthread
	gcc -D_GNU_SOURCE -o server.out tcpserver.c ../../common/common.c ../../common/rawops.c ../../common/tcp_private.c
	
clean:
	rm *.out
	
