//
// Created by Simone on 29/12/22.
//

#ifndef ELOQUENTESP32CAM_PERSONDETECTION_H
#define ELOQUENTESP32CAM_PERSONDETECTION_H

#define MAX_RESOLUTION_1024x768
#define PERSON_DETECTION_ARENA_SIZE 70000L

#include <EloquentTinyML.h>
#include <eloquent_tinyml/tensorflow/person_detection.h>
#include "../JpegDecoderGray.h"
#include "../../traits/HasErrorMessage.h"
#include "../../traits/BenchmarksCode.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Applications {

            /**
             * Perform person detection
             */
            class PersonDetector : public HasErrorMessage, public BenchmarksCode {
            public:
                bool isPerson;
                Cam *cam;
                JpegDecoderGray jpeg;
                Eloquent::TinyML::TensorFlow::PersonDetection<1024/8, 768/8> person;

                /**
                 *
                 * @param camera
                 */
                PersonDetector(Cam& camera) :
                    cam(&camera),
                    isPerson(false) {

                }

                /**
                 * Check person
                 * @return
                 */
                operator bool() const {
                    return isPerson;
                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    cam->grayscale();
                    cam->xga();

                    person.begin();

                    return setErrorMessage(person.isOk() ? "" : person.getErrorMessage());
                }

                /**
                 * Set min person score
                 *
                 * @param confidence
                 */
                inline void minConfidence(uint8_t confidence) {
                    person.setDetectionAbsoluteThreshold(confidence);
                }

                /**
                 *
                 * @return
                 */
                bool detect() {
                    startBenchmark();

                    if (!jpeg.decode(*cam)) {
                        stopBenchmark();
                        return setErrorMessage(jpeg.getErrorMessage());
                    }

                    isPerson = person.detectPerson(jpeg.gray.pixels);
                    stopBenchmark();

                    if (!person.isOk()) {
                        return setErrorMessage(person.getErrorMessage());
                    }

                    return setErrorMessage("");
                }

            protected:
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_PERSONDETECTION_H
