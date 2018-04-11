#ifndef __OpenSSLHelper_hpp_2018_03_26_09_53
#define __OpenSSLHelper_hpp_2018_03_26_09_53

#include <array>
#include <cstdlib>

extern "C"{
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
};


using std::array;


template<typename base> struct CryptoTraits{
    static const constexpr size_t size;
    static const constexpr bool deprecated;
};

template<int version> class SHA{
};

template<> __attribute__(deprecated) class SHA<1>{
public:

	template<size_t inSize> static void digest(const array<char,inSize>& arr,char out[]){
		SHA1(arr.data(),inSize,out);
	}
};
template<> class SHA<224>{
public:
	template<size_t inSize> static void digest(const array<char,inSize>& arr,char out[]){
		SHA224(arr.data(),inSize,out);
	}
};
template<> class SHA<256>{
public:
	template<size_t inSize> static void digest(const array<char,inSize>& arr,char out[]){
		SHA256(arr.data(),inSize,out);
	}
};
template<> class SHA<384>{
public:
	template<size_t inSize> static void digest(const array<char,inSize>& arr,char out[]){
		SHA384(arr.data(),inSize,out);
	}
};

template<> class SHA<512>{
public:
	template<size_t inSize> static void digest(const array<char,inSize>& arr,char out[]){
		SHA512(arr.data(),inSize,out);
	}
};

class NoSuchAlgorithmException{};



class __attribute__(deprecated) MD5{
public:
	template<size_t inSize> static void digest(const array<char,inSize>& arr,char out[]){
		MD5(arr.data(),inSize,out);
	}
};



struct RSAConstants{
	constexpr bignum_st* genPublicExponent(){
		bignum_st* st;
		BN_dec2bn(&st,"65537");
		return st;
	}
public:
	const static bignum_st* e;
};



template<size_t size> class RSAPublicKey{
	bignum_st* key;
public:
	RSAPublicKey(bignum_st* key):key(key){
	}
	RSAPublicKey(const array<char,size>& encoded){
		key = BN_new();
		BN_bin2bn(encoded.data(),size,key);
	}
	RSAPublicKey():key(nullptr){}
	~RSAPublicKey(){
		BN_free(key);
	}
	const bignum_st& getValue()const{
		return key;
	}
	void getEncoded(array<char,size>& arr)const{
		BN_bn2bin(&key,arr.data());
	}
	rsa_st* getAsRsaStruct()const{
		rsa_st* key = RSA_new();
		key->n = this->key;
		key->e = RSAConstants::e;
		key->meth = RSA_get_default_method();
		return key;
	}
	template<size_t inSize> int decrypt(const array<char,inSize>& in,char* out)const{
		return RSA_public_decrypt(inSize,in.data(),out,getAsRsaStruct(),RSA_PKCS1_PADDING);
	}
	template<size_t inSize> int encrypt(const array<char,inSize>& in,char* out)const{
		return RSA_public_encrypt(inSize,in.data(),out,getAsRsaStruct(),RSA_PKCS1_PADDING);
	}
};
template<size_t size> class RSAPrivateKey{
	bignum_st* p;
	bignum_st* q;
	bignum_st* n;
	mutable bool destroyed;
public:
	RSAPrivateKey(bignum_st* p,bignum_st* q):p(p),q(q),destroyed(false){
		n =BN_new();
		BN_mul(n,p,q,NULL);
	}
	RSAPrivateKey(const array<char,size>& arr):destroyed(false){
		p = BN_new();
		q= BN_new();
		BN_bin2bn(arr.data(),size/2,&p);
		BN_bin2bn(arr.data()+size/2,size/2,&q);
		n =BN_new();
		BN_mul(n,p,q,NULL);
	}
	RSAPrivateKey():destroyed(true),p(nullptr),q(nullptr),n(nullptr){}
	~RSAPrivateKey(){
		destroy();
	}
	RSAPublicKey<size> getPublicKey()const{
		if(destroyed)
			throw "Illegal State, cannot obtain the public key from this private key";
		bignum_st* ret = BN_new();
		BN_mul(ret,p,q,NULL);
		return RSAPublicKey<size>(ret);
	}
	void getEncoded(array<char,size>& arr)const{
		if(destroyed)
			throw "Illegal State, this private key has been destroyed";
		BN_bn2bin(p,arr.data());
		BN_bn2bin(q,arr.data()+size/2);
	}
	void destroy()const{
		BN_clear_free(p);
		BN_clear_free(q);
		destroyed = true;
	}

	rsa_st* getAsRsaStruct()const{
		if(destroyed)
			throw "Illegal state, this private key has been destroyed";
		rsa_st* key = RSA_new();
		key->p = p;
		key->q = q;
		key->n = n;
		key->e = RSAConstants::e;
		bignum_st* pm1 = BN_new();
		BN_usub(pm1,p,BN_value_one());
		bignum_st* qm1 = BN_new();
		BN_usub(qm1,q,BN_value_one());
		bignum_st* d = BN_new();
		BN_mul(d,pm1,qm1,nullptr);
		BN_div(d,nullptr,d,RSAConstants::e,nullptr);
		key->d = d;
		key->meth = RSA_get_default_method();
		return key;
	}

	template<size_t inSize> int decrypt(const array<char,inSize>& in,char* out)const{
		return RSA_private_decrypt(inSize,in.data(),out,getAsRsaStruct(),RSA_PKCS1_PADDING);
	}
	template<size_t inSize> int encrypt(const array<char,inSize>& in,char* out)const{
		return RSA_private_encrypt(inSize,in.data(),out,getAsRsaStruct(),RSA_PKCS1_PADDING);
	}

	template<size_t size> friend class RSAKeyPair;

};
template<size_t size> class RSAKeyPair{
private:
	RSAPublicKey<size> pub;
	RSAPrivateKey<size> priv;
public:
	RSAKeyPair(){
		bignum_st* p = BN_new();
		bignum_st* q = BN_new();
		RAND_poll();
		BN_generate_prime_ex(p,size/2,false,nullptr,nullptr,nullptr);
		BN_generate_prime_ex(q,size/2,false,nullptr,nullptr,nullptr);
		priv(p,q);
		pub = priv.getPublicKey();
	}
	~RSAKeyPair(){}
	const RSAPublicKey<size>& getPublicKey()const{
		return pub;
	}
	const RSAPrivateKey<size>& getPrivateKey()const{
		if(priv.destroyed)
			throw "Illegal State, the private key of this generator has been Destroyed";
		return priv;
	}
	void destroy()const{
		priv.destroy();
	}
};


#endif

