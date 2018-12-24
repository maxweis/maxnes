#pragma once
#include <stdint.h>
#include "memory.h"
#include "rom.h"

typedef enum INST_OP {
        NOP,           ADC_OP,        AND_OP,        ASL_OP,
        BCC_OP,        BCS_OP,        BEQ_OP,        BIT_OP,
        BMI_OP,        BNE_OP,        BPL_OP,        BRK_OP,
        BVC_OP,        BVS_OP,        CLC_OP,        CLD_OP,
        CLI_OP,        CLV_OP,        CMP_OP,        CPX_OP,
        CPY_OP,        DEC_OP,        DEX_OP,        DEY_OP,
        EOR_OP,        INC_OP,        INX_OP,        INY_OP,
        JMP_OP,        JSR_OP,        LDA_OP,        LDX_OP,
        LDY_OP,        LSR_OP,        ORA_OP,        PHA_OP,
        PHP_OP,        PLA_OP,        PLP_OP,        ROL_OP,
        ROR_OP,        RTI_OP,        RTS_OP,        SBC_OP,
        SEC_OP,        SED_OP,        SEI_OP,        STA_OP,
        STX_OP,        STY_OP,        TAX_OP,        TAY_OP,
        TSX_OP,        TXA_OP,        TXS_OP,        TYA_OP
} INST_OP;

typedef struct Inst {
        uint8_t *body;                          // operands of instruction
        unsigned size_bytes;                    // size of instruction, including opcode and operands
        unsigned cycles;                        // cycles required to execute instruction
        unsigned page_cross_cycles;             // additional cycles if page crossed
        unsigned branch_succeeds_cycles;        // additional cycles if branch successful
        ADDR_MODE addr_mode;
        INST_OP inst_type;
} Inst;

void classify_inst(uint8_t opcode, Inst *inst);
void parse_insts(Rom *rom);
