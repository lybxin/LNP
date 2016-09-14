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
	gcc -Wall -o $(client_out) $(tcpclient_c) $(common_c) 
	
nowarn:
	gcc -o $(client_out) $(tcpclient_c) $(common_c) 
	
clean:
	rm $(rm_out)
