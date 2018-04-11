#include "Random.hpp"


#include <cmath>
#include "JTime.hpp"
#include "Random.hpp"

#include <array>

seed_t num(65536);
constexpr const seed_t cprime(366313041593);
constexpr const seed_t multiplier(0x5DEECE66D);
constexpr const seed_t incrementer(0xBL);
constexpr const seed_t maxValue((1<<48L)-1);

seed_t nextMultiplier(){
    seed_t val = num;
    num *=2;
    return (val+1)*cprime;
}

seed_t genRandomSeed(){
    int64_t time = Instant::now().get(ChronoUnit::NANOSECONDS);
    return time*nextMultiplier();
}
seed_t initRandomizeSeed(seed_t s){
    return (s^multiplier)&maxValue;
}

Random::Random(){
    this->setSeed(genRandomSeed());
}
Random::Random(seed_t s){
    this->setSeed(s);
}

void Random::setSeed(seed_t s){
	this->mtx.lock();
    this->seed = initRandomizeSeed(s);
    this->haveNextNextGaussian = false;
    this->mtx.unlock();
}
int Random::next(int bits){
	int val;
	this->mtx.lock();
    this->seed = (multiplier*seed)+incrementer;
    val = (this->seed)>>(48-bits);
    this->mtx.unlock();
    return val;
}

int Random::nextInt(){
    return next(32);
}

int Random::nextInt(int bound){
    if(bound<=0)
        throw "Bound must be positive";

    if((bound&-bound)==bound)
        return (int)((bound * (uint64_t)next(31)) >> 31);

   int bits, val;
   do {
       bits = next(31);
       val = bits % bound;
   } while (bits - val + (bound-1) < 0);
   return val;
}

int64_t Random::nextLong(){
    return ((int64_t)nextInt())<<32+nextInt();
}

float Random::nextFloat(){
    return (next(24))/((float)1<<24);
}

double Random::nextDouble(){
    return (((int64_t)next(26) << 27) + next(27)) / (double)(1L << 53);
}

double Random::nextGaussian(){
	double val;
	mtx.lock();
    if(this->haveNextNextGaussian){
        this->haveNextNextGaussian = false;
        val = this->nextNextGaussian;
    }else{
    double v1, v2, s;
    do {
		v1 = 2 * nextDouble() - 1;   // between -1.0 and 1.0
		v2 = 2 * nextDouble() - 1;   // between -1.0 and 1.0
		s = v1 * v1 + v2 * v2;
    } while (s >= 1 || s == 0);
    double mulitplier = sqrt(-2 * log(s)/s);
    this->nextNextGaussian = v2 * mulitplier;
    this->haveNextNextGaussian = true;
    val = v1 * mulitplier;
    }
    mtx.unlock();
    return val;
}
bool Random::nextBoolean(){
    return next(1)!=0;
}



