#/usr/bin/bash
declare -a listOfExecs=("fork" "pthreads")
rm times.output
for i in "${listOfExecs[@]}"; do
  cp ../../build/"$i" .
  echo "$i" >> times.output
  for tst in {1..5}; do
    echo "test case $tst"
    echo "test case $tst" >> times.output
    cp ../testcases/$tst/* .
    for p in {1,2,4,8}; do
      echo "$p processes/threads"
      echo "$p processes/threads" >> times.output
      for testrun in {1..10}; do
	{ time ./"$i" $p ; }  2>&1 | grep real >> times.output
	sleep 5
      done
    done
  done
  rm "$i"
done
rm out.txt in1.txt in2.txt
