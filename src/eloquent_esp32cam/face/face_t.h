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
                    } left_eye, right_eye, nose, left_mounth, right_mounth;
                    std::list<dl::detect::result_t>::iterator result;

                    /**
                     * 
                     */
                    Face(std::list<dl::detect::result_t>::iterator res) : result(result) {
                        x = x0 = result->box[0];
                        y = y0 = result->box[1];
                        x1 = result->box[2];
                        y1 = result->box[3];
                        width = x1 - x0 + 1;
                        height = y1 - y0 + 1;

                        if (result->keypoint.size() == 10) {
                            left_eye.x = result->keypoint[0];
                            left_eye.y = result->keypoint[1];
                            left_mounth.x = result->keypoint[2];
                            left_mounth.y = result->keypoint[3];
                            nose.x = result->keypoint[4];
                            nose.y = result->keypoint[5];
                            right_eye.x = result->keypoint[6];
                            right_eye.y = result->keypoint[7];
                            right_mounth.x = result->keypoint[8];
                            right_mounth.y = result->keypoint[9];
                        }
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

// create alias
namespace e {
    class face_t : public Eloquent::Esp32cam::Face::Face {};
}

#endif