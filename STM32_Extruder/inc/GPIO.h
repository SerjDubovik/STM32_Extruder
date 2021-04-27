#ifndef SRC_GPIO
#define SRC_GPIO

#define on			1
#define off			0

void init_GPIO(void);
void relay_1(unsigned int set);
void relay_2(unsigned int set);
void red_led(unsigned int set);
void blue_led(unsigned int set);

#endif // SRC_GPIO
