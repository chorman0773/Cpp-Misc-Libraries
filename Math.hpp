/*
 * Math.hpp
 *
 *  Created on: Apr 4, 2018
 *      Author: Connor Horman
 */

#ifndef __MATH_HPP__18_04_04_24_19_59
#define __MATH_HPP__18_04_04_24_19_59
#include <string>

using std::string;

class ConstantExpression;

class Expression{
public:
	virtual ~Expression()=default;
	virtual double eval(double)=0;
	Expression& operator^(double);
	Expression& operator^(ConstantExpression&);
	Expression& operator^(Expression&);
	Expression& operator+(double);
	Expression& operator+(Expression&);
	Expression& operator-(double);
	Expression& operator-(Expression&);
	Expression& operator*(double);
	Expression& operator*(Expression&);
	Expression& operator/(double);
	Expression& operator/(Expression&);
	Expression& operator-();
	bool operator==(double);
	bool operator==(ConstantExpression&);
	virtual Expression& derivativeOf() = 0;
	virtual bool isConstant() = 0;
	Expression& callWith(Expression&);
};

Expression& operator^(double,Expression&);
Expression& operator+(double,Expression&);
Expression& operator-(double,Expression&);
Expression& operator*(double,Expression&);
Expression& operator/(double,Expression&);

class Variable : public Expression {
private:
	double val;
public:
	Variable& operator=(double);
	Variable& operator++(int);
	double eval(double);
	virtual Expression& derivativeOf();
	bool isConstant();
};


class Σ final : public Expression{
private:
	Variable& sumVar;
	Expression& max;
	Expression& expr;
public:
	Σ(Variable&);
	Σ& to(Expression&);
	Σ& of(Expression&);
	double eval(double);
	virtual Expression& derivativeOf();
	bool isConstant();
};
template<int base> class log final : public Expression{
private:
	Expression& inner;
public:
	log(Expression& e):inner(e){}
	double eval(double d){
		return (ln(inner)/ln(base)).eval(d);
	}
	Expression& derivativeOf(){
		return 1/ln(base)*inner;
	}
	bool isConstant(){
		return inner.isConstant();
	}
};

class ln final : public Expression{
private:
	Expression& inner;
public:
	ln(Expression&);
	double eval(double);
	Expression& derivativeOf();
	bool isConstant();
};

class ConstantExpression final : public Expression{
private:
	double val;
public:
	constexpr ConstantExpression(double);
	ConstantExpression& operator=(const ConstantExpression&)=delete;
	ConstantExpression& operator=(ConstantExpression&&)=delete;
	double eval(double);
	Expression& derivativeOf();
	bool isConstant();
	operator double();

};

ConstantExpression& operator-(ConstantExpression&);

constexpr ConstantExpression e(2.71828182845904523536028747135266249775724709369995);
constexpr ConstantExpression π(3.14159265358979323846264338327950288419716939937510);
constexpr ConstantExpression inf(reinterpret_cast<double&>(0x7FF0000000000000));

Expression& operator^(ConstantExpression&,Expression&);


class ExponentialExpression final : public Expression{
private:
	ConstantExpression& base;
	Expression& exponent;
public:
	ExponentialExpression(ConstantExpression&,Expression&);
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};

class PowerExpression final : public Expression{
private:
	Expression& base;
	ConstantExpression& exponent;
public:
	PowerExpression(Expression&,ConstantExpression&);
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};
class MultiplicationExpression final : public Expression{
private:
	Expression& a;
	Expression& b;
public:
	MultiplicationExpression(Expression&,Expression&);
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};

class DivisionExpression final : public Expression{
private:
	Expression& a;
	Expression& b;
public:
	DivisionExpression(Expression&,Expression&);
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};

class AdditionExpression final : public Expression{
private:
	Expression& a;
	Expression& b;
public:
	AdditionExpression(Expression&,Expression&);
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};

class SubtractionExpression final : public Expression{
private:
	Expression& a;
	Expression& b;
public:
	SubtractionExpression(Expression&,Expression&);
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};
class NegationExpression final : public Expression{
private:
	Expression& b;
public:
	NegationExpression(Expression&);
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};

class sqrt final : public Expression{
private:
	Expression& inner;
public:
	explicit sqrt(Expression&);
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};

class sin final : public Expression{
private:
	Expression& inner;
public:
	explicit sin(Expression&);
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};
class cos final : public Expression{
private:
	Expression& inner;
public:
	cos(Expression&);
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};

class Parameter final : public Expression{
public:
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};

class function final : public Expression{
private:
	Expression& call;
public:
	function();
	function& operator=(Expression&);
	double operator()(double);
	function& operator()(Parameter&);
	Expression& operator()(Expression&);
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};

class ChainedExpression final : public Expression{
private:
	Expression& base;
	Expression& call;
public:
	ChainedExpression(Expression&,Expression&);
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};

class CompoundPowerExpression final : public Expression{
private:
	Expression& base;
	Expression& exponent;
public:
	CompoundPowerExpression(Expression&,Expression&);
	double eval(double);
	bool isConstant();
	Expression& derivativeOf();
};


Expression& d(Expression&);



#endif /* __MATH_HPP__18_04_04_24_19_59 */
