#ifndef ELOQUENT_ESP32CAM_FACE_DETECTION
#define ELOQUENT_ESP32CAM_FACE_DETECTION

#include <human_face_detect_msr01.hpp>
#include <human_face_detect_mnp01.hpp>
#include "../camera/camera.h"
#include "../extra/exception.h"
#include "../extra/time/benchmark.h"
#include "./msr_config.h"
#include "./mnp_config.h"
#include "./face_t.h"

using eloq::camera;
using eloq::face_t;
using Eloquent::Extra::Exception;
using Eloquent::Extra::Time::Benchmark;

#ifndef MAX_FACES
#define MAX_FACES 10
#endif


namespace Eloquent {
    namespace Esp32cam {
        namespace Face {
            /**
             * Detect faces in camera pictures
             */
            class FaceDetection {
                public:
                    Exception exception;
                    Benchmark benchmark;
                    MSRConfig msr;
                    MNPConfig mnp;
                    uint8_t image[240 * 240 * 3];
                    face_t first;
                    face_t faces[MAX_FACES];

                    /**
                     * Constructor
                     */
                    FaceDetection() :
                        exception("FaceDetection"),
                        _twoStages(false),
                        _confidence(0.5)
                    {
                        memset(image, 0, sizeof(image));
                    }

                    /**
                     * Perform accurate detection of faces (slow)
                     */
                    inline void accurate() {
                        _twoStages = true;
                        msr.config.score_thresh = 0.1f;
                    }

                    /**
                     * Perform fast detection of faces (less accurate)
                     */
                    inline void fast() {
                        _twoStages = false;
                        msr.config.score_thresh = 0.3f;
                    }

                    /**
                     * Set min detection score to be considered valid
                     */
                    inline void confidence(float confidence_) {
                        _confidence = constrain(confidence_, 0.1, 1);
                    }

                    /**
                     * Perform detection
                     */
                    Exception& run() {
                        // assert 240x240
                        if (camera.resolution.getWidth() != 240 || camera.resolution.getHeight() != 240)
                            return exception.set("Face detection only works at 240x240");

                        clear();

                        benchmark.benchmark([this]() {
                            camera.mutex.threadsafe([this]() {
                                camera_fb_t *fb = camera.frame;

                                if (!camera.hasFrame()) {
                                    exception.set("Cannot detect faces in empty frame");
                                    return;
                                }

                                if (!fmt2rgb888(fb->buf, fb->len, fb->format, image)) {
                                    exception.set("Cannot convert JPG frame to RGB888");
                                    return;
                                }

                                std::vector<int> shape = {(int) fb->width, (int) fb->height, 3};

                                if (_twoStages) {
                                    HumanFaceDetectMSR01 s1(
                                        msr.config.score_thresh,
                                        msr.config.nms_thresh,
                                        msr.config.top_k,
                                        msr.config.resize_scale
                                    );
                                    HumanFaceDetectMNP01 s2(
                                        mnp.config.score_thresh,
                                        mnp.config.nms_thresh,
                                        mnp.config.top_k
                                    );

                                    std::list<dl::detect::result_t> &cand = s1.infer(image, shape);
                                    std::list<dl::detect::result_t> &results = s2.infer(image, shape, cand);
                                    copy(results);
                                }
                                else {
                                    HumanFaceDetectMSR01 s1(
                                        msr.config.score_thresh,
                                        msr.config.nms_thresh,
                                        msr.config.top_k,
                                        msr.config.resize_scale
                                    );

                                    std::list<dl::detect::result_t> &results = s1.infer((uint8_t*) image, shape);
                                    copy(results);
                                }
                            }, 1000);
                        });

                        if (!exception.isOk())
                            return exception;

                        if (!camera.mutex.isOk())
                            return exception.set("Cannot acquire camera mutex");
                        
                        return exception.clear();
                    }

                    /**
                     * Test if no face was in the frame
                     */
                    inline bool notFound() {
                        return !found();
                    }

                    /**
                     * Test if any face was in the frame
                     */
                    inline bool found() {
                        return first.isValid();
                    }

                    /**
                     * 
                     */
                    uint8_t count() {
                        uint8_t count = 0;

                        for (uint8_t i = 0; i < MAX_FACES; i++) {
                            if (faces[i].isValid() && faces[i].score >= _confidence)
                                count++;
                        }

                        return count;
                    }

                    /**
                     * Run function on each detected face
                     */
                    template<typename Callback>
                    void forEach(Callback callback) {
                        if (notFound())
                            return;

                        uint8_t idx = 0;

                        for (uint8_t i = 0; i < MAX_FACES; i++) {
                            face_t &face = faces[i];

                            if (face.isValid() && face.score >= _confidence)
                                callback(idx++, face);
                        }
                    }

                protected:
                    bool _twoStages;
                    float _confidence;

                    /**
                     * Clear faces data
                     */
                    void clear() {
                        first.clear();

                        for (uint8_t i = 0; i < MAX_FACES; i++)
                            faces[i].clear();
                    }

                    /**
                     * Copy results into face_t structures
                     */
                    void copy(std::list<dl::detect::result_t> &results) {
                        bool isFirst = true;
                        int i = 0;

                        for (const auto& res : results) {
                            faces[i++].copyFrom(res);

                            if (res.score < _confidence)
                                continue;

                            if (isFirst) {
                                isFirst = false;
                                first.copyFrom(res);
                            }
                        }
                    }
            };
        }
    }
}

namespace eloq {
    namespace face {
        static Eloquent::Esp32cam::Face::FaceDetection detection;
    }
}

#endif