#pragma config(Sensor, S1, ts, sensorEV3_Touch)
#pragma config(Sensor, S2, gs, sensorEV3_Gyro)
#pragma config(Sensor, S3, cs, sensorEV3_Color, modeEV3Color_Color)
#pragma config(Sensor, S4, ss, sensorEV3_Ultrasonic)
#pragma config(Motor, motorB, lm, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor, motorC, rm, tmotorEV3_Large, PIDControl, encoder)

#define Black 1
#define Blue 2
#define Green 3
#define Yellow 4
#define Red 5
#define White 6
#define Brown 7

int count = 0, start_val = 0, speed = 8;
int finish_line, color;
int initial, enc_degree;
float dist, t;

int convert(float dist)
{
	return (int)(360.0 * dist / 17.58);
}

void go(int s)
{
	setMotorSpeed(lm, s);
	setMotorSpeed(rm, s);
}

void sound(int n)
{
	playSound(soundBeepBeep);
	sleep(50);
	if(n >= 2){
		for(int i=2; i<=n; i++)
		{
			playSound(soundBeepBeep);
			sleep(50);
		}
	}
}

void point_turn(int mode, int v)
{
	if(mode == 1){					// point_left_turn
		setMotorSpeed(lm, -v);
		setMotorSpeed(rm, v);
	}
	else{										// point_right_turn
		setMotorSpeed(lm, v);
		setMotorSpeed(rm, -v);
	}
}

void left_90()
{
	go(0);
	initial = getGyroDegrees(gs);
	while(getGyroDegrees(gs) - initial >= -90){			// left point_turn
			point_turn(1, speed/2);
	}
}

void right_90()
{
	go(0);
	initial = getGyroDegrees(gs);
	while(getGyroDegrees(gs) - initial <= 90){			// left point_turn
			point_turn(2, speed/2);
	}
}

void black_distance(int c)
{
	go(0);
	resetMotorEncoder(lm);
	enc_degree = convert(c * 10.0);
	while(getMotorEncoder(lm) < enc_degree)
	{
		go(speed/2);
	}
}

task main()
{
	while(getTouchValue(ts) == 0){}
	while(getTouchValue(ts) == 1){}		// Operation when touch sensor is pressed and released
	while(1)
	{
		color = getColorName(cs);
		dist = getUSDistance(ss);
		go(speed);														// Departure
		displayBigTextLine(2, "Count : %d", count);
		displayBigTextLine(4, "Distance : %.2f", dist);

		if(!start_val && (color != White)			// Recognize the stating point
		{
			clearTimer(T1);
			finish_line = color;									// color : starting point == finish_line
			setLEDColor(ledGreen);								// LED color -> green
			sleep(700);
			start_val++;
			color = getColorName(cs);
		}

		if(dist >= 50 && (color != White))
		{
			go(speed);
			sleep(100);
			go(0);
			sleep(150);
			color = getColorName(cs);

			if(color == Blue)
			{
				sound(count);
			}

			if(color == Black)
			{
				count++;
			}

			if(color == Red)
			{
				left_90();
				black_distance(count);
				left_90();
			}

			if((color == finish_line) || count == 10)
			{
				break;
			}

			while(color != White){
				go(speed);
				color = getColorName(cs);
			}
		}

		if(dist < 50)
		{
			go(0);
			sound(1);

			while(dist < 50)
			{
				go(speed/2);
				dist = getUSDistance(ss);
				color = getColorName(cs);
				displayBigTextLine(2, "Count : %d", count);
				displayBigTextLine(4, "Distance : %.2f", dist);

				if(color != White)
				{
					go(speed/2);
					sleep(200);
					go(0);
					sleep(200);
					color = getColorName(cs);

					if(color == Blue)
					{
						sound(count);
					}

					if(color == Black)
					{
						count++;
					}

					if(color == Red)
					{
						left_90();
						black_distance(count);
						left_90();
					}

					if((color == finish_line) || count == 10)
					{
						break;
					}

					while(color != White){
						go(speed/2);
						color = getColorName(cs);
					}
				}

				if(dist < 10)
				{
					go(0);
					enc_degree = convert(20.0);
					resetMotorEncoder(lm);
					while(getMotorEncoder(lm) > -enc_degree)
					{
						go(-speed/2);
					}
					right_90();
					black_distance(count);
					right_90();
				}
			}
		}
	}
	t = time1[T1];
	displayBigTextLine(1, "record = %.2f sec", t/1000);

	go(0);
	sleep(10000);
}
