//
// Created by Simone on 26/11/22.
//

#ifndef ELOQUENTESP32CAM_MOTION_SLIDINGWINDOW_H
#define ELOQUENTESP32CAM_MOTION_SLIDINGWINDOW_H

#include "./Localizer.h"
#include "../../../traits/BenchmarksCode.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            namespace Localization {
                /**
                 * Try to locate motion region with a sliding window
                 */
                class SlidingWindow : public BenchmarksCode, public Localizer {
                public:
                    /**
                     *
                     */
                    SlidingWindow(Detector& detector) :
                        _thresh(0.5),
                        _earlyStop(true) {
                        this->detector = &detector;
                    }

                    /**
                     * Set minimum coverage percentage (in range 0 - 1)
                     * @param threashold
                     */
                    void setMinCoverage(float threashold) {
                        if (threashold < 0)
                            threashold = 0.5;

                        if (threashold > 1)
                            threashold = 1;

                        _thresh = threashold;
                    }

                    /**
                     * Stop as soon as you find a match
                     */
                    inline void earlyStop() {
                        _earlyStop = true;
                    }

                    /**
                     * Don't stop as soon as you find a match
                     */
                    inline void dontEarlyStop() {
                        _earlyStop = false;
                    }

                    /**
                     * Localize area of motion
                     * @param detector
                     * @return
                     */
                    bool localize() {
                        uint8_t dividers[] = {3, 4, 8};
                        const uint16_t width = detector->getWidth();
                        const uint16_t height = detector->getHeight();

                        startBenchmark();

                        for (uint8_t k = 0; k < sizeof(dividers); k++) {
                            const uint8_t divider = dividers[k];
                            const uint16_t dx = width / divider;
                            const uint16_t dy = height / divider;
                            const size_t area = dx * dy;
                            const size_t minCoverage = _thresh * area;
                            size_t maxForeground = 0;

                            for (uint16_t y = 0; y < height - dy; y += dy / 2) {
                                for (uint16_t x = 0; x < width - dx; x += dx / 2) {
                                    size_t foreground = 0;

                                    for (uint16_t i = y; i < y + dy; i++) {
                                        const size_t left = (y + dy - i) * dx;

                                        // exit early whenever possible
                                        if (foreground < minCoverage && minCoverage - foreground > left)
                                            break;

                                        for (uint16_t j = x; j < x + dx; j++) {
                                            if (detector->isForeground(j, i))
                                                foreground += 1;
                                        }
                                    }

                                    // we found a match
                                    if (foreground >= minCoverage) {
                                        if (_earlyStop) {
                                            this->x = x * 8;
                                            this->y = y * 8;
                                            this->width = dx * 8;
                                            this->height = dy * 8;
                                            this->coverage = ((float) foreground) / area;

                                            stopBenchmark();
                                            return true;
                                        }
                                        else if (foreground > maxForeground) {
                                            this->x = x * 8;
                                            this->y = y * 8;
                                            this->width = dx * 8;
                                            this->height = dy * 8;
                                            this->coverage = ((float) foreground) / area;

                                            maxForeground = foreground;
                                        }
                                    }
                                }
                            }

                            if (maxForeground > 0) {
                                stopBenchmark();
                                return true;
                            }
                        }

                        stopBenchmark();
                        return false;
                    }

                protected:
                    float _thresh;
                    bool _earlyStop;
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_MOTION_SLIDINGWINDOW_H
