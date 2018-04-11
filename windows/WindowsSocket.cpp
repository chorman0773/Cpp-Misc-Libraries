/*
 * WindowsSocket.cpp
 *
 *  Created on: Mar 29, 2018
 *      Author: Connor Horman
 *  This file implements the Platform specific SocketIO code for Windows.
 *  In all cases this file should be compiled if and only if
 *  the intended target platform is Windows, and the SocketIO api is used.
 *  Requires winsock2 to be linked seperately (either via a static library or via a source file.)
 */

#include <array>
#include <string>
#include <memory>

#include "SocketIO.hpp"
#include "IOWrapper.hpp"


#include "winsock2.h"

using std::array;
using std::string;
using std::to_string;

class WinSocketInputStream : public InputStream{
private:
	uint32_t sock;
public:
	WinSocketInputStream(uint32_t sock):sock(sock){}
	void close(){
		//Do nothing, let the handle deal with closing
	}
	int read(){
		char c;
		recv(sock,&c,1,1);
		return c;
	}
	template<size_t size> int read(array<char,size>& a){
		recv(sock,a.data(),1,size);
	}
	vector<uint8_t> readAll(){
		throw "Can't read all bytes from a socket connection";
	}

};
class WinSocketOutputStream : public OutputStream{
private:
	uint32_t sock;
public:
	WinSocketOutputStream(uint32_t sock):sock(sock){}
	void flush(){
		//Do nothing, Outflushed
	}
	void write(uint8_t i){
		send(sock,(const char*)&i,1,1);
	}
	template<size_t size> int write(array<char,size>& a,int offset=0,int len=size){
		send(sock,a.data()+offset,1,len);
	}
};

void connectToServer(uint32_t socket, const char*  addrName,int port){
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_addr.S_un = inet_addr(addrName);
	target.sin_port = htons(port);
	connect(socket,(sockaddr*)&target,(int)sizeof(target.sin_addr));
}

void openServer(uint32_t socket,const char* addrName,int port){
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_addr.S_un = inet_addr(addrName);
	target.sin_port = htons(port);
	bind(socket,(sockaddr*)&target,(int)sizeof(target.sin_addr));
	listen(socket,port);
}



class WinSocketImpl : public SocketImpl{
private:
	uint32_t sock;
	WinSocketInputStream in;
	WinSocketOutputStream out;
public:
	WinSocketImpl(){
		sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		in(sock);
		out(sock);
	}
	WinSocketImpl(uint32_t opened){
		sock = opened;
		in(sock);
		out(sock);
	}
	~WinSocketImpl(){
		closesocket(sock);
	}
	void connectTo(string addr,int port){
		connectToServer(sock,addr.c_str(),port);
	}
	InputStream* getInputStream(){
		return &in;
	}
	OutputStream* getOutputStream(){
		return &out;
	}
};

using std::unique_ptr;
using std::vector;


class WinServerSocketImpl : public ServerSocketImpl{
private:
	uint32_t server;
	vector<unique_ptr<WinSocketImpl>> ptrs;
public:
	WinServerSocketImpl(){
		server = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	}
	~WinServerSocketImpl(){
		closesocket(server);
	}
	SocketImpl* accept(){
		uint32_t target = ::accept(server,NULL,NULL);
		unique_ptr<WinSocketImpl> ret = new WinSocketImpl(target);
		ptrs.push_back(ret);
		return ret.get();
	}
	void bindTo(string addr,int port){
		openServer(server,addr.c_str(),port);
	}
};

Socket::Socket(){
	stats = new WinSocketImpl;
}
Socket::Socket(string addr,int port){
	stats = new WinSocketImpl;
	stats->connectTo(addr,port);
}

ServerSocket::ServerSocket(){
	stats = new WinServerSocketImpl;
}
ServerSocket::ServerSocket(string addr,int port){
	stats = new WinServerSocketImpl;
	stats->bindTo(addr,port);
}



