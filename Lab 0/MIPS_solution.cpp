#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define M 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the 
// memory is still 32-bit addressable.
#define N 1000 // dumpout dmem size

class RF
{
    public:
        bitset<32> ReadData1, ReadData2; 
     	RF()
    	{ 
          Registers.resize(32);  
          Registers[0] = bitset<32> (0);  
        }
	
        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {   
           if(WrtEnable[0])
           {   
               Registers[WrtReg.to_ulong()] = WrtData;                    
           }
            else {
               ReadData1 = Registers[RdReg1.to_ulong()];
               ReadData2 = Registers[RdReg2.to_ulong()];
                }                  
         }
	void OutputFile()
          {
               ofstream rfout;
                  rfout.open("RFresult.txt",std::ios_base::app);
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
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

class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
             {   
                 unsigned int result; 
                 switch(ALUOP.to_ulong()) 
                     {
                                          
                     case ADDU : result = oprand1.to_ulong() + oprand2.to_ulong(); break; 
                     case SUBU : result = oprand1.to_ulong() - oprand2.to_ulong(); break;
                     case AND : result = oprand1.to_ulong() & oprand2.to_ulong();  break;
                     case OR  : result = oprand1.to_ulong() | oprand2.to_ulong();  break;
                     case NOR : result = ~(oprand1.to_ulong() | oprand2.to_ulong()); break;
                      } 
                      
                 return ALUresult = bitset<32>(result);
               }            
};

class INSMem
{
      public:
          bitset<32> Instruction;
          INSMem()
          {       IMem.resize(M); 
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
                
          
          bitset<32> ReadMemory (bitset<32> ReadAddress) 
          {    
               string s1;
               s1 = (IMem[ReadAddress.to_ulong()]).to_string<char,std::string::traits_type,std::string::allocator_type>()               
               + IMem[ReadAddress.to_ulong()+1].to_string<char,std::string::traits_type,std::string::allocator_type>()
               + IMem[ReadAddress.to_ulong()+2].to_string<char,std::string::traits_type,std::string::allocator_type>()
               + IMem[ReadAddress.to_ulong()+3].to_string<char,std::string::traits_type,std::string::allocator_type>();
               Instruction = bitset<32> (s1);
               return Instruction;     
                     }     
      
      private:
           vector<bitset<8> > IMem;
      
};
      
class DataMem    
{
      public:
          bitset<32> readdata;  
          DataMem()
          {
             DMem.resize(M); 
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
          bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
          {    
               
               if (readmem[0]&writemem[0])
                   cout<<"Data Memory can't read and write together"<<endl;
               if (readmem[0])
               {
                string s2;
                s2 = (DMem[Address.to_ulong()]).to_string<char,std::string::traits_type,std::string::allocator_type>()               
               + DMem[Address.to_ulong()+1].to_string<char,std::string::traits_type,std::string::allocator_type>()
               + DMem[Address.to_ulong()+2].to_string<char,std::string::traits_type,std::string::allocator_type>()
               + DMem[Address.to_ulong()+3].to_string<char,std::string::traits_type,std::string::allocator_type>();
             
                readdata = bitset<32>(s2);
              
                }
               else if(writemem[0]){
                string s3 = WriteData.to_string<char,std::string::traits_type,std::string::allocator_type>();    
                
                DMem[Address.to_ulong()] = bitset<8>(s3.substr(0,8));
                DMem[Address.to_ulong()+1] = bitset<8>(s3.substr(8,8));
                DMem[Address.to_ulong()+2] = bitset<8>(s3.substr(16,8));
                DMem[Address.to_ulong()+3] = bitset<8>(s3.substr(24,8));
              
                
               }
               return readdata;     
                     }   
                     
          void OutputFile()
          {
               ofstream dmemout;
                  dmemout.open("dmemresult.txt");
                  if (dmemout.is_open())
                  {
                  for (int j = 0; j<N; j++)
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


   
unsigned long shiftbits(bitset<32> inst, int start)
{
    unsigned long ulonginst;
    return ((inst.to_ulong())>>start);
    
}



bitset<32> signextend (bitset<16> imm)
{
    string sestring;
    if (imm[15]==0){
        sestring = "0000000000000000"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>();    
    }
    else{
        sestring = "1111111111111111"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>();
    }
    return (bitset<32> (sestring));

}
int main()
{
    
    // instruction
    bitset<32> pc=0;
    bitset<32> instruction;
    bitset<6> opcode;
    bitset<6> funct;
    bitset<16> imm;

    //control signals
    bitset<1> IType;
    bitset<1> JType;
    bitset<1> RType;
    bitset<1> IsBranch;
    bitset<1> IsLoad;
    bitset<1> IsStore;
    bitset<1> WrtEnable;

    // RF signals
    bitset<5> RReg1;
    bitset<5> RReg2;
    bitset<5> WReg;
    bitset<32> WData;
 
    // ALU signals
    bitset<3> ALUop;
    bitset<32> ALUin1;
    bitset<32> ALUin2;
    bitset<32> signext;
    bitset<32> ALUOut;


    // DMEM signals
    bitset<32> DMAddr;
    bitset<32> WriteData;
    bitset<1> ReadMem;
    bitset<1> WriteMem;

    
    // pc signals
    bitset<32> pcplusfour;
    bitset<32> jaddr;
    bitset<32> braddr;
    bitset<1> IsEq;


    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

    while (1){
        // Fetch and Decode
        
        instruction = myInsMem.ReadMemory(pc);
        if (instruction.to_string<char,std::string::traits_type,std::string::allocator_type>()=="11111111111111111111111111111111")
        {   
           
         //    myRF.OutputFile();
            break;
        }
    //    cout << "instruction number " << i+1 << endl;
        opcode = bitset<6> (shiftbits(instruction, 26));
        RType = (opcode.to_ulong()==0)?1:0;
        IType = (opcode.to_ulong()!=0 && opcode.to_ulong()!=2)?1:0;
        JType = (opcode.to_ulong()==2)?1:0;
        IsBranch = (opcode.to_ulong()==4)?1:0;
        IsLoad = (opcode.to_ulong()==35)?1:0;
        IsStore = (opcode.to_ulong()==43)?1:0;
        WrtEnable = (IsStore.to_ulong() || IsBranch.to_ulong() || JType.to_ulong())?0:1;


        funct = bitset<6> (shiftbits(instruction, 0));
        RReg1 = bitset<5> (shiftbits(instruction, 21));
        RReg2 = bitset<5> (shiftbits(instruction, 16));
        WReg =  (IType.to_ulong())? RReg2 : bitset<5> (shiftbits(instruction, 11));
        ALUop = (opcode.to_ulong()==35 || opcode.to_ulong()==43)?(bitset<3>(string("001"))):bitset<3> (shiftbits(instruction, 0));
        imm = bitset<16> (shiftbits(instruction, 0)); 
        signext = signextend (imm);
        

        myRF.ReadWrite(RReg1,RReg2,WReg,0,0);

        IsEq = (myRF.ReadData1.to_ulong()==myRF.ReadData2.to_ulong())?1:0;
         

        // Execute
        ALUin1 = myRF.ReadData1;
        ALUin2 = (IType.to_ulong())?signext:myRF.ReadData2; 
        ALUOut = myALU.ALUOperation(ALUop,ALUin1,ALUin2);

        // Memory
        DMAddr = ALUOut;
        WriteData = myRF.ReadData2;
        ReadMem = (IsLoad.to_ulong())?1:0;
        WriteMem = (IsStore.to_ulong())?1:0;
        
        myDataMem.MemoryAccess(DMAddr,WriteData,ReadMem,WriteMem);


        // Write Back
        WData = (IsLoad.to_ulong())?myDataMem.readdata:ALUOut;

        myRF.ReadWrite(RReg1,RReg2,WReg,WData,WrtEnable);


        // pc
        pcplusfour = bitset<32> (pc.to_ulong()+4);

        jaddr = bitset<32>(
                (bitset<4> (shiftbits(pcplusfour,28))).to_string<char,std::string::traits_type,std::string::allocator_type>()
                + (bitset<26> (shiftbits(instruction,0))).to_string<char,std::string::traits_type,std::string::allocator_type>()
                + "00");

        braddr = bitset<32>(
            pc.to_ulong() + 4
            + (bitset<32>((bitset<30> (shiftbits(signext,0))).to_string<char,std::string::traits_type,std::string::allocator_type>()+"00")).to_ulong() );

        
        pc=(IsBranch.to_ulong() && IsEq.to_ulong())?braddr:((JType.to_ulong())?jaddr:pcplusfour);
        //
        if(IsBranch.to_ulong() && IsEq.to_ulong())
        cout<< "branch is coming: "<<braddr<<endl;
        //
        cout << "current pc=" << pc <<endl<<endl;
        getchar();
        myRF.OutputFile();
        
        
    }
        myDataMem.OutputFile();
       // system("pause");// remove this line
        return 0;
        
}
