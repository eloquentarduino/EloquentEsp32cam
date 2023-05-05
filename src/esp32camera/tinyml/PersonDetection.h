/**
 * Run person detection on camera frame
 * Assumes a JPEG decoder named `jpeg` is available
 * at compile time
 */

#ifndef ELOQUENTESP32CAM_PERSONDETECTION_H
#define ELOQUENTESP32CAM_PERSONDETECTION_H

#define PERSON_DETECTION_ARENA_SIZE 70000L

#include <EloquentTinyML.h>
#include <eloquent_tinyml/tensorflow/person_detection.h>
#include "../../traits/HasErrorMessage.h"
#include "../Cam.h"
#include "../features/Benchmark.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace TinyML {
            /**
             * Perform person detection
             */
            class PersonDetection : public HasErrorMessage {
            public:
                bool isPerson;
                Features::Benchmark benchmark;
                Eloquent::TinyML::TensorFlow::PersonDetection<1024/8, 768/8> model;

                /**
                 *
                 * @param camera
                 */
                PersonDetection() :
                    isPerson(false) {
                }

                /**
                 * Check if person is detected
                 *
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
                    if (!camera.resolution.isXGA())
                        return setErrorMessage("Resolution must be XGA");

                    if (!model.begin())
                        return setErrorMessage(model.getErrorMessage());

                    return clearError();
                }

                /**
                 * Set min person score
                 *
                 * @param confidence
                 */
                inline void minConfidence(uint8_t confidence) {
                    model.setDetectionAbsoluteThreshold(confidence);
                }

                /**
                 *
                 * @return
                 */
                bool detect() {
                    benchmark.start();

                    if (!jpeg.decode()) {
                        benchmark.stop();

                        return setErrorMessage(jpeg.getErrorMessage());
                    }

                    isPerson = model.detectPerson(jpeg.gray.pixels);
                    benchmark.stop();

                    if (!model.isOk()) {
                        return setErrorMessage(model.getErrorMessage());
                    }

                    return clearError();
                }
            };
        }
    }
}


Eloquent::Esp32cam::TinyML::PersonDetection personDetection;

#endif //ELOQUENTESP32CAM_PERSONDETECTION_H
