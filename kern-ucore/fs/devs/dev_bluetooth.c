#include <types.h>
#include <arch.h>
#include <mmu.h>
#include <pmm.h>
#include <sem.h>
#include <ide.h>
#include <inode.h>
#include <dev.h>
#include <vfs.h>
#include <iobuf.h>
#include <error.h>
#include <assert.h>
#define inline_assembly()  asm("ori $0, $0, 0x1234")

#define CLK_FREQ 50000000

#define UART_BASE 0xB0501000	//With 1000 offset that axi_uart16550 has
#define UART_BASE2 0xB0401000
#define rbr		0*4
#define ier		1*4
#define fcr		2*4
#define lcr		3*4
#define mcr		4*4
#define lsr		5*4
#define msr		6*4
#define scr		7*4

#define thr		rbr
#define iir		fcr
#define dll		rbr
#define dlm		ier

static semaphore_t disk0_sem;
static void bluetooth_device_init(struct device *dev);
static int usrs=0;
static int rxData;
static int count=0;
void dev_init_bluetooth(void)
{
	struct inode *node;
	if ((node = dev_create_inode()) == NULL) {//得到一个空闲的inode号
		panic("bluetooth: dev_create_node.\n");
	}
	bluetooth_device_init(vop_info(node, device));//初始化device结构体

	int ret;
	if ((ret = vfs_add_dev("bluetooth", node, 0)) != 0) {//mountable=0!!
		panic("bluetooth: vfs_add_dev: %e.\n", ret);
	}
    //TODO:初始化的工作。。。
}
/*
	int (*d_open) (struct device * dev, uint32_t open_flags);
	int (*d_close) (struct device * dev);
	int (*d_io) (struct device * dev, struct iobuf * iob, bool write);
	int (*d_ioctl) (struct device * dev, int op, void *data);
*/

int bluetooth_open(struct device * dev, uint32_t open_flags){
    down(&(disk0_sem));
    if(usrs!=0) {
        kprintf("bluetooth is in use");
        return -E_BUSY;
    }
    usrs++;
     //蓝牙的初始化:
    *WRITE_IO(UART_BASE + lcr) = 0x00000080; // LCR[7]  is 1
	delay();
	*WRITE_IO(UART_BASE + dll) =70; // 326 27   115200 at 50MHz. Formula is Clk/16/baudrate. From axi_uart manual.
	delay();
	*WRITE_IO(UART_BASE + dlm) =1;
	delay(); 
	*WRITE_IO(UART_BASE + lcr) = 0x00000003; // LCR register. 8n1 parity disabled
	delay();
	*WRITE_IO(UART_BASE + ier) = 0x00000000; // IER register. disable interrupts
	*WRITE_IO(UART_BASE + fcr)=0x000000c1;
	//kprintf("lanya okle\n\r");
    up(&(disk0_sem));
    return 0;
}

int bluetooth_close(struct device * dev){
    down(&(disk0_sem));
    usrs--;
    up(&(disk0_sem));
    return 0;
}
int bluetooth_io(struct device * dev, struct iobuf * iob, bool write){//阻塞型IO

	if(!count)
    kprintf("bluetooth is in read or write\n\r");
    //*WRITE_IO(UART_BASE + thr) = '';
    if(!write){//限制了只有一个进程打开蓝牙

        while(!((*READ_IO(UART_BASE + lsr) & 0x00000001)==0x00000001));//等待用户输入数据，读取数据
		rxData = (unsigned int)*READ_IO(UART_BASE + rbr);
        *WRITE_IO(UART_BASE + thr) = rxData;//回显
		++count;
		*((int *)iob->io_base)=rxData & 0xff;
		iob->io_len=4;//生产了4个字符
		iob->io_resid=0;
        return 0;
    }
	else{
		return 0;
	}
}
int bluetooth_ioctl(struct device * dev, int op, void *data){
    return 0;
}
static void bluetooth_device_init(struct device *dev)//注册函数，信号灯
{
	memset(dev, 0, sizeof(*dev));
	dev->d_blocks = 0;
	dev->d_blocksize =0;
	dev->d_open = bluetooth_open;
	dev->d_close = bluetooth_close;
	dev->d_io = bluetooth_io;
	dev->d_ioctl = bluetooth_ioctl;
	sem_init(&(disk0_sem), 1);
    kprintf("bluetooth has been initialed\n\r\n\r");
}
/*
sfs_dirent_search_nolock(struct sfs_fs *sfs, struct sfs_inode *sin,
			 const char *name, uint32_t * ino_store, int *slot,
			 int *empty_slot)

static int
sfs_dirent_create_inode(struct sfs_fs *sfs, uint16_t type,
			struct inode **node_store)

static int
sfs_dirent_link_nolock(struct sfs_fs *sfs, struct sfs_inode *sin, int slot,
		       struct sfs_inode *lnksin, const char *name)
{
	int ret;
	if ((ret =
	     sfs_dirent_write_nolock(sfs, sin, slot, lnksin->ino, name)) != 0) {
		return ret;
	}
	sin->dirty = 1;
	sin->din->dirinfo.slots++;
	lnksin->dirty = 1;
	lnksin->din->nlinks++;
	return 0;
}

*/

