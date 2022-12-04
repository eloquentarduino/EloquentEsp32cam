//
// Created by Simone on 15/11/22.
//

#ifndef ELOQUENTESP32CAM_MOTION_DETECTOR_H
#define ELOQUENTESP32CAM_MOTION_DETECTOR_H

#include "../../traits/HasErrorMessage.h"
#include "../../traits/Debounces.h"
#include "./Algorithm.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            /**
             * Algorithm-independent motion detection
             */
            class Detector : public HasErrorMessage, public Debounces {
            public:
                Algorithm *algorithm;
                size_t foregroundCount;
                bool foreground[MAX_RESOLUTION_WIDTH * MAX_RESOLUTION_HEIGHT / 64] = {false};

                /**
                 *
                 * @param algorithm
                 */
                Detector(Algorithm& algorithm) :
                algorithm(&algorithm),
                foregroundCount(0),
                _width(0),
                _height(0),
                _length(0),
                _epoch(0),
                _trainEpochs(1),
                _retrainAfter(0),
                _countThresh(0),
                _denoise(false) {

                }

                /**
                 * Set training epochs, if any
                 *
                 * @param epochs
                 */
                inline void trainFor(size_t epochs) {
                    _trainEpochs = epochs > 1 ? epochs : 1;
                }

                /**
                 *
                 * @return
                 */
                inline bool isTraining() {
                    return _epoch <= _trainEpochs;
                }

                /**
                 * Set min number of pixels that have to change
                 * to trigger motion
                 * @param threshold
                 */
                inline void triggerAbove(float threshold) {
                    _countThresh = threshold;
                }

                /**
                 * Retrain algorithm after given number of epochs
                 *
                 * @param epochs
                 */
                inline void retrainAfter(size_t epochs) {
                    _retrainAfter = epochs;
                }

                /**
                 * Perform denoising on foreground mask
                 * @param denoise
                 */
                inline void denoise(bool denoise = true) {
                    _denoise = denoise;
                }

                /**
                 * Set frame size
                 * @param width
                 * @param height
                 */
                inline void setSize(size_t width, size_t height) {
                    _width = width;
                    _height = height;
                }

                /**
                 * Restart training
                 */
                inline void forget() {
                    _epoch = 0;
                }

                /**
                 * Set param by string
                 * @param param
                 * @param value
                 */
                void set(const String& param, const float value) {
                    if (param == "denoise")
                        denoise(value > 0);
                    else if (param == "train")
                        trainFor(value);
                    else if (param == "retrain-after")
                        retrainAfter(value);
                    else if (param == "trigger-above")
                        triggerAbove(value);
                    else
                        algorithm->set(param, value);
                }

                /**
                 * Get current configuration as string
                 * @return
                 */
                String getCurrentConfig() {
                    return String("train=")
                           + _trainEpochs
                           + ", retrain-after="
                           + _retrainAfter
                           + ", trigger-above="
                           + _countThresh
                           + ", denoise="
                           + (_denoise ? "yes" : "no");
                }

                /**
                 * Get frame width
                 * @return
                 */
                inline size_t getWidth() {
                    return _width;
                }

                /**
                 * Get frame height
                 * @return
                 */
                inline size_t getHeight() {
                    return _height;
                }

                /**
                 *
                 * @return
                 */
                inline String getTriggerStatus() {
                    return String("Triggered ")
                        + foregroundCount
                        + '/'
                        + _length
                        + " pixels (threshold is "
                        + getCountThresh()
                        + ')';
                }

                /**
                 * Test if x, y is foreground
                 * @param x
                 * @param y
                 * @return
                 */
                inline bool isForeground(size_t x, size_t y) {
                    return x < getWidth() && y < getHeight() && foreground[y * getWidth() + x];
                }

                /**
                 * Alias
                 * @tparam Frame
                 * @param frame
                 * @return
                 */
                template<typename Frame>
                inline bool update(Frame frame) {
                    setSize(frame.width, frame.height);

                    return update(frame.pixels, frame.length);
                }

                /**
                 * Update background model
                 * @param pixels
                 * @param length
                 * @return
                 */
                bool update(uint8_t *pixels, size_t length) {
                    _length = length;
                    _epoch += 1;

                    if (_epoch > _retrainAfter)
                        forget();

                    if (_epoch == 1) {
                        algorithm->initialize(pixels, length);

                        return setErrorMessage("Initializing");
                    }

                    if (isTraining()) {
                        algorithm->train(pixels, length);

                        return setErrorMessage("Training");
                    }

                    for (size_t i = 0; i < length; i++)
                        foreground[i] = algorithm->isForeground(i, pixels[i]);

                    if (_denoise)
                        erode();

                    for (size_t i = 0; i < length; i++)
                        algorithm->update(i, pixels[i], foreground[i]);

                    return setErrorMessage("");
                }

                /**
                 * Detect if motion happened
                 * @return
                 */
                bool triggered() {
                    foregroundCount = 0;

                    for (size_t i = 0; i < _length; i++)
                        if (foreground[i])
                            foregroundCount += 1;

                    return debounced() && foregroundCount >= getCountThresh() && touch();
                }

                /**
                 * Detect if motion happened in RoI
                 * @return
                 */
                template<typename RoI>
                bool triggeredIn(RoI& roi) {
                    uint16_t x1;
                    uint16_t x2;
                    uint16_t y1;
                    uint16_t y2;
                    size_t foregroundCount = 0;

                    roi.forget();
                    roi.setBoundaries(_width, _height);
                    roi.getBoundingBox(&x1, &y1, &x2, &y2);

                    for (size_t y = y1; y < y2; y++) {
                        const size_t offset = y * _width;

                        for (size_t x = x1; x < x2; x++)
                            if (roi.includes(x, y)) {
                                roi.incrementArea();

                                if (foreground[offset + x]) {
                                    foregroundCount += 1;
                                    roi.incrementForeground();
                                }
                            }
                    }

                    return debounced() && roi.triggered() && touch();
                }

                /**
                 * Convert foreground mask to string
                 * @return
                 */
                String toString() {
                    static char buffer[MAX_RESOLUTION_WIDTH / 8 * MAX_RESOLUTION_HEIGHT / 8 + 1] = {'0'};
                    buffer[MAX_RESOLUTION_WIDTH / 8 * MAX_RESOLUTION_HEIGHT / 8] = '\0';

                    for (size_t i = 0; i < _length; i++)
                        buffer[i] = foreground[i] ? '1' : '0';

                    return String(buffer);
                }

                /**
                 *
                 * @tparam Printer
                 * @param printer
                 */
                template<typename Printer>
                void printTo(Printer& printer) {
                    for (size_t i = 0; i < _length; i++)
                        printer.print(foreground[i] ? '1' : '0');
                }

            protected:
                size_t _epoch;
                size_t _trainEpochs;
                size_t _retrainAfter;
                size_t _length;
                size_t _width;
                size_t _height;
                bool _denoise;
                float _countThresh;

                /**
                 * Get number of required changes to trigger motion
                 * @return
                 */
                inline size_t getCountThresh(size_t length = 0) {
                    return _countThresh >= 1 ? _countThresh : _countThresh * (length ? length : _length);
                }

                /**
                 * Approximated erosion of foreground (2x2)
                 */
                void erode() {
                    if (!_width || !_height)
                        return;

                    for (size_t r = 0; r < _height - 1; r++) {
                        const size_t offset = r * _width;

                        for (size_t c = 0; c < _width - 1; c++) {
                            if (foreground[offset + c]) {
                                const size_t nextRowOffset = offset + _width;

                                foreground[offset + c] =
                                        foreground[offset + c + 1] // upper right
                                        && foreground[nextRowOffset + c] // lower left
                                        && foreground[nextRowOffset + c + 1]; // lower right
                            }
                        }
                    }
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_MOTION_DETECTOR_H
