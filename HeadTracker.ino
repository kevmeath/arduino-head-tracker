#include <LSM6DS3.h>

#include <WiFiNINA.h>

#include <MadgwickAHRS.h>

#include "secrets.h"

#define SAMPLE_RATE 104.0f

Madgwick filter;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int wifi_status = WL_IDLE_STATUS;

float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
float yaw, pitch, roll;

void setup()
{
    Serial.begin(9600);
    while (!Serial);

    while (wifi_status != WL_CONNECTED)
    {
        Serial.print("Connecting to network: ");
        Serial.println(ssid);
        
        wifi_status = WiFi.begin(ssid, pass);

        delay(10000);
    }

    Serial.println("Connected to network");

    if (!IMU.begin())
    {
        Serial.println("Failed to initialize IMU!");
        while (true);
    }

    filter.begin(SAMPLE_RATE);
}

void loop()
{   
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable())
    {
        IMU.readAcceleration(accelX, accelY, accelZ);
        IMU.readGyroscope(gyroX, gyroY, gyroZ);

        filter.updateIMU(gyroX, gyroY, gyroZ, accelX, accelY, accelZ);

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
