/*****************************************************************************
  Module Name:

    $Workfile: gpio_rw.c $

  Abstract:

    This program is a test/sample program to set/get gpio value.

  Compile with:

    arm_1103_le-gcc gpio_rw.c -o gpio_rw

  Usage:
        //mknod /dev/gpio c 255 0
        chmod 777 gpio_rw
        ./gpio_rw

  Revision History:

    First created by DavidRau.

*******************************************************************************/

#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>

#define GPIO_DEVICE_PATH "/dev/gpio"

/*-------------------- EXPORTED PRIVATE TYPES---------------------------------*/
struct gpio_reg_op_t{
        unsigned int addr;
        unsigned int bitmap;
        unsigned int regval;
};
/*
struct gpio_operation_t {
        struct  gpio_reg_op_t ctl;
        struct  gpio_reg_op_t oc;
        struct  gpio_reg_op_t od;
        struct  gpio_reg_op_t id;
};
*/
struct gpio_cfg_t {
        struct  gpio_reg_op_t ctl;
        struct  gpio_reg_op_t oc;
};

/*--------------------- EXPORTED PRIVATE MACROS -------------------------------*/
#define GPIO_IOC_MAGIC  '6'

#define GPIOCFG _IOW(GPIO_IOC_MAGIC, 1, void *)
#define GPIOWREG _IOW(GPIO_IOC_MAGIC, 2, void *)
#define GPIORREG _IOWR(GPIO_IOC_MAGIC, 3, void *)

#define FREESYSCACHES _IO(GPIO_IOC_MAGIC, 4)


#define GPIO_IOC_MAXNR  5


struct gpio_cfg_t cfg;
struct gpio_reg_op_t regop;


//-------------------------------------------------------------------------------
//				Write Function
//-------------------------------------------------------------------------------
int set_gpio_reg(void)
{
	int fd = -1;
	int ret;

	fd = open(GPIO_DEVICE_PATH, O_RDWR);
	printf("open %s device\n", GPIO_DEVICE_PATH);
	if(fd == -1){
		printf("can't open %s device\n", GPIO_DEVICE_PATH);
		return -1;
	}
	printf("GPIO driver %s Release successed!!\n", GPIO_DEVICE_PATH);
	
	//Enable GPIO
	printf("Enter GPIO Control Address (Ex:d8110040): ");
        scanf("%x",&(cfg.ctl.addr));
	printf("Enter GPIO Control bit (Ex: 1-32): ");
        scanf("%x",&(cfg.ctl.bitmap));
	printf("Enable GPIO or not (Ex: 1/0): ");
        scanf("%x",&(cfg.ctl.regval));
	
	if(cfg.ctl.regval == 0x0)
	{
		printf("GPIO %x is disabled... exit GPIO control!", cfg.ctl.bitmap);
		return -1;
	}

	//Set GPIO is input or output
	printf("Enter GPIO Mode Address (Ex:d8110080): ");
        scanf("%x",&(cfg.oc.addr));
	printf("Enter GPIO output mode bit (Ex: 1-32): ");
        scanf("%x",&(cfg.oc.bitmap));
        printf("Enable output mode or not (Ex:1/0): ");
        scanf("%x",&(cfg.oc.regval));

	ret = ioctl(fd, GPIOCFG, &cfg);

	printf("Enter GPIO output Address (Ex:d81100C0): ");
	scanf("%x",&(regop.addr));
	printf("Enter GPIO output bit (Ex:1-32): ");
	scanf("%x",&(regop.bitmap));
	printf("Enter GPIO output value (Ex:0/1): ");
	scanf("%x",&(regop.regval));
	printf("Input GPIO addr= %x  offset= %x  data= %x\n", regop.addr, regop.bitmap, regop.regval);
	
	ret = ioctl(fd, GPIOWREG, &regop);

	close(fd);
	return ret;
}
//-------------------------------------------------------------------------------
//				Read Function
//-------------------------------------------------------------------------------
int get_gpio_reg(void)
{
	int fd = -1;
	int ret;

	fd = open(GPIO_DEVICE_PATH, O_RDWR);
	printf("open %s device\n", GPIO_DEVICE_PATH);
	if(fd == -1){
		printf("can't open %s device\n", GPIO_DEVICE_PATH);
		return -1;
	}
	printf("gpio driver %s Release successed!!\n", GPIO_DEVICE_PATH);
	
	printf("Enter GPIO Address (Ex:01): ");
	scanf("%x",&(regop.addr));
	//printf("Enter GPIO Offset (Ex:01): ");
	//scanf("%x",&(regop.bitmap));
	printf("Enter Address (Ex:01): ");;
	scanf("%x",&(regop.regval));
	printf("Input GPIO addr= %x  offset= %x  data= %x\n", regop.addr, regop.bitmap, regop.regval);
	
	ret = ioctl(fd, GPIORREG, &regop);

	close(fd);
	return ret;
}
//-------------------------------------------------------------------------------
//				Main Function
//-------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int ret;
	unsigned int select = 0;

	do {
		printf("**************************** \n\r");
		printf("\t  0) EXIT \n\r");
		printf("\t  1) Set GPIO Reg \n\r");
		printf("\t  2) Get GPIO Reg \n\r");
		printf("**************************** \n\r");
		printf("Please input item:");
	       	scanf("%d", &select); 
		switch(select)
		{
			case 1:
				set_gpio_reg();
				break;
			case 2:
				get_gpio_reg();
				
		}
	}while(select!=0);

	return ret;
}
