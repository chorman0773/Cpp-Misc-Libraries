

#include <stdint.h>
#include <array>
#include <mutex>
#include <cstdlib>

using std::array;

typedef uint64_t seed_t;

using std::mutex;

class Random{
private:
    seed_t seed;
    bool haveNextNextGaussian;
    double nextNextGaussian;
    mutex mtx;
protected:
    virtual int next(int bits);
    void clearNextNextGaussianFlag();
public:
    Random();
    Random(seed_t);
    virtual ~Random();
    virtual void setSeed(seed_t);
    int nextInt();
    int nextInt(int);
    int64_t nextLong();
    float nextFloat();
    double nextDouble();
    double nextGaussian();
    bool nextBoolean();
    template<size_t size> virtual void nextBytes(array<uint8_t,size>& bytes);
};

using std::array;

template<size_t size> void Random::nextBytes(array<uint8_t,size>& bytes){
     for (int i = 0; i < size; )
     for (int rnd = nextInt(), n = __min(size - i, 4);
          n-- > 0; rnd >>= 8)
       bytes[i++] = (char)rnd;
}
