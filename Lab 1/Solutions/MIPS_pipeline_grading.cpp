#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;  
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;  
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;
    
    string      INS;        //instruction type
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;    
    bool        nop;    
    
    string      INS;
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;  
    
    string      INS;
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF()
		{
			ofstream rfout;
			rfout.open("RFresult_grading.txt", std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
				for (int j = 0; j<32; j++)
				{        
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();               
		} 
			
	private:
		vector<bitset<32> >Registers;	
};

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
            ifstream imem;
			string line;
			int i=0;
			imem.open("imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{      
					IMem[i] = bitset<8>(line);
					i++;
				}                    
			}
            else cout<<"Unable to open file";
			imem.close();                     
		}
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public:
        bitset<32> ReadData;  
        DataMem()
        {
            DMem.resize(MemSize); 
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {      
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult_grading.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {     
                    dmemout << DMem[j]<<endl;
                }
                     
            }
            else cout<<"Unable to open file";
            dmemout.close();               
        }             
      
    private:
		vector<bitset<8> > DMem;      
};  

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult_grading.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        // print IF stage
        if (1 == state.IF.nop)
        {
            printstate<<"IF.PC:\t"<<"X"<<endl; 
        }
        else
        {
            printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        }        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        // print ID stage
        if (1 == state.ID.nop)
        {
        printstate<<"ID.Instr:\t"<<"X"<<endl;         
        }
        else
        {
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl;         
        }
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;

        // print EX stage
        if (1 == state.EX.nop)
        {
            printstate<<"EX.Read_data1:\t"<<"X"<<endl;
            printstate<<"EX.Read_data2:\t"<<"X"<<endl;
            printstate<<"EX.Imm:\t"<<"X"<<endl; 
            printstate<<"EX.Rs:\t"<<"X"<<endl;
            printstate<<"EX.Rt:\t"<<"X"<<endl;
            printstate<<"EX.Wrt_reg_addr:\t"<<"X"<<endl;
            printstate<<"EX.is_I_type:\t"<<"X"<<endl; 
            printstate<<"EX.rd_mem:\t"<<"X"<<endl;
            printstate<<"EX.wrt_mem:\t"<<"X"<<endl;        
            printstate<<"EX.alu_op:\t"<<"X"<<endl;
            printstate<<"EX.wrt_enable:\t"<<"X"<<endl;      
        }
        else
        {
            if (("addu" == state.EX.INS) || ("subu" == state.EX.INS))
            {
                printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
                printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
                printstate<<"EX.Imm:\t"<<"X"<<endl; 
                printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
                printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
                printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
                printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
                printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
                printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
                printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
                printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;            
            }            
            
            if ("lw" == state.EX.INS)
            {
                printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
                printstate<<"EX.Read_data2:\t"<<"X"<<endl;
                printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
                printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
                printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
                printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
                printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
                printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
                printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
                printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
                printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;            
            }
            
            if ("sw" == state.EX.INS)
            {
                printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
                printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
                printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
                printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
                printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
                printstate<<"EX.Wrt_reg_addr:\t"<<"X"<<endl;
                printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
                printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
                printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
                printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
                printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;            
            }
            
            if ("beq" == state.EX.INS)
            {
                printstate<<"EX.Read_data1:\t"<<"X"<<endl;
                printstate<<"EX.Read_data2:\t"<<"X"<<endl;
                printstate<<"EX.Imm:\t"<<"X"<<endl; 
                printstate<<"EX.Rs:\t"<<"X"<<endl;
                printstate<<"EX.Rt:\t"<<"X"<<endl;
                printstate<<"EX.Wrt_reg_addr:\t"<<"X"<<endl;
                printstate<<"EX.is_I_type:\t"<<"X"<<endl; 
                printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
                printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
                printstate<<"EX.alu_op:\t"<<"X"<<endl;
                printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;            
            }
        }                    
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;       

        // print MEM stage
        if (1 == state.MEM.nop)
        {
            printstate<<"MEM.ALUresult:\t"<<"X"<<endl;
            printstate<<"MEM.Store_data:\t"<<"X"<<endl; 
            printstate<<"MEM.Rs:\t"<<"X"<<endl;
            printstate<<"MEM.Rt:\t"<<"X"<<endl;   
            printstate<<"MEM.Wrt_reg_addr:\t"<<"X"<<endl;              
            printstate<<"MEM.rd_mem:\t"<<"X"<<endl;
            printstate<<"MEM.wrt_mem:\t"<<"X"<<endl; 
            printstate<<"MEM.wrt_enable:\t"<<"X"<<endl;        
        }
        else
        {
            if (("addu" == state.MEM.INS) || ("subu" == state.MEM.INS) || ("lw" == state.MEM.INS))
            {
                printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
                printstate<<"MEM.Store_data:\t"<<"X"<<endl; 
                printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
                printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
                printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
                printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
                printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
                printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;            
            }
            
            if ("sw" == state.MEM.INS)
            {
                printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
                printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
                printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
                printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
                printstate<<"MEM.Wrt_reg_addr:\t"<<"X"<<endl;              
                printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
                printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
                printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;            
            }
            
            if ("beq" == state.MEM.INS)
            {
                printstate<<"MEM.ALUresult:\t"<<"X"<<endl;
                printstate<<"MEM.Store_data:\t"<<"X"<<endl; 
                printstate<<"MEM.Rs:\t"<<"X"<<endl;
                printstate<<"MEM.Rt:\t"<<"X"<<endl;   
                printstate<<"MEM.Wrt_reg_addr:\t"<<"X"<<endl;              
                printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
                printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
                printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;            
            }
        }                
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl; 
        
        // print WB stage
        if (1 == state.WB.nop)
        {
            printstate<<"WB.Wrt_data:\t"<<"X"<<endl;
            printstate<<"WB.Rs:\t"<<"X"<<endl;
            printstate<<"WB.Rt:\t"<<"X"<<endl;        
            printstate<<"WB.Wrt_reg_addr:\t"<<"X"<<endl;
            printstate<<"WB.wrt_enable:\t"<<"X"<<endl;          
        }
        else
        {
            if (("addu" == state.WB.INS) || ("subu" == state.WB.INS) || ("lw" == state.WB.INS))
            {
                printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
                printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
                printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
                printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
                printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;              
            }
            
            if ("sw" == state.WB.INS)
            {
                printstate<<"WB.Wrt_data:\t"<<"X"<<endl;
                printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
                printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
                printstate<<"WB.Wrt_reg_addr:\t"<<"X"<<endl;
                printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;              
            }
            
            if ("beq" == state.WB.INS)
            {
                printstate<<"WB.Wrt_data:\t"<<"X"<<endl;
                printstate<<"WB.Rs:\t"<<"X"<<endl;
                printstate<<"WB.Rt:\t"<<"X"<<endl;        
                printstate<<"WB.Wrt_reg_addr:\t"<<"X"<<endl;
                printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;              
            }
        }               
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
      
    }
    else cout<<"Unable to open file";
    printstate.close();
}

bitset<32> sign_extend(bitset<16> Imm)
{
    string se0 = "0000000000000000";
    string se1 = "1111111111111111";
    string immediate = Imm.to_string();
    bitset<32> se_imm;
    
    if (immediate.substr(0,1) == "0")							//sign extention
    {
        se0 += immediate;
        se_imm = bitset<32>(se0);
    }					
    else
    {
        se1.append(immediate);
        se_imm = bitset<32>(se1);				
    }
    return se_imm;
}


int main()
{
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    
    IFStruct IF = {
        0,      //PC          
        0,      //nop
    };
    
    IDStruct ID = {
        0,      //Instr
        1,      //nop
    };
    
    EXStruct EX = {
        0,      //Read_data1
        0,      //Read_data2
        0,      //Imm
        0,      //Rs
        0,      //Rt
        0,      //Wrt_reg_addr
        0,      //is_I_type
        0,      //rd_mem
        0,      //wrt_mem
        1,      //alu_op
        0,      //wrt_enable
        1,      //nop
        
        "X",   //INS
    };
    
    MEMStruct MEM = {
        0,      //ALUresult
        0,      //Store_data
        0,      //Rs
        0,      //Rt
        0,      //Wrt_reg_addr
        0,      //rd_mem
        0,      //wrt_mem
        0,      //wrt_enable        
        1,      //nop
        
        "X",   //INS        
    };

    WBStruct WB = {
        0,      //Wrt_data
        0,      //Rs
        0,      //Rt
        0,      //Wrt_reg_addr
        0,      //wrt_enable
        1,      //nop
        
        "X",   //INS        
    };    
   
    stateStruct state = {
        IF,
        ID,
        EX ,
        MEM,
        WB,
    };
        
    stateStruct newState = state;
        
	bitset<32> Instruction;
	bitset<32> Instr;    
	string opcode;
	string func; 
	bitset<5> Rs;
	bitset<5> Rt;
	bitset<5> Rd; 
    bitset<16> Imm;

    int cycle = 0;  
    
             
    while (1) {

        /* --------------------- WB stage --------------------- */
        if (0 == state.WB.nop)
        {
            if (1 == state.WB.wrt_enable)
            {
                myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
            }         
        }


        /* --------------------- MEM stage --------------------- */
        if (0 == state.MEM.nop)
        {
            if (1 == state.MEM.rd_mem)
            {
                newState.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);
            }
            else if (1 == state.MEM.wrt_mem)
            {
                if ((0 == state.WB.nop) && (1 == state.WB.wrt_enable) && (state.WB.Wrt_reg_addr == state.MEM.Rt))
                {
                    state.MEM.Store_data = state.WB.Wrt_data ;    
                    cout<<"MEM-MEM sw Forwarding"<<endl;
                }
                    
                myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);
                newState.WB.Wrt_data = state.MEM.Store_data;    //will not be used
            }
            else if (1 == state.MEM.wrt_enable)
            {
                //cout<<"addu subu ALUresult:\t"<<state.MEM.ALUresult<<endl;
                newState.WB.Wrt_data = state.MEM.ALUresult;
            }   

            newState.WB.Rs = state.MEM.Rs;
            newState.WB.Rt = state.MEM.Rt;             
            newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;                      
            newState.WB.wrt_enable = state.MEM.wrt_enable;             
        }
        newState.WB.nop = state.MEM.nop;  
        newState.WB.INS = state.MEM.INS;


        /* --------------------- EX stage --------------------- */
        if (0 == state.EX.nop)
        {                       
            if ((0 == state.WB.nop) && (1 == state.WB.wrt_enable) && (state.WB.Wrt_reg_addr == state.EX.Rs))
            {
                state.EX.Read_data1 = state.WB.Wrt_data;
                cout<<"MEM-EX Rs Forwarding"<<endl;
            }
            
            if ((0 == state.WB.nop) && (1 == state.WB.wrt_enable) && (state.WB.Wrt_reg_addr == state.EX.Rt))
            {
                if (((0 == state.EX.is_I_type) && (1 == state.EX.wrt_enable)) || (1 == state.EX.wrt_mem))   //addu, subu, sw
                {
                    state.EX.Read_data2 = state.WB.Wrt_data;
                    cout<<"MEM-EX Rt Forwarding"<<endl;                
                }
            }
            
            if ((0 == state.MEM.nop) && (0 == state.MEM.rd_mem) && (0 == state.MEM.wrt_mem) && (1 == state.MEM.wrt_enable) && (state.MEM.Wrt_reg_addr == state.EX.Rs))
            {               
                state.EX.Read_data1 = state.MEM.ALUresult;
                cout<<"EX-EX Rs Forwarding"<<endl;
            }
            
            if ((0 == state.MEM.nop) && (0 == state.MEM.rd_mem) && (0 == state.MEM.wrt_mem) && (1 == state.MEM.wrt_enable) && (state.MEM.Wrt_reg_addr == state.EX.Rt))
            {
                if ((0 == state.EX.is_I_type) && (1 == state.EX.wrt_enable))   // || (1 == state.EX.wrt_mem))   //addu, subu, for sw, we choose MEM-MEM but EX-EX
                {
                    state.EX.Read_data2 = state.MEM.ALUresult;
                    cout<<"EX-EX Rt Forwarding"<<endl; 
                }
            }            
            
            if (0 == state.EX.is_I_type)
            {
                if (1 == state.EX.wrt_enable)
                {
                    if (state.EX.alu_op == 1)
                    {
                        newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong();
                        //cout<<"addu subu ALUresult:\t"<<newState.MEM.ALUresult<<endl;                
                    }
                    else if (state.EX.alu_op == 0)
                    {
                        newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong();
                        //cout<<"addu subu ALUresult:\t"<<newState.MEM.ALUresult<<endl;                
                    }
                }
                else
                {
                    newState.MEM.ALUresult = 0; //case of branch
                }
            }
            else if (1 == state.EX.is_I_type)
            {
                newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() + sign_extend(state.EX.Imm).to_ulong();
            }
            
            newState.MEM.Store_data = state.EX.Read_data2;
            newState.MEM.Rs = state.EX.Rs;
            newState.MEM.Rt = state.EX.Rt;            
            newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;              
            newState.MEM.wrt_enable = state.EX.wrt_enable;           
            newState.MEM.rd_mem = state.EX.rd_mem;
            newState.MEM.wrt_mem = state.EX.wrt_mem;
            //cout<<"Rs_data:\t"<<state.EX.Read_data1.to_ulong()<<endl;
            //cout<<"Rt_data:\t"<<state.EX.Read_data2.to_ulong()<<endl;
            //cout<<"alu_op:\t"<<state.EX.alu_op<<endl;                            
            //cout<<"addu subu ALUresult:\t"<<newState.MEM.ALUresult<<endl;            
        }
        newState.MEM.nop = state.EX.nop;        
        newState.MEM.INS = state.EX.INS;
          

        /* --------------------- ID stage --------------------- */
        if (0 == state.ID.nop)
        {
            Instr = state.ID.Instr;
            opcode = Instr.to_string().substr(0,6);		//decode instruction
            func = Instr.to_string().substr(26,6);

            Rs = bitset<5>(Instr.to_string().substr(6,5));
            newState.EX.Rs = Rs;
            newState.EX.Read_data1 = myRF.readRF(Rs);   
            
            Rt = bitset<5>(Instr.to_string().substr(11,5));
            newState.EX.Rt = Rt;
            newState.EX.Read_data2 = myRF.readRF(Rt);
            
            Imm = bitset<16>(Instr.to_string().substr(16,16)); 
            newState.EX.Imm = Imm;
            
            Rd = bitset<5>(Instr.to_string().substr(16,5));
        
            //cout<<"Rs:\t"<<Rs<<endl;
            //cout<<"Rt:\t"<<Rt<<endl;   
            
            if (opcode == "000000")								//execution
            {                 
                newState.EX.Wrt_reg_addr = Rd; 
                
                newState.EX.is_I_type = 0;
                
                if (func == "100001")			//addu
                {
                    newState.EX.INS = "addu";                    
                    newState.EX.alu_op = 1;     
                }
                
                else if (func == "100011")		//subu
                {
                    newState.EX.INS = "subu";                    
                    newState.EX.alu_op = 0;      
                }
                
                newState.EX.wrt_enable = 1;
                newState.EX.rd_mem = 0;
                newState.EX.wrt_mem = 0;                 
            }            

            else if (opcode == "100011")	//Load Word R[rt] = M[R[rs]+SignExtImm]
            {
                newState.EX.INS = "lw";                
                
                newState.EX.Wrt_reg_addr = Rt;
                
                newState.EX.is_I_type = 1;               
                newState.EX.alu_op = 1;
                newState.EX.wrt_enable = 1;                
                newState.EX.rd_mem = 1;
                newState.EX.wrt_mem = 0;                      
            }

            else if (opcode == "101011")	//Store Word M[R[rs]+SignExtImm] = R[rt]
            {
                newState.EX.INS = "sw";       
                
                newState.EX.Wrt_reg_addr = Rt;  //will not be used

                newState.EX.is_I_type = 1;                
                newState.EX.alu_op = 1;
                newState.EX.wrt_enable = 0;                
                newState.EX.rd_mem = 0;
                newState.EX.wrt_mem = 1;                 
            }
            
            else if (opcode == "000100")	//Branch On Equal if(R[rs]==R[rt]) PC=PC+4+BrachAddr
            {
                newState.EX.INS = "beq";
                
                newState.EX.Wrt_reg_addr = 0;
                
                newState.EX.is_I_type = 1;
                newState.EX.alu_op = 1;
                newState.EX.wrt_enable = 0;
                newState.EX.rd_mem = 0;
                newState.EX.wrt_mem = 0; 
                
                if (myRF.readRF(Rs) != myRF.readRF(Rt))
                {
                    cout<<"Branch not taken"<<endl;
                    newState.EX.nop = 0;
                    newState.ID.nop = 1;
                    
                    newState.IF.PC = state.IF.PC.to_ulong() + bitset<30>(sign_extend(Imm).to_string().substr(2,30)).to_ulong()*4;
                    newState.IF.nop = 0;
                    
                    printState(newState, cycle);     
                    state = newState;
                    cycle ++;
                    
                    continue;                                       
                } 
                
                cout<<"Branch taken"<<endl;
            }
            
            if ((0 == state.EX.nop) && (1 == state.EX.rd_mem))    //stall, will not consider branch after lw
            {
                if ((state.EX.Wrt_reg_addr == Rs) || ((state.EX.Wrt_reg_addr == Rt) && (0 == newState.EX.is_I_type)))  //stall
                {                   
                    newState.EX.nop = 1;

                    newState.ID = state.ID;
                    newState.IF = state.IF;

                    printState(newState, cycle);
                    state = newState;
                    cycle ++;
                    cout<<"Stall"<<endl;
                    continue;
                }  
            }
        }
        newState.EX.nop = state.ID.nop;

        
        /* --------------------- IF stage --------------------- */
        if (0 == state.IF.nop)
        {
            //cout<<"PC:\t"<<state.IF.PC<<endl;
            Instruction = myInsMem.readInstr(state.IF.PC);            
            //cout<<"Instruction:\t"<<Instruction<<endl;

            if (Instruction != 0xffffffff)
            {
                newState.IF.PC = state.IF.PC.to_ulong() + 4;
                newState.IF.nop = 0;                               
            }
            else
            {
                state.IF.nop = 1;
                
                newState.IF.PC = state.IF.PC.to_ulong();                
                newState.IF.nop = 1;
                //cout<<"PC:\t"<<state.IF.PC<<endl;                
            }
            
            newState.ID.Instr = Instruction;            
        }
        newState.ID.nop = state.IF.nop;

        
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;
        
        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
       
        state = newState; //The end of the cycle and updates the current state with the values calculated in this cycle
                
        cycle ++;	
    }
    
    myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 
	
	return 0;
}