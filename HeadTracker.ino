#include <LSM6DS3.h>
#include <WiFiNINA.h>
#include <MadgwickAHRS.h>
#include "secrets.h"

#define SAMPLE_RATE 104.0f

Madgwick filter;

const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
const char ipStr[] = SECRET_IP;
IPAddress ip;
const uint16_t port = SECRET_PORT;
int wifi_status = WL_IDLE_STATUS;
WiFiUDP udp;

float accX, accY, accZ;
float gyroX, gyroY, gyroZ;
float yaw, pitch, roll;

double *packetData = (double *)calloc(6, sizeof(double));

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

    ip.fromString(ipStr);
    udp.begin(port);
    filter.begin(SAMPLE_RATE);
}

void loop()
{
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable())
    {
        IMU.readAcceleration(accX, accY, accZ);
        IMU.readGyroscope(gyroX, gyroY, gyroZ);

        filter.updateIMU(gyroX, gyroY, gyroZ, accX, accY, accZ);

        yaw = filter.getYaw();
        pitch = filter.getPitch();
        roll = filter.getRoll();

        packetData[3] = (double)yaw;
        packetData[4] = (double)pitch;
        packetData[5] = (double)roll;

        for (int i = 0; i < 6; i++)
        {
            Serial.print(packetData[i]);
            Serial.print(',');
        }
        Serial.println();

        udp.beginPacket(ip, port);
        udp.write((byte *)packetData, 6 * sizeof(double));
        udp.endPacket();
    }
}
