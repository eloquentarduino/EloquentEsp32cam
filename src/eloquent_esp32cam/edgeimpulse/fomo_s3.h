#ifndef ELOQUENT_ESP32CAM_EDGEIMPULSE_FOMO_S3_H
#define ELOQUENT_ESP32CAM_EDGEIMPULSE_FOMO_S3_H

#if defined(EI_CLASSIFIER_OBJECT_DETECTION)
    #include "../transform/crop.h"
    #include "./fomo.h"

    namespace Eloquent {
        namespace Esp32cam {
            namespace EdgeImpulse {
                /**
                 * Run Edge Impulse FOMO model
                 */
                class FOMOS3 : public FOMO {
                public:
                    uint8_t input[EI_CLASSIFIER_NN_INPUT_FRAME_SIZE];

                    /**
                     *
                     */
                    FOMOS3() :
                        exception("FOMO"),
                        first(0, 0, 0, 0, 0),
                        _isDebugEnabled(false) {
                            signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
                            signal.get_data = [this](size_t offset, size_t length, float *out) {
#if _FOMO_IS_RGB_
                                for (size_t i = 0, off = offset * 3; i < length; i++, off += 3) {
                                    const uint32_t r = input[off + 0];
                                    const uint16_t g = input[off + 1];
                                    const uint8_t  b = input[off + 2];

                                    out[i] = (r << 16) | (g << 8) | b;
                                }
#else
                                for (size_t i = 0; i < length; i++) {
                                    const uint32_t gray = input[offset + i];
                                    out[i] = (gray << 16) | (gray << 8) | gray;
                                }
#endif

                                return 0;
                            };
                    }

                    /**
                     *
                     * @return
                     */
                    template<typename Frame>
                    Exception& detectObjects(Frame& frame) {
                        if (!frame.length)
                            return exception.set("Cannot run FOMO on empty image");

                        benchmark.start();
                        // crop squash
                        crop
                            .from(frame)
                            .to(EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT)
                            .squash()
                            .rgb(_FOMO_IS_RGB_)
                            .nearest()
                            .apply(input);

                        error = run_classifier(&signal, &result, _isDebugEnabled);
                        benchmark.stop();

                        if (error != EI_IMPULSE_OK)
                            return exception.set(String("Failed to run classifier with error code ") + error);

                        return exception.clear();
                    }
                };
            }
        }
    }

    namespace eloq {
        static Eloquent::Esp32cam::EdgeImpulse::FOMOS3 fomo;
    }

#else
#error "EdgeImpulse FOMO library not found"
#endif

#endif //ELOQUENT_ESP32CAM_EDGEIMPULSE_FOMO
