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
	
	cout<<"start to store tags..."<<endl;
	for(int i=0;i<blocknum;i++){
		int n = snprintf(temp,40,"%s%d","tags/tag_",i);
		cout<<temp<<endl;
		ofstream outtags(temp);
		outtags<<tags[i];
		outtags.close();
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
void GenProof(proof &pf ,const pub_key &pk,const char *filename,const int blocknum, const int blocksize, const chal &ch){
	//ofstream out1("tags/pk_n");
	//out1<<pk.n;
	//out1.close();
	
	//ofstream out2("tags/ch_k1");
	//out2<<ch.k1;
	//out2.close();
	
	Integer T("1");
	Integer P("0");
	int c = ch.c;
	vector<int> lis;
	for(int i = 0;i<blocknum;i++){
		lis.push_back(i);
	}
	srand(ch.k1.ConvertToLong());
	random_shuffle(lis.begin(),lis.end());
	char *tempfilename = new char[20];
	char *buff = new char[1000];
	for(int i=0;i<c;i++){
		snprintf(tempfilename,20,"%s%d","tags/tag_",lis[i]);
		ifstream myfile2(tempfilename);
		//cout<<tempfilename<<endl;
		memset(buff,0,1000);
		myfile2.getline(buff,929);
		Integer k3(buff);
		myfile2.close();
		//cout<<k3<<endl;
		T = a_times_b_mod_c(T,k3,pk.n);
	}
	char *temp = new char[40];
	char *las = new char[10];
	char *buf = new char[blocksize];
	for(int i=0;i<c;i++){
		memset(buf,0,blocksize);
		
		memset(temp,0,40);
		memset(las,0,10);
		sprintf(las,"%d",lis[i]);
		int n = snprintf(temp,40,"%s%s",filename,las);
		ReadBlock(buf,temp,blocksize);
		Integer m((byte*)buf,blocksize);
		P = P + m;
	}
	pf.T = T;
	pf.P = P;
	delete [] temp;
	delete [] las;
	delete [] buf;
}
extern "C"{
void wrapper_genproof(){
	proof pf;
	chal ch;
	pub_key pk;
	char *buff = new char[1000];
	
	ch.c = 350;
	memset(buff,0,1000);
	ifstream myfile("tags/ch_k1");
	myfile.getline(buff,10);
	Integer k1(buff);
	ch.k1 = k1;
	myfile.close();
	//cout<<ch.k1<<endl;
	ifstream myfile1("tags/pk_n");
	memset(buff,0,1000);
	myfile1.getline(buff,929);
	Integer k2(buff);
	pk.n = k2;
	myfile1.close();
	//cout<<pk.n<<endl;
	
	//vector<Integer> tags;
	//char *temp = new char[10];
	//for(int i=0;i<4*1024;i++){
	//	snprintf(temp,10,"%s%d","tags/tag_",i);
	//	ifstream myfile2(temp);
	//	memset(buff,0,1000);
	//	myfile2.getline(buff,929);
	//	Integer k3(buff);
	//	tags.push_back(k3);
	//	myfile2.close();
	//}
	//char *fileName="fileName_B";
	GenProof(pf,pk,"fileName_B",4*1024,1024,ch);
}
}
bool CheckProof(const char* fileName,const int &blocknum,const int &blocksize,const Integer &pk_n, const Integer &pk_g, const Integer sk_e, const Integer sk_v, const int &cc, const Integer &k1, const Integer &pf_p, const Integer &pf_t
//const char* fileName,const int &blocknum,const int &blocksize,const pub_key &pk,const pri_key &sk,const chal &ch, const proof &pf
){
	Integer T = a_exp_b_mod_c(pf_t,sk_e,pk_n);
	int c = cc;
	vector<int> lis;
	for(int i = 0;i<blocknum;i++){
		lis.push_back(i);
	}
	srand(k1.ConvertToLong());
	random_shuffle(lis.begin(),lis.end());
	byte *buf = new byte[3072/8];
	char *buf2 = new char[3072/8+10];
	sk_v.Encode(buf,3072/8);
	char *las = new char[10];
	Integer z("1");
	for(int i=0;i<c;i++){
		memset(las,0,10);
		memset(buf2,0,3072/8+10);
		sprintf(las,"%d",lis[i]);
		strncpy(buf2,(char*)buf,3072/8);
		strncpy(buf2+3072/8,las,10);
		byte digest[256];
		SHA256().CalculateDigest(digest,(byte*)buf2,3072/8+10);
		Integer hw(digest,SHA::DIGESTSIZE);
		z = a_times_b_mod_c(z,hw,pk_n);
	}
	delete [] buf;
	delete [] buf2;
	delete [] las;
	return a_times_b_mod_c(z,a_exp_b_mod_c(pk_g,pf_p,pk_n),pk_n)==T;
	
}

int main(){
	/*
	pub_key pk;
	pri_key sk;
	int blocknum = 4*1024;
	int blocksize = 1024;
	char *fileName = "fileName_B";
	
	Timer timer1;
	TimerWord sta1 = timer1.GetCurrentTimerValue();
	KeyGen(pk,sk);
	double ti1 = ((double)(timer1.GetCurrentTimerValue()-sta1)/(double)timer1.TicksPerSecond())*1000;
	cout<<ti1<<endl;
	
	vector<Integer> tags;
	TagBlocks(tags,fileName,pk,sk,blocknum,blocksize);
	
	int c = 350;
	chal ch;
	proof pf;
	Timer timer11;
	TimerWord sta11 = timer11.GetCurrentTimerValue();
	GenChallenge(c,sk,ch,pk);
	double ti11 = ((double)(timer11.GetCurrentTimerValue()-sta11)/(double)timer11.TicksPerSecond())*1000;
	cout<<ti11<<endl;
	//freopen("res_4m","w",stdout);
	//for(int i=0;i<1;i++){
		//GenChallenge(c,sk,ch,pk);
	
	Timer timer;
	TimerWord sta = timer.GetCurrentTimerValue();

	GenProof(pf,pk,fileName,blocknum,blocksize,ch,tags);

	double ti = ((double)(timer.GetCurrentTimerValue()-sta)/(double)timer.TicksPerSecond())*1000;
	cout<<ti<<endl;
	
	//}
	//fclose(stdout);
	//GenChallenge(c,sk,ch,pk);
	//GenProof(pf,pk,fileName,blocknum,blocksize,ch,tags);
	//const char* fileName,const int &blocknum,const int &blocksize,const Integer &pk_n, const Integer &pk_g, const Integer sk_e, const Integer sk_v, const int &cc, const Integer &k1, const Integer &pf_p, const Integer &pf_t
	/*if(CheckProof(fileName,blocknum,blocksize,pk.n,pk.g,sk.e,sk.v,ch.c,ch.k1,pf.P,pf.T)){
		cout<<"True"<<endl;
	}
	else{
		cout<<"False"<<endl;
	}
	//fclose(stdout);
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
	*/
	
	Timer timer;
	TimerWord sta = timer.GetCurrentTimerValue();
	wrapper_genproof();
	double ti = ((double)(timer.GetCurrentTimerValue()-sta)/(double)timer.TicksPerSecond())*1000;
	cout<<ti<<endl;
	return 0;
}