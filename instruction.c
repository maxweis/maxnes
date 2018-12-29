#include "instruction.h"
#include <stdlib.h>

void parse_insts(ROM *rom) {
     rom->prg_inst = (Inst*) calloc(rom->prg_len, sizeof(Inst)); // liberal estimate that each instruction is single byte

    unsigned inst_amount = 0;

    unsigned byte = 0;
    while (byte < rom->prg_len) {
        Inst *current = &rom->prg_inst[inst_amount];
        classify_inst(rom->prg[byte], current);
        current->body = (uint8_t*) calloc(current->size_bytes - 1, sizeof(uint8_t)); // create array to store body
        for (unsigned i = 0 ; i < current->size_bytes - 1; i++) {
            current->body[i] = rom->prg[++byte]; // add adjacent bytes to opcode to body of instruction
        }
        byte++;
        inst_amount++;
    }

    rom->inst_amount = inst_amount;
    rom->prg_inst = (Inst*) realloc(rom->prg_inst, inst_amount * sizeof(Inst)); // reallocate array to minimum necessary size
}

void exec_adc_op(CPU *cpu, Inst *inst) {
    uint8_t addend;
    switch (inst->addr_mode) {
        /*case IMMEDIATE:*/
            /*addend = inst->body[0];*/
            /*break;*/
        /*case ZERO_PAGE:*/

    }
}

void exec_inst(CPU *cpu, Inst *inst) {
    switch(inst->inst_type) {
        case ADC_OP:
            break;
        case AND_OP:
            break;
        case ASL_OP:
            break;
        case BCC_OP:
            break;
        case BCS_OP:
            break;
        case BEQ_OP:
            break;
        case BIT_OP:
            break;
        case BMI_OP:
            break;
        case BNE_OP:
            break;
        case BPL_OP:
            break;
        case BRK_OP:
            break;
        case BVC_OP:
            break;
        case BVS_OP:
            break;
        case CLC_OP:
            break;
        case CLD_OP:
            break;
        case CLI_OP:
            break;
        case CLV_OP:
            break;
        case CMP_OP:
            break;
        case CPX_OP:
            break;
        case CPY_OP:
            break;
        case DEC_OP:
            break;
        case DEX_OP:
            break;
        case DEY_OP:
            break;
        case EOR_OP:
            break;
        case INC_OP:
            break;
        case INX_OP:
            break;
        case INY_OP:
            break;
        case JMP_OP:
            break;
        case JSR_OP:
            break;
        case LDA_OP:
            break;
        case LDX_OP:
            break;
        case LDY_OP:
            break;
        case LSR_OP:
            break;
        case ORA_OP:
            break;
        case PHA_OP:
            break;
        case PHP_OP:
            break;
        case PLA_OP:
            break;
        case PLP_OP:
            break;
        case ROL_OP:
            break;
        case ROR_OP:
            break;
        case RTI_OP:
            break;
        case RTS_OP:
            break;
        case SBC_OP:
            break;
        case SEC_OP:
            break;
        case SED_OP:
            break;
        case SEI_OP:
            break;
        case STA_OP:
            break;
        case STX_OP:
            break;
        case STY_OP:
            break;
        case TAX_OP:
            break;
        case TAY_OP:
            break;
        case TSX_OP:
            break;
        case TXA_OP:
            break;
        case TXS_OP:
            break;
        case TYA_OP:
            break;
        case NOP:
        default:
            break;
    }
}

void classify_inst(uint8_t opcode, Inst *inst) {
        switch(opcode) {
            case 0x69:
                inst->addr_mode = IMMEDIATE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->inst_type = ADC_OP;
                break;
            case 0x65:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = ADC_OP;
                break;
            case 0x75:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 4;
                inst->inst_type = ADC_OP;
                break;
            case 0x6d:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = ADC_OP;
                break;
            case 0x7d:
                inst->addr_mode = ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = ADC_OP;
                break;
            case 0x79:
                inst->addr_mode = ABSOLUTE_Y;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = ADC_OP;
                break;
            case 0x61:
                inst->addr_mode = INDIRECT_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = ADC_OP;
                break;
            case 0x71:
                inst->addr_mode = INDIRECT_Y;
                inst->size_bytes = 2;
                inst->cycles = 5;
                inst->page_cross_cycles = 1;
                inst->inst_type = ADC_OP;
                break;

            case 0x29:
                inst->addr_mode = IMMEDIATE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->inst_type = AND_OP;
                break;
            case 0x25:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = AND_OP;
                break;
            case 0x35:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 4;
                inst->inst_type = AND_OP;
                break;
            case 0x2d:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = AND_OP;
                break;
            case 0x3d:
                inst->addr_mode = ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = AND_OP;
                break;
            case 0x39:
                inst->addr_mode = ABSOLUTE_Y;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = AND_OP;
                break;
            case 0x21:
                inst->addr_mode = INDIRECT_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = AND_OP;
                break;
            case 0x31:
                inst->addr_mode = INDIRECT_Y;
                inst->size_bytes = 2;
                inst->cycles = 5;
                inst->page_cross_cycles = 1;
                inst->inst_type = AND_OP;
                break;

            case 0x0a:
                inst->addr_mode = ACCUMULATOR;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = ASL_OP;
                break;
            case 0x06:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 5;
                inst->inst_type = ASL_OP;
                break;
            case 0x16:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = ASL_OP;
                break;
            case 0x0e:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 6;
                inst->inst_type = ASL_OP;
                break;
            case 0x1e:
                inst->addr_mode = ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 7;
                inst->inst_type = ASL_OP;
                break;

            case 0x90:
                inst->addr_mode = RELATIVE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->page_cross_cycles = 2;
                inst->branch_succeeds_cycles = 1;
                inst->inst_type = BCC_OP;
                break;

            case 0xb0:
                inst->addr_mode = RELATIVE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->page_cross_cycles = 2;
                inst->branch_succeeds_cycles = 1;
                inst->inst_type = BCS_OP;
                break;

            case 0xf0:
                inst->addr_mode = RELATIVE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->page_cross_cycles = 2;
                inst->branch_succeeds_cycles = 1;
                inst->inst_type = BEQ_OP;
                break;

            case 0x24:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = BIT_OP;
                break;
            case 0x2c:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = BIT_OP;
                break;

            case 0x30:
                inst->addr_mode = RELATIVE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->page_cross_cycles = 2;
                inst->branch_succeeds_cycles = 1;
                inst->inst_type = BMI_OP;
                break;

            case 0xd0:
                inst->addr_mode = RELATIVE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->page_cross_cycles = 2;
                inst->branch_succeeds_cycles = 1;
                inst->inst_type = BNE_OP;
                break;

            case 0x10:
                inst->addr_mode = RELATIVE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->page_cross_cycles = 2;
                inst->branch_succeeds_cycles = 1;
                inst->inst_type = BPL_OP;
                break;

            case 0x00:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 7;
                inst->inst_type = BRK_OP;
                break;

            case 0x50:
                inst->addr_mode = RELATIVE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->page_cross_cycles = 2;
                inst->branch_succeeds_cycles = 1;
                inst->inst_type = BVC_OP;
                break;

            case 0x70:
                inst->addr_mode = RELATIVE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->page_cross_cycles = 2;
                inst->branch_succeeds_cycles = 1;
                inst->inst_type = BVS_OP;
                break;

            case 0x18:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = CLC_OP;
                break;

            case 0xd8:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = CLD_OP;
                break;

            case 0x58:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = CLI_OP;
                break;

            case 0xb8:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = CLV_OP;
                break;

            case 0xc9:
                inst->addr_mode = IMMEDIATE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->inst_type = CMP_OP;
                break;
            case 0xc5:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = CMP_OP;
                break;
            case 0xd5:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 4;
                inst->inst_type = CMP_OP;
                break;
            case 0xcd:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = CMP_OP;
                break;
            case 0xdd:
                inst->addr_mode = ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = CMP_OP;
                break;
            case 0xd9:
                inst->addr_mode = ABSOLUTE_Y;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = CMP_OP;
                break;
            case 0xc1:
                inst->addr_mode = INDIRECT_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = CMP_OP;
                break;
            case 0xd1:
                inst->addr_mode = INDIRECT_Y;
                inst->size_bytes = 2;
                inst->cycles = 5;
                inst->page_cross_cycles = 1;
                inst->inst_type = CMP_OP;
                break;

            case 0xe0:
                inst->addr_mode = IMMEDIATE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->inst_type = CPX_OP;
                break;
            case 0xe4:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = CPX_OP;
                break;
            case 0xec:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = CPX_OP;
                break;

            case 0xc0:
                inst->addr_mode = IMMEDIATE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->inst_type = CPY_OP;
                break;
            case 0xc4:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = CPY_OP;
                break;
            case 0xcc:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = CPY_OP;
                break;

            case 0xc6:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 5;
                inst->inst_type = DEC_OP;
                break;
            case 0xd6:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = DEC_OP;
                break;
            case 0xce:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 6;
                inst->inst_type = DEC_OP;
                break;
            case 0xde:
                inst->addr_mode = ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 7;
                inst->inst_type = DEC_OP;
                break;

            case 0xca:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = DEX_OP;
                break;

            case 0x88:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = DEY_OP;
                break;

            case 0x49:
                inst->addr_mode = IMMEDIATE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->inst_type = EOR_OP;
                break;
            case 0x45:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = EOR_OP;
                break;
            case 0x55:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 4;
                inst->inst_type = EOR_OP;
                break;
            case 0x4d:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = EOR_OP;
                break;
            case 0x5d:
                inst->addr_mode = ABSOLUTE_X; 
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = EOR_OP;
                break;
            case 0x59:
                inst->addr_mode = ABSOLUTE_Y;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = EOR_OP;
                break;
            case 0x41:
                inst->addr_mode = INDIRECT_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = EOR_OP;
                break;
            case 0x51:
                inst->addr_mode = INDIRECT_Y;
                inst->size_bytes = 2;
                inst->cycles = 5;
                inst->page_cross_cycles = 1;
                inst->inst_type = EOR_OP;
                break;

            case 0xe6:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 5;
                inst->inst_type = INC_OP;
                break;
            case 0xf6:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = INC_OP;
                break;
            case 0xee:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 6;
                inst->inst_type = INC_OP;
                break;
            case 0xfe:
                inst->addr_mode = ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 7;
                inst->inst_type = INC_OP;
                break;

            case 0xe8:
                inst->addr_mode = IMPLIED; 
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = INX_OP;
                break;

            case 0xc8:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = INY_OP;
                break;

            case 0x4c:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 3;
                inst->inst_type = JMP_OP;
                break;
            case 0x6c:
                inst->addr_mode = INDIRECT;
                inst->size_bytes = 3;
                inst->cycles = 5;
                inst->inst_type = JMP_OP;
                break;

            case 0x20:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 6;
                inst->inst_type = JSR_OP;
                break;

            case 0xa9:
                inst->addr_mode = IMMEDIATE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->inst_type = LDA_OP;
                break;
            case 0xa5:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = LDA_OP;
                break;
            case 0xb5:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 4;
                inst->inst_type = LDA_OP;
                break;
            case 0xad:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = LDA_OP;
                break;
            case 0xbd:
                inst->addr_mode = ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = LDA_OP;
                break;
            case 0xb9:
                inst->addr_mode = ABSOLUTE_Y;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = LDA_OP;
                break;
            case 0xa1:
                inst->addr_mode = INDIRECT_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = LDA_OP;
                break;
            case 0xb1:
                inst->addr_mode = INDIRECT_Y;
                inst->size_bytes = 2;
                inst->cycles = 5;
                inst->page_cross_cycles = 1;
                inst->inst_type = LDA_OP;
                break;

            case 0xa2:
                inst->addr_mode = IMMEDIATE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->inst_type = LDX_OP;
                break;
            case 0xa6:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = LDX_OP;
                break;
            case 0xb6:
                inst->addr_mode = ZERO_PAGE_Y;
                inst->size_bytes = 2;
                inst->cycles = 4;
                inst->inst_type = LDX_OP;
                break;
            case 0xae:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = LDX_OP;
                break;
            case 0xbe:
                inst->addr_mode = ABSOLUTE_Y;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = LDX_OP;
                break;

            case 0xa0:
                inst->addr_mode = IMMEDIATE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->inst_type = LDY_OP;
                break;
            case 0xa4:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = LDY_OP;
                break;
            case 0xb4:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 4;
                inst->inst_type = LDY_OP;
                break;
            case 0xac:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = LDY_OP;
                break;
            case 0xbc:
                inst->addr_mode = ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = LDY_OP;
                break;

            case 0x4a:
                inst->addr_mode = ACCUMULATOR;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = LSR_OP;
                break;
            case 0x46:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 5;
                inst->inst_type = LSR_OP;
                break;
            case 0x56:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = LSR_OP;
                break;
            case 0x4e:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 6;
                inst->inst_type = LSR_OP;
                break;
            case 0x5e: 
                inst->addr_mode = ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 7;
                inst->inst_type = LSR_OP;
                break;

            default:
            case 0xea:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = NOP;
                break;

            case 0x09:
                inst->addr_mode = IMMEDIATE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->inst_type = ORA_OP;
                break;
            case 0x05:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = ORA_OP;
                break;
            case 0x15:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 4;
                inst->inst_type = ORA_OP;
                break;
            case 0x0d:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = ORA_OP;
                break;
            case 0x1d:
                inst->addr_mode = ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = ORA_OP;
                break;
            case 0x19:
                inst->addr_mode = ABSOLUTE_Y;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = ORA_OP;
                break;
            case 0x01:
                inst->addr_mode = INDIRECT_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = ORA_OP;
                break;
            case 0x11:
                inst->addr_mode = INDIRECT_Y;
                inst->size_bytes = 2;
                inst->cycles = 5;
                inst->page_cross_cycles = 1;
                inst->inst_type = ORA_OP;
                break;

            case 0x48:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 3;
                inst->inst_type = PHA_OP;
                break;

            case 0x08:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 3;
                inst->inst_type = PHP_OP;
                break;

            case 0x68:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 4;
                inst->inst_type = PLA_OP;
                break;

            case 0x28:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 4;
                inst->inst_type = PLP_OP;
                break;

            case 0x2a:
                inst->addr_mode = ACCUMULATOR;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = ROL_OP;
                break;
            case 0x26:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 5;
                inst->inst_type = ROL_OP;
                break;
            case 0x36:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = ROL_OP;
                break;
            case 0x2e:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 6;
                inst->inst_type = ROL_OP;
                break;
            case 0x3e:
                inst->addr_mode = ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 7;
                inst->inst_type = ROL_OP;
                break;

            case 0x6a:
                inst->addr_mode = ACCUMULATOR;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = ROR_OP;
                break;
            case 0x66:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 5;
                inst->inst_type = ROR_OP;
                break;
            case 0x76:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = ROR_OP;
                break;
            case 0x6e:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 6;
                inst->inst_type = ROR_OP;
                break;
            case 0x7e:
                inst->addr_mode = ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 7;
                inst->inst_type = ROR_OP;
                break;

            case 0x40:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 6;
                inst->inst_type = RTI_OP;
                break;

            case 0x60:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 6;
                inst->inst_type = RTS_OP;
                break;

            case 0xe9:
                inst->addr_mode = IMMEDIATE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->inst_type = SBC_OP;
                break;
            case 0xe5:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = SBC_OP;
                break;
            case 0xf5:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 4;
                inst->inst_type = SBC_OP;
                break;
            case 0xed:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = SBC_OP;
                break;
            case 0xfd:
                inst->addr_mode = ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = SBC_OP;
                break;
            case 0xf9:
                inst->addr_mode = ABSOLUTE_Y;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->page_cross_cycles = 1;
                inst->inst_type = SBC_OP;
                break;
            case 0xe1:
                inst->addr_mode = INDIRECT_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = SBC_OP;
                break;
            case 0xf1:
                inst->addr_mode = INDIRECT_Y;
                inst->size_bytes = 2;
                inst->cycles = 5;
                inst->page_cross_cycles = 1;
                inst->inst_type = SBC_OP;
                break;

            case 0x38:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = SEC_OP;
                break;

            case 0xf8:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = SED_OP;
                break;

            case 0x78:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = SEI_OP;
                break;

            case 0x85:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = STA_OP;
                break;
            case 0x95:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 4;
                inst->inst_type = STA_OP;
                break;
            case 0x8d:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = STA_OP;
                break;
            case 0x9d:
                inst->addr_mode =  ABSOLUTE_X;
                inst->size_bytes = 3;
                inst->cycles = 5;
                inst->inst_type = STA_OP;
                break;
            case 0x99:
                inst->addr_mode = ABSOLUTE_Y;
                inst->size_bytes = 3;
                inst->cycles = 5;
                inst->inst_type = STA_OP;
                break;
            case 0x81:
                inst->addr_mode = INDIRECT_X;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = STA_OP;
                break;
            case 0x91:
                inst->addr_mode = INDIRECT_Y;
                inst->size_bytes = 2;
                inst->cycles = 6;
                inst->inst_type = STA_OP;
                break;

            case 0x86:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = STX_OP;
                break;
            case 0x96:
                inst->addr_mode = ZERO_PAGE_Y;
                inst->size_bytes = 2;
                inst->cycles = 4;
                inst->inst_type = STX_OP;
                break;
            case 0x8e:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = STX_OP;
                break;
                
            case 0x84:
                inst->addr_mode = ZERO_PAGE;
                inst->size_bytes = 2;
                inst->cycles = 3;
                inst->inst_type = STY_OP;
                break;
            case 0x94:
                inst->addr_mode = ZERO_PAGE_X;
                inst->size_bytes = 2;
                inst->cycles = 4;
                inst->inst_type = STY_OP;
                break;
            case 0x8c:
                inst->addr_mode = ABSOLUTE;
                inst->size_bytes = 3;
                inst->cycles = 4;
                inst->inst_type = STY_OP;
                break;

            case 0xaa:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = TAX_OP;
                break;

            case 0xa8:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = TAY_OP;
                break;

            case 0xba:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = TSX_OP;
                break;

            case 0x8a:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = TXA_OP;
                break;

            case 0x9a:
                inst->addr_mode =  IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = TXS_OP;
                break;

            case 0x98:
                inst->addr_mode = IMPLIED;
                inst->size_bytes = 1;
                inst->cycles = 2;
                inst->inst_type = TYA_OP;
                break;
        }
}
