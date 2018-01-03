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
	for i in range(c):
		T = (T*tags[lis[i]])%pk[0]
	for i in range(c):
		f=open(fileName+"_B"+str(lis[i]),'rb')
		text = f.read()
		mi = bytes_to_long(text)
		P = P+mi
	#PX = PowMod(chal[3],P,pk[0])
	return (T,P)
	

def CheckProof(fileName,blockNum,pk,sk,chal,V,s):
	t = PowMod(V[0],sk[0],pk[0])
	c = chal[0]
	lis = [i+1 for i in range(blockNum)]
	random.seed(chal[1])
	random.shuffle(lis)
	z=1
	for i in range(c):
		w = str(sk[2])+str(lis[i])
		hash = SHA256.new()
		hash.update(w)
		hw = bytes_to_long(hash.hexdigest())
		z=(z*hw)%pk[0]

	tt = PowMod(t,s,pk[0])
	zz = PowMod(z,s,pk[0])
	kk = (zz*PowMod(chal[3],V[1],pk[0]))%pk[0]
	return kk==tt
