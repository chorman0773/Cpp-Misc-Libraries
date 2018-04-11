#include "IOWrapper.hpp"
#include <array>
#include <vector>
#include <cstdio>

using std::array;
using std::vector;

InputStream::~InputStream(){
	this->close();
}

template<size_t size> int InputStream::read(array<char,size>& a){
	return read(a,0,size);
}

template<size_t size> int InputStream::read(array<char,size>& a,int offset,int len){
	int i=-1,byte =read();
	if(offset+len>size)
		throw "Offset illegal";
	if(byte<0)
		return -1;
	for(i=0;i<len&&byte>=0;i++){
		a[i+offset] = byte;
		byte = read();
	}
}

vector<uint8_t> InputStream::readAll(){
	vector<uint8_t> ret;
	int i;
	while((i=read())>=0)
		ret.push_back(i);
	return ret;
}

void InputStream::writeTo(OutputStream& o){
	int i;
	while((i=read())>=0)
		o.write(i);
}

FilterInputStream::FilterInputStream(InputStream& base){
	this->base = base;
}

void FilterInputStream::close(){
	this->base.close();
}



int FilterInputStream::read(){
	return base.read();
}

vector<uint8_t> FilterInputStream::readAll(){
	return base.readAll();
}

FileInputStream::FileInputStream(char* c){
	buffer = fopen(c,"r");
	if(buffer==NULL)
		throw "File not found or could not be opened";
}

FileInputStream::FileInputStream(string s){
	buffer = fopen(s.c_str(),"r");
	if(buffer==NULL)
		throw "File not found or could not be opened";
}

int FileInputStream::read(){
	uint8_t i;
	if(fread(&i,1,1,this->buffer)<0)
		return -1;
	return i;
}

void FileInputStream::close(){
	fclose(this->buffer);
}

template<size_t size> int FileInputStream::read(array<char,size>& a){
	return fread(a.data(),1,size,this->buffer);
}
template<size_t size> int FileInputStream::read(array<char,size>& a,int offset,int len){
	return fread(a.data()+offset,1,len,this->buffer);
}




void OutputStream::close(){
	this->flush();
}


FilterOutputStream::FilterOutputStream(OutputStream& base):base(base){}

void FilterOutputStream::write(uint8_t b){
	this->base.write(b);
}
void FilterOutputStream::flush(){
	base.flush();
}
void FilterOutputStream::close(){
	base.close();
}

FileOutputStream::FileOutputStream(char* name){
	this->buffer = fopen(name,"w");
	if(buffer==NULL)
		throw "File could not be opened";
}
FileOutputStream::FileOutputStream(string name){
	this->buffer = fopen(name.c_str(),"w");
	if(buffer==NULL)
		throw "File could not be opened";
}

void FileOutputStream::close(){
	fclose(this->buffer);
}
void FileOutputStream::flush(){
	fflush(this->buffer);
}

void FileOutputStream::write(uint8_t byte){
	fwrite(&byte,1,1,this->buffer);
}

template<size_t size> void FileOutputStream::write(array<uint8_t,size> a,int offset=0,int len=size){
	fwrite(a.data()+offset,1,len,this->buffer);
}


void DataOutputStream::writeUnsignedShort(int val){
	this->write(val>>8);
	this->write(val);
}

void NativeDataOutputStream::writeUnsignedShort(int val){
	this->write(val);
	this->write(val>>8);
}

void DataOutputStream::writeInt(int val){
	this->write(val>>24);
	this->write(val>>16);
	this->write(val>>8);
	this->write(val);
}

void NativeDataOutputStream::writeInt(int val){
	this->write(val);
	this->write(val>>8);
	this->write(val>>16);
	this->write(val>>24);
}

void DataOutputStream::writeLong(uint64_t val){
	this->write(val>>56);
	this->write(val>>48);
	this->write(val>>40);
	this->write(val>>32);
	this->write(val>>24);
	this->write(val>>16);
	this->write(val>>8);
	this->write(val);
}
void NativeDataOutputStream::writeLong(uint64_t val){
	this->write(val);
	this->write(val>>8);
	this->write(val>>16);
	this->write(val>>24);
	this->write(val>>32);
	this->write(val>>40);
	this->write(val>>48);
	this->write(val>>56);
}






short DataInputStream::readShort(){
	return this->read()<<8
			|this->read();
}

int DataInputStream::readUnsignedShort(){
	return this->read()<<8
			|this->read();
}

int DataInputStream::readInt(){
	return this->read()<<24
			|this->read()<<16
			|this->read()<<8
			|this->read();
}
int64_t DataInputStream::readLong(){
	 return ((int64_t) this->read())<<56
				|((int64_t) this->read())<<48
				|((int64_t) this->read())<<40
				|((int64_t) this->read())<<32
				|this->read()<<24
				|this->read()<<16
				|this->read()<<8
				|this->read();
}

short NativeDataInputStream::readShort(){
	return this->read()
			|this->read()<<8;
}
int NativeDataInputStream::readUnsignedShort(){
	return this->read()
			|this->read()<<8;
}

int NativeDataInputStream::readInt(){
	return this->read()
			|this->read()<<8
			|this->read()<<16
			|this->read()<<24;
}

int64_t NativeDataInputStream::readLong(){
	return this->read()
		|this->read()<<8
		|this->read()<<16
		|this->read()<<24
		|this->read()<<32
		|this->read()<<40
		|this->read()<<48
		|this->read()<<56;
}


DataOutputStream& operator <<(DataOutputStream& o,char c){
	o.write(c);
	return o;
}
NativeDataOutputStream& operator <<(NativeDataOutputStream& o,char c){
	o.write(c);
	return o;
}
DataOutputStream& operator <<(DataOutputStream& o ,int16_t v){
	o.writeUnsignedShort(v);
	return o;
}
NativeDataOutputStream& operator <<(NativeDataOutputStream& o,int16_t v){
	o.writeUnsignedShort(v);
	return o;
}
DataOutputStream& operator <<(DataOutputStream& o,int v){
	o.writeInt(v);
	return o;
}
NativeDataOutputStream& operator <<(NativeDataOutputStream& o,int v){
	o.writeInt(v);
	return o;
}
DataOutputStream& operator <<(DataOutputStream& o,int64_t v){
	o.writeInt(v);
	return o;
}
NativeDataOutputStream& operator <<(NativeDataOutputStream& o,int64_t v){
	o.writeInt(v);
	return o;
}
DataOutputStream& operator <<(DataOutputStream& o,const string& s){
	o.writeString(s);
	return o;
}
NativeDataOutputStream& operator <<(NativeDataOutputStream& o,const string& s){
	o.writeString(s);
	return o;
}

DataInputStream& operator>>(DataInputStream& i,char& c){
	c = i.read();
	return i;
}
NativeDataInputStream& operator>>(NativeDataInputStream& i,char& c){
	c = i.read();
	return i;
}
DataInputStream& operator>>(DataInputStream& i,int16_t& v){
	v = i.readShort();
	return i;
}
NativeDataInputStream& operator>>(NativeDataInputStream& i,int16_t& v){
	v = i.readShort();
	return i;
}
DataInputStream& operator>>(DataInputStream& i,int& v){
	v = i.readInt();
	return i;
}
NativeDataInputStream& operator>>(NativeDataInputStream& i,int& v){
	v = i.readInt();
	return i;
}
DataInputStream& operator>>(DataInputStream& i,int64_t& v){
	v = i.readLong();
	return i;
}
NativeDataInputStream& operator>>(NativeDataInputStream& i,int64_t& v){
	v = i.readLong();
	return i;
}
DataInputStream& operator>>(DataInputStream& i,string& s){
	s = i.readString();
	return i;
}
NativeDataInputStream& operator>>(NativeDataInputStream& i,string& s){
	s = i.readString();
	return i;
}

OutputStream& operator<(OutputStream& o,InputStream& i){
	i.writeTo(o);
	return o;
}









