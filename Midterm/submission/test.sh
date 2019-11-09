#!/bin/bash
cd branch_taken_rate
tput setaf 2
echo "Test on branch_taken_rate"
tput sgr0
./MIPS_pipeline_base_code_Q1
rm dmemresult.txt
rm RFresult.txt
rm stateresult.txt
rm MIPS_pipeline_base_code_Q2
rm MIPS_pipeline_base_code_Q1
cd ..

cd beq_EX
tput setaf 2
echo "===================================================="
echo "Test on beq in EX"
tput sgr0
./MIPS_pipeline_base_code_Q2
diff dmemresult.txt dmemresult_sample.txt
r1=$?
diff RFresult.txt RFresult_sample.txt
r2=$?
diff stateresult.txt stateresult_sample.txt
r3=$?
if (($r1))|| (($r2)) || (($r3));
	then
		tput setaf 1
		echo "Diff different, please check diff result"
	else
		tput setaf 2	
		echo "Passes all case"
	fi
tput sgr0
rm dmemresult.txt
rm RFresult.txt
rm stateresult.txt
rm MIPS_pipeline_base_code_Q2
cd ..

cd jr_ins
tput setaf 2
echo "===================================================="
echo "Test on ir_ins"
tput sgr0
./MIPS_pipeline_base_code_Q3
diff dmemresult.txt dmemresult_sample.txt
r1=$?
diff RFresult.txt RFresult_sample.txt
r2=$?
diff stateresult.txt stateresult_sample.txt
r3=$?
if (($r1))|| (($r2)) || (($r3));
	then
		tput setaf 1
		echo "Diff different, please check diff result"
	else
		tput setaf 2	
		echo "Passes all case"
	fi
tput sgr0
rm dmemresult.txt
rm RFresult.txt
rm stateresult.txt
rm MIPS_pipeline_base_code_Q3
cd ..

