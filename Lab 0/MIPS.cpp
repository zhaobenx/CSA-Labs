#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR 5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

class RF
{
public:
    bitset<32> ReadData1, ReadData2;
    RF()
    {
        Registers.resize(32);
        Registers[0] = bitset<32>(0);
    }

    void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
    {
        // implement the funciton by you.
        ReadData1 = Registers[RdReg1.to_ulong()];
        ReadData2 = Registers[RdReg2.to_ulong()];
        if (WrtEnable[0])
            Registers[WrtReg.to_ulong()] = WrtData;
    }

    void OutputRF() // write RF results to file
    {
        ofstream rfout;
        rfout.open("RFresult.txt", std::ios_base::app);
        if (rfout.is_open())
        {
            rfout << "A state of RF:" << endl;
            for (int j = 0; j < 32; j++)
            {
                rfout << Registers[j] << endl;
            }
        }
        else
            cout << "Unable to open file";
        rfout.close();
    }

private:
    vector<bitset<32>> Registers;
};

class ALU
{
public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation(bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {
        // implement the ALU operations by you.
        switch (ALUOP.to_ulong())
        {
        case ADDU:
            ALUresult = oprand1.to_ulong() + oprand2.to_ulong();
            break;
        }

        return ALUresult;
    }
};

class INSMem
{
public:
    bitset<32> Instruction;
    INSMem() // read instruction memory
    {
        IMem.resize(MemSize);
        ifstream imem;
        string line;
        int i = 0;
        imem.open("imem.txt");
        if (imem.is_open())
        {
            while (getline(imem, line))
            {
                IMem[i] = bitset<8>(line);
                i++;
            }
        }
        else
            cout << "Unable to open file";
        imem.close();
    }

    bitset<32> ReadMemory(bitset<32> ReadAddress)
    {
        // implement by you. (Read the byte at the ReadAddress and the following three byte).
        Instruction.reset();
        for (auto i = 3; i >= 0; --i)
        {
            auto bit = (bitset<32>(IMem[ReadAddress.to_ulong() + 3 - i].to_ulong())) << i * 8;
            Instruction |= bit; //(bitset<32>(IMem[ReadAddress.to_ulong()].to_ulong())) << i * 8;
        }
        return Instruction;
    }

private:
    vector<bitset<8>> IMem;
};

class DataMem
{
public:
    bitset<32> readdata;
    DataMem() // read data memory
    {
        DMem.resize(MemSize);
        ifstream dmem;
        string line;
        int i = 0;
        dmem.open("dmem.txt");
        if (dmem.is_open())
        {
            while (getline(dmem, line))
            {
                DMem[i] = bitset<8>(line);
                i++;
            }
        }
        else
            cout << "Unable to open file";
        dmem.close();
    }
    bitset<32> MemoryAccess(bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem)
    {

        // implement by you.
        readdata.reset();
        if (readmem.any())
        {
            for (auto i = 3; i >= 0; --i)
            {
                readdata |= bitset<32>(DMem[Address.to_ulong() + 3 - i].to_ulong()) << i * 8;
            }
        }
        else if (writemem.any())
        {
            for (auto i = 0; i < 4; ++i)
            {
                DMem[Address.to_ulong() + i] = (WriteData >> (3 - i) * 8).to_ulong();
            }
        }
        return readdata;
    }

    void OutputDataMem() // write dmem results to file
    {
        ofstream dmemout;
        dmemout.open("dmemresult.txt");
        if (dmemout.is_open())
        {
            for (int j = 0; j < 1000; j++)
            {
                dmemout << DMem[j] << endl;
            }
        }
        else
            cout << "Unable to open file";
        dmemout.close();
    }

private:
    vector<bitset<8>> DMem;
};

int main()
{
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

    bitset<32> PC;
    while (1)
    {
        // Fetch
        auto instruction = myInsMem.ReadMemory(PC);

        // If current insturciton is "11111111111111111111111111111111", then break;
        cout << instruction.to_string() << endl;
        if (instruction.all())
            break;

        // decode(Read RF)
        auto opcode = bitset<6>((instruction >> 26).to_ulong());
        auto rs = bitset<5>((instruction >> 21).to_ulong() & 0x1f);
        auto rt = bitset<5>((instruction >> 16).to_ulong() & 0x1f);
        if (!opcode.any())
        {
            // R-type Instruction
            auto rd = bitset<5>((instruction >> 11).to_ulong() & 0x1f);
            auto shamt = bitset<5>((instruction >> 6).to_ulong() & 0x1f);
            auto funct = bitset<6>((instruction).to_ulong() & 0x3f);
            switch (funct.to_ulong())
            {
            case 0x21: // addu
                cout << "addu r" << rd.to_ulong() << ", r" << rs.to_ulong() << ", r" << rt.to_ulong() << endl;
                myRF.ReadWrite(rs, rt, 0, 0, 0);
                myALU.ALUOperation(ADDU, myRF.ReadData1, myRF.ReadData2);
                myRF.ReadWrite(0, 0, rd, myALU.ALUresult, 1);
                break;
            }
        }
        else
        {
            // I-type Instruction
            auto imm = bitset<16>((instruction).to_ulong() & 0xffff);
            switch (opcode.to_ulong()) // lw
            {
            case 0x09: // addiu
                cout << "addiu r" << rs.to_ulong() << ", r" << rt.to_ulong() << ", " << imm.to_ulong() << endl;
                myRF.ReadWrite(rs, 0, 0, 0, 0);
                cout << imm.to_ulong()<<endl;
                myALU.ALUOperation(ADDU, myRF.ReadData1, imm.to_ulong());
                myRF.ReadWrite(0, 0, rt, myALU.ALUresult, 1);
                break;

            case 0x23: // lw
                // cout << "LW\n";
                cout << "lw r" << rs.to_ulong() << ", r" << rt.to_ulong() << ", " << imm.to_ulong() << endl;
                myRF.ReadWrite(rs, 0, 0, 0, 0);
                myRF.ReadWrite(0, 0, rt, myDataMem.MemoryAccess(myRF.ReadData1.to_ulong() + imm.to_ulong(), 0, 1, 0), 1);
                break;
            case 0x2b: // sw
                // cout << "SW\n";
                cout << "sw r" << rs.to_ulong() << ", r" << rt.to_ulong() << ", " << imm.to_ulong() << endl;
                myRF.ReadWrite(rs, rt, 0, 0, 0); //mem, reg
                myDataMem.MemoryAccess(myRF.ReadData1.to_ulong() + imm.to_ulong(), myRF.ReadData2, 0, 1);

                break;
            }
        }

        // myRF.ReadWrite(instruction);
        // Execute

        // Read/Write Mem

        // Write back to RF

        myRF.OutputRF(); // dump RF;
        PC = PC.to_ulong() + 4;
    }
    myDataMem.OutputDataMem(); // dump data mem

    return 0;
}
