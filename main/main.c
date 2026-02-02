#include "esp_rom_sys.h"	
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define BLINK_GPIO 2
#define SDA 3
#define SCK 4
#define SDA_C 5
#define SCK_C 6

bool sck_low = false;

void half_cycle_sck()
{
	//esp_rom_delay_us(5);
	vTaskDelay(2);
	if(!sck_low)
	{
		gpio_set_direction(SCK, GPIO_MODE_OUTPUT);
		gpio_set_level(SCK, 0);
	}
	else
	{
		gpio_set_direction(SCK, GPIO_MODE_INPUT);
	}
	sck_low = !sck_low;
	vTaskDelay(2);
	//esp_rom_delay_us(5); 
}

void monitor(void* param)
{
	while(1)
	{
		int sck_r = gpio_get_level(SCK_C);
		int sda_r = gpio_get_level(SDA_C);
		//printf("sck = %d, sda = %d\n", sck_r, sda_r);
		vTaskDelay(1);
	}
}

void set_sda(int bit_state)
{
	if(bit_state == 0)
	{
		gpio_set_direction(SDA, GPIO_MODE_OUTPUT);
		gpio_set_level(SDA, 0);
	}
	else
	{
		gpio_set_direction(SDA, GPIO_MODE_INPUT);
	}
}

void init()
{
	printf("init\n");
	set_sda(1);
	gpio_set_direction(SCK, GPIO_MODE_OUTPUT);
	gpio_set_level(SCK, 0);
	gpio_set_direction(SDA_C, GPIO_MODE_INPUT);
	gpio_set_direction(SCK_C, GPIO_MODE_INPUT);
	sck_low = true;
	gpio_set_pull_mode(SDA, GPIO_PULLUP_ONLY);
	gpio_set_pull_mode(SCK, GPIO_PULLUP_ONLY);
}

void start()
{
	printf("Starting\n");
	set_sda(1);
	half_cycle_sck();
	if(sck_low)
	{
		printf("ifwait about to start\n");
		half_cycle_sck();
	}
	set_sda(0);
	half_cycle_sck(); //start finish with sck high (like write)	
}

void stop()
{
	printf("stop\n");
	half_cycle_sck();//wait for sck to go low
	set_sda(0); //set low to be able to lift it up when sck will be high
	half_cycle_sck(); //and high to the second pulseup
	set_sda(1); //pull sda high in the middle of sck high pulse
}

void write_bit(int bit_state)
{
	if(!sck_low) //if sck == high
	{
		half_cycle_sck(); //will be hit when previos was write
	}
	set_sda(bit_state);
	half_cycle_sck();//write finish with sck high
}

void write_byte(int number)
{
	for(int i = 7; i >= 0; i--)
	{
		int bit = number >> i & 1;
		//printf("bit = %d\n", bit);
		write_bit(bit);
	}
}

void switch_to_read()
{
	half_cycle_sck(); //we cannot change sda while sck is high
	set_sda(1); //set sda to read
}

void switch_to_write()
{
	set_sda(0);//now wait, we are at low sck (returned from read)
}

int read_bit()
{
	half_cycle_sck();
	if(sck_low)
	{
		printf("Reading while low!\n");
	}
	int sda_bit = gpio_get_level(SDA);
	half_cycle_sck(); //read finish with sck low
	return sda_bit;
}

int  read_byte()
{
	int result = 0;
	for(int i = 7; i >= 0; i--)
	{
		int bit = read_bit();
		printf("bit = %d\n", bit);
		result += (bit << i);
	}
	return result;
}

signed int convert_to_signed(__int16_t raw)
{
	int negative_indicator = 1;
	if((raw >> 15) & 1)
	{
		printf("Negative no detectedi\n");
		negative_indicator = -1;
		raw -= 1;
		raw = ~raw;
		raw <<= 1;
		raw >>= 1;
		return negative_indicator * raw;
	}
	else
	{
		return raw;
	}
	
}

void app_main(void)
{
	int address = 104;
	printf("hello\n");
	printf("%d\n", address);
	TaskHandle_t handle;
	xTaskCreate(monitor, "monitor", 2048, NULL, 5, &handle);
	vTaskDelay(1);
	init();
	vTaskDelay(3);
	
	//need to wake up from sleep	
	start();
	write_byte((address << 1) + 0);
	switch_to_read();
	int ack = read_bit();
	printf("ack01: %d\n", ack);
	
	switch_to_write();	
	write_byte(107); //address pointer e.g. 117 - who_am_i (test returning 104 - device address) 65 - temp 2 bytes
	switch_to_read();
	ack = read_bit();
	printf("ack02: %d\n", ack);
	switch_to_write();	
	write_byte(0);
	switch_to_read();
	ack = read_bit();
	printf("ack03: %d\n", ack);
	//stop();
	//start real reading	
	//init();
	//vTaskDelay(3);
	start();
	write_byte((address << 1) + 0);
	switch_to_read();
	ack = read_bit();
	printf("ack1: %d\n", ack);
	
	switch_to_write();	
	write_byte(65); //address pointer e.g. 117 - who_am_i (test returning 104 - device address) 65 - temp 2 bytes
	switch_to_read();
	ack = read_bit();
	printf("ack2: %d\n", ack);

	//stop();
	start();
	write_byte((address << 1) + 1);
	switch_to_read();
	ack = read_bit();
	printf("ack3: %d\n", ack);

	
	int result_h = read_byte();
	write_bit(0);
	switch_to_read();
	printf("result_h = %d\n", result_h);
	
	int result_l = read_byte();
	write_bit(1);
	stop();

	int raw = (result_h << 8) + result_l;
	int signedvar = convert_to_signed(raw);

	printf("result = %d\n", signedvar);
	float celcius = (float)signedvar/(float)340 + 36.53;
	printf("temp = %f\n", celcius);
	vTaskDelete(handle);
}

