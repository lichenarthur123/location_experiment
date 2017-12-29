#coding=utf-8
import random
from Setup import PowMod
def GenChallenge(c,pk,s):
	k1=random.getrandbits(256)
	k2=random.getrandbits(256)
	gs=PowMod(pk[1],s,pk[0])
	return (c,k1,k2,gs)