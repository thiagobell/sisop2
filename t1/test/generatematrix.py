col = 30
lin = 30
import random



def generate(filename):
	f = open(filename, 'w')
	f.write("LINHAS = ")
	f.write(str(lin))
	f.write("\nCOLUNAS = ")
	f.write(str(col)+'\n')

	for l in range(lin):
    		f.write(" ".join([str(random.choice(range(1,1000))) for x in range(col)])+'\n')
	f.close	()



generate("m1.txt")
generate("m2.txt")

