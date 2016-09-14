main:
	#gcc -D_REENTRANT -Wall -o rawtest.out testraw.c ../../common/common.c ../../common/rawops.c -lpthread
	gcc -D_REENTRANT -Wall -o server.out tcpserver.c ../../common/common.c ../../common/rawops.c ../../common/tcp_private.c -lpthread
	gcc -D_REENTRANT -Wall -o client.out tcpclient.c  ../../common/common.c ../../common/rawops.c ../../common/tcp_private.c -lpthread

nowarn:
	#gcc -D_REENTRANT -o rawtest.out testraw.c ../../common/common.c ../../common/rawops.c -lpthread
	gcc -D_REENTRANT -o server.out tcpserver.c ../../common/common.c ../../common/rawops.c ../../common/tcp_private.c -lpthread
	gcc -D_REENTRANT -o client.out tcpclient.c  ../../common/common.c ../../common/rawops.c ../../common/tcp_private.c -lpthread
	
clean:
	rm *.out
	
