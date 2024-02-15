#ifndef __ADC_H__
#define __ADC_H__


#ifdef __cplusplus
extern "C" {
#endif

/* define ioctl command. they are fixed. don't modify! */
#define ADC_ENABLE 		0
#define ADC_DISABLE 	1
#define ADC_SET_VREF 	2
#define ADC_PATH_LEN    32

#define ADC_PATH    "/dev/ingenic_adc_aux_0" /* adc channal 0-3 */
#define STD_VAL_VOLTAGE 1800 /* The unit is mv/1000. T10/T20 VREF=3300; T30/T21/T31/T40 VREF=1800 */

#define ENABLE2DISABLE_TIME_DEBUG

int adc_init(void);
int adc_deinit(void);
void *adc_get_voltage_thread(void *argc);

#if __cplusplus
}
#endif

#endif /* __ADC_H__ */