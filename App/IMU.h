/*!
 * \class IMU
 * \brief IMU controls the MPU6050 Chip (accel + gyro)
 *
 *  The MPU6050 is used to detect motion of the shisha (smoking)
 *  \author David Huwiler
 *  \version 1.0
 *  \date 02.07.2019
 */

#ifndef IMU_H_
#define IMU_H_

extern "C"
{
#include "stm32f1xx_hal.h"
}
#include "MPU6050.h"


class IMU {
public:
	IMU();
	void init(void);
	bool sensedActivity(void);
	void getAccel(float* x, float* y, float* z);
	uint8_t getTemp();
	void setLowPowerSamplingMode(bool enable);
	virtual ~IMU();

private:
	MPU6050 mpu;
};

#endif /* IMU_H_ */
