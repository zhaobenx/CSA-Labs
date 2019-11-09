target: MIPS_pipeline_base_code_Q1 MIPS_pipeline_base_code_Q2 MIPS_pipeline_base_code_Q3

MIPS_pipeline_base_code_Q1:
	g++ MIPS_pipeline_base_code_Q1.cpp -o MIPS_pipeline_base_code_Q1
MIPS_pipeline_base_code_Q2:
	g++ MIPS_pipeline_base_code_Q2.cpp -o MIPS_pipeline_base_code_Q2
MIPS_pipeline_base_code_Q3:
	g++ MIPS_pipeline_base_code_Q3.cpp -o MIPS_pipeline_base_code_Q3

test: target
	cp MIPS_pipeline_base_code_Q1 ./branch_taken_rate
	cp MIPS_pipeline_base_code_Q2 ./beq_EX
	cp MIPS_pipeline_base_code_Q3 ./jr_ins
	./test.sh


clean:
	rm MIPS_pipeline_base_code_Q1
	rm MIPS_pipeline_base_code_Q2
	rm MIPS_pipeline_base_code_Q3
