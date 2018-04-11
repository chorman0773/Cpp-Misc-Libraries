/*
 * SocketIO.hpp
 *
 *  Created on: Mar 29, 2018
 *      Author: Connor Horman
 */

#ifndef __NET_SOCKETIO_HPP__18_03_29_09_16_02
#define __NET_SOCKETIO_HPP__18_03_29_09_16_02

#include <string>

#include "IOWrapper.hpp"

using std::string;

class Socket;

class SocketImpl{
public:
	virtual ~SocketImpl();
	virtual void connectTo(string,int)=0;
	virtual InputStream* getInputStream()=0;
	virtual OutputStream* getOutputStream()=0;
};

class ServerSocketImpl{
public:
	virtual ~ServerSocketImpl();
	virtual void bindTo(string,int)=0;
	virtual Socket* accept()=0;
};


class Socket{
private:
	SocketImpl* stats;
public:
	Socket(string,int);
	Socket(SocketImpl*);
	Socket();
	~Socket();
	void connect(string,int);
	void close();
	InputStream* getInputStream();
	OutputStream* getOutputStream();
	bool isOpen();
};

class ServerSocket{
private:
	ServerSocketImpl* stats;
public:
	ServerSocket(string,int);
	ServerSocket();
	~ServerSocket();
	void bindTo(string,int);
	void close();
	Socket* accept();
};



#endif /* __NET_SOCKETIO_HPP__18_03_29_09_16_02 */
