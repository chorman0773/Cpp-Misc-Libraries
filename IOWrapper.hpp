#include <stdio.h>
#include <vector>
#include <array>
#include <string>
#include <stdint.h>

enum class ByteOrder{
    BIG_ENDIAN,
    LITTLE_ENDIAN
};



using std::vector;
using std::array;
using std::string;

class InputStream;
class OutputStream;


class AutoCloseable{
public:
    virtual void close() = 0;
    virtual ~AutoCloseable();
};

class InputStream : public AutoCloseable{
public:
    virtual ~InputStream() = default;
    virtual vector<uint8_t> readAll() = 0;
    virtual int read() = 0;
    template<size_t size> virtual int read(array<char,size>&);
    template<size_t size> virtual int read(array<char,size>&,int,int);
    virtual void writeTo(OutputStream&);
};

class FilterInputStream : public InputStream{
private:
    InputStream& base;
public:
    FilterInputStream(InputStream&);
    virtual ~FilterInputStream() = 0;
    virtual vector<uint8_t> readAll();
    virtual int read();
    virtual void close();
};

template<ByteOrder order> class OrderedDataInputStream : public FilterInputStream{
public:
	OrderedDataInputStream(InputStream&);
    int readUnsignedByte();
    int readUnsignedShort();
    int readInt();
    short readShort();
    int64_t readLong();
    template<size_t size> void readFully(array<char,size>&);
    string readString();
    float readFloat();
    double readDouble();

};
class FileInputStream : public InputStream{
private:
    FILE* buffer;
public:
    FileInputStream(char*);
    FileInputStream(string);
    ~FileInputStream();
    int read();
    void close();
    template<size_t size> virtual int read(array<char,size>&);
    template<size_t size> virtual int read(array<char,size>&,int,int);
};

class OutputStream : public AutoCloseable{
public:
	virtual ~OutputStream();
    virtual void write(uint8_t)=0;
    template<size_t size> virtual void write(array<uint8_t,size>,int=0,int=size);
    virtual void flush()=0;
    virtual void close();
};

class FileOutputStream : public OutputStream{
private:
    FILE* buffer;
public:
    FileOutputStream(char*);
    FileOutputStream(string);
    ~FileOutputStream();
    virtual void write(uint8_t);
    virtual void flush();
    virtual void close();
    template<size_t size> virtual void write(array<uint8_t,size>,int=0,int=size);
};


class FilterOutputStream : public OutputStream{
private:
    OutputStream& base;
public:
    FilterOutputStream(OutputStream&);
    virtual ~FilterOutputStream()=0;
    virtual void write(uint8_t);
    virtual void flush();
    virtual void close();
};

template<ByteOrder order> class OrderedDataOutputStream :public FilterOutputStream{
public:
	OrderedDataOutputStream(OutputStream&);
    void writeUnsignedByte(int);
    void writeUnsignedShort(int);
    void writeInt(int);
    void writeLong(uint64_t);
    void writeFloat(float);
    void writeDouble(double);
    void writeString(string);
};

template<ByteOrder o> OrderedDataOutputStream<o>::OrderedDataOutputStream(OutputStream& base):FilterOutputStream(base){}
template<ByteOrder o> void OrderedDataOutputStream<o>::writeUnsignedByte(int byte){
	this->write(byte);
}

template<ByteOrder o> void OrderedDataOutputStream<o>::writeFloat(float f){
	this->writeInt(reinterpret_cast<int&>(f));
}
template<ByteOrder o> void OrderedDataOutputStream<o>::writeDouble(double d){
	this->writeLong(reinterpret_cast<uint64_t&>(d));
}
template<ByteOrder o> void OrderedDataOutputStream<o>::writeString(string s){
	this->writeUnsignedShort((int)s.length());
	for(char c:s)
		this->write(c);
}

template<ByteOrder o> OrderedDataInputStream<o>::OrderedDataInputStream(InputStream& base):FilterInputStream(base){}

template<ByteOrder o> int OrderedDataInputStream<o>::readUnsignedByte(){
	return this->read();
}
template<ByteOrder o> float OrderedDataInputStream<o>::readFloat(){
	int i = readInt();
	return reinterpret_cast<float&>(i);
}
template<ByteOrder o> double OrderedDataInputStream<o>::readDouble(){
	int64_t l = readLong();
	return reinterpret_cast<double&>(l);
}

template<ByteOrder o> string OrderedDataInputStream<o>::readString(){
	int length = readUnsignedShort();
	char c;
	string s;
	for(int i = 0;i<length;i++){
		c = this->read();
		s +=c;
	}
	return s;
}



using DataOutputStream = OrderedDataOutputStream<ByteOrder::BIG_ENDIAN>;
using DataInputStream = OrderedDataInputStream<ByteOrder::BIG_ENDIAN>;
using NativeDataOutputStream = OrderedDataOutputStream<ByteOrder::LITTLE_ENDIAN>;
using NativeDataInputStream = OrderedDataInputStream<ByteOrder::LITTLE_ENDIAN>;


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


template<size_t size> void OutputStream::write(array<uint8_t,size> a,int offset=0,int len=size){
	if(offset+len>size)
		throw "Offset illegal";
	for(int i = 0;i<len;i++)
		write(a[i+offset]);
}

template<size_t size> void DataInputStream::readFully(array<char,size>& a){
	if(this->InputStream::read(a)<size)
		throw "EOF reached before end of read";
}
template<size_t size> void NativeDataInputStream::readFully(array<char,size>& a){
	if(this->InputStream::read(a)<size)
		throw "EOF reached before end of read";
}

/*
 * Write to stream operators.
 * Similar in syntax to STL ostream operators
 */
DataOutputStream& operator <<(DataOutputStream&,char);
NativeDataOutputStream& operator <<(NativeDataOutputStream&,char);
DataOutputStream& operator <<(DataOutputStream&,int16_t);
NativeDataOutputStream& operator <<(NativeDataOutputStream&,int16_t);
DataOutputStream& operator <<(DataOutputStream&,int);
NativeDataOutputStream& operator <<(NativeDataOutputStream&,int);
DataOutputStream& operator <<(DataOutputStream&,int64_t);
NativeDataOutputStream& operator <<(NativeDataOutputStream&,int64_t);
DataOutputStream& operator <<(DataOutputStream&,const string&);
NativeDataOutputStream& operator <<(NativeDataOutputStream&,const string&);

/*
 * Read from stream operators.
 * Similar in syntax to STL istream operators
 */
DataInputStream& operator>>(DataInputStream&,char&);
NativeDataInputStream& operator>>(NativeDataInputStream&,char&);
DataInputStream& operator>>(DataInputStream&,int16_t&);
NativeDataInputStream& operator>>(NativeDataInputStream&,int16_t&);
DataInputStream& operator>>(DataInputStream&,int&);
NativeDataInputStream& operator>>(NativeDataInputStream&,int&);
DataInputStream& operator>>(DataInputStream&,int64_t&);
NativeDataInputStream& operator>>(NativeDataInputStream&,int64_t&);
DataInputStream& operator>>(DataInputStream&,string&);
NativeDataInputStream& operator>>(NativeDataInputStream&,string&);

/*
 * Pipe to operator.
 */
OutputStream& operator <(OutputStream&,InputStream&);



