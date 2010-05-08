#ifndef ALIASES_H_
#define ALIASES_H_

#define handle_op_ADC    _handle_op_ADC_ADD
#define handle_op_ADD    _handle_op_ADC_ADD
#define handle_op_AND    _handle_op_AND_BIT
#define handle_op_BIT    _handle_op_AND_BIT
#define handle_op_ASR    _handle_op_ASR_LSR_ROR
#define handle_op_ASRA   _handle_op_ASR_LSR_ROR
#define handle_op_ASRX   _handle_op_ASR_LSR_ROR
#define handle_op_LSR    _handle_op_ASR_LSR_ROR
#define handle_op_LSRA   _handle_op_ASR_LSR_ROR
#define handle_op_LSRX   _handle_op_ASR_LSR_ROR
#define handle_op_ROR    _handle_op_ASR_LSR_ROR
#define handle_op_RORA   _handle_op_ASR_LSR_ROR
#define handle_op_RORX   _handle_op_ASR_LSR_ROR
#define handle_op_BRA    _handle_op_BRANCHES
#define handle_op_BLE    _handle_op_BRANCHES
#define handle_op_BGT    _handle_op_BRANCHES
#define handle_op_BLT    _handle_op_BRANCHES
#define handle_op_BGE    _handle_op_BRANCHES
#define handle_op_BNE    _handle_op_BRANCHES
#define handle_op_BEQ    _handle_op_BRANCHES
#define handle_op_BLS    _handle_op_BRANCHES
#define handle_op_BHI    _handle_op_BRANCHES
#define handle_op_BCS    _handle_op_BRANCHES
#define handle_op_BCC    _handle_op_BRANCHES
#define handle_op_BPL    _handle_op_BRANCHES
#define handle_op_BMI    _handle_op_BRANCHES
#define handle_op_BMC    _handle_op_BRANCHES
#define handle_op_BMS    _handle_op_BRANCHES
#define handle_op_BHCC   _handle_op_BRANCHES
#define handle_op_BHCS   _handle_op_BRANCHES
#define handle_op_BIL    _handle_op_BRANCHES
#define handle_op_BIH    _handle_op_BRANCHES
#define handle_op_BRN    _handle_op_BRANCHES
#define handle_op_BRA    _handle_op_BRANCHES
#define handle_op_BRSET0 _handle_op_BRANCHES
#define handle_op_BRSET1 _handle_op_BRANCHES
#define handle_op_BRSET2 _handle_op_BRANCHES
#define handle_op_BRSET3 _handle_op_BRANCHES
#define handle_op_BRSET4 _handle_op_BRANCHES
#define handle_op_BRSET5 _handle_op_BRANCHES
#define handle_op_BRSET6 _handle_op_BRANCHES
#define handle_op_BRSET7 _handle_op_BRANCHES
#define handle_op_BRCLR0 _handle_op_BRANCHES
#define handle_op_BRCLR1 _handle_op_BRANCHES
#define handle_op_BRCLR2 _handle_op_BRANCHES
#define handle_op_BRCLR3 _handle_op_BRANCHES
#define handle_op_BRCLR4 _handle_op_BRANCHES
#define handle_op_BRCLR5 _handle_op_BRANCHES
#define handle_op_BRCLR6 _handle_op_BRANCHES
#define handle_op_BRCLR7 _handle_op_BRANCHES
#define handle_op_BSET0  _handle_op_BSET_BCLR
#define handle_op_BSET1  _handle_op_BSET_BCLR
#define handle_op_BSET2  _handle_op_BSET_BCLR
#define handle_op_BSET3  _handle_op_BSET_BCLR
#define handle_op_BSET4  _handle_op_BSET_BCLR
#define handle_op_BSET5  _handle_op_BSET_BCLR
#define handle_op_BSET6  _handle_op_BSET_BCLR
#define handle_op_BSET7  _handle_op_BSET_BCLR
#define handle_op_BCLR0  _handle_op_BSET_BCLR
#define handle_op_BCLR1  _handle_op_BSET_BCLR
#define handle_op_BCLR2  _handle_op_BSET_BCLR
#define handle_op_BCLR3  _handle_op_BSET_BCLR
#define handle_op_BCLR4  _handle_op_BSET_BCLR
#define handle_op_BCLR5  _handle_op_BSET_BCLR
#define handle_op_BCLR6  _handle_op_BSET_BCLR
#define handle_op_BCLR7  _handle_op_BSET_BCLR
#define handle_op_CBEQ   _handle_op_CBEQ_CBEQA_CBEQX
#define handle_op_CBEQA  _handle_op_CBEQ_CBEQA_CBEQX
#define handle_op_CBEQX  _handle_op_CBEQ_CBEQA_CBEQX
#define handle_op_CLR    _handle_op_CLR
#define handle_op_CLRA   _handle_op_CLR
#define handle_op_CLRH   _handle_op_CLR
#define handle_op_CLRX   _handle_op_CLR
#define handle_op_COM    _handle_op_COM_COMA_COMX
#define handle_op_COMA   _handle_op_COM_COMA_COMX
#define handle_op_COMX   _handle_op_COM_COMA_COMX
#define handle_op_DBNZ   _handle_op_DBNZ_DBNZA_DBNZX
#define handle_op_DBNZA  _handle_op_DBNZ_DBNZA_DBNZX
#define handle_op_DBNZX  _handle_op_DBNZ_DBNZA_DBNZX
#define handle_op_DEC    _handle_op_DEC
#define handle_op_DECA   _handle_op_DEC
#define handle_op_DECX   _handle_op_DEC
#define handle_op_ORA    _handle_op_EOR_ORA
#define handle_op_EOR    _handle_op_EOR_ORA
#define handle_op_INC    _handle_op_INC_INCA_INCX
#define handle_op_INCA   _handle_op_INC_INCA_INCX
#define handle_op_INCX   _handle_op_INC_INCA_INCX
#define handle_op_BSR    _handle_op_JSR_BSR
#define handle_op_JSR    _handle_op_JSR_BSR
#define handle_op_LDA    _handle_op_LDA_LDX_STA_STX
#define handle_op_LDX    _handle_op_LDA_LDX_STA_STX
#define handle_op_STA    _handle_op_LDA_LDX_STA_STX
#define handle_op_STX    _handle_op_LDA_LDX_STA_STX
#define handle_op_LDHX   _handle_op_LDHX_STHX
#define handle_op_STHX   _handle_op_LDHX_STHX
#define handle_op_LSL    _handle_op_LSL_ROL
#define handle_op_LSLA   _handle_op_LSL_ROL
#define handle_op_LSLX   _handle_op_LSL_ROL
#define handle_op_ROL    _handle_op_LSL_ROL
#define handle_op_ROLA   _handle_op_LSL_ROL
#define handle_op_ROLX   _handle_op_LSL_ROL
#define handle_op_NEG    _handle_op_NEG
#define handle_op_NEGA   _handle_op_NEG
#define handle_op_NEGX   _handle_op_NEG
#define handle_op_PSHA   _handle_op_PSH
#define handle_op_PSHH   _handle_op_PSH
#define handle_op_PSHX   _handle_op_PSH
#define handle_op_PULA   _handle_op_PUL
#define handle_op_PULH   _handle_op_PUL
#define handle_op_PULX   _handle_op_PUL
#define handle_op_SBC    _handle_op_SBC_SUB_CMP_CPX
#define handle_op_SUB    _handle_op_SBC_SUB_CMP_CPX
#define handle_op_CMP    _handle_op_SBC_SUB_CMP_CPX
#define handle_op_CPX    _handle_op_SBC_SUB_CMP_CPX
#define handle_op_TST    _handle_op_TST_TSTA_TSTX
#define handle_op_TSTA   _handle_op_TST_TSTA_TSTX
#define handle_op_TSTX   _handle_op_TST_TSTA_TSTX

#define ALIAS_DECLS \
	R_(_handle_op_ADC_ADD) \
	R_(_handle_op_AND_BIT) \
	R_(_handle_op_ASR_LSR_ROR) \
	R_(_handle_op_BRANCHES) \
	R_(_handle_op_BSET_BCLR) \
	R_(_handle_op_CBEQ_CBEQA_CBEQX) \
	R_(_handle_op_CLR) \
	R_(_handle_op_COM_COMA_COMX) \
	R_(_handle_op_DBNZ_DBNZA_DBNZX) \
	R_(_handle_op_DEC) \
	R_(_handle_op_EOR_ORA) \
	R_(_handle_op_INCA_INCX) \
	R_(_handle_op_JSR_BSR) \
	R_(_handle_op_LDA_LDX_STA_STX) \
	R_(_handle_op_LDHX_STHX) \
	R_(_handle_op_LSL_ROL) \
	R_(_handle_op_NEG) \
	R_(_handle_op_PSH) \
	R_(_handle_op_PUL) \
	R_(_handle_op_SBC_SUB_CMP_CPX) \
	R_(_handle_op_TST_TSTA_TSTX)

#define R_(Op) int handle_op_##Op(struct hc_state *state, const struct opinfo *info);
	ALIAS_DECLS
#undef R_

#endif

