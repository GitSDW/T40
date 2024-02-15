#ifndef __GPIO_H__
#define __GPIO_H__


#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_OUTPUT 0
#define GPIO_INPUT 1
#define GPIO_HIGH  1
#define GPIO_LOW  0

#define GPIO_NONE  "none"
#define GPIO_FALLING "falling"
#define GPIO_RISING "rising"
#define GPIO_BOTH  "both"

#define SYSFS_GPIO_DIR "/sys/class/gpio"

#define PORTA	0
#define PORTB	32
#define PORTC	64
#define PORTD	96

int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_get_dir(unsigned int gpio);
int gpio_set_dir(unsigned int gpio, unsigned int dir, unsigned int val);
int gpio_get_val(unsigned int gpio);
int gpio_set_val(unsigned int gpio, unsigned int val);
int gpio_set_edge(unsigned int gpio, char *edge);
int gpio_open(unsigned int gpio);
int gpio_close(int fd);
int gpio_read(int fd);

#if __cplusplus
}
#endif

#endif /* __GPIO_H__ __GPIO_H__ */