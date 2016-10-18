
import random



def generate(filename,lin,col):
	f = open(filename, 'w')
	f.write("LINHAS = ")
	f.write(str(lin))
	f.write("\nCOLUNAS = ")
	f.write(str(col)+'\n')

	for l in range(lin):
    		f.write(" ".join([str(random.choice(range(1,10))) for x in range(col)])+'\n')
	f.close	()


generate("in1.txt",10,10)
generate("in2.txt",10,10)

