/*
 * Nbt.cpp
 *
 *  Created on: Apr 6, 2018
 *      Author: Connor Horman
 */

#include "Nbt.hpp"
#include <array>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <cstdlib>
#include "IOWrapper.hpp"
#include <functional>

using namespace nbt;

using std::function;
using std::shared_ptr;
const static function<NBTBase*(DataInputStream&)> NBTBase::constructors[]={
			[](DataInputStream& strm){
				return new NBTTagEnd(strm);
			},[](DataInputStream& strm){
				return new(NBTTagByte(strm));
			},[](DataInputStream& strm){
				return new(NBTTagShort(strm));
			},[](DataInputStream& strm){
				return new(NBTTagInt(strm));
			},[](DataInputStream& strm){
				return new(NBTTagLong(strm));
			},[](DataInputStream& strm){
				return new(NBTTagFloat(strm));
			},[](DataInputStream& strm){
				return new(NBTTagDouble(strm));
			},[](DataInputStream& strm){
				return new(NBTTagString(strm));
			},[](DataInputStream& strm){
				return new(NBTTagByteArray(strm));
			},[](DataInputStream& strm){
				return new(NBTTagList(strm));
			},[](DataInputStream& strm){
				return new(NBTTagCompound(strm));
			},[](DataInputStream& strm){
				return new(NBTTagIntArray(strm));
			},[](DataInputStream& strm){
				return new(NBTTagLongArray(strm));
			}
};

NBTBase* NBTBase::operator new(size_t size){
	NBTBase* val = ::operator new(size);
	val->wasConstructed = true;
	return val;
}

void operator delete(NBTBase* base){
	if(base->wasConstructed)
		::operator delete(base);

}

///
//Implementations for hidden template wrappers


////
//Implementation for the Hidden Wrapper type NBTArray
template<typename T,int tag>
const int _hidden::NBTArray<T,tag>::tag_type = tag;

int _hidden::NBTArray::getTagType()const{
	return tag_type;
}

_hidden::NBTArray::NBTArray():data(nullptr),size(0),delOnDestruct(false){}
_hidden::NBTArray::NBTArray(int size):data(new type[size]),size(size),delOnDestruct(true){}
_hidden::NBTArray::NBTArray(DataInputStream& stream):data(nullptr),size(0),delOnDestruct(true),NBTBase(stream){}
_hidden::NBTArray::NBTArray(const NBTArray& src){
	this->delOnDestruct = true;
	this->size = src.size;
	data = new type[size];
	memcpy(data,src.data,size*sizeof(type));
}
_hidden::NBTArray::NBTArray(NBTArray&& src){
	this->delOnDestruct = src.delOnDestruct;
	this->size = src.size;
	this->data = src.data;
	src.delOnDestruct = false;
}
_hidden::NBTArray::~NBTArray(){
	if(delOnDestruct)
		delete data;
}

int _hidden::NBTArray::getSize()const{
	return size;
}

void _hidden::NBTArray::write(DataOutputStream& strm)const{
	strm << size;
	for(const_reference v:*this)
		strm << v;
}

int _hidden::NBTArray::hashCode()const{
	int hashcode = NBTBase::hashCode();
	for(type v:*this){
		hashcode *=hashPrime;
		hashcode += hashcode(v);
	}
	return hashcode;
}

bool _hidden::NBTArray::operator ==(const NBTBase& o)const{
	if(o.getTagType()!=tag_type)
		return false;
	else{
		catype oarray = dynamic_cast<catype>(o);
		if(oarray.size!=size)
			return false;
		return memcmp(data,oarray.data,size);
	}
}

void _hidden::NBTArray::read(DataInputStream& strm){
	strm>>size;
	data = new type[size];
	for(reference r:*this)
		strm>>r;
}

///
//Implementation for Hidden Wrapper Type NBTTagPrimitive
template<typename T,int type>
int _hidden::NBTTagPrimitive<T,type>::tag_type = type;

_hidden::NBTTagPrimitive::NBTTagPrimitive(type val):val(val){}


int _hidden::NBTTagPrimitive::getInteger()const{
	return (int)val;
}
float _hidden::NBTTagPrimitive::getFloat()const{
	return (float)val;
}
double _hidden::NBTTagPrimitive::getDouble()const{
	return (double)val;
}

int64_t _hidden::NBTTagPrimitive::getLong()const{
	return (int64_t)val;
}

int _hidden::NBTTagPrimitive::getTagType()const{
	return tag_type;
}

int _hidden::NBTTagPrimitive::hashCode()const{
	return NBTBase::hashCode()*hashPrime+hashcode<type>(val);
}
bool _hidden::NBTTagPrimitive::operator==(const NBTBase& o)const{
	return tag_type==o.getTagType()&&val==reinterpret_cast<cprim>(o).val;
}

void _hidden::NBTTagPrimitive::read(DataInputStream& strm){
	strm >> val;
}
void _hidden::NBTTagPrimitive::write(DataOutputStream& strm)const{
	strm << val;
}



int NBTTagEnd::getTagType()const{
	return TAG_END;
}

bool NBTBase::operator==(const NBTBase& o)const{
	return getTagType()==o.getTagType();
}

int NBTBase::hashCode()const{
	return getTagType();
}

bool NBTBase::operator==(const NBTBase& o)const{
	return getTagType()==o.getTagType();
}

NBTBase::NBTBase(DataInputStream& in){
	if(wasConstructed!=true)
		wasConstructed = false;
	this->read(in);

}

NBTBase::NBTBase(){
	if(wasConstructed!=true)
		wasConstructed = false;
}

NBTBase::~NBTBase(){}


NBTTagString::NBTTagString(const string& str):s(str){}
NBTTagString::NBTTagString(const char* str):s(str){}
NBTTagString::NBTTagString(DataInputStream& strm):NBTBase(strm){}

const string& NBTTagString::get()const{
	return s;
}

int NBTTagString::hashCode()const{
	return hashcode(s)*hashPrime+getTagType();
}

bool NBTTagString::operator==(const NBTBase& o)const{
	if(getTagType()!=o.getTagType())
		return false;
	return s==dynamic_cast<const NBTTagString&>(o).s;
}

void NBTTagString::read(DataInputStream& strm){
	strm >> s;
}
void NBTTagString::write(DataOutputStream& strm)const{
	strm << s;
}

NBTTagList::NBTTagList():tagType(TAG_END){}
NBTTagList::NBTTagList(DataInputStream& strm):tagType(TAG_END),NBTBase(strm){}

void NBTTagList::add(NBTBase& tag){
	if(tagType==TAG_END||data.size()==0||tag.getTagType()==tagType){
		tagType = tag.getTagType();
		data.push_back(tag);
	}
}

void NBTTagList::clear(){
	data.clear();
}

int NBTTagList::getListTagType()const{
	return tagType;
}
int NBTTagList::getSize()const{
	return data.size();
}

int NBTTagList::getTagType()const{
	return TAG_LIST;
}

int NBTTagList::hashCode()const{
	int hash = getTagType();
	hash*=hashPrime;
	hash+=tagType;
	for(const NBTBase& b:*this){
		hash*=hashPrime;
		hash+=b.hashCode();
	}
	return hash;
}

NBTTagList::iterator NBTTagList::begin(){
	return data.begin();
}
NBTTagList::const_iterator NBTTagList::begin()const{
	return data.begin();
}
NBTTagList::iterator NBTTagList::end(){
	return data.end();
}
NBTTagList::const_iterator NBTTagList::end()const{
	return data.end();
}

const NBTBase& NBTTagList::operator[](int idx)const{
	return data[idx];
}

bool NBTTagList::operator ==(const NBTBase& o)const{
	if(getTagType()!=o.getTagType())
		return false;
	else{
		const NBTTagList& olist = dynamic_cast<const NBTTagList&>(o);
		if(getSize()==0&&olist.getSize()==0)
			return true;
		else if(tagType!=olist.tagType)
			return false;
		else if(getSize()!=olist.getSize())
			return false;
		else for(int i = 0;i<getSize();i++)
			if(*this[i]!=o[i])
				return false;
	}
	return true;
}

void NBTTagList::write(DataOutputStream& strm)const{
	strm<<tagType<<data.size();
	for(const NBTBase& v:*this)
		v.write(strm);
}

void NBTTagList::read(DataInputStream& strm){
	int size(0);
	strm>>tagType>>size;
	for(int i = 0;i<size;i++)
		add(*constructors[tagType](strm));

}


NBTTagCompound::NBTTagCompound(){}
NBTTagCompound::NBTTagCompound(DataInputStream& strm):NBTBase(strm){}

NBTTagCompound::~NBTTagCompound(){
	for(const pair<string,NBTBase*>& p:m)
		delete p.second;//Specialized New/Delete Semantics.
}

int NBTTagCompound::hashCode()const{
	return hashcode(m)*hashPrime+getTagType();
}

void NBTTagCompound::write(DataOutputStream& strm)const{
	for(const pair<string,NBTBase*>& p:m){
		const NBTBase& b = *p.second;
		const string& name = p.first;
		strm.writeUnsignedByte(b.getTagType());
		strm << name;
		b.write(strm);
	}
	strm.writeUnsignedByte(TAG_END);
}

void NBTTagCompound::read(DataInputStream& strm){
	int tagType;
	do{
		strm >> tagType;
		if(tagType==TAG_END)
			return;
		string name;
		strm >> name;
		m[name] = constructors[tagType](strm);
	}while(tagType!=TAG_END);
}

NBTBase& NBTTagCompound::operator[](const string& str){
	return *m[str];
}
NBTBase& NBTTagCompound::operator[](const char* str){
	return *m[string(str)];
}
const NBTBase& NBTTagCompound::operator[](const string& str)const{
	return *m[str];
}
const NBTBase& NBTTagCompound::operator[](const char* str)const{
	return *m[string(str)];
}

const NBTBase& NBTTagCompound::getTag(const string& str)const{
	return *m[str];
}

bool NBTTagCompound::hasTag(const string& str)const{
	return m.count(str)==1;
}
bool NBTTagCompound::hasTag(const char* str)const{
	return m.count(string(str))==1;
}

bool NBTTagCompound::hasTag(const string& str,int type)const{
	if(!hasTag(str))
		return false;
	else if(type==TAG_ANY_NUMERIC){
		NBTBase* ptr = &getTag(str);
		if(dynamic_cast<NBTPrimitive*>(ptr)!=nullptr)
			return true;
		else
			return false;
	}
	else
		return getTag(str).getTagType()==type;
}
bool NBTTagCompound::hasTag(const char* str,int type)const{
	return hasTag(string(str),type);
}
UUID NBTTagCompound::getUUID(const string& str)const{
	string least = str+"Least";
	string most = str+"Most";
	uint64_t high = getLong(most);
	uint64_t low = getLong(least);
	return UUID(high,low);
}

UUID NBTTagCompound::getUUID(const char* str)const{
	return getUUID(string(str));
}

void NBTTagCompound::setTag(const string& str,NBTBase* b){
	m[str] = b;
}

void NBTTagCompound::setBoolean(const char* str,bool val){
	setTag(str,new NBTTagByte(val?1:0));
}
void NBTTagCompound::setBoolean(const string& str,bool val){
	setTag(str,new NBTTagByte(val?1:0));
}

void NBTTagCompound::setByte(const string& str,int8_t val){
	setTag(str,new NBTTagByte(val));
}
void NBTTagCompound::setShort(const string& str,int16_t val){
	setTag(str,new NBTTagShort(val));
}
void NBTTagCompound::setInteger(const string& str,int val){
	setTag(str,new NBTTagInt(val));
}
void NBTTagCompound::setFloat(const string& str,float val){
	setTag(str,new NBTTagFloat(val));
}
void NBTTagCompound::setDouble(const string& str,double val){
	setTag(str,new NBTTagDouble(val));
}

void NBTTagCompound::setByte(const char* str,int8_t val){
	setTag(str,new NBTTagByte(val));
}
void NBTTagCompound::setShort(const char* str,int16_t val){
	setTag(str,new NBTTagShort(val));
}
void NBTTagCompound::setInteger(const char* str,int val){
	setTag(str,new NBTTagInt(val));
}
void NBTTagCompound::setFloat(const char* str,float val){
	setTag(str,new NBTTagFloat(val));
}
void NBTTagCompound::setDouble(const char* str,double val){
	setTag(str,new NBTTagDouble(val));
}

bool NBTTagCompound::getBoolean(const string& str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<const NBTPrimitive&>(getTag(str)).getBool();
}

int8_t NBTTagCompound::getByte(const string& str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<const NBTPrimitive&>(getTag(str)).getByte();
}

int16_t NBTTagCompound::getShort(const string& str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<NBTPrimitive&>(getTag(str)).getShort();
}
int NBTTagCompound::getInteger(const string& str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<NBTPrimitive&>(getTag(str)).getInteger();
}

int64_t NBTTagCompound::getLong(const string& str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<NBTPrimitive&>(getTag(str)).getLong();
}
float NBTTagCompound::getFloat(const string& str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<NBTPrimitive&>(getTag(str)).getFloat();
}

double NBTTagCompound::getDouble(const string& str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<NBTPrimitive&>(getTag(str)).getBool();
}

NBTTagCompound& NBTTagCompound::getCompoundTag(const string& str){
	if(!hasTag(str,TAG_COMPOUND))
		setTag(str,new NBTTagCompound());
	return static_cast<NBTTagCompound&>(getTag(str));
}
NBTTagList& NBTTagCompound::getListTag(const string& str){
	if(!hasTag(str,TAG_LIST))
		setTag(str,new NBTTagList());
	return static_cast<NBTTagList&>(getTag(str));
}

bool NBTTagCompound::getBoolean(const char* str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<const NBTPrimitive&>(getTag(str)).getBool();
}

int8_t NBTTagCompound::getByte(const char* str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<const NBTPrimitive&>(getTag(str)).getByte();
}

int16_t NBTTagCompound::getShort(const char* str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<NBTPrimitive&>(getTag(str)).getShort();
}
int NBTTagCompound::getInteger(const char* str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<NBTPrimitive&>(getTag(str)).getInteger();
}

int64_t NBTTagCompound::getLong(const char* str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<NBTPrimitive&>(getTag(str)).getLong();
}
float NBTTagCompound::getFloat(const char* str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<NBTPrimitive&>(getTag(str)).getFloat();
}

double NBTTagCompound::getDouble(const char* str)const{
	if(!hasTag(str,TAG_ANY_NUMERIC))
		return false;
	return static_cast<NBTPrimitive&>(getTag(str)).getBool();
}

NBTTagCompound& NBTTagCompound::getCompoundTag(const char* str){
	if(!hasTag(str,TAG_COMPOUND))
		setTag(str,new NBTTagCompound());
	return static_cast<NBTTagCompound&>(getTag(str));
}
NBTTagList& NBTTagCompound::getListTag(const char* str){
	if(!hasTag(str,TAG_LIST))
		setTag(str,new NBTTagList());
	return static_cast<NBTTagList&>(getTag(str));
}


string NBTTagCompound::getString(const string& str)const{
	if(!hasTag(str,TAG_STRING))
		return "";
	else
		return static_cast<NBTTagCompound&>(getTag(str));
}
void NBTTagCompound::setString(const string& str,const string& val){
	setTag(str,new NBTTagString(val));
}

string NBTTagCompound::getString(const char* str)const{
	if(!hasTag(str,TAG_STRING))
		return "";
	else
		return static_cast<NBTTagCompound&>(getTag(str));
}
void NBTTagCompound::setString(const char* str,const string& val){
	setTag(str,new NBTTagString(val));
}

NBTTagString::NBTTagString(const string& str):s(str){}
NBTTagString::NBTTagString(const char* str):s(str){}
NBTTagString::NBTTagString(DataInputStream& strm):NBTBase(strm){}

const string& NBTTagString::get()const{
	return s;
}
int NBTTagString::getTagType()const{
	return TAG_STRING;
}

int NBTTagString::hashCode()const{
	return hashcode(s)*hashPrime+getTagType();
}

bool NBTTagString::operator==(const NBTBase& o)const{
	if(getTagType()!=o.getTagType())
		return false;
	else
		return static_cast<NBTTagString&>(o).s==s;
}

void NBTTagString::write(DataOutputStream& strm)const{
	strm << s;
}
void NBTTagString::read(DataInputStream& strm){
	strm >> s;
}

DataOutputStream& operator <<(DataOutputStream& strm,const NBTTagCompound& comp){
	comp.write(strm);
	return strm;
}

DataInputStream& operator>>(DataInputStream& strm,NBTTagCompound& comp){
	comp.read(strm);
	return strm;
}



