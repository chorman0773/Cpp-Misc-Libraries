/*
 * Helpers.hpp
 *
 *  Created on: Apr 3, 2018
 *      Author: Connor Horman
 */

#ifndef __HELPERS_HPP__18_04_03_09_18_15
#define __HELPERS_HPP__18_04_03_09_18_15



template<bool cond,const char* msg> class _static_warn{};

template<const char* msg> class _static_warn<false,msg>{};

template<const char* msg> __attribute__((deprecated)) class  _static_warn<true,msg>{};

#define static_warn(cond,msg) typedef _static_warn<cond,msg> __null__;




#endif /* __HELPERS_HPP__18_04_03_09_18_15 */
