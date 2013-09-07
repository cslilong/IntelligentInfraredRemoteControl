#include <linux/module.h>/*MODULE_AUTHOR();MODULE_LICENSE()*/
//#include <linux/kernel.h>
#include <linux/fs.h>/*struct file_operations*/
#include <linux/device.h>/*class/device_create/destrory()*/
#include <linux/slab.h>/*kmalloc(),kfree()*/
#include <linux/irq.h>/*IRQ_TYPE*/
#include <linux/interrupt.h>/*IRQF_DISABLED*/
#include <asm-arch/include/asm/io.h>/*ioremap()*/
#include <asm-arch/include/asm/uaccess.h>/*copy_to/from_user()*/
#include <asm-arch/plat-s3c64xx/include/plat/irqs.h>/*IRQ_ENT(x)*/

#define GPACON 0x7f008000
#define GPADAT 0x7f008004
#define GPAPUD 0x7f008008
#define GPLCON1 0x7f008814
#define GPLDAT  0x7f008818
#define GPLPUD  0x7f00881c
#define GPNCON  0x7f008830
#define GPNDAT  0x7f008834
#define GPNPUD  0x7f008838

#define VIC0IRQSTATUS	0x71200000
#define VIC0INTSELECT	0x7120000c
#define VIC0INTENABLE	0x71200010

#define TCFG0 0x7f006000
#define TCFG1 0x7f006004
#define TCON 0x7f006008
#define TCNTB0 0x7f00600c
#define TCMPB0 0x7f006010
#define TCNTO0 0x7f006014
#define TCNTB1 0x7f006018
#define TCMPB1 0x7f00601C
#define TCNTO1 0x7f006020
#define INIT_CSTAT 0x7f006044

#define IR_1 297000
#define IR_2 148500
#define IR_3 17820
#define IR_4 19800
#define IR_5 57750
#define IR_6 1287000
#define IR_7 66000
#define IR_DEFAULT 289

#define MAX_IR_CMD 	15
#define MAX_SEND_OFF 75*MAX_IR_CMD

#define LED3_OPEN *gpldat = (*gpldat)&(~(0x1 << 13))
#define LED3_CLOSE *gpldat = (*gpldat)|(0x1 << 13)
#define LED4_OPEN *gpndat = (*gpndat)&(~(0x1 << 6))
#define LED4_CLOSE	*gpndat = (*gpndat)|(0x1 << 6)
#define IR_HL	*gpadat = (*gpadat)|(0x1 << 6)
#define IR_LL	*gpadat = (*gpadat)&(~(0x1 << 6))

static struct class * timer_class;
static struct device * timer_device;
volatile static int timer_major;
volatile static dev_t timer_lma;
volatile static int leds_stat= 0;
volatile static unsigned long time_cnt = 0;

volatile static unsigned char t38k_flag = 0;
volatile static unsigned char tcod_flag = 0;
volatile static unsigned char tout_flag = 0;
volatile static unsigned long t0_cnt = 0;
volatile static unsigned long t1_cnt = 0;
volatile static unsigned char ir_send_off = 0;
volatile static unsigned char ir_cmd_off = 0;
volatile static unsigned char ir_power[MAX_IR_CMD][MAX_SEND_OFF] = 
{
//0:3,4                  1:3,5
//power
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,4,3,4,3,5,3,5,3,5,3,4,3,4,3,4,
	3,5,3,5,3,4,3,4,3,4,3,5,3,5,3,5,
	3,6,1,7,3,0,0,0,0},
//ch+	
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,5,3,4,3,5,3,4,3,5,3,4,3,5,3,4,
	3,4,3,5,3,4,3,5,3,4,3,5,3,4,3,5,
	3,6,1,7,3,0,0,0,0},
//ch-	
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,4,3,5,3,4,3,4,3,5,3,4,3,5,3,4,
	3,5,3,4,3,5,3,5,3,4,3,5,3,4,3,5,
	3,6,1,7,3,0,0,0,0},
//Vol+	
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,4,3,4,3,4,3,5,3,4,3,4,3,5,3,4,
	3,5,3,5,3,5,3,4,3,5,3,5,3,4,3,5,
	3,6,1,7,3,0,0,0,0},
//Vol-	
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,5,3,5,3,5,3,5,3,5,3,4,3,4,3,4,
	3,4,3,4,3,4,3,4,3,4,3,5,3,5,3,5,
	3,6,1,7,3,0,0,0,0},
//	1
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,5,3,5,3,5,3,5,3,4,3,4,3,4,3,4,
	3,4,3,4,3,4,3,4,3,5,3,5,3,5,3,5,
	3,6,1,7,3,0,0,0,0},
//	2 72 0111 0010
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,4,3,5,3,5,3,5,3,4,3,4,3,5,3,4,
	3,5,3,4,3,4,3,4,3,5,3,5,3,4,3,5,
	3,6,1,7,3,0,0,0,0},
//	3 62 0110 0010
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,4,3,5,3,5,3,4,3,4,3,4,3,5,3,4,
	3,5,3,4,3,4,3,5,3,5,3,5,3,4,3,5,
	3,6,1,7,3,0,0,0,0},
//	4
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,4,3,4,3,5,3,5,3,4,3,4,3,4,3,4,
	3,5,3,5,3,4,3,4,3,5,3,5,3,5,3,5,
	3,6,1,7,3,0,0,0,0},
//	5
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,4,3,5,3,5,3,4,3,4,3,4,3,4,3,4,
	3,5,3,4,3,4,3,5,3,5,3,5,3,5,3,5,
	3,6,1,7,3,0,0,0,0},
//	6
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,5,3,4,3,5,3,4,3,4,3,4,3,4,3,4,
	3,4,3,5,3,4,3,5,3,5,3,5,3,5,3,5,
	3,6,1,7,3,0,0,0,0},
//	7
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,5,3,5,3,5,3,5,3,4,3,4,3,5,3,4,
	3,4,3,4,3,4,3,4,3,5,3,5,3,4,3,5,
	3,6,1,7,3,0,0,0,0},
//	8
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,5,3,4,3,5,3,5,3,4,3,4,3,5,3,4,
	3,4,3,5,3,4,3,4,3,5,3,5,3,4,3,5,
	3,6,1,7,3,0,0,0,0},
//	9
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,4,3,4,3,5,3,5,3,4,3,4,3,5,3,4,
	3,5,3,5,3,4,3,4,3,5,3,5,3,4,3,5,
	3,6,1,7,3,0,0,0,0},
//	0
	{1,2,
	3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,5,3,5,3,5,3,5,3,5,
	3,4,3,4,3,4,3,5,3,4,3,4,3,4,3,4,
	3,5,3,5,3,5,3,4,3,5,3,5,3,5,3,5,
	3,6,1,7,3,0,0,0,0}

};


//volatile static int key_press_cd = 0;
//volatile static	char key_val[5] = {0};

volatile unsigned long * gpacon = NULL;
volatile unsigned long * gpadat = NULL;
volatile unsigned long * gpapud = NULL;
volatile unsigned long * gplcon1 = NULL;
volatile unsigned long * gpldat = NULL;
volatile unsigned long * gplpud = NULL;
volatile unsigned long * gpncon = NULL;
volatile unsigned long * gpndat = NULL;
volatile unsigned long * gpnpud = NULL;
volatile unsigned long * vic0irqstatus = NULL;
volatile unsigned long * vic0intselect = NULL;
volatile unsigned long * vic0intenable = NULL;
volatile unsigned long * tcfg0 = NULL;
volatile unsigned long * tcfg1 = NULL;
volatile unsigned long * tcon = NULL;
volatile unsigned long * tcntb0 = NULL;
volatile unsigned long * tcmpb0 = NULL;
volatile unsigned long * tcnto0 = NULL;
volatile unsigned long * tcntb1 = NULL;
volatile unsigned long * tcmpb1 = NULL;
volatile unsigned long * tcnto1 = NULL;
volatile unsigned long * init_cstat = NULL;



static DECLARE_WAIT_QUEUE_HEAD(timer_waitq);

irqreturn_t timer_irq0(int irq_num, void * dev_id)
{
	if(t0_cnt%3 == 0)
	{
		t0_cnt = 0;
		t38k_flag = 1;
		//todo gpio high
		//IR_HL;
	}
	else
	{
		//todo gpio low
		t38k_flag = 0;
		//IR_LL;
	}
	tout_flag = t38k_flag&tcod_flag;
	if(tout_flag)
	{
		IR_HL;
	}
	else
	{
		IR_LL;
	}
	t0_cnt++;
	return IRQ_HANDLED;
}

irqreturn_t timer_irq1(int irq_num, void * dev_id)
{
	unsigned char tmp;
	if((ir_send_off <= MAX_SEND_OFF)&&(ir_send_off > 0))
	{
		tmp = ir_power[ir_cmd_off][ir_send_off-1];
		switch(tmp)
		{
		case 1:
			*tcntb1 = IR_1;
			tcod_flag = 1;
			break;
		case 2:
			*tcntb1 = IR_2;
			tcod_flag = 0;
			break;
		case 3:
			*tcntb1 = IR_3;
			tcod_flag = 1;
			break;
		case 4:
			*tcntb1 = IR_4;
			tcod_flag = 0;
			break;
		case 5:
			*tcntb1 = IR_5;
			tcod_flag = 0;
			break;
		case 6:
			*tcntb1 = IR_6;
			tcod_flag = 0;
			break;
		case 7:
			*tcntb1 = IR_7;
			tcod_flag = 0;
			break;
		default:
			*tcntb1 = IR_DEFAULT;
			tcod_flag = 0;
			break;
		}
		ir_send_off++;
	}
	else
	{
		*tcntb1 = IR_DEFAULT;
		ir_cmd_off = 0;
		ir_send_off = 0;
		tcod_flag = 0;
	}
	/*
	if(tcod_flag)
	{
		IR_HL;
	}
	else
	{
		IR_LL;
	}*/
	*tcon = (*tcon)&(~(0xf << 8))|(0x2 << 8);	
	*tcon = (*tcon)&(~(0x3 << 8))|(0x1 << 8);
	return IRQ_HANDLED;
}


static int timer_open(struct inode *timer_inode,struct file *timer_file)
{
	int ret,i;
	printk("\nOPEN!\n");
	//ioremap register
	gpacon = (unsigned long *)ioremap(GPACON,4);
	gpadat = (unsigned long *)ioremap(GPADAT,4);
	gpapud = (unsigned long *)ioremap(GPAPUD,4);
	gplcon1 = (unsigned long *)ioremap(GPLCON1,4);
	gpldat = (unsigned long *)ioremap(GPLDAT,4);
	gplpud = (unsigned long *)ioremap(GPLPUD,4);
	gpncon = (unsigned long *)ioremap(GPNCON,4);
	gpndat = (unsigned long *)ioremap(GPNDAT,4);
	gpnpud = (unsigned long *)ioremap(GPNPUD,4);
	vic0irqstatus = (unsigned long *)ioremap(VIC0IRQSTATUS,4);
	vic0intselect = (unsigned long *)ioremap(VIC0INTSELECT,4);
	vic0intenable = (unsigned long *)ioremap(VIC0INTENABLE,4);
	tcfg0 = (unsigned long *)ioremap(TCFG0,4);
	tcfg1 = (unsigned long *)ioremap(TCFG1,4);
	tcon = (unsigned long *)ioremap(TCON,4);
	tcntb0 = (unsigned long *)ioremap(TCNTB0,4);
	tcmpb0 = (unsigned long *)ioremap(TCMPB0,4);
	tcnto0 = (unsigned long *)ioremap(TCNTO0,4);
	tcntb1 = (unsigned long *)ioremap(TCNTB1,4);
	tcmpb1 = (unsigned long *)ioremap(TCMPB1,4);
	tcnto1 = (unsigned long *)ioremap(TCNTO1,4);
	init_cstat = (unsigned long *)ioremap(INIT_CSTAT,4);
	//config register
		//init GPIO for leds
	*gpacon = (*gpacon)&(~(0xf << 24))|(0x01 << 24);
	*gpapud = (*gpapud)&(~(0x3) << 12)|(0x01 << 12);
	//*gplcon1 = (*gplcon1)&(~(0xf << 20))|(0x1 << 20);
	//*gplpud = (*gplpud)&(~(0x3 << 26));
	//*gpncon = (*gpncon)&(~(0x3 << 12))|(0x1 << 12);
	//*gpnpud = (*gplpud)&(~(0x3 << 12));
		//0:led open 1:led close 
		//led3 open led4 close
	//LED3_OPEN;
	//LED4_OPEN;
		//init GPIO for pwm output
			//pwm out don't care now...
		//config timer reg
			//step1. Prescaler 0 = 255;Divider MUX0/1 = 1/16;Timer0/1 AutoReload
	//*tcfg0 = (*tcfg0)&(0xffffff00)|(0x01);
	*tcfg0 = (*tcfg0)&(0xffffff00)|(0x01);
	*tcfg1 = (*tcfg1)&(0xffffff00);
	*tcon = (*tcon)&(~(0xf << 8));
	*tcon = (*tcon)&(~(0xf))|(0x8);
			//step2. Interrupt Cycle = 1s;Inverter 50%;
	//*tcntb0 = 869;
	*tcntb0 = 289;
	//*tcmpb0 = 289;
	*tcmpb0 = 145;
	*tcntb1 = 289;
	*tcmpb1 = 145;
			//step3. Enable timer0/1 interrupt;
				//config timer0/1 as irq
	*vic0intselect = (*vic0intselect)&(~(0x03 << 23));
				//enable interrupt
	*vic0intenable = (*vic0intenable)|=(0x03 << 23);
	*init_cstat = (*init_cstat)|(0x3);
			//step4. update tcntb0/1,tcmpb0/1;
	*tcon = (*tcon)&(~(0xf << 8))|(0x2 << 8);
	*tcon = (*tcon)&(~(0xf))|(0xa);
	//*tcon = (*tcon)|(0x1 << 8)|(0x1);
			//step5. request irq	
	ret = request_irq(IRQ_TIMER0,timer_irq0,IRQ_TYPE_EDGE_FALLING,"timer0",1);
	if(ret)
	{
		printk("request irq_timer0 error:%d\n",ret);
	}
	ret = request_irq(IRQ_TIMER1,timer_irq1,IRQ_TYPE_EDGE_FALLING,"timer1",2);
	if(ret)
	{
		printk("request irq_timer1 error:%d\n",ret);
	}
			//step6. start timer1
	*tcon = (*tcon)&(~(0x03 << 8))|(0x1 << 8);
			//delay timer0 for sometime than start it
	*tcon = (*tcon)&(~(0x03))|(0x1);
	//printk("vic0irqstatus:%x\nvic0intselect:%x\nvic0intenable:%x\n",*vic0irqstatus,*vic0intselect,*vic0intenable);
	//printk("init_cstat:%x\ntcon:%x\n",*init_cstat,*tcon);
	
	return 0;
}

/*
ssize_t timer_read(struct file * timer_file_p, char __user * user_buf, size_t buf_size, loff_t * buf_loff)
{
	return 0;
}
	
ssize_t timer_write(struct file * timer_file_p, const char __user * user_buf, size_t buf_size, loff_t * buf_loff)
{
	return 0;
}
*/

int timer_ioctl (struct inode *timer_inode, struct file *timer_file_p, unsigned int cmd, unsigned long arglen)
{
	if(cmd == 0x1c)
	{
		ir_cmd_off = 0;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:POWER(0x1C)... \n");
	}
	else if(cmd == 0x55)
	{
		ir_cmd_off = 1;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:CH+(0x55)... \n");
	}
	else if(cmd == 0x52)
	{
		ir_cmd_off = 2;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:CH-(0x52)... \n");
	}
	else if(cmd == 0x48)
	{
		ir_cmd_off = 3;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:VOL+(0x48)... \n");
	}
	else if(cmd == 0x1f)
	{
		ir_cmd_off = 4;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:VOL-(0x1f)... \n");
	}
	
	else if(cmd == 0xf0)
	{
		ir_cmd_off = 5;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:1... \n");
	}
		else if(cmd == 0x72)
	{
		ir_cmd_off = 6;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:2... \n");
	}
		else if(cmd == 0x62)
	{
		ir_cmd_off = 7;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:3... \n");
	}
		else if(cmd == 0x30)
	{
		ir_cmd_off = 8;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:4... \n");
	}
		else if(cmd == 0x60)
	{
		ir_cmd_off = 9;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:5... \n");
	}
		else if(cmd == 0xa0)
	{
		ir_cmd_off = 10;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:6... \n");
	}
		else if(cmd == 0xf2)
	{
		ir_cmd_off = 11;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:7... \n");
	}
		else if(cmd == 0xb2)
	{
		ir_cmd_off = 12;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:8... \n");
	}
		else if(cmd == 0x32)
	{
		ir_cmd_off = 13;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:9... \n");
	}
		else if(cmd == 0x10)
	{
		ir_cmd_off = 14;
		ir_send_off = 1;
		printk("\nexecute io_ctrl cmd:0... \n");
	}
	else
	{
		ir_send_off = 0;
		printk("\nillegle io_ctrl cmd... \n");
	}
	return 0;
}


int timer_close (struct inode *timer_inode, struct file *timer_file)
{
	printk("\nCLOSE!\n");
	//unrequest irq
	free_irq(IRQ_TIMER0,1);
	free_irq(IRQ_TIMER1,2);
	//iounmap
	iounmap(gpacon);
	iounmap(gpadat);
	iounmap(gpapud);
	iounmap(gplcon1);
	iounmap(gpldat);
	iounmap(gplpud);
	iounmap(gpncon);
	iounmap(gpndat);
	iounmap(gpnpud);
	iounmap(vic0irqstatus);
	iounmap(vic0intselect);
	iounmap(vic0intenable);
	iounmap(tcfg0);
	iounmap(tcfg1);
	iounmap(tcon);
	iounmap(tcntb0);
	iounmap(tcmpb0);
	iounmap(tcnto0);
	iounmap(tcntb1);
	iounmap(tcmpb1);
	iounmap(tcnto1);
	iounmap(init_cstat);

	return 0;
}

static struct file_operations s3c6410_timer_fops = 
{
	.owner = THIS_MODULE,
	.open = timer_open,
//	.write = timer_write,
//	.read = timer_read,
	.ioctl = timer_ioctl,
	.release = timer_close,
};

static int __init s3c6410_timer_init(void)
{
	//register driver and get major
	timer_major = register_chrdev(0,"S3cPwmTimer",&s3c6410_timer_fops);
	printk("s3c6410_timer init!\nMajor:%d\n",timer_major);
	//auto create device info for mdev
	timer_class = class_create(THIS_MODULE,"timer_class");
	timer_lma = MKDEV(timer_major,0);
	printk("Logic Major : %d\n",timer_lma);
	timer_device = device_create(timer_class,NULL,timer_lma,NULL,"timer_device");
	printk("create timer_calss&timer_device\n");
	
	return 0;
}

static void __exit s3c6410_timer_exit(void)
{
	//destroy device info
	device_destroy(timer_class,timer_lma);
	class_destroy(timer_class);
	//unregister driver
	unregister_chrdev(timer_major,"s3c6410_timer");
	printk("destroy timer_calss&timer_device\n");
	printk("s3c6410_timer exit!\n");
}


module_init(s3c6410_timer_init);
module_exit(s3c6410_timer_exit);

MODULE_AUTHOR("Darmac");
MODULE_LICENSE("GPL");

