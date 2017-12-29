#coding=utf-8
blocksize = 1024*10
def GenBlocks(fileName):
	global blocksize
	with open(fileName,'rb') as f:
		file = f.read()
		blocks = []
		if len(file)%blocksize!=0:
			fillNum = blocksize - len(file)%blocksize
			for i in range(fillNum):
				file = file + '\0'
		blockNum = len(file)/blocksize
		for i in range(blockNum):
			block = file[i*blocksize:(i+1)*blocksize]
			blocks.append(block)
			with open('fileName'+'_B'+str(i),'wb') as subf:
				subf.write(block)
		return blocks

if __name__ == "__main__":
	fileName = 'test_file_1M'
	blocks = GenBlocks(fileName)
	print len(blocks)
	#print blocks
			