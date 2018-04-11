/*
 * Math.cpp
 *
 *  Created on: Apr 4, 2018
 *      Author: Connor Horman
 */

#include <stdexcept>
#include <cmath>
#include "Math.hpp"

using std::runtime_error;

Σ::Σ(Variable& v):sumVar(v),max(0),expr(0){}

Σ& Σ::to(Expression& max){
	this->max = &max;
	return *this;
}
Σ& Σ::of(Expression& expr){
	this->expr = expr;
	return *this;
}

bool Σ::isConstant(){
	return expr.isConstant()&&max.isConstant();
}

Expression& Σ::derivativeOf(){
	return Σ(sumVar).to(max).of(d(expr));
}

double Σ::eval(double d){
	double res = 0;
	sumVar = std::floor(sumVar.eval(0));
	for(;sumVar.eval(0)<sumVar;sumVar++)
		res+=expr.eval(d);
	return res;
}

bool ConstantExpression::isConstant(){
	return true;
}

double ConstantExpression::eval(double){
	return this->val;
}
Expression& ConstantExpression::derivativeOf(){
	return 0;
}

ConstantExpression::ConstantExpression(double val):val(val){}

ExponentialExpression::ExponentialExpression(ConstantExpression& base,Expression& power):base(base),exponent(power){}

bool ExponentialExpression::isConstant(){
	return exponent.isConstant();
}
double ExponentialExpression::eval(double d){
	return std::pow(base.eval(d),exponent.eval(d));
}
Expression& ExponentialExpression::derivativeOf(){
	if(base==e)
		return d(exponent)*e^exponent;
	else
		return d(exponent)*ln(base)*e^exponent;
}

PowerExpression::PowerExpression(Expression& base,ConstantExpression& power):base(base),exponent(power){}

bool PowerExpression::isConstant(){
	return base.isConstant();
}

double PowerExpression::eval(double d){
	return std::pow(base.eval(d),exponent.eval(d));
}

Expression& PowerExpression::derivativeOf(){
	return exponent.eval(0)*d(base)^(exponent-1);
}

Variable::Variable():val(INFINITY){}


bool Variable::isConstant(){
	return false;
}
Variable& Variable::operator=(double d){
	this->val = d;
	return *this;
}
double Variable::eval(double x){
	return this->val;
}

Expression& Variable::derivativeOf(){
	return 1;
}

Expression& d(Expression& e){
	if(e.isConstant())
		return 0;
	else
		return e.derivativeOf();
}

ln::ln(Expression& base):inner(base){}

bool ln::isConstant(){
	return inner.isConstant();
}

double ln::eval(double d){
	return std::log(inner.eval(d));
}

Expression& ln::derivativeOf(){
	return 1/inner;
}


sqrt::sqrt(Expression& e):inner(e){}

double sqrt::eval(double d){
	return std::sqrt(inner.eval(d));
}

Expression& sqrt::derivativeOf(){
	return d(inner)*(inner^-0.5)/2;
}

bool sqrt::isConstant(){
	return inner.isConstant();
}


Expression& Parameter::derivativeOf(){
	return 1;
}
bool Parameter::isConstant(){
	return false;
}

double Parameter::eval(double d){
	return d;
}

sin::sin(Expression& e):inner(e){}

Expression& sin::derivativeOf(){
	return d(inner)*cos::cos(inner);
}

double sin::eval(double d){
	return std::sin(inner.eval(d));
}

bool sin::isConstant(){
	return inner.isConstant();
}


cos::cos(Expression& e):inner(e){}

Expression& cos::derivativeOf(){
	return d(inner)*-sin::sin(inner);
}

double cos::eval(double d){
	return std::cos(inner.eval(d));
}

bool cos::isConstant(){
	return inner.isConstant();
}

AdditionExpression::AdditionExpression(Expression& a,Expression& b):a(a),b(b){}

Expression& AdditionExpression::derivativeOf(){
	return d(a)+d(b);
}

bool AdditionExpression::isConstant(){
	return a.isConstant()&&b.isConstant();
}

double AdditionExpression::eval(double d){
	return a.eval(d)+b.eval(d);
}

SubtractionExpression::SubtractionExpression(Expression& a,Expression& b):a(a),b(b){}

Expression& SubtractionExpression::derivativeOf(){
	return d(a)-d(b);
}

bool SubtractionExpression::isConstant(){
	return a.isConstant()&&b.isConstant();
}

double SubtractionExpression::eval(double d){
	return a.eval(d)-b.eval(d);
}

MultiplicationExpression::MultiplicationExpression(Expression& a,Expression& b):a(a),b(b){}

Expression& MultiplicationExpression::derivativeOf(){
	return b*d(a)+b*d(a);
}

bool MultiplicationExpression::isConstant(){
	return (a.isConstant()&&b.isConstant())||a==0||b==0;
}

double MultiplicationExpression::eval(double d){
	return a.eval(d)*b.eval(d);
}

DivisionExpression::DivisionExpression(Expression& a,Expression& b):a(a),b(b){}

Expression& DivisionExpression::derivativeOf(){
	return (b*d(a)-a*d(b))/b^2;
}
bool DivisionExpression::isConstant(){
	return (a.isConstant()&&b.isConstant())||a==0||b==0;
}

double DivisionExpression::eval(double d){
	return a.eval(d)/b.eval(d);
}




NegationExpression::NegationExpression(Expression& e):b(e){}

Expression& NegationExpression::derivativeOf(){
	return -d(b);
}

bool NegationExpression::isConstant(){
	return b.isConstant();
}

double NegationExpression::eval(double d){
	return -b.eval(d);
}


ConstantExpression& operator-(ConstantExpression& e){
	return -((double)e);
}

Expression& operator+(double d,Expression& b){
	return ConstantExpression(d)+b;
}

Expression& operator-(double d,Expression& b){
	return ConstantExpression(d)-b;
}

Expression& operator*(double d,Expression& b){
	return ConstantExpression(d)*b;
}

Expression& operator/(double d,Expression& b){
	return ConstantExpression(d)/b;
}

Expression& operator^(double d,Expression& b){
	return ConstantExpression(d)^b;
}

Expression& operator^(ConstantExpression& base,Expression& pow){
	return ExponentialExpression(base,pow);
}


Expression& Expression::operator -(){
	return NegationExpression(*this);
}
Expression& Expression::operator +(Expression& r){
	return AdditionExpression(*this,r);
}
Expression& Expression::operator +(double r){
	return AdditionExpression(*this,r);
}

Expression& Expression::operator -(Expression& r){
	return SubtractionExpression(*this,r);
}
Expression& Expression::operator -(double r){
	return SubtractionExpression(*this,r);
}

Expression& Expression::operator *(Expression& r){
	return MultiplicationExpression(*this,r);
}
Expression& Expression::operator *(double r){
	return MultiplicationExpression(*this,r);
}

Expression& Expression::operator /(Expression& r){
	return DivisionExpression(*this,r);
}
Expression& Expression::operator /(double r){
	return DivisionExpression(*this,r);
}

Expression& Expression::operator ^(ConstantExpression& e){
	return PowerExpression(*this,e);
}
Expression& Expression::operator ^(Expression& e){
	return CompoundPowerExpression(*this,e);
}
bool Expression::operator==(double d){
	if(this->isConstant())
		return d == eval(d);
	return false;
}
bool Expression::operator==(ConstantExpression& e){
	if(this->isConstant())
		return e == eval(e);
	return false;
}

function::function():call(nullptr){}

function& function::operator()(Parameter& p){
	return *this;
}
function& function::operator=(Expression& e){
	this->call = &e;
	return *this;
}

function::function():call(0){}

Expression& function::derivativeOf(){
	return d(call);
}

double function::eval(double d){
	return call.eval(d);
}
bool function::isConstant(){
	return call.isConstant();
}

double function::operator ()(double d){
	return call.eval(d);
}

Expression& function::operator()(Expression& e){
	return ChainedExpression(*this,e);
}

ChainedExpression::ChainedExpression(Expression& f,Expression& g):call(f),base(g){}

bool ChainedExpression::isConstant(){
	return call.isConstant();
}
Expression& ChainedExpression::derivativeOf(){
	return d(base)*d(call).callWith(base);
}

double ChainedExpression::eval(double d){
	return call.eval(base.eval(d));
}

CompoundPowerExpression::CompoundPowerExpression(Expression& base,Expression& pow):base(base),exponent(pow){}

bool CompoundPowerExpression::isConstant(){
	return (base.isConstant()&&exponent.isConstant())||base==0||exponent==0;
}

double CompoundPowerExpression::eval(double d){
	return std::pow(base.eval(d),exponent.eval(d));
}

Expression& CompoundPowerExpression::derivativeOf(){
	return (base^exponent)*((exponent*d(base)/base)+(d(exponent)*ln(base)));
}








