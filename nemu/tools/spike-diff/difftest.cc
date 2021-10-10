#define IN_SPIKE
#define ISA64
#include "sim.h"
#include "../../include/common.h"
#include <difftest-def.h>
#include <processor.h>

#ifdef CONFIG_ISA_riscv32
#undef DEFAULT_ISA
#define DEFAULT_ISA "RV32IM"
#endif

static std::vector<std::pair<reg_t, abstract_device_t*>> difftest_plugin_devices;
static std::vector<std::string> difftest_htif_args;
static std::vector<std::pair<reg_t, mem_t*>> difftest_mem(
    1, std::make_pair(reg_t(DRAM_BASE), new mem_t(reg_t(1) << 28)));
static std::vector<int> difftest_hartids;
static debug_module_config_t difftest_dm_config = {
  .progbufsize = 2,
  .max_bus_master_bits = 0,
  .require_authentication = false,
  .abstract_rti = 0,
  .support_hasel = true,
  .support_abstract_csr_access = true,
  .support_haltgroups = true,
  .support_impebreak = true
};

struct diff_context_t {
  word_t gpr[32];
  word_t pc;
  word_t csr[0x1000];
  word_t privilege;
};

static sim_t* s = NULL;
static processor_t *p = NULL;
static state_t *state = NULL;

void sim_t::diff_init(int port) {
  p = get_core("0");
  state = p->get_state();
  is_diff_ref = true;
}

void sim_t::diff_step(uint64_t n) {
  step(n);
}
static int csrs[] = { CSR_SSTATUS, CSR_SIE, CSR_STVEC, CSR_SSCRATCH, CSR_SEPC, 
              CSR_SCAUSE, CSR_STVAL, CSR_SIP, CSR_SATP,
              CSR_MEDELEG, CSR_MIDELEG, CSR_MIE, CSR_MTVEC, CSR_MSCRATCH, CSR_MEPC,
              CSR_MCAUSE, CSR_MTVAL, CSR_MIP, CSR_PMPCFG0, CSR_PMPADDR0, CSR_MSTATUS,
              CSR_MHARTID
              };

void sim_t::diff_get_regs(void* diff_context) {
  struct diff_context_t* ctx = (struct diff_context_t*)diff_context;
  for (int i = 0; i < NXPR; i++) {
    ctx->gpr[i] = state->XPR[i];
  }
  ctx->pc = state->pc;
  for(int i = 0; i < sizeof(csrs)/sizeof(int); i++){
    ctx->csr[csrs[i]] = p->get_csr(csrs[i]);
  }
  ctx->privilege = state->prv;
}

void sim_t::diff_set_regs(void* diff_context) {
  struct diff_context_t* ctx = (struct diff_context_t*)diff_context;
  for (int i = 0; i < NXPR; i++) {
    state->XPR.write(i, (sword_t)ctx->gpr[i]);
  }
  state->pc = ctx->pc;

}

void sim_t::diff_memcpy(reg_t dest, void* src, size_t n) {
  mmu_t* mmu = p->get_mmu();
  for (size_t i = 0; i < n; i++) {
    mmu->store_uint8(dest+i, *((uint8_t*)src+i));
  }
}

extern "C" {

void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
  if (direction == DIFFTEST_TO_REF) {
    s->diff_memcpy(addr, buf, n);
  } else {
    assert(0);
  }
}

void difftest_regcpy(void* dut, bool direction) {
  if (direction == DIFFTEST_TO_REF) {
    s->diff_set_regs(dut);
  } else {
    s->diff_get_regs(dut);
  }
}

void difftest_exec(uint64_t n) {
  s->diff_step(n);
}

void difftest_init(int port) {
  difftest_htif_args.push_back("");
  s = new sim_t(DEFAULT_ISA, DEFAULT_PRIV, DEFAULT_VARCH, 1, false, false,
      0, 0, NULL, reg_t(-1), difftest_mem, difftest_plugin_devices, difftest_htif_args,
      std::move(difftest_hartids), difftest_dm_config, nullptr, false, NULL);
  s->diff_init(port);
  printf("aft init\n");
}

void difftest_raise_intr(uint64_t NO) {
  // assert(0);
  reg_t mip = p->get_csr(CSR_MIP);
  mip |= 1 << NO;
  state->mip = mip;
  // printf("no: %lx\n", NO);
}

void difftest_clear_mip() {
  reg_t mip = p->get_csr(CSR_MIP);
  mip = set_field(mip, MIP_MTIP, 0);
  state->mip = mip;
}

}