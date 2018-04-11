/*
 * SocketIOShared.cpp
 *
 *  Created on: Mar 29, 2018
 *      Author: Connor Horman
 *
 *   This file contains all Shared code that the Socket API uses.
 *   This file should always be compiled if you are using SocketIO
 */
#include "SocketIO.hpp"

Socket::Socket(SocketImpl* impl):stats(impl){}

void Socket::close(){
	delete stats;
	stats = nullptr;
}
void Socket::connect(string addr,int port){
	stats->connectTo(addr,port);
}
InputStream* Socket::getInputStream(){
	return stats->getInputStream();
}
OutputStream* Socket::getOutputStream(){
	return stats->getOutputStream();
}
bool Socket::isOpen(){
	return stats!=nullptr;
}

Socket::~Socket(){
	close();
}

ServerSocket::~ServerSocket(){
	close();
}

Socket* ServerSocket::accept(){
	return stats->accept();
}

void ServerSocket::bindTo(string addr,int port){
	stats->bindTo(addr,port);
}




