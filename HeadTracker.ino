#include <MadgwickAHRS.h>
#include <LSM6DS3.h>

#define SAMPLE_RATE 104.0f

Madgwick filter;

void setup()
{
    Serial.begin(9600);
    while (!Serial);

    if (!IMU.begin())
    {
        Serial.println("Failed to initialize IMU!");
        while (true);
    }

    filter.begin(SAMPLE_RATE);
}

void loop()
{
    float acc[3];
    float gyro[3];
    float yaw, pitch, roll;
    
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable())
    {
        IMU.readAcceleration(acc[0], acc[1], acc[2]);
        IMU.readGyroscope(gyro[0], gyro[1], gyro[2]);

        filter.updateIMU(gyro[0], gyro[1], gyro[2], acc[0], acc[1], acc[2]);

        yaw = filter.getYaw();
        pitch = filter.getPitch();
        roll = filter.getRoll();

        Serial.print("Yaw: ");
        Serial.println(yaw);
        Serial.print("Pitch: ");
        Serial.println(pitch);
        Serial.print("Roll: ");
        Serial.println(roll);
    }
}
