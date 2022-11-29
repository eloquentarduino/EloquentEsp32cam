//
// Created by Simone on 28/11/22.
//

#ifndef ELOQUENTESP32CAM_FACEDETECTION_H
#define ELOQUENTESP32CAM_FACEDETECTION_H

#include <esp_camera.h>
#include <fd_forward.h>
#include <fr_forward.h>
#include <fr_flash.h>
#include "./Cam.h"
#include "../traits/HasErrorMessage.h"
#include "../traits/BenchmarksCode.h"


namespace Eloquent {
    namespace Esp32cam {
        /**
         * Detect faces in frames
         */
        class FaceDetection : public HasErrorMessage, public BenchmarksCode {
        public:
            Cam *cam;
            mtmn_config_t mtmn;
            box_array_t* boxes;
            int8_t id;
            int8_t enrolledId;
            dl_matrix3du_t *person;

            /**
             * Constructor
             * @param cam
             */
            FaceDetection(Cam& cam) :
                cam(&cam),
                boxes(NULL),
                _maxFaces(7),
                _confimTimes(5) {
                person = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);

                mtmn.type = FAST;
                mtmn.min_face = 80;
                mtmn.pyramid = 0.707;
                mtmn.pyramid_times = 4;
                mtmn.p_threshold.score = 0.6;
                mtmn.p_threshold.nms = 0.7;
                mtmn.p_threshold.candidate_number = 20;
                mtmn.r_threshold.score = 0.7;
                mtmn.r_threshold.nms = 0.7;
                mtmn.r_threshold.candidate_number = 10;
                mtmn.o_threshold.score = 0.7;
                mtmn.o_threshold.nms = 0.7;
                mtmn.o_threshold.candidate_number = 1;
            }

            /**
             * Limit the number of faces to enroll
             * @param maxFaces
             */
            inline void limit(uint8_t maxFaces) {
                _maxFaces = maxFaces;
            }

            /**
             * Set how many frames are required to confirm enrollment
             * @param confirmTimes
             */
            inline void confirm(uint8_t confirmTimes) {
                _confimTimes = confirmTimes;
            }

            /**
             * Read faces from flash
             */
            void beginFromFlash() {
                face_id_init(&_ids, _maxFaces, _confimTimes);
                face_id_name_init(&_names, _maxFaces, _confimTimes);
                read_face_id_from_flash(&_ids);
                read_face_id_from_flash_with_name(&_names);
            }

            /**
             *
             * @return
             */
            bool begin() {
                if (!person)
                    return setErrorMessage("Cannot allocate face buffer");

                face_id_init(&_ids, _maxFaces, _confimTimes);
                //read_face_id_from_flash(&_ids);

                return setErrorMessage("");
            }

            /**
             * Delete all faces from flash
             */
            inline void eraseFlash() {
                delete_face_all_in_flash_with_name(&_names);
            }

            /**
             * Free memory
             */
            void free() {
                /*if (boxes != NULL && boxes && boxes->score != NULL) {
                    dl_lib_free(boxes->score);
                    dl_lib_free(boxes->box);
                    dl_lib_free(boxes->landmark);
                    dl_lib_free(boxes);
                    boxes = NULL;
                }*/
            }

            /**
             *
             * @param intervalInMillis
             * @return
             */
            bool enroll() {
                if (!allocate())
                    return false;

                //remaining = enroll_face(&_ids, person);
                int8_t remaining = enroll_face_id_to_flash(&_ids, person);

                if (remaining == _confimTimes - 1)
                    enrolledId = _ids.tail;

                return setErrorMessage(remaining > 0 ? "Enrolling" : "");
            }

            /**
             * Detect face (any face)
             * @return true if any face is detected
             */
            bool detect() {
                free();
                setErrorMessage("");
                startBenchmark();

                if (!allocate()) {
                    stopBenchmark();
                    return false;
                }

                boxes = face_detect(_image, &mtmn);
                stopBenchmark();

                if (boxes)
                    return setErrorMessage("");

                boxes = NULL;
                return false;
            }

            /**
             * Detect known face
             * @return
             */
            bool recognize() {
                setErrorMessage("");

                if (!boxes)
                    return setErrorMessage("No face detected");

                startBenchmark();

                if (align_face(boxes, _image, person) == ESP_OK) {
                    id = recognize_face(&_ids, person);

                    if (id >= 0) {
                        stopBenchmark();
                        return true;
                    }

                    /*personId = get_face_id(person);

                    if (_ids.count > 0) {
                        face_id_node *face_recognized = recognize_face_with_name(&_ids, faceId);

                        if (face_recognized)
                            return true;
                    }*/
                }

                stopBenchmark();

                return false;
            }


        protected:
            uint8_t _maxFaces;
            uint8_t _confimTimes;
            face_id_list _ids;
            face_id_name_list _names;
            dl_matrix3du_t *_image;
            face_id_node *_recognized;

            /**
             * Allocate buffers
             *
             * @return
             */
            bool allocate() {
                if (!_image)
                    _image = dl_matrix3du_alloc(1, cam->getWidth(), cam->getHeight(), 3);

                if (!_image)
                    return setErrorMessage("Cannot allocate frame buffer");

                if (!fmt2rgb888(cam->frame->buf, cam->frame->len, PIXFORMAT_JPEG, _image->item)) {
                    return setErrorMessage("Cannot convert frame to RGB888");
                }

                return setErrorMessage("");
            }
        };
    }
}

#endif //ELOQUENTESP32CAM_FACEDETECTION_H
