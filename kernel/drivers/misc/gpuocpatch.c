/* drivers/misc/gpuocpatch.c
 *
 * Copyright 2013 XpauM gpu oc adreno 205 driver
 *
 */

#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/gpuocpatch.h>
#include <linux/mutex.h>

bool gpu_oc = true; /* gpu oc */
struct kgsl_device* kgsl_dev= NULL;

//kgsl dev
void set_gpuoc_dev(struct kgsl_device* dev){
	kgsl_dev=dev;
}

/* enable disable */
static ssize_t gpu_status_read(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", (gpu_oc ? 1 : 0));
}

static ssize_t gpu_status_write(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size){

	unsigned int data;
	if(sscanf(buf, "%u\n", &data) == 1) {
		if (data == 1) {
			pr_info("%s: gpu oc enabled\n", __FUNCTION__);
			gpu_oc = true;
			kgsl_dev->pwrctrl.clk_freq[3] = 368640000; // 3 == KGSL_MAX_FREQ
		} else if (data == 0) {
			pr_info("%s: gpu oc disabled\n", __FUNCTION__);
			gpu_oc = false;
			kgsl_dev->pwrctrl.clk_freq[3] = 245760000;
		} else {
			pr_info("%s: invalid input range %u\n", __FUNCTION__,
					data);
		}
	} else {
		pr_info("%s: invalid input\n", __FUNCTION__);
	}

	return size;
}


/* attrs */
static DEVICE_ATTR(enabled, S_IRUGO | S_IWUGO,
		gpu_status_read,
		gpu_status_write);


static struct attribute *gpu_notification_attributes[] = {
	&dev_attr_enabled.attr,
	NULL
};

static struct attribute_group gpu_notification_group = {
	.attrs  = gpu_notification_attributes,
};


//init
static struct miscdevice gpu_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "gpuocpatch",
};

static int __init gpu_oc_patch_control_init(void)
{
	int ret;

	pr_info("%s misc_register(%s)\n", __FUNCTION__, gpu_device.name);
	ret = misc_register(&gpu_device);
	if (ret) {
		pr_err("%s misc_register(%s) fail\n", __FUNCTION__,
				gpu_device.name);
		return 1;
	}

	if (sysfs_create_group(&gpu_device.this_device->kobj,
				&gpu_notification_group) < 0) {
		pr_err("%s sysfs_create_group fail\n", __FUNCTION__);
		pr_err("Failed to create sysfs group for device (%s)!\n",
				gpu_device.name);
	}

	return 0;
}

device_initcall(gpu_oc_patch_control_init);
