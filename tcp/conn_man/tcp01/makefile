main:tcpserver.c servecho.c tcpclient.c clistr.c readline.c ../../common/common.c
	gcc -Wall -o server.out tcpserver.c servecho.c ../../common/common.c
	gcc -Wall -o client.out tcpclient.c clistr.c readline.c ../../common/common.c
	
nowarn:tcpserver.c servecho.c tcpclient.c clistr.c readline.c ../../common/common.c
	gcc -o server.out tcpserver.c servecho.c ../../common/common.c
	gcc -o client.out tcpclient.c clistr.c readline.c ../../common/common.c
	
clean:
	rm *.out *.o

