#include <types.h>
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

static semaphore_t car_sem;
static void car_device_init(struct device *dev);
static int usrs=0;
static int rxData;
#define PWM_BASE        0xB0700000
#define DIR_BASE        0xB0800000
void dev_init_car(void)
{
	struct inode *node;
	if ((node = dev_create_inode()) == NULL) {//得到一个空闲的inode号
		panic("car: dev_create_node.\n");
	}
	car_device_init(vop_info(node, device));//初始化device结构体

	int ret;
	if ((ret = vfs_add_dev("car", node, 0)) != 0) {//mountable=0!!
		panic("car: vfs_add_dev: %e.\n", ret);
	}
}

int car_open(struct device * dev, uint32_t open_flags){
    down(&(car_sem));
    if(usrs!=0) {
        kprintf("car is in use");
        return -E_BUSY;
    }
    usrs++;
    up(&(car_sem));
    return 0;
}

int car_close(struct device * dev){
    down(&(car_sem));
    usrs--;
    up(&(car_sem));
    return 0;
}
int car_io(struct device * dev, struct iobuf * iob, bool write){//写值到GPIO中
    //kprintf("car is in read or write");
    off_t offset = iob->io_offset;
	size_t resid = iob->io_resid;
    //kprintf("resid =%d\n\r", resid);
    if(write){
        int *begin=(int *)(iob->io_base);
        *WRITE_IO(PWM_BASE)=begin[0];
        *WRITE_IO(DIR_BASE)=begin[1];
        kprintf("PWM =%d, DIR=%d\n\r", begin[0],begin[1]);
        iob->io_len=8;
        iob->io_resid=0;
        return 0;
    }
    else{
        return 0;
    }


}
int car_ioctl(struct device * dev, int op, void *data){
    return 0;
}
static void car_device_init(struct device *dev)//注册函数，信号灯
{
	memset(dev, 0, sizeof(*dev));
	dev->d_blocks = 0;
	dev->d_blocksize =0;
	dev->d_open = car_open;
	dev->d_close = car_close;
	dev->d_io = car_io;
	dev->d_ioctl = car_ioctl;
	sem_init(&(car_sem), 1);
    kprintf("car has been initialed\n\r\n\r");
}