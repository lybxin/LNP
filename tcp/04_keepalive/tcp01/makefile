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

rm_out := $(mkfile_dir)*.out
main:
	@echo "\n-------------------$(mkfile_dir)------------------------------"
	gcc -D_REENTRANT -Wall -o $(rawtest_out) $(testraw_c) $(common_c) $(rawops_c) -lpthread
	gcc -D_REENTRANT -Wall -o $(server_out) $(tcpserver_c) $(common_c) $(rawops_c) $(tcp_private_c) -lpthread
	#gcc -D_REENTRANT -Wall -o $(client_out) $(tcpclient_c)  $(common_c) $(rawops_c) $(tcp_private_c) -lpthread

nowarn:
	@echo "test nowarn"
	gcc -D_REENTRANT -o $(rawtest_out) $(testraw_c) $(common_c) $(rawops_c) -lpthread -g
	gcc -D_REENTRANT -o $(server_out) $(tcpserver_c) $(common_c) $(rawops_c) $(tcp_private_c) -lpthread -g
	#gcc -D_REENTRANT -o $(client_out) $(tcpclient_c)  $(common_c) $(rawops_c) $(tcp_private_c) -lpthread
	
clean:
	rm $(rm_out)
	@echo "test clean"
	
