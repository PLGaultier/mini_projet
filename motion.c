#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <chprintf.h>
#include <audio/microphone.h>
#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <arm_math.h>
#include <motion.h>
#include "motors.h"
#include <epuck1x/utility/utility.h>
#include "sensors/proximity.h"
#include "leds.h"

#define ORIENTATION_SPEED 300 //define in step/s
#define PI 3.14
#define THRESHOLD_FOR_COLLISION 200
#define PROXIMITY_SENSOR_IN_FRONT 0
#define STRAIGHT_SPEED 600
#define NBRE_STEP_FOR_ANG_A 47.88
#define S_TO_MS 1000 //second to ms

static _Bool obstacle_detect = false;

static double temps_rotation;

void orientation_robot (int angle_rad)
{
	set_led(LED7,0);
	set_led(LED5,0);
	set_led(LED3,0);
	set_led(LED1,0);
	set_body_led(0);
	left_motor_set_speed(ORIENTATION_SPEED);
	right_motor_set_speed(-ORIENTATION_SPEED);

	temps_rotation = (S_TO_MS*angle_rad*NBRE_STEP_FOR_ANG_A/ORIENTATION_SPEED);

	chThdSleepMilliseconds(temps_rotation);

	left_motor_set_speed(0);
	right_motor_set_speed(0);

	chThdSleepMilliseconds(2000);
}


void orientation_robot_back(void)
{

	left_motor_set_speed(-ORIENTATION_SPEED);
	right_motor_set_speed(ORIENTATION_SPEED);

	chThdSleepMilliseconds(temps_rotation);

	left_motor_set_speed(0);
	right_motor_set_speed(0);

	chThdSleepMilliseconds(2000);
}


_Bool avance_to_obstacle()
{
	//Init for the proximity sensors
	messagebus_topic_t *prox_topic_test = messagebus_find_topic_blocking(&bus, "/proximity");
	proximity_msg_t prox_values; //mieux dans le main.h
	messagebus_topic_wait(prox_topic_test, &prox_values, sizeof(prox_values));


	if (!(obstacle_detect))
	{
		if(get_prox(PROXIMITY_SENSOR_IN_FRONT) < THRESHOLD_FOR_COLLISION)
		{
			right_motor_set_speed(STRAIGHT_SPEED);
			left_motor_set_speed(STRAIGHT_SPEED);
			return true;


		}
		else
		{
			right_motor_set_speed(0);
			left_motor_set_speed(0);
			set_front_led(1);
			chThdSleepMilliseconds(2000);
			set_front_led(0);
			obstacle_detect = true;
			return true;
		}

	}
	else
	{
		//We go back to our starting position
		if ((left_motor_get_pos() <= 0) || (right_motor_get_pos() <=0))
		{
			right_motor_set_speed(0);
			left_motor_set_speed(0);
			obstacle_detect = false;
			return false;


		}
		else
		{
			right_motor_set_speed(-STRAIGHT_SPEED);
			left_motor_set_speed(-STRAIGHT_SPEED);
			return true;
		}
	}
}

