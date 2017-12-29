#coding=utf-8
from PseudoRandom import pseudo_random_function
from Setup import PowMod
from Crypto.Util.number import inverse,bytes_to_long
from Crypto.Hash import SHA256
import random
from datetime import datetime

def p_i(fileName,id,a,gs,n):
	f=open(fileName+"_B"+str(id),'rb')
	text = f.read()
	mi = bytes_to_long(text)
	f.close()
	return PowMod(gs,a*mi,n)

def GenProof(pk,fileName,blockNum,chal,tags):
	T=1
	P=1
	c=chal[0]
	lis = [i+1 for i in range(blockNum)]
	random.seed(chal[1])
	random.shuffle(lis)
	prfList = []
	for i in range(1,c+1):
		prfList.append(pseudo_random_function(chal[2],i,256))
	for i in range(c):
		T = (T*PowMod(tags[lis[i]-1],prfList[i],pk[0]))%pk[0]
	for i in range(c):
		P = (P*p_i(fileName,lis[i]-1,prfList[i],chal[3],pk[0]))%pk[0]
	hash = SHA256.new()
	hash.update(str(P))
	hP = hash.hexdigest()
	return (T,hP,P)
	

def CheckProof(fileName,blockNum,pk,sk,chal,V,s):
	t = PowMod(V[0],sk[0],pk[0])
	c = chal[0]
	lis = [i+1 for i in range(blockNum)]
	random.seed(chal[1])
	random.shuffle(lis)
	prfList = []
	for i in range(1,c+1):
		prfList.append(pseudo_random_function(chal[2],i,256))
	z=1
	for i in range(c):
		w = str(sk[2])+str(lis[i])
		hash = SHA256.new()
		hash.update(w)
		hw = bytes_to_long(hash.hexdigest())
		#temp = PowMod(hw*p_i(fileName,lis[i]-1,1,pk[1],pk[0])%pk[0],sk[1],pk[0])
		hwa = PowMod(hw,prfList[i],pk[0])
		#gg = p_i(fileName,lis[i]-1,prfList[i],pk[1],pk[0])%pk[0]
		#tim1.append(PowMod(hwa*gg%pk[0],sk[1],pk[0]))
		#tim1.append(temp)
		#gim1.append(gg)
		z=(z*hwa)%pk[0]
		hwaInverse = inverse(hwa,pk[0])
		#t = (t*hwaInverse)%pk[0]
	ts = PowMod(t,s,pk[0])
	hash.update(str(ts))
	hts = hash.hexdigest()

	tt = PowMod(t,s,pk[0])
	zz = PowMod(z,s,pk[0])
	kk = (zz*V[2])%pk[0]
	#return ((kk-tt)%pk[0],tim1,gim1)

	#return V[1]==hts
	return kk==tt
