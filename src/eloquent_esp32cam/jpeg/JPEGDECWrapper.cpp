#include <Arduino.h>
#include "JPEGDECWrapper.h"

#if defined(__JPEGDEC__)
#include <JPEGDEC.h>

using Eloquent::Esp32cam::JPEG::JPEGDECWrapper;

/**
 *
 * @param mcu
 * @return
 */
int __handleMCU__(jpeg_draw_tag *mcu) {
    JPEGDECWrapper *jpeg = (JPEGDECWrapper*) mcu->pUser;
    const auto crop = jpeg->crop;
    const uint8_t bpp = mcu->iBpp / 8;

    for (size_t i = 0; i < mcu->iHeight; i++) {
        const size_t inputOffset = i * mcu->iWidth * bpp;

        if (!jpeg->addMCU(mcu->x, mcu->y + i, bpp, ((uint8_t*) mcu->pPixels) + inputOffset, mcu->iWidthUsed))
            return 0;
    }

    return 1;
}

#endif