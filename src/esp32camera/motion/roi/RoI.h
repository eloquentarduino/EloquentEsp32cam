//
// Created by Simone on 15/11/22.
//

#ifndef ELOQUENTESP32CAM_ROI_H
#define ELOQUENTESP32CAM_ROI_H

namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            namespace RoI {

                /**
                 * Abstract Region of Interest
                 */
                class RegionOfInterest {
                public:
                    size_t area;
                    size_t foreground;

                    /**
                     *
                     * @param threshold
                     */
                    inline void setThreshold(float threshold) {
                        _threshold = threshold;
                    }

                    /**
                     *
                     * @param threshold
                     */
                    inline float getThreshold() {
                        return _threshold;
                    }

                    /**
                     * Test if RoI triggered motion
                     * @param count
                     */
                    bool triggered() {
                        return foreground >= getThresholdCount();
                    }

                    /**
                     * Reset state
                     */
                    inline void forget() {
                        area = 0;
                        foreground = 0;
                    }

                    /**
                     * Increment area
                     */
                    void incrementArea() {
                        area += 1;
                    }

                    /**
                     * Increment foreground
                     */
                    void incrementForeground() {
                        foreground += 1;
                    }

                    /**
                     * Get debug message on trigger status
                     * @return
                     */
                    String getTriggerStatus() {
                        return String("Triggered ")
                               + foreground
                               + '/'
                               + area
                               + " pixels (threshold is "
                               + getThresholdCount()
                               + ')';
                    }

                    /**
                     * Chain RoI
                     * (for motion live feed)
                     * @param next
                     */
                    void chain(RegionOfInterest& next) {
                        if (_next == NULL)
                            _next = &next;
                        else
                            _next->chain(next);
                    }

                    /**
                     * Get next RoI
                     * (for motion live feed)
                     * @return
                     */
                    RegionOfInterest* next() {
                        return _next;
                    }

                    /**
                     * Set frame boundaries
                     * @param width
                     * @param height
                     */
                    virtual void setBoundaries(const uint16_t width, const uint16_t height) = 0;

                    /**
                     * Get bounding box to speed calculation
                     * @param x1
                     * @param y1
                     * @param x2
                     * @param y2
                     */
                    virtual void getBoundingBox(uint16_t* x1, uint16_t* y1, uint16_t* x2, uint16_t* y2) = 0;

                    /**
                     * Test if RoI includes given point
                     * @param x
                     * @param y
                     * @return
                     */
                    virtual bool includes(const uint16_t x, const uint16_t y) = 0;

                    /**
                     * Get Javascript code to render RoI on canvas
                     * @return
                     */
                    virtual String drawToCanvas() = 0;

                protected:
                    float _threshold;
                    RegionOfInterest *_next;

                    /**
                     *
                     * @return
                     */
                    inline uint16_t getThresholdCount() {
                        const float t = getThreshold();

                        return (t >= 1) ? t : t * area;
                    }
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_ROI_H
