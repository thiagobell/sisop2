col = 1000
lin = 1000
import random
f = open("m2.txt",'w')
f.write("LINHAS = ")
f.write(str(lin))
f.write("\nCOLUNAS = ")
f.write(str(col)+'\n')

for l in range(lin):
    f.write(" ".join([str(random.choice(range(1,1000))) for x in range(col)])+'\n')
f.close()
