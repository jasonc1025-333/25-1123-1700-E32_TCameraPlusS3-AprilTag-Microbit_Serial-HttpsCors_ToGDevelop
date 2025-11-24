//// jwc 25-0411-1800 convert to April-Tag Detect 
////     * libraries\Mylibrary\pin_config.h
////     * Libraries\
////       * C:\11i-GD-C\09j-E3\20e-Arduino-Sketchbook_Default-NOW\libraries\Apriltag_library_for_Arduino_ESP32

//// jwc 25-0409-1600 April-Tag Pose
//// jwc 25-0410-1500 'C:\11i-GD-C\09j-E3\24-1226-1005-Esp32-AllInOne-Waveshare-2.0in-CameraOptrion\ESP32-S3-Touch-LCD-2-Demo\Arduino\examples\09_lvgl_camera-Z01-SimpleTagDetect_April\09_lvgl_camera-Z01-SimpleTagDetect_April.ino'
//// jwc 25-0410-1500 Convert from St7735 to St7789
//// jwc * Esp32s3 'printf' to 'Serial.printf'
//// jwc 25-0410-1500
////     * C:\11i-GD-C\09j-E3\20e-Arduino-Sketchbook_Default-NOW\libraries\TFT_eSPI\User_Setup-Esp32_Wroom-Tft_St7735-25-0410-1500-Waveshare_Esp32s3_2p0In_St7789.h
////       * #define TFT_WIDTH  240 // ST7789 240 x 240 and 240 x 320
////       * #define TFT_HEIGHT 240 // ST7789 240 x 320
////     * C:\11i-GD-C\09j-E3\24-1114-0752-AAA_AAa-TYJ-AprilTag--Github-Raspiduino-Apriltag\apriltag-esp32\examples\simpletagdetect_25_0409_1601_Ws_Esp32s3_2p0In_AprilTag_Pose_NOW\simpletagdetect_25_0409_1601_Ws_Esp32s3_2p0In_AprilTag_Pose_NOW.ino
////       * config.frame_size = FRAMESIZE_240X240;
////       * tft.setRotation(0);
////     * NOT WORK
////       * #define TFT_WIDTH  240 // ST7789 240 x 240 and 240 x 320
////       * #define TFT_HEIGHT 320 // ST7789 240 x 320
////
////       * config.frame_size = FRAMESIZE_QVGA;

//// jwc 25-0416-1400 Add 'C:\11i-GD-C\09j-E3\24-1226-1001-Esp32-Display-AllInOne-Waveshare_2.8in\Arduino-NOW-AllInOne_TftCapacitance_Waveshare\examples\250201-121953\src\main.cpp'
////     * Serial Test w/ Mb
////     * Bonus: Compass w/ I2C

    //// jwc 24-1229-1000 https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-2.8
    //// jwc 24-1230-0630 Add Serial2: Actuall works: 43, 44, but need to disconect loopback: to accurately test disconeected first, then connected
    //// * C:\09j-Esp32\24-1206-2001-Esp32-2432S024C-CheapYellowDisplay-Rzeldent\esp32-smartdisplay-demo-SNAPSHOT-24-1222--0730-ABA_Aa-TYJ-Esp_28R_24C_Serial_2__35_22__22_27_NoMbYet\src\main.cpp
    //// jwc 24-1230-0900 Add I2c:Compass_Mpu6050
    //// * C:\09j-Esp32\24-1213-1900-Esp32-Compass_Mp6050-Arduino_Mp6050-Shilleh\24-1213-1900-basic_readings-Adafruit_Mp6050-Example-Shilleh--SNAPSHOT-24-1214-0200-AAA_a-TYJ-Works-ButDegreesConfusing
    //// jwc 24-1213-2000 Adafruit Mpu6050 Library: Example: basic_reading.ino
    //// jwc https://www.youtube.com/watch?v=H9e1Up7xHjc
    //// * 'Serial.println' >> 'printf'

    //// jwc 24-1231-0420: yyy Serial2.begin(115200, SERIAL_8N1, 43, 44); //
    //// jwc 24-1231-0420: yyy Serial2.begin(115200, SERIAL_8N1, 15, 18); //

    //// jwc 25-0125-0500 Esp32 no longer return back Mb packets since seems corrupted, but sends back new, clean packets


//// jwc \/
// * simple_tag_detect_24_1115_0530_ACa_a__Commit_24_0416_Wrover_Vid
//   * Flash_Jpg_24_1115_1040_ABC_a_Esp32_FnWroverCamToTft7735.ino
//     * TFT_graphicstest_one_lib_24_1114_0800_ACA_a_FnWrover_St7735_1p4.ino

// * 24-1127-1850 Appears that at small 'FRAMESIZE_QVGA' to reduce April-Tag processing
//   * for 2.5x2.5cm Tft, need April-Tag min. 0.5x0.5cm for recognition.
//   * 2.5x2.5 = 6.25 || 0.5x0.5 = 0.25 || 0.25/6.25 = 0.04 = 4% of screen min

/*
 * AprilTag detector demo on AI Thinker ESP32-CAM
 * Created by gvl610
 * Based on https://github.com/AprilRobotics/apriltag
 * with some modifications (for adaption and performance)
 */

// ===================
// Select camera model
// ===================
//// jwc \/
//// jwc oy REPLACED BELOW \/ #define CAMERA_MODEL_WROVER_KIT // Has PSRAM
#define CAMERA_MODEL_LILYGO_T_CAMERA_PLUS_S3_ESP32S3_TFT1P3IN


/*
 * @Description(CN):
 *       这是一个将OV2640摄像头数据投影到屏幕上的例程
 *
 * @Description(EN):
 *      This is a routine that projects the OV2640 camera data onto the T-CameraPlus-S3 screen.
 *
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-11-15 16:57:21
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-11-25 09:04:35
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include <esp_camera.h>
#include "camera_index.h"
#include "app_httpd.tpp"

//// \/ jwc 25-0411-1800 convert to April-Tag Detect 

// Apriltag headers
// We choose 36h11 family to use in this demo, but you can use
// any family of your choice. Note that due to memory limitation,
// you might have to reduce the number of tag in `codedata` array
// in the tag family source file.
#include "apriltag.h"
#include "tag36h11.h" // Tag family. You can change
#include "common/image_u8.h"
#include "common/zarray.h"

//// jwc o http://esp32.io/viewtopic.php?t=31461
/** jwc \/
"Failed to get the frame on time!" after a while working fine.
* I have a script that in the loop does some kind of "motion detection". Later I plan to take a high-res pic and send it to a server if a motion is detected. Beside this I want to have the possibility to connect via a Webserver. The part in the loop works fine already. When I connect via the Webserver it works fine at the beginning but after taking about 40-50 frames I get this error and the esp32 reboots:
* [E][camera.c:1483] esp_camera_fb_get(): Failed to get the frame on time!
Guru Meditation Error: Core 1 panic'ed (LoadProhibited). Exception was unhandled.
* The Webserver on its own seems to work fine. When I comment out everything in the loop function and insert the then missing "Config(LOW)" in the setup function the stream goes on and on without any problems.
**/
#include <img_converters.h>


//// jwc 25-0409-1600 April-Tag Pose
//
#include "apriltag_pose.h" // For pose estimation
#include "common/matd.h"

// Config for pose estimation
// Tag size (in meter). See original AprilTag readme for how to measure
// You have to put your value here. This value is of NO standard and
// is just my own tag size.
//// jwc Measure Full April-Tag Dimension if 100% Black
#define TAG_SIZE 0.05 // 0.05m = 5cm

// Camera calibration data
// This information is obtained by calibrating your camera using software like 3DF Zephyr
// You have to calibrate and put your own values here, this value is just for my camera
// and likely not work on your camera.
#define FX 924.713610878 // fx (in pixel)
#define FY 924.713610878 // fy (in pixel)
#define CX 403.801748132 // cx (in pixel)
#define CY 305.082642826 // cy (in pixel)

/*
 * Define this macro to enable debug mode
 * Level 0: Absolutely no debug at all. Suitable for
 *  production use.
 * Level 1: Simple debug messages , like simple events
 *  notifications.
 * Level 2: Low level debug messages
 * Level 3: Debug messages that in a loop
 */
//// jwc o #define DEBUG 2
//// jwc \/ get as much info as possible
//// jwc y #define DEBUG 1
//// jwc y #define D EBUG 3
//// jwc y cause lag when on batt? #define DEBUG 1
#define DEBUG 0

// Create tag family object
apriltag_family_t *tf = tag36h11_create();
// Create AprilTag detector object
apriltag_detector_t *td = apriltag_detector_create();

//// /\ jwc 25-0411-1800 convert to April-Tag Detect 


static bool OV2640_Initialization_Flag = false;

// fp-133h01d
Arduino_DataBus *bus = new Arduino_HWSPI(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */);

Arduino_TFT *gfx = new Arduino_ST7789(
    bus, LCD_RST /* RST */, 0 /* rotation */, true /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

bool OV2640_Initialization(void)
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;

    // Set clock frequency
    config.xclk_freq_hz = 20000000;

    // Set frame config
    // • FRAMESIZE_UXGA (1600 x 1200)
    // * FRAMESIZE_SXGA (1280 x 1024)
    // • FRAMESIZE_XGA (1024 x 768)
    // • FRAMESIZE_SVGA (800 x 600)
    // • FRAMESIZE_VGA (640 x 480)
    // • FRAMESIZE_QVGA (320 x 240)
    // • FRAMESIZE_CIF (352 x 288)
    //
    //// jwc typedef enum {
    //// jwc   FRAMESIZE_96X96,    // 96x96
    //// jwc   FRAMESIZE_QQVGA,    // 160x120
    //// jwc   FRAMESIZE_QCIF,     // 176x144
    //// jwc   FRAMESIZE_HQVGA,    // 240x176
    //// jwc   FRAMESIZE_240X240,  // 240x240
    //// jwc   FRAMESIZE_QVGA,     // 320x240
    //// jwc   FRAMESIZE_CIF,      // 400x296
    //// jwc   FRAMESIZE_HVGA,     // 480x320
    //// jwc   FRAMESIZE_VGA,      // 640x480
    //// jwc   FRAMESIZE_SVGA,     // 800x600
    //// jwc   FRAMESIZE_XGA,      // 1024x768
    //// jwc   FRAMESIZE_HD,       // 1280x720
    //// jwc   FRAMESIZE_SXGA,     // 1280x1024
    //// jwc   FRAMESIZE_UXGA,     // 1600x1200
    //// jwc   // 3MP Sensors
    //// jwc   FRAMESIZE_FHD,      // 1920x1080
    //// jwc   FRAMESIZE_P_HD,     //  720x1280
    //// jwc   FRAMESIZE_P_3MP,    //  864x1536
    //// jwc   FRAMESIZE_QXGA,     // 2048x1536
    //// jwc   // 5MP Sensors
    //// jwc   FRAMESIZE_QHD,      // 2560x1440
    //// jwc   FRAMESIZE_WQXGA,    // 2560x1600
    //// jwc   FRAMESIZE_P_FHD,    // 1080x1920
    //// jwc   FRAMESIZE_QSXGA,    // 2560x1920
    //// jwc   FRAMESIZE_INVALID
    //// jwc } framesize_t;

    //// jwc y but maybe too big? \/ config.frame_size = FRAMESIZE_VGA;
    //// jwc y From: pose_estimate.ino
    //// * config.frame_size = FRAMESIZE_SVGA; // You can change the resolution to fit your need

    //// jwc y Using a 240x240 Suqare screen \/config.frame_size = FRAMESIZE_QVGA;

    //// jwc y config.frame_size = FRAMESIZE_240X240;
    //// jwc yy config.frame_size = FRAMESIZE_QVGA;

    config.frame_size = FRAMESIZE_240X240;

    // config.pixel_format = PIXFORMAT_JPEG; // for streaming
    //// jwc 25-0411-1800 oy    config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    //// jwc ? config.pixel_format = PIXFORMAT_GRAYSCALE; // for April-Tag Detection
    //// jwc config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    config.pixel_format = PIXFORMAT_GRAYSCALE; // for April-Tag Detection

    //// jwc 25-0411-1800 oy config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.grab_mode = CAMERA_GRAB_LATEST; // Has to be in this mode, or detection will be lag

    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 0;
    config.fb_count = 2;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        gfx->printf("Camera init failed with error 0x%x \n", err);

        return false;
    }

    sensor_t *s = esp_camera_sensor_get();
    if (s)
    {
        Serial.print("camera id:");
        Serial.println(s->id.PID);
        gfx->print("camera id:");
        gfx->println(s->id.PID);
        gfx->println();

        //// \/ jwc 25-0411-1800 convert to April-Tag Detect 

        // Custom camera configs should go here \/
        //
        s->set_brightness(s, 0);     // -2 to 2
        s->set_contrast(s, 0);       // -2 to 2
        s->set_saturation(s, 0);     // -2 to 2
        s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
        s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
        s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
        s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
        s->set_aec2(s, 1);           // 0 = disable , 1 = enable
        s->set_ae_level(s, 0);       // -2 to 2
        s->set_aec_value(s, 168);    // 0 to 1200
        s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
        s->set_agc_gain(s, 0);       // 0 to 30
        s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
        s->set_bpc(s, 0);            // 0 = disable , 1 = enable
        s->set_wpc(s, 1);            // 0 = disable , 1 = enable
        s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
        s->set_lenc(s, 1);           // 0 = disable , 1 = enable
        s->set_hmirror(s, 1);        // 0 = disable , 1 = enable
        s->set_vflip(s, 1);          // 0 = disable , 1 = enable
        s->set_dcw(s, 1);            // 0 = disable , 1 = enable
        
        //// /\ jwc 25-0411-1800 convert to April-Tag Detect 



        camera_sensor_info_t *sinfo = esp_camera_sensor_get_info(&(s->id));
        if (sinfo)
        {
            Serial.print("camera model:");
            Serial.println(sinfo->name);
            gfx->print("camera model:");
            gfx->println(sinfo->name);
            gfx->println();
        }
    }
    // initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID)
    {
        s->set_vflip(s, 1);       // flip it back
        s->set_brightness(s, 1);  // up the brightness just a bit
        s->set_saturation(s, -2); // lower the saturation
    }

    //// \/ jwc 25-0411-1800 convert to April-Tag Detect 

    //// jwc 25-0411-1800 TODO
    //// jwc 25-0410-1500 TODO #if defined(CAMERA_MODEL_WAVESHARE_ESP32S3_TFT2P0IN_CAM)
    //// jwc 25-0410-1500 TODO   //// jwc n even with '1', April-Tag Detected: s->set_vflip(s, 1);  
    //// jwc 25-0410-1500 TODO   s->set_vflip(s, 0);  
    //// jwc 25-0410-1500 TODO   //// jwc n and with '1', April-Tag Detected-NOT: s->set_hmirror(s, 1);
    //// jwc 25-0410-1500 TODO   s->set_hmirror(s, 0);
    //// jwc 25-0410-1500 TODO #endif
#if defined(CAMERA_MODEL_LILYGO_T_CAMERA_PLUS_S3_ESP32S3_TFT1P3IN)
    //// jwc n even with '1', April-Tag Detected: s->set_vflip(s, 1);  
    //// jwc y s->set_vflip(s, 0);  
    //// jwc n w/ set_rotation(0): s->set_vflip(s, 1);  
    //// jwc 25-0411 ? s->set_vflip(s, 0);  
    //// jwc n s->set_vflip(s, 1);  
    //// jwc n s->set_vflip(s, 0);  
    //// jwc n s->set_vflip(s, 0);  
    s->set_vflip(s, 1);  

    //// jwc n and with '1', April-Tag Detected-NOT: s->set_hmirror(s, 1);
    //// jwc 25-0410-1500 NOT WORK: s->set_hmirror(s, 0);
    //// jwc 25-0410-1500 WORK:
    s->set_hmirror(s, 1);
#endif

    // Done init camera
#if DEBUG >= 1
    //// jwc o \/ Serial.println("done");
    Serial.println("*** Camera Init: End");
#endif


    // Setup AprilTag detection
#if DEBUG >= 1
    Serial.print("Init AprilTag detector... ");
#endif

    //// jwc move to global space above: // Create tag family object
    //// jwc move to global space above: apriltag_family_t *tf = tag36h11_create();
    //// jwc move to global space above: 
    //// jwc move to global space above: // Create AprilTag detector object
    //// jwc move to global space above: apriltag_detector_t *td = apriltag_detector_create();

    // Add tag family to the detector
    apriltag_detector_add_family(td, tf);
    
    // Tag detector configs
    // quad_sigma is Gaussian blur's sigma
    // quad_decimate: small number = faster but cannot detect small tags
    //                big number = slower but can detect small tags (or tag far away)
    // With quad_sigma = 1.0 and quad_decimate = 4.0, ESP32-CAM can detect 16h5 tag
    // from the distance of about 1 meter (tested with tag on screen. not on paper)
    td->quad_sigma = 0.0;
    td->quad_decimate = 4.0;
    td->refine_edges = 0;
    //// jwc TODO From: pose_estimate.ino
    //// jwc: td->decode_sharpening = 0.25;
    //// jwc: td->nthreads = 2; // The optimal (after many tries) is 2 thread on 2 cores ESP32
    td->decode_sharpening = 0;

    //// jwc 25-0410-1300 td->nthreads = 1;
    td->nthreads = 2;

    td->debug = 0;

    // Done init AprilTag detector
#if DEBUG >= 1
    Serial.println("done");
    Serial.println("Start detecting...");
#endif

    //// /\ jwc 25-0411-1800 convert to April-Tag Detect 

    return true;
}

void setup()
{
    Serial.begin(115200);
    //// jwc o Need English Instead: Serial.println("Ciallo");

    //// jwc ? //// jwc 'Serial2.begin(baud-rate, protocol, RX pin, TX pin);'
    //// jwc ? //// * https://www.waveshare.com/esp32-s3-touch-lcd-2.8.htm
    //// jwc y DOES SEEM IMORTANT, NOT WORK IF MISSING:   Serial2.begin(115200, SERIAL_8N1, 44, 43); //
    //// jwc ? Serial2.begin(115200, SERIAL_8N1, 43, 44); //
    //// jwc yy Serial2.begin(115200, SERIAL_8N1, 15, 18); //
    //// jwc n Serial2.begin(115200, SERIAL_8N1, 1, 2); //
    //// jwc yyy 24-1231-0420: Serial2.begin(115200, SERIAL_8N1, 43, 44); //
    //// jwc yyy 24-1231-0420: Serial2.begin(115200, SERIAL_8N1, 15, 18); //
    //// jwc o 25-0416-1500 Serial2.begin(115200, SERIAL_8N1, 43, 44); //
    
    //// jwc ? //// jwc 25-0416-1500 Lg T-CameraPlus-S3: U0RxD = 50 (Right Pin), U0TxD = 49 (Left Pin)
    //// jwc ? Serial2.begin(115200, SERIAL_8N1, 50, 49); //

  
    //// jwc ? Serial.printf("*** Serial_0 & Serial_2: Setup Done\n");
    //// jwc ? printf("*** Serial_0 & Serial_2: Setup Done\n");
    Serial.println("*** Serial_0 & Serial_2: Setup Done\n");
    
    pinMode(LCD_BL, OUTPUT);
    ledcAttachPin(LCD_BL, 1);
    ledcSetup(1, 20000, 8);
    ledcWrite(1, 255); // brightness 0 - 255

    gfx->begin();
    gfx->fillScreen(WHITE);

    gfx->setTextColor(BLACK);

    delay(3000);

    OV2640_Initialization_Flag = OV2640_Initialization();
}

void loop()
{
    if (OV2640_Initialization_Flag == true)
    {
        camera_fb_t *frame = esp_camera_fb_get();
        if (frame)
        {

            //// jwc 25-0411 gfx->draw16bitBeRGBBitmap(0, 0, (uint16_t *)frame->buf, frame->width, frame->height);
            //// jwc yy gfx->draw16bitBeRGBBitmap(0, 0, (uint16_t *)frame->buf, frame->width, frame->height);
            //// jwc n gfx->drawGrayscaleBitmap(0, 0, (uint16_t *)frame->buf, frame->width, frame->height);
            gfx->drawGrayscaleBitmap(0, 0, (uint8_t *)frame->buf, frame->width, frame->height);

            //// \/ jwc 25-0411-1800 convert to April-Tag Detect 

            // Convert our framebuffer to detector's input format
            #if DEBUG >= 3
            Serial.println("Converting frame to detector's input format... ");
        #endif
            image_u8_t im = {
            .width = frame->width,
            .height = frame->height,
            .stride = frame->width,
            .buf = frame->buf
            };
        #if DEBUG >= 3
            Serial.println("done");
            Serial.println("Detecting... ");
        #endif

            // Detect
            zarray_t *detections = apriltag_detector_detect(td, &im);
        #if DEBUG >= 3
            Serial.println("done. Result:");
        #endif

            //// jwc ? /// jwc Clear Tft Screen
            //// jwc ? ///
            //// jwc ? /// jwc y tft.fillScreen(TFT_BLACK);  
            //// jwc ? ft.fillScreen(TFT_BLACK);  
            //// jwc ? /// jwc y Provide more upper and left margin: tft.setCursor(0,0);
            //// jwc ? ft.setCursor(10,10);
            //// jwc ? ft.setTextColor(TFT_YELLOW);  
            //// jwc ? /// jwc tft.setTextSize(3);
            //// jwc ? /// jwc y tft.setTextSize(6);
            //// jwc ? ft.setTextSize(4);

            //// jwc 25-0411-1800 tft.setCursor(1,1);
            gfx->setCursor(1,1);
            //// jwc y good for nomral, but increase for VideoMeet-Cam \/: tft.setTextSize(2); tft.setTextSize(3);
            //// jwc y tft.setTextSize(4);
            //// jwc y seems just right to fit 6 max
            //// jwc 25-0411-1800 tft.setTextSize(5);
            gfx->setTextSize(5);
            gfx->printf(".");

            if(zarray_size(detections) > 0){
            
            // Print result
            for (int i = 0; i < zarray_size(detections); i++) {
                apriltag_detection_t *det;
                zarray_get(detections, i, &det);
                //// jwc \/
                //// jwc y Serial.print(" *** ");
                
                if(det->id % 2 == 0){
                    // Even #
                    //// jwc 25-0411-1800 tft.setTextColor(TFT_BLUE);      
                    gfx->setTextColor(BLUE);      
                }
                else{
                    // Odd #
                    //// jwc 25-0411-1800 tft.setTextColor(TFT_RED);
                    gfx->setTextColor(RED);
                }

                //// jwc add pose: //// jwc tft.printf(" *** %d", (det->id));
                //// jwc 25-0411-1800 tft.printf("%d ", (det->id));
                gfx->printf("%d ", (det->id));
                //// jwc add pose: Serial.print(det->id);
                //// jwc add pose: //// jwc o Serial.print(", ");

        #if DEBUG >= 1
                // Print tag ID
                //// jwc o Serial.print("ID: ");
                //// jwc o Serial.println(det->id);
                //// jwc n Serial.println("*** *** *** ", det->id, det->family, det->decision_margin);
                // Print tag ID and decision margin
                //// jwc y Serial.printf("*** *** *** [DET]%d,%f,", det->id, det->decision_margin);
                Serial.printf("*** *** *** [DETECT ID:] %5.0d,%5.0f,", det->id, det->decision_margin);
        #endif

                // Creating detection info object to feed into pose estimator
                apriltag_detection_info_t info;
                info.det = det;
                info.tagsize = TAG_SIZE;
                info.fx = FX;
                info.fy = FY;
                info.cx = CX;
                info.cy = CY;

                // Estimate the pose
                apriltag_pose_t pose;
                double err = estimate_tag_pose(&info, &pose);

        #if DEBUG >= 1    
                // Print result (position of the tag in the camera's coordinate system)
                //matd_print(pose.R, "%15f"); // Rotation matrix
                //matd_print(pose.t, "%15f"); // Translation matrix
                Serial.printf("\n");
                Serial.printf("    *** pose.R: \n");
                //// jwc y matd_print(pose.R, "%15f"); // Rotation matrix
                matd_print(pose.R, "%15f"); // Rotation matrix
                Serial.printf("    *** pose.t: \n");
                //// jwc y matd_print(pose.t, "%15f"); // Translation matrix
                matd_print(pose.t, "%15f"); // Translation matrix
        #endif

                // Compute the yaw, pitch, and roll from the rotation matrix (and convert to degree)
                double yaw = atan2(MATD_EL(pose.R, 1, 0), MATD_EL(pose.R, 0, 0)) * RAD_TO_DEG;
                double pitch = atan2(-MATD_EL(pose.R, 2, 0), sqrt(pow(MATD_EL(pose.R, 2, 1), 2) + pow(MATD_EL(pose.R, 2, 2), 2))) * RAD_TO_DEG;
                double roll = atan2(MATD_EL(pose.R, 2, 1), MATD_EL(pose.R, 2, 2)) * RAD_TO_DEG;
        #if DEBUG >= 1    
                // Print the yaw, pitch, and roll of the camera
                //// jwc y Serial.printf("y,p,r: %15f, %15f, %15f\n", yaw, pitch, roll);
                Serial.printf(" *** y,p,r: %5.0f, %5.0f, %5.0f", yaw, pitch, roll);
        #endif         
                // Compute the transpose of the rotation matrix
                matd_t *R_transpose = matd_transpose(pose.R);

                // Negate the translation vector
                for (int i = 0; i < pose.t->nrows; i++) {
                    MATD_EL(pose.t, i, 0) = -MATD_EL(pose.t, i, 0);
                }

                // Compute the position of the camera in the tag's coordinate system
                matd_t *camera_position = matd_multiply(R_transpose, pose.t);
        #if DEBUG >= 1    
                //// jwc y Serial.printf("x,y,z: %15f, %15f, %15f\n", MATD_EL(camera_position, 0, 0), MATD_EL(camera_position, 1, 0), MATD_EL(camera_position, 2, 0));
                //// jwc o Serial.printf("*** x,y,z: %5.0f, %5.0f, %5.0f\n", MATD_EL(camera_position, 0, 0), MATD_EL(camera_position, 1, 0), MATD_EL(camera_position, 2, 0));
                Serial.printf(" *** x,y,z: %5.0f, %5.0f, %5.0f", MATD_EL(camera_position, 0, 0), MATD_EL(camera_position, 1, 0), MATD_EL(camera_position, 2, 0));
        #endif         
                // Free the matrices
                matd_destroy(R_transpose);
                matd_destroy(camera_position);
        #if DEBUG >= 1    
                //// jwc o Serial.println("");
                Serial.printf("\n");
        #endif         
                }
        #if DEBUG >= 1    
                //// jwc o Serial.println("");
            Serial.printf("\n");
        #endif         
            } 

            // Free detection result object
            apriltag_detections_destroy(detections);

            //// /\ jwc 25-0411-1800 convert to April-Tag Detect 
            
            esp_camera_fb_return(frame);
        }
        delay(1);
    }

    //// jwc \/ 
    ////
    // read from port 0, send to port 1:
    //// jwc y if (Serial.available()) {
    //// jwc yyy if (Serial.available()) {

    //// jwc ? Serial.printf("Received %d bytes\n", Serial.available());
    //// jwc ? Serial.printf("First byte is '%c' [0x%02x]\n", Serial.peek(), Serial.peek());
    uint8_t charPerLine = 0;

    while (Serial.available()) {
        char c = Serial.read();
        //// jwc This appears to work-NOT via Uart0 Port: Serial.printf("P:0< '%c' [0x%02x] ", c, c);
        //// jwc y printf("p:0< '%c' [0x%02x] ", c, c);
        //// jwc yy printf("0< '%c' [0x%02x] ", c, c);
        printf("0<'%c'", c);

        //// jwc oy if (++charPerLine >= 10) {
        //// jwc oy     charPerLine = 0;
        //// jwc oy     //// jwc This appears to work-NOT via Uart0 Port: Serial.printf("P:\n");
        //// jwc oy     //// jwc y printf("p:\n");
        //// jwc oy     printf("\n");
        //// jwc oy }
    }
    //// jwc yyy //// jwc yy int inByte = Serial.read();
    //// jwc yyy String str = Serial.readString();
    //// jwc yyy str.trim();

    //
    //// jwc n Serial.write("**Serial_0\n");
    //// jwc n Serial.write(inByte);
    //// jwc y Serial.print("**Serial_0\n");
    //// jwc n Serial.print("*** Serial_0:%d\n", inByte);
    //// jwc n Serial0.print("*** Serial_0:%d\n", inByte);
    //// jwc yy printf("0:Serial_0:%d\n", inByte);
    //// jwc yyy Serial.print  (">0");
    //// jwc yyy Serial.println(str);

    //// jwc o Serial.write(inByte);
    //// jwc yyy }
    
    //// jwc ? //// jwc y Serial2.print("Hello\n");
    //// jwc ? //// jwc yy Serial2.print("123");
    //// jwc ? //// jwc y Serial2 not received: printf("123");
    //// jwc ? //// jwc y Add '\n' for microbit \/: Serial2.print("123");
    //// jwc ? //// jwc yy Serial2.print("123\n");

    //// jwc ? const int BUFFER_SIZE = 10;
    //// jwc ? char buf[BUFFER_SIZE];
    //// jwc ? 
    //// jwc ? //// jwc yyy if (Serial2.available()) {
    //// jwc ? 
    //// jwc ? 
    //// jwc ? //// jwc ? Serial2.printf("Received %d bytes\n", Serial2.available());
    //// jwc ? //// jwc ? Serial2.printf("First byte is '%c' [0x%02x]\n", Serial2.peek(), Serial2.peek());
    //// jwc ? uint8_t charPerLine_2 = 0;
    //// jwc ? 
    //// jwc ? while (Serial2.available()) {
    //// jwc ?   char c = Serial2.read();
    //// jwc ?   //// jwc 25-0125-0500 Serial2.printf("2< '%c' [0x%02x] ", c, c);
    //// jwc ? 
    //// jwc ?   //Serial2.write(c);
    //// jwc ?   printf("2< '%c' [0x%02x] ", c, c);
    //// jwc ? 
    //// jwc ?   //// jwc 25-0125-0500 if (++charPerLine_2 >= 10) {
    //// jwc ?   //// jwc 25-0125-0500   charPerLine_2 = 0;
    //// jwc ?   //// jwc 25-0125-0500   Serial2.println();
    //// jwc ?   //// jwc 25-0125-0500 }
    //// jwc ? }
    

    int key_Int = random(0,4);
    //// jwc y int value_Int = random(5,9);
    int value_Int = random(7,10);
  
    //// jwc oy String string_String = "2>" + String(value_Int) + "\n";
    String string_String = "0>" + String(value_Int) + "\n";
  
    //// jwc y Serial2.printf("2>%d\n", value_Int);
    //// jwc y printf("2>:%d\n", value_Int);
    //// jwc n Serial2.printf(string_String);
    //// jwc n Serial2.printf("%S", string_String);
    //// jwc oy Serial2.printf("%s", string_String);
    //// jwc This appears to work-NOT via Uart0 Port: Serial.printf("P:%s", string_String);
    //// jwc This appears to work via Uart0 Port \/
    printf("%s", string_String);
  

    
    //// jwc This appears to work-NOT via Uart0 Port
    //// jwc y Serial.printf("P: .0.\n");
    Serial.printf("*");
    //// jwc This appears to work via Uart0 Port \/
    //// jwc y printf("p: .0.\n");
    printf(".");

}  

