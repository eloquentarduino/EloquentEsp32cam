/**
 * Alter camera pixels before sending them via MJPEG stream
 * (requires enough RAM to run)
 * (expect 0.5 - 2 FPS)
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"
#define HOSTNAME  "esp32cam"

#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/viz/mjpeg.h>

using namespace eloq;
using namespace eloq::viz;

uint16_t jpeg_length = 0;
size_t tick = 0;


// prototype of the function that will
// re-encode the frame on-the-fly
void reencode_frame(WiFiClient *client, camera_fb_t* frame);

// prototype of the functon that will
// put JPEG-encoded data back into the frame
size_t buffer_jpeg(void * arg, size_t index, const void* data, size_t len);


/**
 *
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("__RE-ENCODE MJPEG STREAM__");

    // camera settings
    // replace with your own model!
    camera.pinout.aithinker();
    camera.brownout.disable();
    // higher resolution cannot be handled
    camera.resolution.qvga();
    camera.quality.best();

    // since we want to access the raw pixels
    // capture in RGB565 format
    // keep in mind that you need a lot of RAM to store
    // all this data at high resolutions
    // (e.g. QVGA = 320 x 240 x 2 = 1536 kB)
    camera.pixformat.rgb565();

    // MJPEG settings
    mjpeg.onFrame(&reencode_frame);

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    // connect to WiFi
    while (!wifi.connect().isOk())
        Serial.println(wifi.exception.toString());

    // start mjpeg http server
    while (!mjpeg.begin().isOk())
        Serial.println(mjpeg.exception.toString());

    // assert camera can capture frames
    while (!camera.capture().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera OK");
    Serial.println("ToF OK");
    Serial.println("WiFi OK");
    Serial.println("MjpegStream OK");
    Serial.println(mjpeg.address());
}

/**
 *
 */
void loop() {
    // nothing to do here, MJPEG server runs in background
}


/**
 * Apply your custom processing to pixels
 * then encode to JPEG.
 * You will need to modify this
 */
void reencode_frame(WiFiClient *client, camera_fb_t* frame) {
    // log how much time elapsed from last frame
    const size_t now = millis();
    const uint16_t height = camera.resolution.getHeight();
    const uint16_t width = camera.resolution.getWidth();

    ESP_LOGI("benchmark", "%d ms elapsed from last frame", now - tick);
    tick = now;

    // frame->buf contains RGB565 data
    // that is, 2 bytes per pixel
    //
    // in this test, we're going to do a "negative" effect
    // feel free to replace this with your own code
    for (uint16_t y = 0; y < height; y++) {
        uint16_t *row = (uint16_t*) (frame->buf + width * 2 * y);

        for (uint16_t x = 0; x < width; x++) {
            // read pixel and parse to R, G, B components
            const uint16_t pixel = row[x];
            uint16_t r = (pixel >> 11) & 0b11111;
            uint16_t g = (pixel >> 5) & 0b111111;
            uint16_t b = pixel & 0b11111;

            // actual work: make negative
            r = 31 - r;
            g = 63 - g;
            b = 31 - b;

            // re-pack to RGB565
            row[x] = (r << 11) | (g << 5) | b;
        }
    }

    // encode to jpeg
    uint8_t quality = 90;

    frame2jpg_cb(frame, quality, &buffer_jpeg, NULL);
    ESP_LOGI("var_dump", "JPEG size=%d", jpeg_length);
}


/**
 * Put JPEG-encoded data back into the original frame
 * (you don't have to modify this)
 */
size_t buffer_jpeg(void *arg, size_t index, const void* data, size_t len) {
    if (index == 0) {
        // first MCU block => reset jpeg length
        jpeg_length = 0;
    }

    if (len == 0) {
        // encoding is done
        camera.frame->len = jpeg_length;
        return 0;
    }

    jpeg_length += len;

    // override input data
    memcpy(camera.frame->buf + index, (uint8_t*) data, len);

    return len;
}
