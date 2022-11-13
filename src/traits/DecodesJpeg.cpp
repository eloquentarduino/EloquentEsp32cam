//
// Created by Simone on 07/11/22.
//

#include "./DecodesJpeg.h"


/**
 * Callback for picojpeg decoding
 *
 * @param dest
 * @param chunkSize
 * @param read
 * @param data
 * @return
 */
unsigned char pjpeg_need_bytes_callback(unsigned char* dest, unsigned char chunkSize, unsigned char *read, void *data) {
    Eloquent::Esp32cam::DecodesJpeg *cam = (Eloquent::Esp32cam::DecodesJpeg *) data;
    uint16_t len = cam->jpegLength;
    uint16_t offset = cam->getJpegOffset();

    if (!len) {
        *read = 0;
        return 0;
    }

    if (offset > len)
        return PJPG_STREAM_READ_ERROR;

    uint16_t n = len - offset > chunkSize ? chunkSize : len - offset;

    memcpy(dest, cam->jpegBuf + offset, n);
    *read = n;
    cam->consumeJpeg(n);

    return 0;
}


/**
 *
 * @return
 */
bool Eloquent::Esp32cam::DecodesJpeg::decode() {
    int status;
    uint16_t i = 0;

    if (pjpeg_decode_init(&_jpeg, pjpeg_need_bytes_callback, (void *) this, 1))
        return this->setErrorMessage("Jpeg decode error");

    while ((status = pjpeg_decode_mcu()) != PJPG_NO_MORE_BLOCKS) {
        if (i > MAX_RESOLUTION_WIDTH * MAX_RESOLUTION_HEIGHT / 64)
            continue;

        switch (_jpeg.m_scanType) {
            case PJPG_GRAYSCALE:
                Luma.buf[i] = _jpeg.m_pMCUBufR[0];
                Cb.buf[i] = 0;
                Cr.buf[i++] = 0;
                break;
            case PJPG_YH1V1:
                Luma.buf[i] = _jpeg.m_pMCUBufR[0];
                Cb.buf[i] = _jpeg.m_pMCUBufG[0];
                Cr.buf[i++] = _jpeg.m_pMCUBufB[0];
                break;
            case PJPG_YH1V2:
                Luma.buf[i] = _jpeg.m_pMCUBufR[0];
                Cb.buf[i] = _jpeg.m_pMCUBufG[0];
                Cr.buf[i++] = _jpeg.m_pMCUBufB[0];
                Luma.buf[i] = _jpeg.m_pMCUBufR[128];
                Cb.buf[i] = _jpeg.m_pMCUBufG[128];
                Cr.buf[i++] = _jpeg.m_pMCUBufB[128];
                break;
            case PJPG_YH2V1:
                Luma.buf[i] = _jpeg.m_pMCUBufR[0];
                Cb.buf[i] = _jpeg.m_pMCUBufG[0];
                Cr.buf[i++] = _jpeg.m_pMCUBufB[0];
                Luma.buf[i] = _jpeg.m_pMCUBufR[64];
                Cb.buf[i] = _jpeg.m_pMCUBufG[64];
                Cr.buf[i++] = _jpeg.m_pMCUBufB[64];
                break;
            case PJPG_YH2V2:
                Luma.buf[i] = _jpeg.m_pMCUBufR[0];
                Cb.buf[i] = _jpeg.m_pMCUBufG[0];
                Cr.buf[i++] = _jpeg.m_pMCUBufB[0];
                Luma.buf[i] = _jpeg.m_pMCUBufR[64];
                Cb.buf[i] = _jpeg.m_pMCUBufG[64];
                Cr.buf[i++] = _jpeg.m_pMCUBufB[64];
                Luma.buf[i] = _jpeg.m_pMCUBufR[128];
                Cb.buf[i] = _jpeg.m_pMCUBufG[128];
                Cr.buf[i++] = _jpeg.m_pMCUBufB[128];
                Luma.buf[i] = _jpeg.m_pMCUBufR[192];
                Cb.buf[i] = _jpeg.m_pMCUBufG[192];
                Cr.buf[i++] = _jpeg.m_pMCUBufB[192];
                break;
        }
    }

    Luma.len = i;
    Cb.len = i;
    Cr.len = i;
}