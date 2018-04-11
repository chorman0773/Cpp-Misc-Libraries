/*
 * DynamicLinkController.hpp
 *
 *  Created on: Apr 3, 2018
 *      Author: Connor Horman
 */

#ifndef __DYNAMICLINKCONTROLLER_HPP__18_04_03_12_47_31
#define __DYNAMICLINKCONTROLLER_HPP__18_04_03_12_47_31

#include <string>

using std::string;

class Hook;

class ExtensibleLibrary{
public:
	void addLibraryPath(const string&);
	void loadLibraries();
	void callStartupHooks();
	void callShutdownHooks();
};

struct HookInfo{
	const ptrdiff_t callOffset;
	const unsigned char hash[32];
	void (*getCallHook(ExtensibleLibrary&))();
};

class Extension{
public:
	HookInfo* getCallHooks();
	const unsigned char* computeSHA256();
};



class Hook{
	HookInfo* hook;
public:
	~Hook();
	Hook(HookInfo*);
	void operator()(ExtensibleLibrary&);
};



#endif /* __DYNAMICLINKCONTROLLER_HPP__18_04_03_12_47_31 */
