/*
 * Author: Manivannan Sadhasivam <manivannan.sadhsivam@linaro.org>
 * Copyright (c) 2017 Linaro Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "mraa/gpio_chardev.h"
#include "mraa_internal.h"

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/gpio.h>

mraa_result_t
mraa_gpio_ioctl(int fd, unsigned long req_flag, void *value)
{
    int status;

    status = ioctl(fd, req_flag, value);
    if (status < 0) {
        syslog(LOG_ERR, "gpio: ioctl call failed: %s", strerror(errno));
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

void
print_version()
{   
    fprintf(stdout, "Version %s on %s\n", mraa_get_version(), mraa_get_platform_name());
}

void
print_help()
{
    fprintf(stdout, "list pins         List pins for a gpiochip\n");
    fprintf(stdout, "set pin level     Set pin to level (0/1)\n");
    fprintf(stdout, "get pin           Get pin level\n");
    fprintf(stdout, "version           Get mraa version and board name\n");
}

void
print_command_error()
{
    fprintf(stdout, "Invalid command, options are:\n");
    print_help();
}

int
list_pins(int chip)
{
	mraa_result_t res;

    mraa_gpio_char_context dev = (mraa_gpio_char_context) malloc(sizeof(struct _gpio));
    if (dev == NULL) {
        syslog(LOG_CRIT, "gpio: Failed to allocate memory for context");
        return -1;
    }

    char filepath[MAX_BUF_SIZE];
    snprintf(filepath, MAX_BUF_SIZE, CDEV_PREFIX "%d", dev->chip);

    int fd = open(filepath, O_RDWR);
    if (fd == -1) {
        syslog(LOG_ERR, "gpio: Failed to open gpio chip%i: %s", chip, strerror(errno));
        free(dev);
        return -1;
    }

    res = mraa_gpio_ioctl(dev->chip_fd, GPIO_GET_CHIPINFO_IOCTL, &dev->cinfo);
    if (res != MRAA_SUCCESS) {
        syslog(LOG_ERR, "gpio: ioctl call failed for chip%i: %s", dev->chip, strerror(errno));
        close(dev->chip_fd);
        free(dev);
        return -1;
    }

	fprintf(stdout, "Total number of gpio lines in chip %d is: %d", chip, dev->cinfo.lines);

	close(fd);
	free(dev);
	return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        print_command_error();
    }

    if (argc > 1) {
		if (strcmp(argv[1], "list") == 0) {
            list_pins(atoi(argv[1]));
		} else 
			print_command_error();
	}
	
	return 0;
}
