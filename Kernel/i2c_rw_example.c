/* Register I2C device */

#define DRV_NAME  YOUR_DRV_NAME
struct i2c_board_info drv_i2c_board_info = {
        .type          = DRV_NAME,
        .flags         = 0x00,
		.addr          = 0x1A    // I2C device address
        .platform_data = NULL,
        .archdata      = NULL,
        .irq           = -1,
};
/* define I2C device info. */

int drv_i2c_register_device (void)
{
        int ret = 0;
        struct i2c_board_info *drv_i2c_bi;
        struct i2c_adapter *adapter = NULL;
        struct i2c_client *client   = NULL;
        /* assign this pointer to the true device board info. */
drv_i2c_bi = &drv_i2c_board_info;
	    int bus_num = 0;  
        /* get I2C adapter with I2C bus 0. */
adapter = i2c_get_adapter(bus_num);
        if (NULL == adapter) {
                printk("can not get i2c adapter, client address error\n");
                return -1;
        }
        /* register a new I2C device client with your drv board info on I2C bus 0. */
client = i2c_new_device(adapter, drv_i2c_bi);
        if (client == NULL) {
                printk("allocate i2c client failed\n");
                i2c_unregister_device(client);
                return -1;
        }
        /* release I2C adapter.*/
        i2c_put_adapter(adapter);  
        return 0;
}


/*
i2c_read(struct drv_data *drv, u8 addr, u8 *data, int len)
addr: I2C device register address
data: buffer
len: data length
*/
static int drv_i2c_read(struct drv_data *drv, u8 addr, u8 *data, int len)
{
        int err;

        struct i2c_msg msgs[] = {
                {/* Send read command*/
                 .addr = drv ->client->addr,
                 .flags = drv ->client->flags,
                 .len = 1,
                 .buf = &addr,
                 },
                {/* Read data and save to buf*/
                 .addr = drv ->client->addr,
                 .flags = (drv ->client->flags) | I2C_M_RD,
                 .len = len,
                 .buf = data,
                 },
        };
        /* execute two I2C message*/
        err = i2c_transfer(drv ->client->adapter, msgs, 2);
        /* returns negative errno, else the number of messages executed.*/
        if (err != 2)
                dev_err(&drv ->client->dev, "read transfer error\n");
        else
                err = 0;

        return err;
}


/*
i2c_write(struct drv _data * drv, u8 addr, u8 *data, int len)
addr: I2C device register address
data: buffer
len: data length
*/
static int drv _i2c_write(struct drv _data * drv, u8 addr, u8 *data, int len)
{
        int err;
        int i;
        u8 buf[len + 1]; /*address + write data*/

        struct i2c_msg msgs[] = {
                {
                 .addr = drv ->client->addr,
                 .flags = drv ->client->flags,
                 .len = len + 1, /*address + write data*/
                 .buf = buf,
                 },
        };

        buf[0] = addr;
        for (i = 0; i < len; i++) {
                buf[i + 1] = data[i];
        }
        /* execute one I2C message*/
        err = i2c_transfer(ti9->client->adapter, msgs, 1);
        /* returns negative errno, else the number of messages executed.*/
        if (err != 1)
                dev_err(&drv ->client->dev, "write transfer error\n");
        else
                err = 0;

        return err;
}



