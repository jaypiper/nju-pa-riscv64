static inline def_EHelper(amoswap){
    rtl_lm(s, s0, dsrc1, 0, s->width);
    rtl_sm(s, dsrc1, 0, dsrc2, s->width);
    rtl_mv(s, ddest, s0);
}