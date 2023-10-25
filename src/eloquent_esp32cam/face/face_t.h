#ifndef ELOQUENT_ESP32CAM_FACE_FACE_T
#define ELOQUENT_ESP32CAM_FACE_FACE_T


namespace Eloquent {
    namespace Esp32cam {
        namespace Face {
            /**
             * Face datatype
             */
            class Face {
                public:
                    uint16_t x;
                    uint16_t y;
                    uint16_t x0;
                    uint16_t y0;
                    uint16_t x1;
                    uint16_t y1;
                    uint16_t width;
                    uint16_t height;
                    struct {
                        uint16_t x;
                        uint16_t y;
                    } leftEye, rightEye, nose, leftMouth, rightMouth;
                    std::list<dl::detect::result_t>::iterator result;

                    /**
                     * 
                     */
                    Face() {
                    }

                    /**
                     * Check if face has keypoints
                     */
                    bool hasKeypoints() {
                        return leftEye.x != rightEye.x;
                    }

                    /**
                     * Fill data from face detection result
                     */
                    void fill(std::list<dl::detect::result_t>::iterator res) {
                        result = res;

                        x = x0 = result->box[0];
                        y = y0 = result->box[1];
                        x1 = result->box[2];
                        y1 = result->box[3];
                        width = x1 - x0 + 1;
                        height = y1 - y0 + 1;

                        if (result->keypoint.size() == 10) {
                            leftEye.x = result->keypoint[0];
                            leftEye.y = result->keypoint[1];
                            leftMouth.x = result->keypoint[2];
                            leftMouth.y = result->keypoint[3];
                            nose.x = result->keypoint[4];
                            nose.y = result->keypoint[5];
                            rightEye.x = result->keypoint[6];
                            rightEye.y = result->keypoint[7];
                            rightMouth.x = result->keypoint[8];
                            rightMouth.y = result->keypoint[9];
                        }
                    }

                    /**
                     * Clear all coordinates
                     */
                    void clear() {
                        x = x0 = 0;
                        y = y0 = 0;
                        x1 = 0;
                        y1 = 0;
                        width = 0;
                        height = 0;

                        leftEye.x = 0;
                        leftEye.y = 0;
                        leftMouth.x = 0;
                        leftMouth.y = 0;
                        nose.x = 0;
                        nose.y = 0;
                        rightEye.x = 0;
                        rightEye.y = 0;
                        rightMouth.x = 0;
                        rightMouth.y = 0;
                    }

                    /**
                     * Execute function on each keypoint
                    */
                    template<typename Callback>
                    void forEachKeypoint(Callback callback) {
                        if (result->keypoint.size() == 10) {
                            ESP_LOGE("Face", "No keypoints found for current face");
                            return;
                        }

                        for (uint8_t i = 0, j = 0; j < 10; i += 1, j +=2) {
                            uint16_t x = result->keypoint[j];
                            uint16_t y = result->keypoint[j + 1];

                            callback(i, x, y);
                        }
                    }
            };
        }
    }
}

// create class alias
namespace eloq {
    typedef Eloquent::Esp32cam::Face::Face face_t; 
}

#endif