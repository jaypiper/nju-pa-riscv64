#include <device/map.h>

#define PLIC 0x0c000000L
#define PLIC_PRIORITY (PLIC + 0x0)
#define PLIC_PENDING (PLIC + 0x1000)
#define PLIC_MENABLE(hart) (PLIC + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart) (PLIC + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart) (PLIC + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart) (PLIC + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart) (PLIC + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) (PLIC + 0x201004 + (hart)*0x2000)

static uint8_t* plic_priority = NULL;
static uint8_t* plic_pending = NULL;
static uint8_t* plic_menable = NULL;
static uint8_t* plic_senable = NULL;
static uint8_t* plic_mpriority = NULL;
static uint8_t* plic_spriority = NULL;
static uint8_t* plic_mclain = NULL;
static uint8_t* plic_sclain = NULL;

void init_plic(){
    plic_priority = new_space(0x1000);
    plic_pending = new_space(0x1000);
    plic_menable = new_space(4);
    plic_senable = new_space(4);
    plic_mpriority = new_space(4);
    plic_spriority = new_space(4);
    plic_mclain = new_space(4);
    plic_sclain = new_space(4);

    add_mmio_map("plic", PLIC_PRIORITY, plic_priority, 0x3000, NULL);
    add_mmio_map("plic", PLIC_PENDING, plic_pending, 0x1000, NULL);
    add_mmio_map("plic", PLIC_MENABLE(0), plic_menable, 4, NULL);
    add_mmio_map("plic", PLIC_SENABLE(0), plic_senable, 4, NULL);
    add_mmio_map("plic", PLIC_MPRIORITY(0), plic_mpriority, 4, NULL);
    add_mmio_map("plic", PLIC_SPRIORITY(0), plic_spriority, 4, NULL);
    add_mmio_map("plic", PLIC_MCLAIM(0), plic_mclain, 4, NULL);
    add_mmio_map("plic", PLIC_SCLAIM(0), plic_sclain, 4, NULL);
}