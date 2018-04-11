/*
 * UUID.cpp
 *
 *  Created on: Apr 5, 2018
 *      Author: Connor Horman
 */

#include "UUID.hpp"

#include "StringHelper.hpp"
#include "Random.hpp"
//#include "OpenSSLHelpers.hpp"//Breaks something. Likely need to fix it.
#ifndef __OpenSSLHelper_hpp_2018_03_26_09_53
extern "C"{
#include <openssl/sha.h>
#include <openssl/md5.h>
};
#endif
#include "IOWrapper.hpp"
#include "JTime.hpp"

Random uuidRand;



#include <string>
#include <iostream>
#include <stdint.h>
#include <iomanip>
#include <array>
using std::string;
using std::stoull;
using std::ostringstream;
using std::setw;
using std::setfill;
using std::hex;
using std::array;

UUID::UUID(){}

UUID::UUID(uint64_t high,uint64_t low):high(high),low(low){}

UUID::UUID(const string& str){
	string reduced;
	string cpy = str;
	if(count(cpy,"-")==0)
		reduced = str;
	else for(int i = 0;i<5;i++)
		reduced+=tokenize(cpy,"-");
	string top(reduced,0,16);
	string bottom(reduced,16,16);
	high = stoull(top);
	low = stoull(bottom);
}

extern const int32_t hashPrime;

int32_t UUID::hashCode()const{
	return hashcode(high)*hashPrime+hashcode(low);
}
bool UUID::operator==(const UUID& o)const{
	return high==o.high&&low==o.low;
}

uint64_t UUID::getHigh()const{
	return high;
}
uint64_t UUID::getLow()const{
	return low;
}

string UUID::toString()const{
	ostringstream ss;
	string ret;
	uint64_t h1 = high/0xFFFFFFF;
	uint64_t h2 = (high/0xFFFF)&0xFFFF;
	uint64_t h3 = high&0xFFFF;
	uint64_t l1 = low/0xFFFFFFFFFFFF;
	uint64_t l2 = low&0xFFFFFFFFFFFF;
	ss << setw(8)<<setfill('0')<<hex<<h1<<setw(0)<<"-"
		<<setw(4)<<setfill('0')<<hex<<h2<<setw(0)<<"-"
		<<setw(4)<<setfill('0')<<hex<<h3<<setw(0)<<"-"
		<<setw(4)<<setfill('0')<<hex<<l1<<setw(0)<<"-"
		<<setw(12)<<setfill('0')<<hex<<l2<<setw(0)<<"-";
	ret = ss.str();
	return ret;
}

UUID UUID::newNilUUID(){
	return UUID(0,0);
}

const Instant UUID_EPOCH = Instant::fromEpochSecond(-12244089600);

UUID UUID::newTimebasedUUID(){
	Instant now = Instant::now();
	Duration d = Duration::between(UUID_EPOCH, now);
	int64_t ts = d.getSeconds()*10000000+d.getNanos()/100;
	int64_t rnode = (uuidRand.nextLong()&0x7FFFFFFFFFFF)|0x800000000000;
	int64_t low = rnode|(0xE000|(d.hashCode()&0xCFFF))<<56LL;
	int64_t high = (ts&0xFFFFFFFF)<<32|((ts>>32)&0xFFFF)<<16|0x1000|((ts>>48)&0xFFF);
	return UUID(high,low);
}

UUID UUID::randomUUID(){
	array<uint8_t,8> bits;
	array<uint8_t,32> output;
	uuidRand.nextBytes<8>(bits);
	SHA256(bits.data(),8,output.data());
	uint64_t high = (output[0]^output[1])<<56|(output[2]^output[3])<<48
					|(output[4]^output[5])<<40|(output[6]^output[7])<<32
					|(output[8]^output[9])<<24|(output[10]^output[11])<<16
					|((output[12]^output[3])&0xf)<<8|(output[14]^output[15])
					|0x4000;
	uint64_t low = (output[16]^output[17])<<58|(output[18]^output[19])<<48
				|(output[20]^output[21])<<40|(output[22]^output[23])<<32
				|(output[24]^output[25])<<24|(output[26]^output[27])<<16
				|(output[28]^output[29])<<8|(output[30]^output[31]);
	return UUID(high,low);
}

UUID UUID::newNamespaceUUID(const char* in,size_t s){
	array<uint8_t,16> output;
	MD5((const unsigned char*)in,s,output.data());
	uint64_t high = output[0]<<56|output[1]<<48
					|output[2]<<40|output[3]<<32
					|output[4]<<24|output[5]<<16
					|(output[6]&0xf)<<8|output[7]
				    |0x3000;
	uint64_t low = output[8]<<56|output[9]<<48
					|output[10]<<40|output[11]<<32
					|output[12]<<24|output[13]<<16
					|output[14]<<8|output[15];
	return UUID(high,low);
}

UUID UUID::newSHANamespaceUUID(const char* in,size_t s){
	array<uint8_t,20> output;
	SHA1((const unsigned char*)in,s,output.data());
	uint64_t high = output[0]<<56|output[1]<<48
					|output[2]<<40|output[3]<<32
					|output[4]<<24|output[5]<<16
					|(output[6]&0xf)<<8|output[7]
				    |0x5000;
	uint64_t low = output[8]<<56|output[9]<<48
					|output[10]<<40|output[11]<<32
					|output[12]<<24|output[13]<<16
					|output[14]<<8|output[15];
	return UUID(high,low);
}


ostream& operator<<(ostream& o,const UUID& id){
	uint64_t high = id.getHigh();
	uint64_t low = id.getLow();
	uint64_t h1 = high/0xFFFFFFF;
	uint64_t h2 = (high/0xFFFF)&0xFFFF;
	uint64_t h3 = high&0xFFFF;
	uint64_t l1 = low/0xFFFFFFFFFFFF;
	uint64_t l2 = low&0xFFFFFFFFFFFF;
	o << setw(8)<<setfill('0')<<hex<<h1<<setw(0)<<"-"
		<<setw(4)<<setfill('0')<<hex<<h2<<setw(0)<<"-"
		<<setw(4)<<setfill('0')<<hex<<h3<<setw(0)<<"-"
		<<setw(4)<<setfill('0')<<hex<<l1<<setw(0)<<"-"
		<<setw(12)<<setfill('0')<<hex<<l2<<setw(0)<<"-";
	return o;
}
istream& operator>>(istream& i,UUID& id){
	string s;
	i>>s;
	id = s;
	return i;
}

DataOutputStream& operator<<(DataOutputStream& o,const UUID& id){
	return o << id.getHigh() << id.getLow();
}

DataInputStream& operator>>(DataInputStream& i,UUID& id){
	uint64_t high;
	uint64_t low;
	i >> high >> low;
	id = UUID(high,low);
	return i;
}
