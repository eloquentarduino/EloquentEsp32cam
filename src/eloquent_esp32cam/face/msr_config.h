#ifndef ELOQUENT_ESP32CAM_FACE_MSRCONFIG
#define ELOQUENT_ESP32CAM_FACE_MSRCONFIG


namespace Eloquent {
    namespace Esp32cam {
        namespace Face {
            /**
             * Detect MSR detector
             */
            class MSRConfig {
                public:
                    struct {
                        float score_thresh;
                        float nms_thresh;
                        int top_k;
                        float resize_scale;
                    } config;

                    /**
                     * Constructor
                     */
                    MSRConfig() {
                        config.score_thresh = 0.1f;
                        config.nms_thresh = 0.5f;
                        config.top_k = 10;
                        config.resize_scale = 0.2f;
                    }
            };
        }
    }
}

#endif