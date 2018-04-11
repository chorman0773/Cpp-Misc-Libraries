#include <stdint.h>


const int32_t hashPrime;
const int32_t uniqueSmartPrime;
const int32_t sharedSmartPrime;

/**
 * A class that can be hashed into a 32-bit signed integer value.
 * This class should not be instantiated on its own and should only be extended.
 * In order to properly use Hashable, the single method hashCode() should be extended.
 * By default this returns hashcode((const void*)this); (which results in returning the address of this)
 * All Subclasses should overload the == operator to make it consistant with hashCode() (Definition of Consitancy Below).
 * The general contract of hashcode is that for any distinct object, obtaining the hashCode() of that object at 2 arbitrary
 * points in execution should have the same value unless any values used in hashcode are modified between.
 * Comparing the hashcode of an object with the hashcode of any object that is copy or move constructed should
 * To make the == operator consistant with hashcode:
 * For any Hashable a and b, if a.hashCode()==b.hashCode() then a==b must be true.
 * This does not have to be, but should be inversely true. That is if a.hashCode()!=b.hashCode() then a!=b should be false.
 *
 * When hashCode() is overriden it should be const qualified.
 */
class Hashable{
public:
 virtual ~Hashable();
 virtual int32_t hashCode() const;
};

/*
    Computes the hashcode of the given value. This function is specialized to all known primitive types and some stl class types
    Normally rather than specializing this you should always extend Hashable with your classes, but it is available for specialization.
*/
template<typename T> int32_t hashcode(T);

template<typename T> int32_t arrayHashcode(T* arr,int size){
	int32_t hashcode = 0;
	for(int i = 0;i<size;i++){
		hashcode *=hashPrime;
		hashcode+=hashcode(arr[i]);
	}
	return hashcode;
}

/*
 * Template Implementations for Templated specializations (Such as array, vector, map, etc.)
 */
#include <array>
using std::array;
template<typename T,int size> int32_t hashcode(const array<T,size>& vals){
    int32_t hashcode = 0;
    for(int i = 0;i<size;i++){
        hashcode*=hashPrime;
        hashcode+=hashcode(vals[i]);
    }
    return hashcode;
}

#include <vector>
using std::vector;
template<typename T> int32_t hashcode(const vector<T>& vals){
    int32_t hashcode = 0;
    for(int i = 0;i<vals.length();i++)
    {
        hashcode*=hashPrime;
        hashcode+=vals[i];
    }
    return hashcode;
}
using std::initializer_list;
template<typename T> int32_t hashcode(const initializer_list<T>& vals){
    int32_t hashcode = 0;
    for(T& t:vals){
        hashcode*=hashPrime;
        hashcode+=hashcode(t);
    }
    return hashcode;
}

#include <memory>
using std::unique_ptr;
template<typename T> int32_t hashcode(unique_ptr<T>& ptr){
    return hashcode(ptr.get())*uniqueSmartPrime;
}
using std::shared_ptr;
template<typename T> int32_t hashcode(shared_ptr<T>& ptr){
    return hashcode(ptr.get())*sharedSmartPrime;
}

#include <utility>
using std::pair;
template<typename A,typename B> int32_t hashcode(const pair<A,B>& p){
    return hashcode(p.first)*hashPrime+hashcode(p.second);
}
#include <map>
using std::map;

template<typename K,typename V> int32_t hashcode(const map<K,V>& m){
    map<K,V>::iterator itr = m.begin();
    int32_t hashcode = 0;
    if(m.empty())
        return 0;
    for(pair<const K,V>& p:m){
        hashcode*=hashPrime;
        hashcode+=hashcode(p);
    }
    return hashcode;
}


template<typename E> int hashcode(std::enable_if<std::is_enum<E>::value,E>::type e){
	return hashcode<std::underlying_type<E>::type>(reinterpret_cast<std::underlying_type<E>::type>(e))*hashPrime+hashcode(typeid(E));
}

