#ifndef ELOQUENT_ESP32CAM_FACE_DETECTION
#define ELOQUENT_ESP32CAM_FACE_DETECTION

#include <human_face_detect_msr01.hpp>
#include <human_face_detect_mnp01.hpp>
#include "../camera/camera.h"
#include "../extra/exception.h"
#include "../extra/time/benchmark.h"
#include "../extra/pubsub.h"
#include "./msr_config.h"
#include "./mnp_config.h"
#include "./face_t.h"
#include "./daemon.h"

using eloq::camera;
using eloq::face_t;
using Eloquent::Error::Exception;
using Eloquent::Extra::Time::Benchmark;
#if defined(ELOQUENT_EXTRA_PUBSUB_H)
using Eloquent::Extra::PubSub;
#endif

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
                    /**
                     * @added 2.7.3
                     */
                    std::list<dl::detect::result_t> results;
                    Daemon<FaceDetection> daemon;
                    #if defined(ELOQUENT_EXTRA_PUBSUB_H)
                    PubSub<FaceDetection> mqtt;
                    #endif
                    uint8_t *image;
                    face_t first;
                    face_t faces[MAX_FACES];

                    /**
                     * Constructor
                     */
                    FaceDetection() :
                        exception("FaceDetection"),
                        daemon(this),
                        image(NULL),
                        #if defined(ELOQUENT_EXTRA_PUBSUB_H)
                        mqtt(this),
                        #endif
                        _twoStages(false),
                        _confidence(0.5)
                    {
                    }

                    /**
                     * Destructor
                     */
                    ~FaceDetection() {
                        free(image);
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
                        allocateImageBuffer();

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
                                    results = s2.infer(image, shape, cand);
                                    copy(results);
                                }
                                else {
                                    HumanFaceDetectMSR01 s1(
                                        msr.config.score_thresh,
                                        msr.config.nms_thresh,
                                        msr.config.top_k,
                                        msr.config.resize_scale
                                    );

                                    results = s1.infer((uint8_t*) image, shape);
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
                    
                    /**
                     * @brief Convert to JSON
                     */
                    String toJSON() {
                        static char buf[MAX_FACES * 33] = {' '};
                        String json(buf);
                        
                        json = "[]";
                        
                        if (notFound())
                            return json;
                            
                        json = '[';
                        
                        forEach([&json](int i, face_t face) {
                            if (i > 0)
                                json += ',';
                                
                            json += "{\"x\":";
                            json += face.x;
                            json += ",\"y\":";
                            json += face.y;
                            json += ",\"w\":";
                            json += face.width;
                            json += ",\"h\":";
                            json += face.height;
                            json += ",\"proba\":";
                            json += face.score;
                            json += "}";
                        });
                        
                        json += ']';
                        
                        return json;
                            
                    }
                    
                    /**
                     * @brief Test if an MQTT message should be published
                     */
                    bool shouldPub() {
                        return found();
                    }

                    /**
                     * @added 27.3
                     * @return
                     */
                    std::vector<int>& keypoints() {
                        static std::vector<int> empty{};

                        if (!found())
                            return empty;

                        return results.front().keypoint;
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

                    /**
                     *
                     */
                    void allocateImageBuffer() {
                        if (image != NULL)
                            return;

                        const uint32_t memsize = 240L * 240L * 3;

                        if (psramFound()) {
                            ESP_LOGD("FaceDetection", "Allocating %d bytes into PSRAM", memsize);
                            image = (uint8_t *) ps_malloc(memsize);
                        }
                        else {
                            ESP_LOGD("FaceDetection", "Allocating %d bytes into RAM", memsize);
                            image = (uint8_t *) malloc(memsize);
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