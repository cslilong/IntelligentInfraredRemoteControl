/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "MokoidLedStub"

#include <hardware/hardware.h>

#include <fcntl.h>
#include <errno.h>

#include <cutils/log.h>
#include <cutils/atomic.h>

#include <mokoid/led.h>

/*****************************************************************************/
int fd;
#define GPG3DAT2_ON  0x4800
#define GPG3DAT2_OFF 0x4801

int led_device_close(struct hw_device_t* device)
{
	struct led_control_device_t* ctx = (struct led_control_device_t*)device;
	if (ctx) {
		free(ctx);
	}
	return 0;
}

int led_on(struct led_control_device_t *dev, int32_t led)
{
	int i;
	LOGI("LED Stub: set %x on.", led);
	//ioctl(fd,GPG3DAT2_ON,NULL);
	LOGI("# fd == %d .",fd);
	ioctl(fd,led,NULL);
	for(i = 0;i < 2000000;++i);
	ioctl(fd,led,NULL);
	LOGI("LED Stub: set on end.");
	return 0;
}

int led_off(struct led_control_device_t *dev, int32_t led)
{
	LOGI("LED Stub: set %d off.", led);
	ioctl(fd,GPG3DAT2_OFF,NULL);
	return 0;
}

static int led_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device) 
{
	struct led_control_device_t *dev;

	dev = (struct led_control_device_t *)malloc(sizeof(*dev));
	memset(dev, 0, sizeof(*dev));

	dev->common.tag =  HARDWARE_DEVICE_TAG;
	dev->common.version = 0;
	dev->common.module = module;
	dev->common.close = led_device_close;

	dev->set_on = led_on;
	dev->set_off = led_off;

	*device = &dev->common;
	fd = open("/dev/timer_device",O_RDWR);
	if(fd==-1)      //open device 
        {
                LOGE("LED open error");
        }
        else
                LOGI("open ok");

success:
	return 0;
}

static struct hw_module_methods_t led_module_methods = {
    open: led_device_open
};

const struct led_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: LED_HARDWARE_MODULE_ID,
        name: "Sample LED Stub",
        author: "The Mokoid Open Source Project",
        methods: &led_module_methods,
    }
    /* supporting APIs go here */
};

