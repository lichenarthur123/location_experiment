#coding=utf-8
import random
from Crypto.Util.number import long_to_bytes,bytes_to_long

# prf implement using prng
def pseudo_random_function(k, x, N):
	seed = k
	for i in range(N):
		random.seed(seed)
		tempValue = random.getrandbits(N*2)
		tempValue = long_to_bytes(tempValue,N/4)
		if x%2==1:
			tempValue = tempValue[N/8:N/4]
		else:
			tempValue = tempValue[0:N/8]
		seed = bytes_to_long(tempValue)
	return seed
	
if __name__=="__main__":
	k = 5
	x = random.getrandbits(256)
	print pseudo_random_function(k,x,256)
	print pseudo_random_function(k,x,256)
