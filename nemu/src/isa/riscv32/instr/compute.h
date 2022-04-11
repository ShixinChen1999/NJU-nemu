def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

//////////////////////////////////////////////
//  addi-branch
/////////////////////////////////////////////
def_EHelper(addi){
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(slti){
 rtl_setrelopi(s, RELOP_LT, ddest, dsrc1, id_src2->imm);
}

def_EHelper(sltiu){
 rtl_setrelopi(s, RELOP_LTU, ddest, dsrc1, id_src2->imm);
}
def_EHelper(xori){
 rtl_xori(s, ddest, dsrc1, id_src2->imm);
}
def_EHelper(ori){
 rtl_ori(s, ddest, dsrc1, id_src2->imm);
}
def_EHelper(andi){
 rtl_andi(s, ddest, dsrc1, id_src2->imm);
}
def_EHelper(slli){
 rtl_slli(s, ddest, dsrc1, id_src2->imm);
}
def_EHelper(srli){
 rtl_srli(s, ddest, dsrc1, id_src2->imm);
}
def_EHelper(srai){
 rtl_srai(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(auipc) {
  rtl_li(s, ddest, s->pc+id_src1->imm); 
}

///////////////////////////////////////////////
// add_branch instr
//////////////////////////////////////////////
def_EHelper(add) {
    rtl_add(s,ddest,dsrc1,dsrc2);

}
def_EHelper(sub) {
    rtl_sub(s,ddest,dsrc1,dsrc2);

}
def_EHelper(sll) {
    rtl_sll(s,ddest,dsrc1,dsrc2);

}
def_EHelper(srl) {
    rtl_srl(s,ddest,dsrc1,dsrc2);

}
def_EHelper(sra) {
    rtl_sra(s,ddest,dsrc1,dsrc2);

}
def_EHelper(slt) {
    rtl_setrelop(s,RELOP_LT,ddest,dsrc1,dsrc2);

}
def_EHelper(sltu) {
    rtl_setrelop(s,RELOP_LTU,ddest,dsrc1,dsrc2);

}
def_EHelper(xor) {
    rtl_xor(s,ddest,dsrc1,dsrc2);

}
def_EHelper(or) {
    rtl_or(s,ddest,dsrc1,dsrc2);

}
def_EHelper(and) {
    rtl_and(s,ddest,dsrc1,dsrc2);

}

///////////////////////////////////////////////
/////   B_branch instr
///////////////////////////////////////////////
def_EHelper(beq){
	 rtl_jrelop(s, RELOP_EQ, dsrc1, dsrc2, id_dest->imm);	

}

def_EHelper(bne){
	 rtl_jrelop(s, RELOP_NE, dsrc1, dsrc2, id_dest->imm);	
}

def_EHelper(blt){
	 rtl_jrelop(s, RELOP_LT, dsrc1, dsrc2, id_dest->imm);	
}

def_EHelper(bge){
	 rtl_jrelop(s, RELOP_GE, dsrc1, dsrc2, id_dest->imm);	
}

def_EHelper(bltu){
	 rtl_jrelop(s, RELOP_LTU, dsrc1, dsrc2, id_dest->imm);	
}

def_EHelper(bgeu){
	 rtl_jrelop(s, RELOP_GEU, dsrc1, dsrc2, id_dest->imm);	
}

def_EHelper(jal) {
//  rtl_sext(s, ddest, &(id_src2->imm),4);//ddest: x1 register imm=offset x1= pc+offset
    rtl_li(s, ddest, id_src2->imm);
	rtl_j (s, id_src1->imm); //pc =pc+4
}

def_EHelper(jalr) {
    rtl_addi(s, s0, dsrc1, id_src2->imm);//
	rtl_li (s, ddest, s->snpc); //pc =pc+4
    rtl_jr (s, s0);
}
