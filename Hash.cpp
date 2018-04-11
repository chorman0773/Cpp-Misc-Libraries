/*
    Hashcode calculation definitions.
    This file contains all possible specilizations for template<typename T> int32_t hashcode(T) defined in Hash.hpp
    Each section is defined with the types that the section specializes for
*/

#include "Hash.hpp"
#include <string>
#include <string.h>
#include <vector>
#include <array>
#include <initializer_list>
#include <typeinfo>
#include <memory>
#include <map>
#include <utility>

const int32_t hashPrime = 31;

const int32_t uniqueSmartPrime = 11443717;
const int32_t sharedSmartPrime = 540283;

Hashable::~Hashable(){}//Do nothing, virtual for subclasses

using std::vector;
using std::string;
using std::array;
using std::initializer_list;
using std::type_info;
using std::unique_ptr;
using std::shared_ptr;
using std::nullptr_t;
using std::pair;
using std::basic_string;


/*
    Hashcode for primitives (int, short, long, float, double) (uses int64_t for val)
*/
template<> int32_t hashcode<int>(int val){
    return val;
}
template<> int32_t hashcode<short>(short val){
    return val;
}
template<> int32_t hashcode<int64_t>(int64_t val){
    return (val>>32)^(val&((1LL<<32-1)));
}
template<> int32_t hashcode<float>(float val){
    return *reinterpret_cast<int32_t*>(&val);
}
template<> int32_t hashcode<double>(double val){
    return hashcode(reinterpret_cast<long&>(val));
}
template<> int32_t hashcode<char>(char val){
    return val;
}
template<> int32_t hashcode<uint32_t>(uint32_t val){
    return (int32_t)val;
}

/*
    Hashcode for Strings (string and char*)
*/

template<> int32_t hashcode(string val){
    int32_t hashcode = 0;
    for(int i = 0;i<val.size();i++)
        {
            hashcode *=hashPrime;
            hashcode +=val[i];
        }
    return hashcode;
}
template<> int32_t hashcode(const char* val){
    int32_t hashcode = 0;
    for(int i = 0;i<strlen(val);i++){
        hashcode *=hashPrime;
        hashcode +=val[i];
    }
    return hashcode;
}



template<> int32_t hashcode(const Hashable& val){
    return val.hashCode();
}
template<> int32_t hashcode(Hashable&& val){
    return val.hashCode();
}
template<> int32_t hashcode(const Hashable* val){
    return val->hashCode();
}

/**
 *  Reference hashcodes for unknown pointer types or nullptr
 *  hashcode(nullptr) is a constant expressions and always resolves to 0.
 *  The hashcode of a unspecialized pointer type is the address of that pointer (analogous to Object.hashcode() from java)
 */
template<> constexpr int32_t hashcode(nullptr_t val){
    return 0;
}


template<> int32_t hashcode(const void* val){
    return (int32_t)val;
}


template<> int32_t hashcode(const int* val){
    if(val==nullptr)
        return 0;
    return hashcode((int32_t) *val);
}
template<> int32_t hashcode(const float* val){
    if(val==nullptr)
        return 0;
    return hashcode((float) *val);
}
template<> int32_t hashcode(const int64_t* val){
    if(val==nullptr)
        return 0;
    return hashcode((int64_t) *val);
}


template<> int32_t hashcode(type_info* inf){
   return inf->hash_code();
}
template<> int32_t hashcode(type_info& inf){
	return inf.hash_code();
}
template<> int32_t hashcode(type_info&& inf){
	return inf.hash_code();
}










