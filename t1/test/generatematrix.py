
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


generate("m1.txt",3,7)
generate("m2.txt",7,5)

