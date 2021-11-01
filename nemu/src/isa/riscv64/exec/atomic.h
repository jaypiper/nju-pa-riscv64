static inline def_EHelper(lr){
    rtl_lm(s, s0, dsrc1, 0, s->width);
    if(s->is_trap) return;
    set_pre_lr(*dsrc1);
    if(s->width == 4){
        rtl_li(s, s0, c_sext32to64(*s0));
    }
    rtl_mv(s, ddest, s0);
}

static inline def_EHelper(sc){
    if(pre_lr_valid() && *dsrc1 == get_pre_lr()){
        rtl_sm(s, dsrc1, 0, dsrc2, s->width);
        if(!s->is_trap) rtl_li(s, ddest, 0);
    }else{
        rtl_li(s, ddest, 1);
    }
}

static inline def_EHelper(amoswap){
    rtl_lm(s, s0, dsrc1, 0, s->width);
    rtl_sm(s, dsrc1, 0, dsrc2, s->width);
    rtl_mv(s, ddest, s0);
}

static inline def_EHelper(amoadd){
    rtl_lm(s, s0, dsrc1, 0, s->width);
    rtl_add(s, s1, s0, dsrc2);
    rtl_sm(s, dsrc1, 0, s1, s->width);
    if(s->width == 4){
        rtl_li(s, s0, c_sext32to64(*s0));
    }
    rtl_mv(s, ddest, s0);
}

static inline def_EHelper(amoxor){
    rtl_lm(s, s0, dsrc1, 0, s->width);
    rtl_xor(s, s1, s0, dsrc2);
    rtl_sm(s, dsrc1, 0, s1, s->width);
    if(s->width == 4){
        rtl_li(s, s0, c_sext32to64(*s0));
    }
    rtl_mv(s, ddest, s0);
}

static inline def_EHelper(amoand){
    rtl_lm(s, s0, dsrc1, 0, s->width);
    rtl_and(s, s1, s0, dsrc2);
    rtl_sm(s, dsrc1, 0, s1, s->width);
    if(s->width == 4){
        rtl_li(s, s0, c_sext32to64(*s0));
    }
    rtl_mv(s, ddest, s0);
}

static inline def_EHelper(amoor){
    rtl_lm(s, s0, dsrc1, 0, s->width);
    rtl_or(s, s1, s0, dsrc2);
    rtl_sm(s, dsrc1, 0, s1, s->width);
    if(s->width == 4){
        rtl_li(s, s0, c_sext32to64(*s0));
    }
    rtl_mv(s, ddest, s0);
}

static inline def_EHelper(amomin){
    rtl_lm(s, s0, dsrc1, 0, s->width);
    if(s->is_trap) return;
    rtl_li(s, s1, *dsrc2);
    if(s->width == 4){
        rtl_li(s, s1, c_minw(*s0, *s1));
        rtl_li(s, s0, c_sext32to64(*s0));
    }else{
        rtl_li(s, s1, c_mind(*s0, *s1));
    }
    rtl_sm(s, dsrc1, 0, s1, s->width);
    rtl_mv(s, ddest, s0);
}

static inline def_EHelper(amomax){
    rtl_lm(s, s0, dsrc1, 0, s->width);
    if(s->is_trap) return;
    rtl_li(s, s1, *dsrc2);
    if(s->width == 4){
        rtl_li(s, s1, c_maxw(*s0, *s1));
        rtl_li(s, s0, c_sext32to64(*s0));
    }else{
        rtl_li(s, s1, c_maxd(*s0, *s1));
    }
    rtl_sm(s, dsrc1, 0, s1, s->width);
    rtl_mv(s, ddest, s0);
}

static inline def_EHelper(amominu){
  rtl_lm(s, s0, dsrc1, 0, s->width);
    if(s->is_trap) return;
    rtl_li(s, s1, *dsrc2);
    if(s->width == 4){
        rtl_li(s, s1, c_minuw(*s0, *s1));
        rtl_li(s, s0, c_sext32to64(*s0));
    }else{
        rtl_li(s, s1, c_minud(*s0, *s1));
    }
    rtl_sm(s, dsrc1, 0, s1, s->width);
    rtl_mv(s, ddest, s0);
}

static inline def_EHelper(amomaxu){
    rtl_lm(s, s0, dsrc1, 0, s->width);
    if(s->is_trap) return;
    rtl_li(s, s1, *dsrc2);
    if(s->width == 4){
        rtl_li(s, s1, c_maxuw(*s0, *s1));
        rtl_li(s, s0, c_sext32to64(*s0));
    }else{
        rtl_li(s, s1, c_maxud(*s0, *s1));
    }
    rtl_sm(s, dsrc1, 0, s1, s->width);
    rtl_mv(s, ddest, s0);
}