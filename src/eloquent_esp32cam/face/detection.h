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

using namespace eloq;
using Eloquent::Extra::Exception;
using Eloquent::Extra::Time::Benchmark;


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
                    std::list<dl::detect::result_t> *results;
                    face_t first;
                    uint8_t image[240 * 240 * 3];

                    /**
                     * Constructor
                     */
                    FaceDetection() :
                        useTwoStageDetection(false),
                        results(NULL),
                        exception("FaceDetection")
                    {
                        memset(image, 0, sizeof(image));
                    }

                    /**
                     * Perform accurate detection of faces (slow)
                     */
                    inline void accurate() {
                        useTwoStageDetection = true;
                        msr.config.score_thresh = 0.1f;
                    }

                    /**
                     * Perform fast detection of faces (less accurate)
                     */
                    inline void fast() {
                        useTwoStageDetection = false;
                        msr.config.score_thresh = 0.3f;
                    }

                    /**
                     * Perform detection
                     */
                    Exception& detect() {
                        camera_fb_t *fb = camera.frame;

                        // assert 240x240
                        if (camera.resolution.getWidth() != 240 || camera.resolution.getHeight() != 240)
                            return exception.set("Face detection only works at 240x240");

                        if (!camera.hasFrame())
                            return exception.set("Cannot detect faces in empty frame");

                        benchmark.start();

                        if (!fmt2rgb888(fb->buf, fb->len, fb->format, image))
                            return exception.set("Cannot convert JPG frame to RGB888");

                        int width = fb->width;
                        int height = fb->height;
                        std::vector<int> shape = {width, height, 3};

                        first.clear();

                        if (useTwoStageDetection) {
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
                            results = &(s2.infer(image, shape, cand));
                        }
                        else {
                            HumanFaceDetectMSR01 s1(
                                msr.config.score_thresh,
                                msr.config.nms_thresh,
                                msr.config.top_k,
                                msr.config.resize_scale
                            );

                            results = &(s1.infer((uint8_t*) image, shape));
                        }

                        benchmark.stop();

                        // keep track of first face, if any
                        if (found()) {
                            for (std::list<dl::detect::result_t>::iterator prediction = results->begin(); prediction != results->end(); prediction++) {
                                first.fill(prediction);
                                break;
                            }
                        }
                        
                        return exception.clear();
                    }

                    /**
                     * Test if no face was in the frame
                     */
                    inline bool notFound() {
                        return results == NULL || results->size() == 0;
                    }

                    /**
                     * Test if any face was in the frame
                     */
                    inline bool found() {
                        return !notFound();
                    }

                    /**
                     * Run function on each detected bbox
                     */
                    template<typename Callback>
                    void forEach(Callback callback) {
                        if (notFound())
                            return;

                        int i = 0;

                        for (std::list<dl::detect::result_t>::iterator prediction = results->begin(); prediction != results->end(); prediction++, i++) {
                            face_t face;
                            face.fill(prediction);
                            callback(i, face);
                        }
                    }

                protected:
                    bool useTwoStageDetection;
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