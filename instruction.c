#include "instruction.h"
#include <stdlib.h>

const char* inst_names[] = {
        "NOP",        "ADC",        "AND",        "ASL",
        "BCC",        "BCS",        "BEQ",        "BIT",
        "BMI",        "BNE",        "BPL",        "BRK",
        "BVC",        "BVS",        "CLC",        "CLD",
        "CLI",        "CLV",        "CMP",        "CPX",
        "CPY",        "DEC",        "DEX",        "DEY",
        "EOR",        "INC",        "INX",        "INY",
        "JMP",        "JSR",        "LDA",        "LDX",
        "LDY",        "LSR",        "ORA",        "PHA",
        "PHP",        "PLA",        "PLP",        "ROL",
        "ROR",        "RTI",        "RTS",        "SBC",
        "SEC",        "SED",        "SEI",        "STA",
        "STX",        "STY",        "TAX",        "TAY",
        "TSX",        "TXA",        "TXS",        "TYA"
};

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

void update_inst_operand(NES *nes, Inst *inst) {
    uint16_t addr;
    switch(inst->addr_mode) {
        case IMPLIED:
            // no operand storing necessary
            break;
        case ACCUMULATOR:
            inst->operand_val = nes->cpu->acc_reg;
            break;
        case IMMEDIATE:
            inst->operand_val = (uint16_t) inst->body[0];
            break;
        case ZERO_PAGE:
            inst->operand_val = *access_ram(nes->ram, inst->body[0]);
            inst->operand_mem_addr = inst->body[0];
            break;
        case  ZERO_PAGE_X:
            addr = (inst->body[0] + nes->cpu->x_reg) % ZERO_PAGE_SIZE;
            if (!page_crossed(inst->body[0], addr)) { // page boundary not crossed, no need for extra cycles
                inst->page_cross_cycles = 0;
            }
            inst->operand_val = *access_ram(nes->ram, addr);
            inst->operand_mem_addr = addr;
            break;
        case ZERO_PAGE_Y:
            addr = (inst->body[0] + nes->cpu->y_reg) % ZERO_PAGE_SIZE;
            if (!page_crossed(inst->body[0], addr)) { // page boundary not crossed, no need for extra cycles
                inst->page_cross_cycles = 0;
            }
            inst->operand_val = *access_ram(nes->ram, addr);
            inst->operand_mem_addr = addr;
            break;
        case RELATIVE:
            inst->operand_val = inst->body[0];
            break;
        case ABSOLUTE:
            addr = (inst->body[1] << 8) | inst->body[0];
            inst->operand_val = *access_ram(nes->ram, addr);
            inst->operand_mem_addr = addr;
            break;
        case ABSOLUTE_X:
            addr = ((inst->body[1] << 8) | inst->body[0]) + nes->cpu->x_reg;
            inst->operand_val = *access_ram(nes->ram, addr);
            inst->operand_mem_addr = addr;
            break;
        case ABSOLUTE_Y:
            addr = ((inst->body[1] << 8) | inst->body[0]) + nes->cpu->y_reg;
            inst->operand_val = *access_ram(nes->ram, addr);
            inst->operand_mem_addr = addr;
            break;
        case INDIRECT:
            addr = (inst->body[1] << 8) | inst->body[0];
            inst->operand_val = (*access_ram(nes->ram, addr + 1) << 8) | *access_ram(nes->ram, addr);
            inst->operand_mem_addr = addr;
            break;
        case INDIRECT_X:
            addr = inst->body[0] + nes->cpu->x_reg;
            inst->operand_val = (*access_ram(nes->ram, addr + 1) << 8) | *access_ram(nes->ram, addr);
            inst->operand_mem_addr = addr;
            break;
        case INDIRECT_Y:
            addr = *access_ram(nes->ram, inst->body[0]) + nes->cpu->y_reg;
            if (!page_crossed(inst->body[0], addr)) { // page boundary not crossed, no need for extra cycles
                inst->page_cross_cycles = 0;
            }
            inst->operand_val = *access_ram(nes->ram, addr);
            inst->operand_mem_addr = addr;
        default:
            delete_nes(nes);
            fprintf(stderr, "Instruction uses invalid memory addressing mode\n");
            exit(-1);
            break;
    }
}

// generalized branch instruction
inline void exec_branch(NES *nes, Inst *inst, bool condition) {
    uint16_t old_program_c = nes->cpu->program_c;
    if (condition) { // follow branch
        nes->cpu->program_c += inst->operand_val;
    } else {
        inst->branch_succeeds_cycles = 0;
    }

    if (!page_crossed(old_program_c, nes->cpu->program_c)) {
        inst->page_cross_cycles = 0;
    }
}

// common case of updating zero and negative flags 
inline void update_cpu_status(NES *nes, uint8_t value) {
    set_cpu_status_bit(nes->cpu, ZERO, !value);
    set_cpu_status_bit(nes->cpu, NEGATIVE, get_bit(value, 7));
}

// individual instruction execution functions

void exec_adc_op(NES *nes, Inst *inst) {
    uint8_t sum = nes->cpu->acc_reg + inst->operand_val + get_cpu_status_bit(nes->cpu, CARRY); // signed, higher-precision value to check for overflow, carry
    set_cpu_status_bit(nes->cpu, OVERFLOW, 
            (nes->cpu->acc_reg ^ sum) & (inst->operand_val ^ sum) & 0x80); // overflow formula courtesy of http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
    nes->cpu->acc_reg = sum;
    set_cpu_status_bit(nes->cpu, CARRY, sum > 255);
    set_cpu_status_bit(nes->cpu, ZERO, !sum);
    set_cpu_status_bit(nes->cpu, NEGATIVE, get_bit(sum, 7));
}

void exec_and_op(NES *nes, Inst *inst) {
    nes->cpu->acc_reg &= inst->operand_val;
    update_cpu_status(nes, nes->cpu->acc_reg);
}

void exec_asl_op(NES *nes, Inst *inst) {
    inst->page_cross_cycles = 0; // resolve difference in absolute x addressing mode cycles

    uint8_t operand;
    if (inst->addr_mode == ACCUMULATOR) {
        set_cpu_status_bit(nes->cpu, CARRY, get_bit(nes->cpu->acc_reg, 7)); // most significant bit moved to carry
        operand = (nes->cpu->acc_reg <<= 1);
    } else { // shift memory contents
        set_cpu_status_bit(nes->cpu, CARRY, get_bit(*access_ram(nes->ram, inst->operand_mem_addr), 7)); // most significant bit moved to carry
        operand = (*access_ram(nes->ram, inst->operand_mem_addr) <<= 1);
    }
    set_cpu_status_bit(nes->cpu, ZERO, !operand);
    set_cpu_status_bit(nes->cpu, NEGATIVE, get_bit(operand, 7));
}

void exec_bcc_op(NES *nes, Inst *inst) {
    exec_branch(nes, inst, !get_cpu_status_bit(nes->cpu, CARRY));
}

void exec_bcs_op(NES *nes, Inst *inst) {
    exec_branch(nes, inst, get_cpu_status_bit(nes->cpu, CARRY));
}

void exec_beq_op(NES *nes, Inst *inst) {
    exec_branch(nes, inst, get_cpu_status_bit(nes->cpu, ZERO));
}

void exec_bit_op(NES *nes, Inst *inst) {
    uint8_t result = nes->cpu->acc_reg & inst->operand_val;
    set_cpu_status_bit(nes->cpu, ZERO, result);
    set_cpu_status_bit(nes->cpu, OVERFLOW, get_bit(result, 6));
    set_cpu_status_bit(nes->cpu, NEGATIVE, get_bit(result, 7));
}

void exec_bmi_op(NES *nes, Inst *inst) {
    exec_branch(nes, inst, get_cpu_status_bit(nes->cpu, NEGATIVE));
}

void exec_bne_op(NES *nes, Inst *inst) {
    exec_branch(nes, inst, !get_cpu_status_bit(nes->cpu, ZERO));
}

void exec_bpl_op(NES *nes, Inst *inst) {
    exec_branch(nes, inst, !get_cpu_status_bit(nes->cpu, NEGATIVE));
}

void exec_brk_op(NES *nes) { // force interrupt
    stack_push16(nes, nes->cpu->program_c);
    stack_push(nes, nes->cpu->status_reg);
    nes->cpu->program_c = 0xfffe;
    set_cpu_status_bit(nes->cpu, BRK, 1);
}

void exec_bvc_op(NES *nes, Inst *inst) {
    exec_branch(nes, inst, !get_cpu_status_bit(nes->cpu, OVERFLOW));
}

void exec_bvs_op(NES *nes, Inst *inst) {
    exec_branch(nes, inst, get_cpu_status_bit(nes->cpu, OVERFLOW));
}

void exec_clc_op(NES *nes) {
    set_cpu_status_bit(nes->cpu, CARRY, 0);
}

void exec_cld_op(NES *nes) {
    set_cpu_status_bit(nes->cpu, DECIMAL, 0);
}

void exec_cli_op(NES *nes) {
    set_cpu_status_bit(nes->cpu, IRQ_DISABLE, 0);
}

void exec_clv_op(NES *nes) {
    set_cpu_status_bit(nes->cpu, OVERFLOW, 0);
}

void exec_cmp_op(NES *nes, Inst *inst) {
    uint8_t result = nes->cpu->acc_reg - inst->operand_val;
    set_cpu_status_bit(nes->cpu, CARRY, nes->cpu->acc_reg >= inst->operand_val);
    set_cpu_status_bit(nes->cpu, ZERO, nes->cpu->acc_reg == inst->operand_val);
    set_cpu_status_bit(nes->cpu, NEGATIVE, get_bit(result, 7));
}

void exec_cpx_op(NES *nes, Inst *inst) {
    uint8_t result = nes->cpu->x_reg - inst->operand_val;
    set_cpu_status_bit(nes->cpu, CARRY, nes->cpu->x_reg >= inst->operand_val);
    set_cpu_status_bit(nes->cpu, ZERO, nes->cpu->x_reg == inst->operand_val);
    set_cpu_status_bit(nes->cpu, NEGATIVE, get_bit(result, 7));
}

void exec_cpy_op(NES *nes, Inst *inst) {
    uint8_t result = nes->cpu->y_reg - inst->operand_val;
    set_cpu_status_bit(nes->cpu, CARRY, nes->cpu->y_reg >= inst->operand_val);
    set_cpu_status_bit(nes->cpu, ZERO, nes->cpu->y_reg == inst->operand_val);
    set_cpu_status_bit(nes->cpu, NEGATIVE, get_bit(result, 7));
}

void exec_dec_op(NES *nes, Inst *inst) {
    inst->page_cross_cycles = 0; // resolve difference in absolute x addressing mode cycles
    update_cpu_status(nes, --(*access_ram(nes->ram, inst->operand_mem_addr)));
}

void exec_dex_op(NES *nes) {
    update_cpu_status(nes, --nes->cpu->x_reg);
}

void exec_dey_op(NES *nes) {
    update_cpu_status(nes, --nes->cpu->y_reg);
}

void exec_eor_op(NES *nes, Inst *inst) {
    update_cpu_status(nes, nes->cpu->acc_reg ^= inst->operand_val);
}

void exec_inc_op(NES *nes, Inst *inst) {
    inst->page_cross_cycles = 0; // resolve difference in absolute x addressing mode cycles
    update_cpu_status(nes, ++(*access_ram(nes->ram, inst->operand_mem_addr)));
}

void exec_inx_op(NES *nes) {
    update_cpu_status(nes, ++nes->cpu->x_reg);
}

void exec_iny_op(NES *nes) {
    update_cpu_status(nes, ++nes->cpu->y_reg);
}

void exec_jmp_op(NES *nes, Inst *inst) {
    uint16_t operand;
    if (inst->addr_mode == ABSOLUTE) {
        operand = inst->operand_mem_addr;
    } else { // INDIRECT memory addressing mode
        if ((inst->operand_mem_addr & 0xff) == 0xff) { // emulate 6502 page boundary bug
            operand = (*access_ram(nes->ram, inst->operand_mem_addr & 0xff00) << 8) | // most significant bits from 0x__00
                *access_ram(nes->ram, inst->operand_mem_addr); // normal least significant bits
        } else {
            operand = inst->operand_val;
        }
    }

    nes->cpu->program_c = operand;
}

void exec_jsr_op(NES *nes, Inst *inst) {
    stack_push16(nes, inst->operand_mem_addr - 1);
    nes->cpu->program_c = inst->operand_mem_addr;
} 

void exec_lda_op(NES *nes, Inst *inst) {
    update_cpu_status(nes, nes->cpu->acc_reg = inst->operand_val);
}

void exec_ldx_op(NES *nes, Inst *inst) {
    update_cpu_status(nes, nes->cpu->x_reg = inst->operand_val);
}

void exec_ldy_op(NES *nes, Inst *inst) {
    update_cpu_status(nes, nes->cpu->y_reg = inst->operand_val);
}

void exec_lsr_op(NES *nes, Inst *inst) {
    inst->page_cross_cycles = 0; // resolve difference in absolute x addressing mode cycles

    uint8_t operand;
    if (inst->addr_mode == ACCUMULATOR) {
        set_cpu_status_bit(nes->cpu, CARRY, get_bit(nes->cpu->acc_reg, 7)); // most significant bit moved to carry
        operand = (nes->cpu->acc_reg >>= 1);
    } else { // shift memory contents
        set_cpu_status_bit(nes->cpu, CARRY, get_bit(*access_ram(nes->ram, inst->operand_mem_addr), 7)); // most significant bit moved to carry
        operand = (*access_ram(nes->ram, inst->operand_mem_addr) >>= 1);
    }
    set_cpu_status_bit(nes->cpu, ZERO, !operand);
    set_cpu_status_bit(nes->cpu, NEGATIVE, get_bit(operand, 7));
}

void exec_ora_op(NES *nes, Inst *inst) {
    update_cpu_status(nes, nes->cpu->acc_reg |= inst->operand_val);
}

void exec_pha_op(NES *nes) {
    stack_push(nes, nes->cpu->acc_reg);
}

void exec_php_op(NES *nes) {
    stack_push(nes, nes->cpu->status_reg);
}

void exec_pla_op(NES *nes) {
    update_cpu_status(nes, nes->cpu->acc_reg = stack_pull(nes));
}

void exec_plp_op(NES *nes) {
    nes->cpu->status_reg = stack_pull(nes);
}

void exec_rol_op(NES *nes, Inst *inst) {
    inst->page_cross_cycles = 0; // resolve difference in absolute x addressing mode cycles

    if (inst->addr_mode == ACCUMULATOR) {
        bool new_carry = get_bit(nes->cpu->acc_reg, 7);
        nes->cpu->acc_reg <<= 1;
        set_bit(&nes->cpu->acc_reg, 0, get_cpu_status_bit(nes->cpu, CARRY));
        set_cpu_status_bit(nes->cpu, CARRY, new_carry);
        set_cpu_status_bit(nes->cpu, ZERO, !nes->cpu->acc_reg);
        set_cpu_status_bit(nes->cpu, NEGATIVE, get_bit(nes->cpu->acc_reg, 7));
    } else { // rotate memory value
        bool new_carry = get_bit(inst->operand_val, 7);
        uint8_t *byte = access_ram(nes->ram, inst->operand_mem_addr);
        *byte <<= 1;
        set_bit(byte, 0, get_cpu_status_bit(nes->cpu, CARRY));
        set_cpu_status_bit(nes->cpu, CARRY, new_carry);
        set_cpu_status_bit(nes->cpu, ZERO, !(*byte));
        set_cpu_status_bit(nes->cpu, NEGATIVE, get_bit(*byte, 7));
    }
}

void exec_ror_op(NES *nes, Inst *inst) {
    inst->page_cross_cycles = 0; // resolve difference in absolute x addressing mode cycles

    if (inst->addr_mode == ACCUMULATOR) {
        bool new_carry = get_bit(nes->cpu->acc_reg, 0);
        nes->cpu->acc_reg >>= 1;
        set_bit(&nes->cpu->acc_reg, 7, get_cpu_status_bit(nes->cpu, CARRY));
        set_cpu_status_bit(nes->cpu, CARRY, new_carry);
        set_cpu_status_bit(nes->cpu, ZERO, !nes->cpu->acc_reg);
        set_cpu_status_bit(nes->cpu, NEGATIVE, get_bit(nes->cpu->acc_reg, 7));
    } else { // rotate memory value
        bool new_carry = get_bit(inst->operand_val, 0);
        uint8_t *byte = access_ram(nes->ram, inst->operand_mem_addr);
        *byte >>= 1;
        set_bit(byte, 7, get_cpu_status_bit(nes->cpu, CARRY));
        set_cpu_status_bit(nes->cpu, CARRY, new_carry);
        set_cpu_status_bit(nes->cpu, ZERO, !(*byte));
        set_cpu_status_bit(nes->cpu, NEGATIVE, get_bit(*byte, 7));
    }
}

void exec_rti_op(NES *nes) {
    nes->cpu->status_reg = stack_pull(nes);
    nes->cpu->program_c = stack_pull16(nes);
}

void exec_rts_op(NES *nes) {
    nes->cpu->program_c = stack_pull16(nes);
}

void exec_sbc_op(NES *nes, Inst *inst) {
    uint16_t difference = nes->cpu->acc_reg - inst->operand_val - !get_cpu_status_bit(nes->cpu, CARRY); // signed, higher-precision value to check for overflow, carry
    set_cpu_status_bit(nes->cpu, OVERFLOW, 
            (nes->cpu->acc_reg ^ difference) & (inst->operand_val ^ difference) & 0x80); // overflow formula courtesy of http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
    nes->cpu->acc_reg -= inst->operand_val - !get_cpu_status_bit(nes->cpu, CARRY);
    set_cpu_status_bit(nes->cpu, CARRY, difference > 255);
    set_cpu_status_bit(nes->cpu, ZERO, !difference);
    set_cpu_status_bit(nes->cpu, NEGATIVE, get_bit(nes->cpu->acc_reg, 7));
}

void exec_sec_op(NES *nes) {
    set_cpu_status_bit(nes->cpu, CARRY, 1);
}

void exec_sed_op(NES *nes) {
    set_cpu_status_bit(nes->cpu, DECIMAL, 1);
}

void exec_sei_op(NES *nes) {
    set_cpu_status_bit(nes->cpu, IRQ_DISABLE, 1);
}

void exec_sta_op(NES *nes, Inst *inst) {
    *access_ram(nes->ram, inst->operand_mem_addr) = nes->cpu->acc_reg;
}

void exec_stx_op(NES *nes, Inst *inst) {
    *access_ram(nes->ram, inst->operand_mem_addr) = nes->cpu->x_reg;
}

void exec_sty_op(NES *nes, Inst *inst) {
    *access_ram(nes->ram, inst->operand_mem_addr) = nes->cpu->y_reg;
}

void exec_tax_op(NES *nes) {
    update_cpu_status(nes, nes->cpu->x_reg = nes->cpu->acc_reg);
}

void exec_tay_op(NES *nes) {
    update_cpu_status(nes, nes->cpu->y_reg = nes->cpu->acc_reg);
}

void exec_tsx_op(NES *nes) {
    update_cpu_status(nes, nes->cpu->x_reg = nes->cpu->stack_p);
}

void exec_txa_op(NES *nes) {
    update_cpu_status(nes, nes->cpu->acc_reg = nes->cpu->x_reg);
}

void exec_txs_op(NES *nes) {
    nes->cpu->stack_p = nes->cpu->x_reg;
}

void exec_tya_op(NES *nes) {
    update_cpu_status(nes, nes->cpu->acc_reg = nes->cpu->y_reg);
}

void exec_inst(NES *nes, Inst *inst) {
    update_inst_operand(nes, inst);
    switch(inst->inst_type) {
        case ADC_OP:
            exec_adc_op(nes, inst);
            break;
        case AND_OP:
            exec_and_op(nes, inst);
            break;
        case ASL_OP:
            exec_asl_op(nes, inst);
            break;
        case BCC_OP:
            exec_bcc_op(nes, inst);
            break;
        case BCS_OP:
            exec_bcs_op(nes, inst);
            break;
        case BEQ_OP:
            exec_beq_op(nes, inst);
            break;
        case BIT_OP:
            exec_bit_op(nes, inst);
            break;
        case BMI_OP:
            exec_bmi_op(nes, inst);
            break;
        case BNE_OP:
            exec_bne_op(nes, inst);
            break;
        case BPL_OP:
            exec_bpl_op(nes, inst);
            break;
        case BRK_OP:
            exec_brk_op(nes);
            break;
        case BVC_OP:
            exec_bvc_op(nes, inst);
            break;
        case BVS_OP:
            exec_bvs_op(nes, inst);
            break;
        case CLC_OP:
            exec_clc_op(nes);
            break;
        case CLD_OP:
            exec_cld_op(nes);
            break;
        case CLI_OP:
            exec_cli_op(nes);
            break;
        case CLV_OP:
            exec_clv_op(nes);
            break;
        case CMP_OP:
            exec_cmp_op(nes, inst);
            break;
        case CPX_OP:
            exec_cpx_op(nes, inst);
            break;
        case CPY_OP:
            exec_cpx_op(nes, inst);
            break;
        case DEC_OP:
            exec_dec_op(nes, inst);
            break;
        case DEX_OP:
            exec_dex_op(nes);
            break;
        case DEY_OP:
            exec_dey_op(nes);
            break;
        case EOR_OP:
            exec_eor_op(nes, inst);
            break;
        case INC_OP:
            exec_inc_op(nes, inst);
            break;
        case INX_OP:
            exec_inx_op(nes);
            break;
        case INY_OP:
            exec_iny_op(nes);
            break;
        case JMP_OP:
            exec_jmp_op(nes, inst);
            break;
        case JSR_OP:
            exec_jsr_op(nes, inst);
            break;
        case LDA_OP:
            exec_lda_op(nes, inst);
            break;
        case LDX_OP:
            exec_ldx_op(nes, inst);
            break;
        case LDY_OP:
            exec_ldy_op(nes, inst);
            break;
        case LSR_OP:
            exec_lsr_op(nes, inst);
            break;
        case ORA_OP:
            exec_ora_op(nes, inst);
            break;
        case PHA_OP:
            exec_pha_op(nes);
            break;
        case PHP_OP:
            exec_php_op(nes);
            break;
        case PLA_OP:
            exec_pla_op(nes);
            break;
        case PLP_OP:
            exec_plp_op(nes);
            break;
        case ROL_OP:
            exec_rol_op(nes, inst);
            break;
        case ROR_OP:
            exec_ror_op(nes, inst);
            break;
        case RTI_OP:
            exec_rti_op(nes);
            break;
        case RTS_OP:
            exec_rts_op(nes);
            break;
        case SBC_OP:
            exec_sbc_op(nes, inst);
            break;
        case SEC_OP:
            exec_sec_op(nes);
            break;
        case SED_OP:
            exec_sed_op(nes);
            break;
        case SEI_OP:
            exec_sei_op(nes);
            break;
        case STA_OP:
            exec_sta_op(nes, inst);
            break;
        case STX_OP:
            exec_stx_op(nes, inst);
            break;
        case STY_OP:
            exec_sty_op(nes, inst);
            break;
        case TAX_OP:
            exec_sty_op(nes, inst);
            break;
        case TAY_OP:
            exec_tay_op(nes);
            break;
        case TSX_OP:
            exec_tsx_op(nes);
            break;
        case TXA_OP:
            exec_txa_op(nes);
            break;
        case TXS_OP:
            exec_txs_op(nes);
            break;
        case TYA_OP:
            exec_tya_op(nes);
            break;
        case NOP:
            break;
        default:
            delete_nes(nes);
            fprintf(stderr, "Unsupported opcode");
            exit(-1);
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
                inst->page_cross_cycles = 1;
                inst->branch_succeeds_cycles = 1;
                inst->inst_type = BCC_OP;
                break;

            case 0xb0:
                inst->addr_mode = RELATIVE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->page_cross_cycles = 1;
                inst->branch_succeeds_cycles = 1;
                inst->inst_type = BCS_OP;
                break;

            case 0xf0:
                inst->addr_mode = RELATIVE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->page_cross_cycles = 1;
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
                inst->page_cross_cycles = 1;
                inst->branch_succeeds_cycles = 1;
                inst->inst_type = BMI_OP;
                break;

            case 0xd0:
                inst->addr_mode = RELATIVE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->page_cross_cycles = 1;
                inst->branch_succeeds_cycles = 1;
                inst->inst_type = BNE_OP;
                break;

            case 0x10:
                inst->addr_mode = RELATIVE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->page_cross_cycles = 1;
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
                inst->page_cross_cycles = 1;
                inst->branch_succeeds_cycles = 1;
                inst->inst_type = BVC_OP;
                break;

            case 0x70:
                inst->addr_mode = RELATIVE;
                inst->size_bytes = 2;
                inst->cycles = 2;
                inst->page_cross_cycles = 1;
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
