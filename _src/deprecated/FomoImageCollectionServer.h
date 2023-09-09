//
// Created by Simone on 09/02/23.
//

#ifndef ELOQUENTESP32CAM_FOMO_IMAGE_COLLECTION_H
#define ELOQUENTESP32CAM_FOMO_IMAGE_COLLECTION_H

#include <WebServer.h>
#include "../Cam.h"
#include "../JpegDecoderGray.h"
#include "./features/DisplaysJpegFeed.h"

#ifndef COLLECT_IMAGE_WIDTH
#define COLLECT_IMAGE_WIDTH 96
#endif

#ifndef COLLECT_IMAGE_HEIGHT
#define COLLECT_IMAGE_HEIGHT COLLECT_IMAGE_WIDTH
#endif


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            namespace FOMO {
                const char *fomoImageCollectionHTML PROGMEM = R"===(
<style>
#jpeg {max-width: 640px; max-height: 480px;}
</style>
<div id="app" x-data="CollectImages()" class="p-2">
    <div id="frame">
        <img id="jpeg" />
        <div class="mt-4" x-ref="images">
            <div x-show="!collecting" class="flex space-x-4">
                <button class="bg-green-200 text-green-500 rounded-lg p-2" @click.prevent="start()">Start collecting</button>
                <button x-show="images.length > 0" class="bg-red-200 text-red-500 rounded-lg p-2" @click.prevent="reset()">Clear</button>
                <button x-show="images.length > 0" class="bg-purple-200 text-purple-500 rounded-lg p-2" @click.prevent="download()">Download</button>
            </div>
            <div x-show="collecting" class="">
                <button class="bg-red-200 text-ted-500 rounded-lg p-2" @click.prevent="stop()">Pause collection</button>
            </div>
            <div class="sm:flex sm:flex-wrap mt-4 images">
                <template x-for="image, i in images" :key="image.timestamp">
                    <div class="image relative group">
                        <img :src="image.src" :style="style" />
                        <button class="absolute top-1 right-1 group-hover:visible text-red-500 rounded-lg w-4 h-4" @click.prevent="remove(i)">
                            <svg class="fill-current w-full h-full" viewBox="0 0 1920 1920" xmlns="http://www.w3.org/2000/svg"><path d="M1581.176 1750.588c0 31.06-25.411 56.47-56.47 56.47H395.294c-31.059 0-56.47-25.41-56.47-56.47V564.706H225.882v1185.882c0 93.403 76.01 169.412 169.412 169.412h1129.412c93.402 0 169.412-76.01 169.412-169.412V564.706h-112.942v1185.882Zm-903.529-169.412h112.941V677.647h-112.94v903.53Zm451.765 0h112.94V677.647h-112.94v903.53Zm211.211-1242.352L1217.065 0H694.6L571.268 338.824H.01v112.94h1920v-112.94h-579.388Zm-649.299 0 82.334-225.883h364.462l82.334 225.883h-529.13Z" fill-rule="evenodd"/></svg>
                        </button>
                    </div>
                </template>
            </div>
        </div>
    </div>
</div>
<script>
window.CollectImages = function() {
    return {
        images: [],
        collecting: false,

        init() {
            this.loop()
        },

        loop() {
            if (this.collecting) {
                fetch("/collect")
                    .then(res => res.text())
                    .then(hex => hex.match(/[0-9A-Z]{2}/g))
                    .then(chunks => chunks.map(chunk => parseInt(chunk, 16)))
                    .then(pixels => {
                        console.log("pixels", pixels)
                        this.images.push({
                            timestamp: +(new Date()),
                            src: this.toImageSrc(pixels)
                        })
                    })
                    .catch(() => null)
                    .finally(() => this.loop())
            }
            else {
                setTimeout(() => this.loop(), 100)
            }
        },

        start() {
            this.collecting = true
        },

        stop() {
            this.collecting = false
        },

        reset() {
            this.images.splice(0, this.images.length)
        },

        download() {
            const zip = new JSZip()
            const classname = prompt("Enter label")

            this.images.forEach(image => {
                const data = image.src.replace("data:image/png;base64,", "")
                zip.file(`${classname}_${image.timestamp}.jpg`, data, {base64: true})
            })

            zip.generateAsync({type:"blob"})
                .then(content => saveAs(content, `${classname}.zip`))

        },

        remove(i) {
            this.images.splice(i, 1)
        },

        /**
         * Convert pixel data into a img.src
         * @param pixels
         */
        toImageSrc(pixels) {
            const canvas = document.createElement("canvas")

            canvas.setAttribute("width", window.APP.collectWidth);
            canvas.setAttribute("height", window.APP.collectHeight);

            const ctx = canvas.getContext("2d")
            const img = pixels.map(pixel => [pixel, pixel, pixel, 255]).flat()
            console.log("num pixels", pixels.length, pixels.length / 96)
            const palette = ctx.getImageData(0, 0, window.APP.collectWidth, window.APP.collectHeight)

            palette.data.set(new Uint8ClampedArray(img))
            ctx.putImageData(palette, 0, 0)

            return canvas.toDataURL()
        },

        get style() {
            return `width: ${window.APP.collectWidth}px; height: ${window.APP.collectHeight}px`
        }
    }
}
</script>
    )===";

                /**
                 * Collect images for TinyML in the browser
                 */
                class CollectImagesServer : public HasErrorMessage {
                public:
                    Cam *cam;
                    JpegDecoderGray decoder;
                    Features::HTML html;

                    /**
                     *
                     * @param cam
                     */
                    CollectImagesServer(Cam &camera) :
                            cam(&camera),
                            jpeg(html.webServer, camera) {

                    }

                    /**
                     *
                     * @return
                     */
                    bool begin() {
                        if (!jpeg.registerRoute())
                            return setErrorMessage(jpeg.getErrorMessage());

                        if (cam->getHeight() < COLLECT_IMAGE_HEIGHT * 8)
                            return setErrorMessage(
                                    String("Image size is too small. It MUST be at least ") + (COLLECT_IMAGE_HEIGHT * 8));

                        onIndex();
                        onCollect();

                        html.webServer.begin();
                        return setErrorMessage("");
                    }

                    /**
                     *
                     */
                    void handle() {
                        html.webServer.handleClient();
                    }

                    /**
                     * Get endpoints of HTTP server
                     *
                     * @return
                     */
                    String getWelcomeMessage() {
                        return String("CollectImages http server available at ")
                               + cam->getAddress()
                               + ". JPEG feed available at /jpeg. 1/8 th grayscale pixels available at /gray";
                    }

                protected:
                    Features::DisplaysJpegFeed jpeg;


                    /**
                     *
                     */
                    void onIndex() {
                        html.webServer.on("/", [this]() {
                            html.html(fomoImageCollectionHTML);
                            html.component(jpeg);
                            html.tailwindcss();
                            html.alpinejs();
                            html.remoteJs("https://cdnjs.cloudflare.com/ajax/libs/FileSaver.js/2.0.5/FileSaver.min.js");
                            html.remoteJs("https://cdnjs.cloudflare.com/ajax/libs/jszip/3.10.1/jszip.min.js");
                            html.declareJsVariable("collectWidth", COLLECT_IMAGE_WIDTH);
                            html.declareJsVariable("collectHeight", COLLECT_IMAGE_HEIGHT);
                            html.end();
                        });
                    }


                    /**
                     * Send cropped (hex, grayscale) image
                     */
                    void onCollect() {
                        html.webServer.on("/collect", [this]() {
                            if (!decoder.decode(*cam)) {
                                html.webServer.send(500, "text/plain", "Jpeg decode error");
                                return;
                            }

                            const uint16_t width = decoder.gray.width;
                            const uint16_t height = decoder.gray.height;
                            const uint16_t y0 = (height - COLLECT_IMAGE_HEIGHT) / 2;
                            const uint16_t x0 = (width - COLLECT_IMAGE_WIDTH) / 2;
                            const char *alphabet = "0123456789ABCDEF";

                            for (size_t y = y0; y < y0 + COLLECT_IMAGE_HEIGHT; y++) {
                                const size_t offset = y * width;
                                char line[COLLECT_IMAGE_WIDTH * 2 + 1];
                                uint8_t j = 0;

                                for (size_t x = x0; x < x0 + COLLECT_IMAGE_WIDTH; x++) {
                                    const uint8_t pixel = decoder.gray.pixels[offset + x];

                                    line[j++] = alphabet[pixel >> 4];
                                    line[j++] = alphabet[pixel & 0xF];
                                }

                                line[j] = '\0';
                                html.text(line);
                            }

                            html.end();
                        });
                    }

                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_FOMO_IMAGE_COLLECTION_H
