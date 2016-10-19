data = {}
filename = "times.output"
f = open(filename, 'r')

for line in f:
  line = line.replace('\n','').replace('\t',' ')
  if 'fork' in line:
    current_exec = 'fork'
  elif 'pthreads' in line:
    current_exec = 'pthreads'
  elif 'test case' in line:
    current_test_case = line.split(' ')[2]
  elif 'processes/threads' in line:
    current_proc_num=line.split(' ')[0]
  elif 'real' in line:
    if current_exec not in data.keys():
      data[current_exec] = {}
    if current_test_case not in data[current_exec].keys():
      data[current_exec][current_test_case] = {}
    if current_proc_num not in data[current_exec][current_test_case].keys():
      data[current_exec][current_test_case][current_proc_num] = []
    #print data[current_exec][current_test_case][current_proc_num]
    line = line.split(' ')[1].replace('0m','')
    data[current_exec][current_test_case][current_proc_num].append(line)
  else:
    print 'unknown line:'+ line

  
for execName in data.keys():
  print execName+'\n'
  for testCase in data[execName].keys():
    print 'testcase:'+testCase+'\n'
    procNums = data[execName][testCase].keys()
    
    print '   '.join(procNums)  
    for i in range(10):
      out = []
      for procNum in procNums:
	out.append(data[execName][testCase][procNum][i])
	
      print ' '.join(out)
#print data
