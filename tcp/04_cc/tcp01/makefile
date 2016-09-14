mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))
common_c := $(mkfile_dir)../../common/common.c
rawops_c := $(mkfile_dir)../../common/rawops.c
tcp_private_c := $(mkfile_dir)../../common/tcp_private.c
testraw_c := $(mkfile_dir)testraw.c
tcpserver_c := $(mkfile_dir)tcpserver.c
tcpclient_c := $(mkfile_dir)tcpclient.c

rawtest_out := $(mkfile_dir)rawtest.out
server_out := $(mkfile_dir)server.out
client_out := $(mkfile_dir)client.out

main:
	gcc -D_REENTRANT -Wall -o $(rawtest_out) $(testraw_c) $(common_c) $(rawops_c) -lpthread
	gcc -D_REENTRANT -Wall -o $(server_out) $(tcpserver_c) $(common_c) $(rawops_c) $(tcp_private_c) -lpthread
	#gcc -D_REENTRANT -Wall -o client.out tcpclient.c  ../../common/common.c ../../common/rawops.c ../../common/tcp_private.c -lpthread

nowarn:
	gcc -D_REENTRANT -o rawtest.out testraw.c ../../common/common.c ../../common/rawops.c -lpthread
	gcc -D_REENTRANT -o server.out tcpserver.c ../../common/common.c ../../common/rawops.c ../../common/tcp_private.c -lpthread
	#gcc -D_REENTRANT -o client.out tcpclient.c  ../../common/common.c ../../common/rawops.c ../../common/tcp_private.c -lpthread
	
clean:
	rm *.out
	
