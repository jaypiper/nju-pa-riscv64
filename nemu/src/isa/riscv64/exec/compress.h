static inline def_EHelper(c_beqz){
    rtl_jrelop(s, RELOP_EQ, id_src1->preg, id_src2->preg, cpu.pc + id_dest->simm);
}

static inline def_EHelper(c_bnez){
    rtl_jrelop(s, RELOP_NE, id_src1->preg, id_src2->preg, cpu.pc + id_dest->simm);
}