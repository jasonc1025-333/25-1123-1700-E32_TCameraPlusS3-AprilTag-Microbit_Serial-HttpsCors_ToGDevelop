//// jwc 25-0411-1800 convert to April-Tag Detect 
////     * libraries\Mylibrary\pin_config.h

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

CAMERA_MODEL_WAVESHARE_ESP32S3_TFT2P0IN_CAM

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
//// jwc y #define DEBUG 3
#define DEBUG 1

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
        //// jwc ? s->set_brightness(s, 0);     // -2 to 2
        //// jwc ? s->set_contrast(s, 0);       // -2 to 2
        //// jwc ? s->set_saturation(s, 0);     // -2 to 2
        //// jwc ? s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
        //// jwc ? s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
        //// jwc ? s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
        //// jwc ? s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
        //// jwc ? s->set_aec2(s, 1);           // 0 = disable , 1 = enable
        //// jwc ? s->set_ae_level(s, 0);       // -2 to 2
        //// jwc ? s->set_aec_value(s, 168);    // 0 to 1200
        //// jwc ? s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
        //// jwc ? s->set_agc_gain(s, 0);       // 0 to 30
        //// jwc ? s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
        //// jwc ? s->set_bpc(s, 0);            // 0 = disable , 1 = enable
        //// jwc ? s->set_wpc(s, 1);            // 0 = disable , 1 = enable
        //// jwc ? s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
        //// jwc ? s->set_lenc(s, 1);           // 0 = disable , 1 = enable
        //// jwc ? s->set_hmirror(s, 1);        // 0 = disable , 1 = enable
        //// jwc ? s->set_vflip(s, 1);          // 0 = disable , 1 = enable
        //// jwc ? s->set_dcw(s, 1);            // 0 = disable , 1 = enable
        
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
    s->set_vflip(s, 0);  
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

    // Create tag family object
    apriltag_family_t *tf = tag36h11_create();

    // Create AprilTag detector object
    apriltag_detector_t *td = apriltag_detector_create();

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
    Serial.println("Ciallo");

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

            gfx->draw16bitBeRGBBitmap(0, 0, (uint16_t *)frame->buf, frame->width, frame->height);
            esp_camera_fb_return(frame);
        }
        delay(1);
    }
}