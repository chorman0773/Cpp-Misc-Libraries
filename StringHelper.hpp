/*
 * StringHelper.hpp
 *
 *  Created on: Apr 4, 2018
 *      Author: Connor Horman
 */

#ifndef __STRINGHELPER_HPP__18_04_04_11_44_03
#define __STRINGHELPER_HPP__18_04_04_11_44_03

#include <string>

using std::string;

string tokenize(string&,string);
string tokenize(string&,const char*);
string& replace(string&,string,string);
int count(string,string);




#endif /* __STRINGHELPER_HPP__18_04_04_11_44_03 */
