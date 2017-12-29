#coding=utf-8
from PseudoRandom import pseudo_random_function
from Setup import PowMod
from Crypto.Util.number import inverse,bytes_to_long
from Crypto.Hash import SHA256
import random

def p_i(fileName,id,gs,n):
	f=open(fileName+"_B"+str(id),'rb')
	text = f.read()
	mi = bytes_to_long(text)
	f.close()
	return PowMod(gs,mi,n)

def GenProof(pk,fileName,blockNum,chal,tags):
	T=1
	P=0
	c=chal[0]
	lis = [i for i in range(blockNum)]
	random.seed(chal[1])
	random.shuffle(lis)
	prfList = []
	#for i in range(1,c+1):
	#	prfList.append(pseudo_random_function(chal[2],i,256))
	#for i in range(c):
		#print "we chose no."+str(lis[i]+1)
	for i in range(c):
		T = (T*tags[lis[i]])%pk[0]
	for i in range(c):
		f=open(fileName+"_B"+str(lis[i]),'rb')
		text = f.read()
		mi = bytes_to_long(text)
		P = P+mi
	PX = PowMod(chal[3],P,pk[0])
		
		
	#print P
	hash = SHA256.new()
	hash.update(str(P))
	hP = hash.hexdigest()
	return (T,hP,PX)
	

def CheckProof(fileName,blockNum,pk,sk,chal,V,s):
	t = PowMod(V[0],sk[0],pk[0])
	c = chal[0]
	lis = [i+1 for i in range(blockNum)]
	random.seed(chal[1])
	random.shuffle(lis)
	#prfList = []
	#for i in range(1,c+1):
		#prfList.append(pseudo_random_function(chal[2],i,256))
	z=1
	#for i in range(c):
		#print "we check no."+str(lis[i])
	for i in range(c):
		w = str(sk[2])+str(lis[i])
		hash = SHA256.new()
		hash.update(w)
		hw = bytes_to_long(hash.hexdigest())
		#hwa = PowMod(hw,prfList[i],pk[0])
		z=(z*hw)%pk[0]
		#hwaInverse = inverse(hwa,pk[0])
	#ts = PowMod(t,s,pk[0])
	#hash.update(str(ts))
	#hts = hash.hexdigest()

	tt = PowMod(t,s,pk[0])
	zz = PowMod(z,s,pk[0])
	#v2 = PowMod(pk[1],V[2],pk[0])
	kk = (zz*V[2])%pk[0]
	#print kk
	#print tt%pk[0]
	return kk==tt
