#coding=utf-8

from Setup import GenKey,PowMod,TagBlock
from Challenge import GenChallenge
from Proof import GenProof,CheckProof
import random
from Crypto.Util.number import isPrime
import time

blocknum = 100

pk,sk = GenKey()
print "keys generated."
tags = []
for i in range(blocknum):
	print "taging block no."+str(i)
	f = open("fileName_B"+str(i),'rb')
	text = f.read()
	f.close()
	tags.append(TagBlock(pk,sk,text,i+1))
	
print "taging blocks"

s = random.getrandbits(256)
chal = GenChallenge(20,pk,s)

print "challenge generated"


V = GenProof(pk,'fileName',blocknum,chal,tags)

print "proof generated"
ti = []
for i in range(1):
	a = datetime.utcnow()  
	V=GenProof(pk,'fileName',blocknum,chal,tags)
	t = datetime.utcnow()-a
	ti.append(t.total_seconds())
	print "NO:",str(i),"test: ",str(t.total_seconds())

#f = open('A1','wb')
#for i in ti:
#	f.write(str(i)+'\n')
	
v = CheckProof('fileName',blocknum,pk,sk,chal,V,s)

print v
