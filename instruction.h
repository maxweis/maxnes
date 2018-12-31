#pragma once

#include <stdint.h>
#include "ram.h"
#include "rom.h"
#include "cpu.h"

typedef struct ROM ROM;
typedef struct NES NES;
typedef enum ADDR_MODE ADDR_MODE;

extern const char* inst_names[];

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
        ADDR_MODE addr_mode;                     // addressing mode of instruction
        INST_OP inst_type;                      // operation executed by instruction
        uint16_t operand_val;                   // value of operand used by instruction (memory locations accessed, jump relative distance)
        uint16_t operand_mem_addr;              // memory address of operand for stores
} Inst;

void classify_inst(uint8_t opcode, Inst *inst);
void parse_insts(ROM *rom);
void exec_inst(NES *nes, Inst *inst);
void exec_branch(NES *nes, Inst *inst, bool condition);
void update_cpu_status(NES *nes, uint8_t value);
void exec_adc_op(NES *nes, Inst *inst);
void exec_and_op(NES *nes, Inst *inst);
void exec_asl_op(NES *nes, Inst *inst);
void exec_bcc_op(NES *nes, Inst *inst);
void exec_bcs_op(NES *nes, Inst *inst);
void exec_beq_op(NES *nes, Inst *inst);
void exec_bit_op(NES *nes, Inst *inst);
void exec_bmi_op(NES *nes, Inst *inst);
void exec_bne_op(NES *nes, Inst *inst);
void exec_bpl_op(NES *nes, Inst *inst);
void exec_brk_op(NES *nes);
void exec_bvc_op(NES *nes, Inst *inst);
void exec_bvs_op(NES *nes, Inst *inst);
void exec_clc_op(NES *nes);
void exec_cld_op(NES *nes);
void exec_cli_op(NES *nes);
void exec_clv_op(NES *nes);
void exec_cmp_op(NES *nes, Inst *inst);
void exec_cpx_op(NES *nes, Inst *inst);
void exec_cpy_op(NES *nes, Inst *inst);
void exec_dec_op(NES *nes, Inst *inst);
void exec_dex_op(NES *nes);
void exec_dey_op(NES *nes);
void exec_eor_op(NES *nes, Inst *inst);
void exec_inc_op(NES *nes, Inst *inst);
void exec_inx_op(NES *nes);
void exec_iny_op(NES *nes);
void exec_jmp_op(NES *nes, Inst *inst);
void exec_jsr_op(NES *nes, Inst *inst);
void exec_lda_op(NES *nes, Inst *inst);
void exec_ldx_op(NES *nes, Inst *inst);
void exec_ldy_op(NES *nes, Inst *inst);
void exec_lsr_op(NES *nes, Inst *inst);
void exec_ora_op(NES *nes, Inst *inst);
void exec_pha_op(NES *nes);
void exec_php_op(NES *nes);
void exec_pla_op(NES *nes);
void exec_plp_op(NES *nes);
void exec_rol_op(NES *nes, Inst *inst);
void exec_ror_op(NES *nes, Inst *inst);
void exec_rti_op(NES *nes);
void exec_rts_op(NES *nes);
void exec_sbc_op(NES *nes, Inst *inst);
void exec_sec_op(NES *nes);
void exec_sed_op(NES *nes);
void exec_sei_op(NES *nes);
void exec_sta_op(NES *nes, Inst *inst);
void exec_stx_op(NES *nes, Inst *inst);
void exec_sty_op(NES *nes, Inst *inst);
void exec_tax_op(NES *nes);
void exec_tay_op(NES *nes);
void exec_tsx_op(NES *nes);
void exec_txa_op(NES *nes);
void exec_txs_op(NES *nes);
void exec_tya_op(NES *nes);
