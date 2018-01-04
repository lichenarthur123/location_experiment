#coding=utf-8
blocksize = 1024
def GenBlocks(fileName):
	global blocksize
	with open(fileName,'rb') as f:
		file = f.read()
		blocknum = 4*1024
		blocksize = len(file)/blocknum
		print len(file)
		print blocksize
		if len(file)%blocknum!=0:
			blocksize = blocksize+1
		blocks = []
		
		for i in range(blocknum):
			print i
			ed = (i+1)*blocksize
			if ed>len(file):
				ed = len(file)
			block = file[i*blocksize:ed]
			blocks.append(block)
			with open('fileName'+'_B'+str(i),'wb') as subf:
				subf.write(block)
		print len(file)
		print blocksize
		return blocks

if __name__ == "__main__":
	fileName = 'test_file_4mb'
	blocks = GenBlocks(fileName)
	#print len(blocks)
	#print blocks