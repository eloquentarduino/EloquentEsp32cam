//
// Created by Simone on 13/02/23.
//

#ifndef ELOQUENTESP32CAM_FOMO_H
#define ELOQUENTESP32CAM_FOMO_H

#if defined(EI_CLASSIFIER_OBJECT_DETECTION)

    #include "edge-impulse-sdk/dsp/image/image.hpp"
    #include "../../../traits/HasErrorMessage.h"
    #include "../../../traits/BenchmarksCode.h"
    #include "../../Cam.h"
    #include "../../JpegDecoderGray.h"

    namespace Eloquent {
        namespace Esp32cam {
            namespace TinyML {
                namespace EdgeImpulse {
                    /**
                     * Run Edge Impulse FOMO model
                     */
                    class FOMO : public HasErrorMessage, public BenchmarksCode {
                    public:
                        ei::signal_t signal;
                        ei_impulse_result_t result;
                        EI_IMPULSE_ERROR error;

                        /**
                         *
                         */
                        FOMO() :
                            _debug(false) {
                            signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
                        }


                        /**
                         *
                         * @param enabled
                         */
                        void debug(bool enabled = true) {
                            _debug = enabled;
                        }

                        /**
                         *
                         * @return
                         */
                        size_t getDspTime() {
                            return result.timing.dsp;
                        }

                        /**
                         *
                         * @return
                         */
                        size_t getClassificationTime() {
                            return result.timing.classification;
                        }

                        #if EI_CLASSIFIER_HAS_ANOMALY == 1
                            /**
                             *
                             * @return
                             */
                            size_t getAnomalyTime() {
                                return result.timing.anomaly;
                            }

                            /**
                             *
                             */
                            float getAnomalyScore() {
                                return result.anomaly;
                            }

                            /**
                             *
                             */
                            size_t getTotalTime() {
                                return getDspTime() + getClassificationTime() + getAnomalyTime();
                            }
                        #else
                            /**
                             *
                             */
                            size_t getTotalTime() {
                                return getDspTime() + getClassificationTime();
                            }

                        #endif


                        /**
                         *
                         * @return
                         */
                        bool detectObjects(Cam& cam) {
                            signal.get_data = [cam, this](size_t offset, size_t length, float *out) {
                                if (!this->_decoder.decode(cam))
                                    return 0;

                                const size_t top = (this->_decoder.gray.height - EI_CLASSIFIER_INPUT_HEIGHT) / 2;
                                const size_t left = (this->_decoder.gray.width - EI_CLASSIFIER_INPUT_WIDTH) / 2;
                                size_t done = offset;
                                size_t i = 0;
                                size_t remaining = length;

                                while (remaining-- > 0) {
                                    const size_t row = done / EI_CLASSIFIER_INPUT_WIDTH;
                                    const size_t col = done % EI_CLASSIFIER_INPUT_WIDTH;
                                    const size_t ix = (top + row) * this->_decoder.gray.width + left + col;
                                    const uint32_t pixel = this->_decoder.at(left + col, top + row);

                                    done++;
                                    out[i++] = (pixel << 16) + (pixel << 8) + pixel;
                                }

                                return 0;
                            };

                            startBenchmark();
                            error = run_classifier(&signal, &result, _debug);
                            stopBenchmark();

                            if (error != EI_IMPULSE_OK) {
                                return setErrorMessage("Failed to run classifier");
                            }

                            return setErrorMessage("");
                        }

                        /**
                         * Check if objects were found
                         */
                        bool hasObjects() {
                            return result.bounding_boxes[0].value != 0;
                        }

                        /**
                         * Run function on each bounding box found
                         */
                        template<typename Callback>
                        void forEach(Callback callback) {
                            for (size_t ix = 0, i = 0; ix < result.bounding_boxes_count; ix++) {
                                auto bb = result.bounding_boxes[ix];

                                if (bb.value != 0)
                                    callback(i++, bb);
                            }
                        }

                        /**
                         *
                         */
                        ei_impulse_result_bounding_box_t at(size_t ix) {
                            return result.bounding_boxes[ix];
                        }

                        /**
                         * Get count of (non background) bounding boxes
                         */
                        size_t count() {
                            size_t count = 0;

                            for (size_t ix = 0, i = 0; ix < result.bounding_boxes_count; ix++) {
                                auto bb = result.bounding_boxes[ix];

                                if (bb.value != 0)
                                    count++;
                            }

                            return count;
                        }

                    protected:
                        bool _debug;
                        Eloquent::Esp32cam::JpegDecoderGray _decoder;
                    };
                }
            }
        }
    }
#else
#error "FOMO model not found"
#endif

#endif //ELOQUENTESP32CAM_FOMO_H
