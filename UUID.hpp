/*
 * UUID.hpp
 *
 *  Created on: Apr 5, 2018
 *      Author: Connor Horman
 */

#ifndef __UUID_HPP__18_04_05_09_14_24
#define __UUID_HPP__18_04_05_09_14_24

#include <stdint.h>
#include <string>
#include <iostream>
#include "IOWrapper.hpp"
#include "Hash.hpp"
using std::string;
using std::istream;
using std::ostream;

/*
 * Class that can store a 128-bit Universaly Unique Identifier.
 */
class UUID: public Hashable{
private:
	uint64_t high;
	uint64_t low;
public:
	/*
	 * Default Construtor.
	 * Does not produce a NIL UUID
	 */
	UUID();
	/*
	 * Constructs a UUID given 2 unsigned 64-bit values.
	 * First value is the high, second value is the low
	 */
	UUID(uint64_t,uint64_t);
	/*
	 * Reads a UUID from a String.
	 * This Constructor is to be used when you have a string representation
	 * of a UUID.
	 * Use newNamespaceUUID or newSHANamespaceUUID for namespaced UUIDs.
	 */
	UUID(const string&);

	/*
	 * Factory to produce a Version-1, Varient-2 UUID with a random MAC address
	 */
	static UUID newTimebasedUUID();
	/*
	 * Factory to produce a random (version-4) UUID.
	 */
	static UUID randomUUID();
	/*
	 * Factory to produce a Version 3 (MD5 Hash) Namespace UUID.
	 * Not to be used for Cryptographic Purposes
	 */
	static UUID newNamespaceUUID(const char*,size_t);
	/*
	 * Factory to produce a Version 5 (SHA Hash) Namespace UUID.
	 * Not to be used for Cryptographic Purposes
	 */
	static UUID newSHANamespaceUUID(const char*,size_t);
	/*
	 * Constructs a UUID with 0 in both fields
	 */
	static UUID newNilUUID();
	uint64_t getHigh()const;
	uint64_t getLow()const;
	string toString()const;
	int32_t hashCode()const;
	bool operator==(const UUID&)const;
};

ostream& operator<<(ostream&,const UUID&);
istream& operator>>(istream&,UUID&);
DataOutputStream& operator<<(DataOutputStream&,const UUID&);
DataInputStream& operator>>(DataInputStream&,const UUID&);



#endif /* __UUID_HPP__18_04_05_09_14_24 */
