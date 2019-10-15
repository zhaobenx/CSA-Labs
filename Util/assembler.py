# -*- coding: utf-8 -*-
"""
Created on 2019-10-14 18:41:41
@Author: ZHAO Lingfeng
@Version : 0.0.1
"""
import re

FUNCT = {"addu": 0x21, "subu": 0x23}
OPCODE = {"lw": 0x23, "sw": 0x2B, "beq": 0x04, "halt": 0xFF}


def asm_to_int(asm: str):
    if asm.lower() == "halt":
        return 0xFFFFFFFF
    R_patterns = re.findall(r"(\w+)\s+r(\d+)[\s,]+r(\d+)[\s,]+r(\d+)", asm, re.I)
    I_patterns = re.findall(r"(\w+)\s+r(\d+)[\s,]+r(\d+)[\s,]+(\d+)", asm, re.I)
    I_patterns += re.findall(r"(\w+)\s+r(\d+)[\s,]+(\d+)\(r(\d+)\)", asm, re.I)  # lw sw
    J_patterns = re.findall(r"(\w+)\s+(\d+)", asm, re.I)
    # print(R_patterns)
    # print(I_patterns)
    # print(J_patterns)
    if R_patterns:
        # r type:
        opcode = 0x00
        if R_patterns[0][0].lower() in FUNCT:
            funct = FUNCT[R_patterns[0][0].lower()]
            rd, rs, rt = map(int, R_patterns[0][1:])
            return (
                ((opcode & 0x3F) << 26)
                + ((rs & 0x1F) << 21)
                + ((rt & 0x1F) << 16)
                + ((rd & 0x1F) << 11)
                + (funct & 0x3F)
            )
    if I_patterns:
        if I_patterns[0][0].lower() in OPCODE:
            opcode = OPCODE[I_patterns[0][0].lower()]
            if I_patterns[0][0].lower() == "sw" or I_patterns[0][0].lower() == "lw":
                rt, imm, rs = map(int, I_patterns[0][1:])
            else:
                rs, rt, imm = map(int, I_patterns[0][1:])
            # print(rs, rt, imm)
            return (
                ((opcode & 0x3F) << 26)
                + ((rs & 0x1F) << 21)
                + ((rt & 0x1F) << 16)
                + (imm & 0xFFFF)
            )
    if J_patterns:
        if J_patterns[0][0].lower() == "j":
            opcode = 0x02
            address = J_patterns[0][1]
            return ((opcode & 0x3F) << 26) + (address & 0x3FFFFFF)


def int_to_binary_string(asmi: int):
    if not asmi:
        return ''
    return f"{(asmi>>24)&0xff:0>8b}\n{(asmi>>16)&0xff:0>8b}\n{(asmi>>8)&0xff:0>8b}\n{(asmi)&0xff:0>8b}"

def asms_to_binary_string(asms:str):
    res = []
    for i in asms.splitlines():
        res.append(int_to_binary_string(asm_to_int(i)))
    return '\n'.join(res)


def test():
    # print(re.match(r"\w+", "add", re.I))
    # print(re.match(r"(\w+)\s+(r\d+)[\s,]+(r\d+)[\s,]+(r\d+)", "add r3, r5, r2", re.I))
    # print(re.match(r"(\w+)\s+(r\d+)[\s,]+(r\d+)[\s,]+(r\d+)", "lw r1, r2, imm", re.I))
    test = []
    # test.append("lw r1, r2, 333")
    # test.append("add r3, r2, 32")
    # test.append("  addi  r3  ,   r2 , 32")
    # test.append("j 324")
    test.append("addu r3, r1, r2")
    test.append("addu r3, r0, r31")
    test.append("subu r21, r31, r0")
    test.append("subu r3, r1, r2")
    test.append("lw r1,0(r0)")
    test.append("lw r2,4(r0)")
    test.append("Sw r3,8(r0)")
    test.append("Beq r1,r2,1")
    for i in test:
        print(int_to_binary_string(asm_to_int(i)))
        print()
    # print(int_to_binary_string(123+(123<<8)+(123<<16)+(123<<24)))
    # print(int_to_binary_string(2234401))


def main():
    # test()
    a = """
addu r3, r2, r1
subu r5, r2, r0
lw r1, 4(r1)
beq r1,r2, 1
halt"""
    print(asms_to_binary_string(a))


if __name__ == "__main__":
    main()
