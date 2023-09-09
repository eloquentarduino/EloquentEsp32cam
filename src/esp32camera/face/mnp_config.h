#ifndef ELOQUENT_ESP32CAM_FACE_MNPCONFIG
#define ELOQUENT_ESP32CAM_FACE_MNPCONFIG


namespace Eloquent {
    namespace Esp32cam {
        namespace Face {
            /**
             * Configure MNP detector
             */
            class MNPConfig {
                public:
                    struct {
                        float score_thresh;
                        float nms_thresh;
                        int top_k;
                    } config;

                    /**
                     * Constructor
                     */
                    MNPConfig() {
                        config.score_thresh = 0.5f;
                        config.nms_thresh = 0.3f;
                        config.top_k = 5;
                    }
            };
        }
    }
}

#endif