//// jwc 25-0417-1000 Add Mpu6050 Compass from: 'C:\11i-GD-C\09j-E3\24-1226-1001-Esp32-Display-AllInOne-Waveshare_2.8in\Arduino-NOW-AllInOne_TftCapacitance_Waveshare\examples\250201-121953\src\main.cpp'
    //// jwc 24-1230-0900 Add I2c:Compass_Mpu6050
        //// Libraries:
            //// * C:\11i-GD-C\09j-E3\24-1226-1001-Esp32-Display-AllInOne-Waveshare_2.8in\Arduino-NOW-AllInOne_TftCapacitance_Waveshare\examples\250201-121953\lib\Adafruit_MPU6050
            //// * C:\11i-GD-C\09j-E3\24-1226-1001-Esp32-Display-AllInOne-Waveshare_2.8in\Arduino-NOW-AllInOne_TftCapacitance_Waveshare\examples\250201-121953\lib\Adafruit_BusIO
            //// * C:\11i-GD-C\09j-E3\24-1226-1001-Esp32-Display-AllInOne-Waveshare_2.8in\Arduino-NOW-AllInOne_TftCapacitance_Waveshare\examples\250201-121953\lib\Adafruit_Unified_Sensor

        //// * C:\09j-Esp32\24-1213-1900-Esp32-Compass_Mp6050-Arduino_Mp6050-Shilleh\24-1213-1900-basic_readings-Adafruit_Mp6050-Example-Shilleh--SNAPSHOT-24-1214-0200-AAA_a-TYJ-Works-ButDegreesConfusing
        //// jwc 24-1213-2000 Adafruit Mpu6050 Library: Example: basic_reading.ino
        //// jwc https://www.youtube.com/watch?v=H9e1Up7xHjc
        //// * 'Serial.println' >> 'printf'

    //// jwc TODO C:\11i-GD-C\09j-E3\24-1213-1902-Esp32-Compass_Mp6050-SensorFusion-Shlleh\24-1213-1902-Esp32-Compass_Mp6050-SensorFusion-Shlleh-NOW\src\MPU6050_DMP64
        //// jwc 24-1214-0230 
        //// * https://shillehtek.com/blogs/news/measure-angles-easily-with-mpu6050-and-esp32-part-1-library-walkthrough?utm_source=youtube&utm_medium=product_shelf
        //// * https://www.youtube.com/watch?v=C93SoHYeuz0
        //// * https://github.com/jrowberg/i2cdevlib/tree/master
        //// * Override I2c Pinouts: https://randomnerdtutorials.com/esp32-i2c-communication-arduino-ide/

#include <Arduino.h>
#include <Wire.h>

#define SDA 1
#define SCL 2

//// jwc 24-1213-2000 \/\/ 
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
Adafruit_MPU6050 mpu;




void scan_i2c_device(TwoWire &i2c)
{
    Serial.println("Scanning for I2C devices ...");
    Serial.print("      ");
    for (int i = 0; i < 0x10; i++)
    {
        Serial.printf("0x%02X|", i);
    }
    uint8_t error;
    for (int j = 0; j < 0x80; j += 0x10)
    {
        Serial.println();
        Serial.printf("0x%02X |", j);
        for (int i = 0; i < 0x10; i++)
        {
            Wire.beginTransmission(i | j);
            error = Wire.endTransmission();
            if (error == 0)
                Serial.printf("0x%02X|", i | j);
            else
                Serial.print(" -- |");
        }
    }
    Serial.println();
}

void setup()
{
    Serial.begin(115200);
    //// jwc o Serial.println("Ciallo");

    printf("*** Serial_0: Setup Done\n");

    Wire.begin(SDA, SCL);
    scan_i2c_device(Wire);

    //// jwc 25-0417-1000 \/\/
    ////

    // Try to initialize!
    if (!mpu.begin()) {
        printf("Failed to find MPU6050 chip\n");
        while (1) {
        delay(10);
        }
    }
    printf("MPU6050 Found!\n");

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    printf("Accelerometer range set to: ");
    switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
        printf("+-2G\n");
        break;
    case MPU6050_RANGE_4_G:
        printf("+-4G\n");
        break;
    case MPU6050_RANGE_8_G:
        printf("+-8G\n");
        break;
    case MPU6050_RANGE_16_G:
        printf("+-16G\n");
        break;
    }
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    printf("Gyro range set to: ");
    switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
        printf("+- 250 deg/s\n");
        break;
    case MPU6050_RANGE_500_DEG:
        printf("+- 500 deg/s\n");
        break;
    case MPU6050_RANGE_1000_DEG:
        printf("+- 1000 deg/s\n");
        break;
    case MPU6050_RANGE_2000_DEG:
        printf("+- 2000 deg/s\n");
        break;
    }

    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    printf("Filter bandwidth set to: ");
    switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
        printf("260 Hz\n");
        break;
    case MPU6050_BAND_184_HZ:
        printf("184 Hz\n");
        break;
    case MPU6050_BAND_94_HZ:
        printf("94 Hz\n");
        break;
    case MPU6050_BAND_44_HZ:
        printf("44 Hz\n");
        break;
    case MPU6050_BAND_21_HZ:
        printf("21 Hz\n");
        break;
    case MPU6050_BAND_10_HZ:
        printf("10 Hz\n");
        break;
    case MPU6050_BAND_5_HZ:
        printf("5 Hz\n");
        break;
    }

    printf("\n");

    //// jwc added:
    scan_i2c_device(Wire);

    //// jwc o Provide time for 'scan_i2c_device' to print: delay(100);
    delay(3000);

}

void loop()
{
    //// jwc o scan_i2c_device(Wire);


    //// jwc 24-1213-2000 \/\/ 

    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    //// jwc yy TYJ decently real-time: //// jwc less prints for more real-time \/\/
    //// jwc yy TYJ decently real-time: printf("* Z: ");
    //// jwc yy TYJ decently real-time: printf("%f", g.gyro.z);
    //// jwc yy TYJ decently real-time: printf(" rad/s");
    //// jwc yy TYJ decently real-time: 
    //// jwc yy TYJ decently real-time: printf("\n");
    //// jwc yy TYJ decently real-time: delay(0);
    
    //// jwc o if(false){
    if(true){
        //// jwc full prints for less real-time \/\/
        /* Print out the values */
        printf("Acceleration X: ");
        printf("%f", a.acceleration.x);
        printf(", Y: ");
        printf("%f", a.acceleration.y);
        printf(", Z: ");
        printf("%f", a.acceleration.z);
        printf(" m/s^2 ");
    //
        printf("Rotation X: ");
        printf("%f", g.gyro.x);
        printf(", Y: ");
        printf("%f", g.gyro.y);
        printf(", Z: ");
        printf("%f", g.gyro.z);
        printf(" rad/s");
    //
        printf("Temperature: ");
        printf("%f", temp.temperature);
        printf(" degC");
    //
        printf("\n");
        delay(500);
    }
    //// jwc \/\/ TEMP DEACTIVATE FOR SERIAL-TESTING ABOVE
    else if (false) {
        //// jwc less prints for more real-time \/\/
        printf("* Z: ");
        printf("%f", g.gyro.z);
        printf(" rad/s");
        
        printf("\n");
        //// jwc y delay(0);
        //// jwc y delay(1000);
        delay(500);

    }


    //// jwc o delay(1000);

    //// jwc delay to not overload Mb
    //// jwc not needed, allow real-time processing: delay(3000);
    //// jwc slow down main loop a little \/
    //// jwc yy increase to 3000 to sync lockstep with mb: delay(100);  
    //// jwc not needed: delay(3000);

}