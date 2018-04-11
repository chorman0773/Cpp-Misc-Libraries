/*
 * Nbt.hpp
 *
 *  Created on: Apr 6, 2018
 *      Author: Connor Horman
 */

#ifndef __NBT_HPP__18_04_06_11_32_48
#define __NBT_HPP__18_04_06_11_32_48

#include "Hash.hpp"
#include "IOWrapper.hpp"
#include "UUID.hpp"
#include <cstdint>
#include <array>
#include <vector>
#include <map>
#include <stdexcept>
#include <functional>
#include <memory>


#define TAG_COUNT 13

namespace nbt{
	const constexpr int TAG_END = 0;
	const constexpr int TAG_BYTE = 1;
	const constexpr int TAG_SHORT = 2;
	const constexpr int TAG_INT = 3;
	const constexpr int TAG_LONG = 4;
	const constexpr int TAG_FLOAT = 5;
	const constexpr int TAG_DOUBLE = 6;
	const constexpr int TAG_STRING = 7;
	const constexpr int TAG_BYTEARRAY = 8;
	const constexpr int TAG_LIST = 9;
	const constexpr int TAG_COMPOUND = 10;
	const constexpr int TAG_INTARRAY = 11;
	const constexpr int TAG_LONGARRAY = 12;
	const constexpr int TAG_ANY_NUMERIC = 99;
	const constexpr int VERSION = 19133;
	class NBTBase : public Hashable{
		using std::function;
		using std::shared_ptr;
	protected:
		bool wasConstructed;
		static function<NBTBase*(DataInputStream&)> constructors[TAG_COUNT];
	public:
		NBTBase();
		NBTBase(DataInputStream&);
		virtual ~NBTBase();
		virtual int getTagType()const=0;
		virtual void write(DataOutputStream&)const=0;
		virtual void read(DataInputStream&)=0;
		virtual int hashCode()const;
		virtual bool operator==(const NBTBase&)const;
		virtual NBTBase* operator new(size_t);
		friend void operator delete(NBTBase*);
	};

	class NBTPrimitive : public NBTBase{
	public:
		NBTPrimitive();
		NBTPrimitive(DataInputStream&);
		virtual ~NBTPrimitive();
		virtual int getInteger()const=0;
		virtual int8_t getByte()const;
		virtual int16_t getShort()const;
		virtual int64_t getLong()const;
		virtual float getFloat()const;
		virtual double getDouble()const;
		virtual bool getBool()const;
		virtual int hashCode()const;
		virtual bool operator==(const NBTBase&)const;
		operator int()const;
		operator int8_t()const;
		operator int16_t()const;
		operator int32_t()const;
		operator float()const;
		operator double()const;
		operator bool()const;
	};

	namespace _hidden{
		using std::array;
		template<typename T,int tag> class NBTArray final: public NBTBase{
		public:
			typedef T type;
			typedef T* pointer;
			typedef T& reference;
			typedef const T& const_reference;
			typedef T* iterator;
			typedef const T* const_iterator;
			typedef NBTArray<T,tag>& atype;
			typedef const NBTArray<T,tag>& catype;
			static const int tag_type;
			using std::out_of_range;
		private:
			pointer data;
			int size;
			bool delOnDestruct;
		public:
			NBTArray();
			NBTArray(int size);
			template<size_t size> inline NBTArray(array<T,size>& a){
				data = a.data();
				delOnDestruct = false;
				this->size = size;
			}
			NBTArray(DataInputStream&);
			NBTArray(const NBTArray&);
			NBTArray(NBTArray&&);
			~NBTArray();
			virtual int getTagType()const;
			virtual void write(DataOutputStream&)const;
			virtual void read(DataInputStream&);
			virtual int hashCode()const;
			virtual bool operator==(const NBTBase&)const;
			reference operator[](int i){
				if(i>size||i<0)
					throw out_of_range(i);
				return data[i];
			}
			const_reference operator[](int i)const{
				if(i>size||i<0)
					throw out_of_range(i);
				return data[i];
			}
			int getSize()const;
			iterator begin(){
				return data;
			}
			const_iterator begin()const{
				return data;
			}
			iterator end(){
				return data+size;
			}
			const_iterator end()const{
				return data+size;
			}
		};
		template<typename T,int type> class NBTTagPrimitive : public NBTPrimitive{
		private:
			type val;
			static const int tag_type;
		public:
			typedef T type;
			typedef NBTTagPrimitive<T,type>& tprim;
			typedef const NBTTagPrimitive<T,type>& cprim;
			NBTTagPrimitive(type val);
			NBTTagPrimitive(DataInputStream&);
			int getInteger()const;
			int64_t getLong()const;
			float getFloat()const;
			double getDouble()const;
			int hashCode()const;
			int getTagType()const;
			void write(DataOutputStream&)const;
			void read(DataInputStream&);
			bool operator==(const NBTBase&)const;
		};
	};
	using NBTTagByte = _hidden::NBTTagPrimitive<int8_t,TAG_BYTE>;
	using NBTTagShort = _hidden::NBTTagPrimitive<int16_t,TAG_SHORT>;
	using NBTTagInt = _hidden::NBTTagPrimitive<int,TAG_INT>;
	using NBTTagLong = _hidden::NBTTagPrimitive<long,TAG_LONG>;
	using NBTTagFloat = _hidden::NBTTagPrimitive<float,TAG_FLOAT>;
	using NBTTagDouble = _hidden::NBTTagPrimitive<double,TAG_DOUBLE>;
	using NBTTagByteArray = _hidden::NBTArray<int8_t,TAG_BYTEARRAY>;
	using NBTTagIntArray = _hidden::NBTArray<int,TAG_INTARRAY>;
	using NBTTagLongArray = _hidden::NBTArray<int64_t,TAG_LONGARRAY>;

	class NBTTagEnd final: public NBTBase{
	public:
		NBTTagEnd();
		NBTTagEnd(DataInputStream&);
		int getTagType()const;
		void write(DataOutputStream&)const;
		void read(DataInputStream&);
	};

	using std::vector;
	class NBTTagList final: public NBTBase{
		typedef vector<NBTBase>::iterator iterator;
		typedef vector<NBTBase>::const_iterator const_iterator;
	private:
		int tagType;
		vector<NBTBase> data;
	public:
		NBTTagList();
		NBTTagList(DataInputStream&);
		virtual int getTagType()const;
		virtual void write(DataOutputStream&)const;
		virtual void read(DataInputStream&);
		virtual int hashCode()const;
		virtual bool operator==(const NBTBase&)const;
		const NBTBase& operator[](int)const;
		void add(NBTBase&);
		void clear();
		int getSize()const;
		int getListTagType()const;
		iterator begin();
		const_iterator begin()const;
		iterator end();
		const_iterator end()const;
	};

	class NBTTagString final: public NBTBase{
	private:
		string s;
	public:
		NBTTagString(const string&);
		NBTTagString(const char*);
		NBTTagString(DataInputStream&);
		virtual int getTagType()const;
		virtual void write(DataOutputStream&)const;
		virtual void read(DataInputStream&);
		virtual int hashCode()const;
		virtual bool operator==(const NBTBase&)const;
		const string& get()const;
	};

	using std::map;
	class NBTTagCompound final: public NBTBase{
	private:
		map<string,NBTBase*> m;
	public:
		NBTTagCompound();
		NBTTagCompound(DataInputStream&);
		int getTagType()const;
		void write(DataOutputStream&)const;
		void read(DataInputStream&);
		int hashCode()const;
		bool operator==(const NBTBase&)const;
		bool hasTag(const string&)const;
		bool hasTag(const char*)const;
		bool hasTag(const string&,int)const;
		bool hasTag(const char*,int)const;

		NBTBase& operator[](const string&);
		NBTBase& operator[](const char*);
		const NBTBase& operator[](const string&)const;
		const NBTBase& operator[](const char*)const;
		void setTag(const string&,NBTBase*);
		const NBTBase& getTag(const string&)const;
		NBTTagCompound& getCompoundTag(const string&);
		NBTTagCompound& getCompoundTag(const char*);
		NBTTagList& getListTag(const string&);
		NBTTagList& getListTag(const char*);

		int getInteger(const string&)const;
		int getInteger(const char*)const;
		int8_t getByte(const string&)const;
		int8_t getByte(const char*)const;
		int16_t getShort(const string&)const;
		int16_t getShort(const char*)const;
		int64_t getLong(const string&)const;
		int64_t getLong(const char*)const;
		float getFloat(const string&)const;
		float getFloat(const char*)const;
		double getDouble(const string&)const;
		double getDouble(const char*)const;
		bool getBoolean(const string&)const;
		bool getBoolean(const char*)const;
		string getString(const string&)const;
		string getString(const char*)const;

		void setInteger(const string&,int);
		void setInteger(const char*,int);
		void setByte(const string&,int8_t);
		void setByte(const char*,int8_t);
		void setShort(const string&,int16_t);
		void setShort(const char*,int16_t);
		void setLong(const string&,int64_t);
		void setLong(const char*,int64_t);
		void setFloat(const string&,float);
		void setFloat(const char*,float);
		void setDouble(const string&,double);
		void setDouble(const char*,double);
		void setBoolean(const string&,bool);
		void setBoolean(const char*,bool);
		void setString(const string&,const string&);
		void setString(const char*,const string&);

		UUID getUUID(const string&)const;
		UUID getUUID(const char*)const;

		void setUUID(const string&,UUID);
		void setUUID(const char*,UUID);
	};

	DataInputStream& operator>>(DataInputStream&,NBTTagCompound&);
	DataOutputStream& operator<<(DataOutputStream&,const NBTTagCompound&);


};

#endif /* __NBT_HPP__18_04_06_11_32_48 */
