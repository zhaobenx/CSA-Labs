#!/bin/bash
cd "$(dirname "$0")"
SAVEIFS=$IFS
IFS=$(echo -en "\n\b")
########
src_dir="../Lab 0"
test_dir="test0"
temp="temp"
execuable="Lab0"


g++ $src_dir/MIPS.cpp -o $execuable

# cd $temp

# rm -f dmem.txt 
# rm -f dmemresult_answer.txt
# rm -f imem.txt
# rm -f Instructions.txt
# rm -f RFresult_answer.txt

successful=0
failed=0

mkdir $temp
for dir in ./$test_dir/*
do

	for file in $dir/*
	do
		cp $file ./$temp
	done
	cp $execuable ./$temp/$execuable

	cd $temp
	if [ "$1" = "show" ]; then
		./$execuable 
	else
		./$execuable > /dev/null
	fi


	cmp RFresult_answer.txt  RFresult.txt -s
	s1=$?
	cmp dmemresult_answer.txt dmemresult.txt -s
	s2=$?
	if (($s1))|| (($s2));
	then
		tput setaf 1
		((failed++))
	else
		tput setaf 2	
		((successful++))
	fi
	
	echo testing $dir
	tput sgr0

	rm *
	cd ..
	# rm -f dmem.txt 
	# rm -f dmemresult_answer.txt
	# rm -f dmemresult.txt
	# rm -f imem.txt
	# rm -f Instructions.txt
	# rm -f RFresult_answer.txt
	# rm -f RFresult.txt
	
done

rm -r $temp
rm $execuable
echo $successful successful, $failed failed

if [[ $failed -gt 0 ]]; 
then
	exit 1;
else
	exit 0;
fi
