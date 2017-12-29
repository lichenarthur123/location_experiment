#coding=utf-8
from Crypto.PublicKey import RSA
from Crypto.Hash import SHA256
from Crypto.Util.number import bytes_to_long
import random
def GenKey(k=256):
	rsa = RSA.generate(1024)
	n = rsa.n
	e = rsa.e 
	d = rsa.d 
	p = rsa.p 
	q = rsa.q
	g = 5
	v = random.getrandbits(k)
	pk = (n,g)
	sk = (e,d,v)
	return (pk,sk)

def PowMod(x,y,n):
	res = 1
	x = x%n
	while y>0:
		if y%2==1:
			res = (res*x)%n
		y = y/2
		x = (x*x)%n
	return res
	
def TagBlock(pk,sk,m,i):
	hash = SHA256.new()
	hash.update(str(sk[2])+str(i))
	hWi = bytes_to_long(hash.hexdigest())
	tMi = ((hWi%pk[0])*(PowMod(pk[1],bytes_to_long(m),pk[0])))%pk[0]
	tMi = PowMod(tMi,sk[1],pk[0])
	return tMi
