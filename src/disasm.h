/**
 * Disassembles an instruction in a given buffer.
 *
 * @param buf           the buffer from which to disassemble instructs
 * @param size          the size of the buffer
 * @param info  [out]   the decoded opinfo, or the opinfo for OP_INVALID
 * @param dlen  [out]   the length of the decoded instruction
 *
 * @return zero on success, non-zero on undifferentiated failure
 *
 * @p info is never set to null.
 */
int hc_disasm_instr(void *buf, size_t size, struct opinfo * const *info, size_t *dlen);

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */

