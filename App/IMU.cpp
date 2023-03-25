#include <IMU.h>

IMU::IMU() {}
IMU::~IMU() {}

/*! \fn IMU::init
 *  \brief Inits the IMU to detect movements
*/
void IMU::init()
{
	mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G);
	mpu.setAccelPowerOnDelay(MPU6050_DELAY_3MS);
	mpu.setIntFreeFallEnabled(false);
	mpu.setIntZeroMotionEnabled(false);
	mpu.setIntMotionEnabled(false);
	mpu.setDHPFMode(MPU6050_DHPF_5HZ);
	mpu.setMotionDetectionThreshold(1);
	mpu.setMotionDetectionDuration(1);
	mpu.setZeroMotionDetectionThreshold(3);
	mpu.setZeroMotionDetectionDuration(100);
}

/*! \fn IMU::sensedActivity
 *  \brief Checks the IMU if there was activity
 *  \return true if there was activity
*/
bool IMU::sensedActivity()
{
	mpu.readRawAccel(); //TODO needed???
	Activites act = mpu.readActivites();
	return act.isActivity;
}

/*! \fn IMU::getAccel
 *  \brief Requests the Accel Values from the IMU
 *  \param x output, acceleration in x direction
 *  \param y output, acceleration in y direction
 *  \param z output, acceleration in z direction
*/
void IMU::getAccel(float* x, float* y, float* z)
{
	Vector v = mpu.readRawAccel();
	*x = v.XAxis;
	*y = v.YAxis;
	*z = v.ZAxis;
}

/*! \fn IMU::getTemp
 *  \brief Requests the Temperature from the IMU
 *  \return temperature in °C
*/
uint8_t IMU::getTemp()
{
	return (uint8_t)mpu.readTemperature();
}

/*! \fn IMU::setLowPowerSamplingMode
 *  \brief sets the IMU into lowPowerSampling Mode
 *  The sampling freq. is set to 5Hz and an Interrupt is
 *  generated as soon as motion is deteced to wake up the controller
 *  \return temperature in °C
*/
void IMU::setLowPowerSamplingMode(bool enable)
{
	mpu.setIntMotionEnabled(enable);
	mpu.setLowPowerSampling(enable, true, MPU6050_LPSF_5HZ);
}
