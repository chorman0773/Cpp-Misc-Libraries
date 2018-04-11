/*
 * Registry.hpp
 *
 *  Created on: Apr 10, 2018
 *      Author: Connor Horman
 */

#ifndef __GAME_REGISTRY_HPP__18_04_10_15_45_46
#define __GAME_REGISTRY_HPP__18_04_10_15_45_46

#include <string>
#include "Hash.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <stdexcept>

using std::string;
using std::map;
using std::unordered_map;
using std::vector;
using std::invalid_argument;

class ResourceLocation final: public Hashable{
private:
	string domain;
	string path;
public:
	ResourceLocation();
	ResourceLocation(const string&);
	ResourceLocation(const string&,const string&);
	const string& getDomain()const;
	const string& getPath()const;
	string toString()const;
	int hashCode()const;
	bool operator==(const ResourceLocation&)const;
	bool operator<(const ResourceLocation&)const;
	bool operator>(const ResourceLocation&)const;
	bool operator<=(const ResourceLocation&)const;
	bool operator>=(const ResourceLocation&)const;
};

template<typename T> class Registry{
private:
	vector<T> objArray;
	unordered_map<T,size_t> inverseObjectArray;
	map<ResourceLocation,T> reg;
public:
	inline Registry(){}
	inline void registerObject(const ResourceLocation& loc,const T& value){
		if(reg.count(loc)!=0)
			throw invalid_argument(loc.toString()+" is already registered.");
		size_t pos = objArray.size();
		objArray.add(value);
		inverseObjectArray[value] = pos;
		reg[loc] = value;
	}
	inline size_t getIndexForObject(const T& value){
		if(inverseObjectArray.count(value)==0)
			throw invalid_argument("This value is not registered in this registry");
		return inverseObjectArray[value];
	}
	inline const ResourceLocation& getObjectName(const T& value)const{
		if(inverseObjectArray.count(value)==0)
			throw invalid_argument("This value is not registered in this registry");
		return value.getLocation();
	}
	inline const T& getByName(const ResourceLocation& name)const{
		if(reg.count(name)==0)
			throw invalid_argument("The key "+name.toString()+" does not exist in this registry");
		return reg[name];
	}
	inline void registerObject(const T& value){
		registerObject(value.getLocation(),value);
	}

};



#endif /* __GAME_REGISTRY_HPP__18_04_10_15_45_46 */
