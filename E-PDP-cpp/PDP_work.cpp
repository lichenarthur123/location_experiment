#include<iostream>
#include "cryptopp/randpool.h"
#include "cryptopp/osrng.h"
#include "cryptopp/hex.h"
#include "cryptopp/rsa.h"
#include "cryptopp/integer.h"
#include "cryptopp/sha.h"
#include <algorithm>
#include <vector>
#include <string>
#include <iterator>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "cryptopp/hrtimer.h"
#include <unistd.h>

using namespace CryptoPP;
using namespace std;

struct pub_key{
	Integer n;
	Integer g;
};
struct pri_key{
	Integer e;
	Integer d;
	Integer v;
	Integer s;
};

struct chal{
	int c;
	Integer k1;
	Integer k2;
	Integer gs;
};

struct proof{
	Integer T;
	Integer P;
};

Integer ExpMod(const Integer &a, const Integer &e, const Integer &m){
	Integer res("1");
	Integer x = a % m;
	Integer y = e;
	Integer zero("0");
	while(y.Compare(zero)>0){
		if(y.GetBit(0))
			res = (res*x)%m;
		y >>= 1;
		x = (x*x)%m;
	}
	return res;
	
}

void KeyGen(pub_key &pk, pri_key &sk){
	//PRNG 
	AutoSeededRandomPool rng;
	
	// Generate Parameters
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 3072);
	
	///////////////////////////////////////
	// Generated Parameters
	pk.n = params.GetModulus();
	pk.g = Integer("5");
	//const Integer& p = params.GetPrime1();
	//const Integer& q = params.GetPrime2();
	sk.d = params.GetPrivateExponent();
	sk.e = params.GetPublicExponent();
	sk.v = Integer(rng,3072);
}

void ReadBlock(char *buf,char *filename,int blocksize){
	ifstream in(filename);
	istreambuf_iterator<char> begin(in);
	istreambuf_iterator<char> end;
	string some_str(begin, end);
	some_str.copy(buf,blocksize,0);
	in.close();
}

void TagBlocks(vector<Integer> &tags, char *filenameprex,const pub_key &pk,const pri_key &sk,int &blocknum,int &blocksize){
	char *temp = new char[40];
	char *las = new char[10];
	char *buf = new char[blocksize];
	byte *buf2 = new byte[3072/8];
	char *buf3 = new char[3072/8+10];
	byte digest[SHA::DIGESTSIZE];
	for(int i=0;i<blocknum;i++){
		cout<<i<<endl;
		memset(temp,0,40);
		memset(las,0,10);
		sprintf(las,"%d",i);
		int n = snprintf(temp,40,"%s%s",filenameprex,las);
		memset(buf,0,blocksize);
		ReadBlock(buf,temp,blocksize);
		
		Integer m((byte*)buf,blocksize);
		memset(buf2,0,3072/8);
		sk.v.Encode(buf2,3072/8);
		memset(buf3,0,3072/8+10);
		strncpy(buf3,(char*)buf2,3072/8);
		strncpy(buf3+3072/8,las,10);
		byte digest[256];
		
		SHA256().CalculateDigest(digest,(byte*)buf3,3072/8+10);
		Integer hw(digest,SHA::DIGESTSIZE);
		Integer Tmi = a_exp_b_mod_c(a_times_b_mod_c(hw,a_exp_b_mod_c(pk.g,m,pk.n),pk.n),sk.d,pk.n);
		tags.push_back(Tmi);
	}
	delete [] buf;
	delete [] buf2;
	delete [] buf3;
	delete [] las;
	delete [] temp;
}

void GenChallenge(int &c,pri_key &sk, chal &ch ,const pub_key &pk){
	AutoSeededRandomPool rng;
	Integer s = Integer(rng,3072);
	Integer k1 = Integer(rng,32);
	Integer k2 = Integer(rng,32);
	Integer gs = a_exp_b_mod_c(pk.g,s,pk.n);
	ch.c = c;
	ch.k1 = k1;
	ch.k2 = k2;
	ch.gs = gs;
	sk.s = s;
}

void GenProof(proof &pf ,const pub_key &pk,const char *filename,const int blocknum, const int blocksize, const chal &ch, const vector<Integer> &tags){
	Integer T("1");
	Integer P("1");
	int c = ch.c;
	vector<int> lis;
	for(int i = 0;i<blocknum;i++){
		lis.push_back(i);
	}
	srand(ch.k1.ConvertToLong());
	random_shuffle(lis.begin(),lis.end());
	for(int i=0;i<c;i++){
		T = a_times_b_mod_c(T,tags[i],pk.n);
	}
	char *temp = new char[40];
	char *las = new char[10];
	char *buf = new char[blocksize];
	for(int i=0;i<c;i++){
		memset(buf,0,blocksize);
		
		memset(temp,0,40);
		memset(las,0,10);
		sprintf(las,"%d",i);
		int n = snprintf(temp,40,"%s%s",filename,las);
		ReadBlock(buf,temp,blocksize);
		Integer m((byte*)buf,blocksize);
		P = (P + m)%pk.n;
	}
	pf.T = T;
	pf.P = P;
	delete [] temp;
	delete [] las;
	delete [] buf;
}

bool CheckProof(const char* fileName,const int &blocknum,const int &blocksize,const pub_key &pk,const pri_key &sk,const chal & ch, const proof &pf){
	Integer T = a_exp_b_mod_c(pf.T,sk.e,pk.n);
	int c = ch.c;
	vector<int> lis;
	for(int i = 0;i<blocknum;i++){
		lis.push_back(i);
	}
	srand(ch.k1.ConvertToLong());
	random_shuffle(lis.begin(),lis.end());
	byte *buf = new byte[3072/8];
	char *buf2 = new char[3072/8+10];
	sk.v.Encode(buf,3072/8);
	char *las = new char[10];
	Integer z("1");
	for(int i=0;i<c;i++){
		memset(las,0,10);
		memset(buf2,0,3072/8+10);
		sprintf(las,"%d",i);
		
		strncpy(buf2,(char*)buf,3072/8);
		strncpy(buf2+3072/8,las,10);
		byte digest[256];
		SHA256().CalculateDigest(digest,(byte*)buf2,3072/8+10);
		Integer hw(digest,SHA::DIGESTSIZE);
		z = a_times_b_mod_c(z,hw,pk.n);
	}
	delete [] buf;
	delete [] buf2;
	delete [] las;
	return a_exp_b_mod_c(z,sk.s,pk.n)*a_exp_b_mod_c(a_exp_b_mod_c(pk.g,sk.s,pk.n),pf.P,pk.n)==a_exp_b_mod_c(T,sk.s,pk.n);
	
}

int main(){
	cout<<"0.1"<<endl;
	pub_key pk;
	pri_key sk;
	int blocknum = 100;
	int blocksize = 41;
	char *fileName = "fileName_B";
	KeyGen(pk,sk);
	vector<Integer> tags;
	TagBlocks(tags,fileName,pk,sk,blocknum,blocksize);
	
	int c = 20;
	chal ch;
	proof pf;
	freopen("res_4m","w",stdout);
	for(int i=0;i<1;i++){
		GenChallenge(c,sk,ch,pk);
	
		Timer timer;
		TimerWord sta = timer.GetCurrentTimerValue();
	
		GenProof(pf,pk,fileName,blocknum,blocksize,ch,tags);
	
		double ti = ((double)(timer.GetCurrentTimerValue()-sta)/(double)timer.TicksPerSecond())*1000;
		cout<<ti<<endl;
	}
	//fclose(stdout);
	GenChallenge(c,sk,ch,pk);
	GenProof(pf,pk,fileName,blocknum,blocksize,ch,tags);
	if(CheckProof(fileName,blocknum,blocksize,pk,sk,ch,pf)){
		cout<<"True"<<endl;
	}
	else{
		cout<<"False"<<endl;
	}
	fclose(stdout);
	//cout<<pf.T<<endl;
	//cout<<pf.P<<endl;
	//Integer x = a_exp_b_mod_c(a,sk.e,pk.n);
	//x = a_exp_b_mod_c(x,sk.d,pk.n);
	//cout<<"a+e:"<<x<<endl;
	//int blocksize = 10*1024;
	//char *buf = new char[10240];
	//ReadBlock(buf,"fileName_B0",blocksize);
	//Integer tt((byte*)buf,10240);
	//cout<<tt<<endl;
	cout<<"end"<<endl;
	return 0;
}