data = {}
filename = "times.output"
f = open(filename, 'r')

for line in f:
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
    if current_test_case not in data.keys():
      data[current_exec][current_test_case] = {}
    if current_proc_num not in data[current_exec][current_test_case].keys():
      data[current_exec][current_test_case][current_proc_num] = []
    data[current_exec][current_test_case][current_proc_num].append(line)
    print 'appending line'+ line
  else:
    print 'unknown line:'+ line
return
for execName in data.keys():
  print execName+'\n'
  for testCase in data[execName].keys():
    print testCase+'\n'
    for i in range(10):
      for procNum in data[execName][testCase].keys():
	print len(data[execName][testCase][procNum])
	print data[execName][testCase][procNum]
	break;
	print data[execName][testCase][procNum][i]
	
    print '\n'
  
  
   