#include <device/map.h>

#define VIRTIO0 0x10001000
// from qemu virtio_mmio.h
#define VIRTIO_MMIO_MAGIC_VALUE		0x000 // 0x74726976
#define VIRTIO_MMIO_VERSION		0x004 // version; 1 is legacy
#define VIRTIO_MMIO_DEVICE_ID		0x008 // device type; 1 is net, 2 is disk
#define VIRTIO_MMIO_VENDOR_ID		0x00c // 0x554d4551
#define VIRTIO_MMIO_DEVICE_FEATURES	0x010
#define VIRTIO_MMIO_DRIVER_FEATURES	0x020
#define VIRTIO_MMIO_GUEST_PAGE_SIZE	0x028 // page size for PFN, write-only
#define VIRTIO_MMIO_QUEUE_SEL		0x030 // select queue, write-only
#define VIRTIO_MMIO_QUEUE_NUM_MAX	0x034 // max size of current queue, read-only
#define VIRTIO_MMIO_QUEUE_NUM		0x038 // size of current queue, write-only
#define VIRTIO_MMIO_QUEUE_ALIGN		0x03c // used ring alignment, write-only
#define VIRTIO_MMIO_QUEUE_PFN		0x040 // physical page number for queue, read/write
#define VIRTIO_MMIO_QUEUE_READY		0x044 // ready bit
#define VIRTIO_MMIO_QUEUE_NOTIFY	0x050 // write-only
#define VIRTIO_MMIO_INTERRUPT_STATUS	0x060 // read-only
#define VIRTIO_MMIO_INTERRUPT_ACK	0x064 // write-only
#define VIRTIO_MMIO_STATUS		0x070 // read/write

static uint8_t* virtio = NULL;

#define VIRT(r) (uint32_t *)(virtio + r)

void init_virtio(){
    virtio = new_space(0x1000);
    add_mmio_map("virtio", VIRTIO0, virtio, 0x1000, NULL);
    *VIRT(VIRTIO_MMIO_MAGIC_VALUE) = 0x74726976;
    *VIRT(VIRTIO_MMIO_VERSION) = 1;
    *VIRT(VIRTIO_MMIO_DEVICE_ID) = 2;
    *VIRT(VIRTIO_MMIO_VENDOR_ID) = 0x554d4551;
    *VIRT(VIRTIO_MMIO_QUEUE_NUM_MAX) = 8;
}