#ifndef ELOQUENT_ESP32CAM_FACE_RECOGNITION_H
#define ELOQUENT_ESP32CAM_FACE_RECOGNITION_H

#include <SPIFFS.h>
#include <dl_image.hpp>
#include <face_recognition_tool.hpp>
#include <face_recognition_112_v1_s8.hpp>
#include "../extra/exception.h"
#include "../extra/time/benchmark.h"
#include "./detection.h"

using Eloquent::Error::Exception;
using Eloquent::Extra::Time::Benchmark;
using eloq::face::detection;

typedef struct {
    int id;
    String name;
    float similarity;
} matched_face_t;

typedef struct {
    int id;
    char name[17];
    float embedding[512];
    uint8_t ctrl[2];
} enrolled_face_t;


namespace Eloquent {
    namespace Esp32cam {
        namespace Face {

            /**
             * Recognize (not detect!) faces
             */
            class FaceRecognition {
            public:
                Exception exception;
                Benchmark benchmark;
                FaceRecognition112V1S8 recognizer;
                matched_face_t match;

                /**
                 * Constructor
                 */
                FaceRecognition() :
                    exception(""),
                    thresh(0.85) {

                    match.id = -1;
                    match.name = "";
                    match.similarity = 0;

                    // bind input tensor to aligned face
                    this->input.set_element(aligned).set_shape({112, 112, 3}).set_auto_free(false);
                }

                /**
                 * Set recognition threshold
                 * @param threshold
                 */
                void confidence(float threshold) {
                    this->thresh = threshold;
                }

                /**
                 * Load existing faces, if any
                 * @return
                 */
                Exception& begin() {
                    detection.accurate();
                    SPIFFS.begin(true);
                    this->recognizer.set_thresh(this->thresh);

                    // delete all enrolled ids
                    for (uint8_t i = 0, count = this->recognizer.get_enrolled_id_num(); i < count; i++)
                        this->recognizer.delete_id(i);

                    if (!SPIFFS.exists(this->filename)) {
                        ESP_LOGI("FR", "No faces storage found: creating...");
                        SPIFFS.open(this->filename, "wb").close();
                    }

                    File file = SPIFFS.open(this->filename, "rb");

                    if (!file) {
                        file.close();
                        SPIFFS.remove(this->filename);
                        return this->exception.set("Can't open file for faces storage. Try to restart the board.");
                    }

                    bool parseError = false;

                    // read all faces
                    while (file.available()) {
                        Tensor<float> tensor;
                        enrolled_face_t prototype;

                        file.read((uint8_t*) &prototype, sizeof(enrolled_face_t));

                        if (prototype.ctrl[0] != 0x14 || prototype.ctrl[1] != 0x8) {
                            parseError = true;
                            break;
                        }

                        tensor.set_element(prototype.embedding).set_shape({512});
                        this->recognizer.enroll_id(tensor, prototype.name, false);
                        ESP_LOGI("FR", "Enrolled face %s", prototype.name);
                    }

                    file.close();

                    if (parseError) {
                        ESP_LOGE("FR", "Parse error. Deleting faces storage...");
                        this->deleteAll();
                        return this->begin();
                    }
                    else {
                        ESP_LOGI("FR", "Enrolled %d faces in total", this->recognizer.get_enrolled_id_num());
                    }

                    return this->exception.clear();
                }

                /**
                 * Delete all faces
                 * @return
                 */
                Exception& deleteAll()  {
                    SPIFFS.open(this->filename, "wb").close();

                    return this->exception.clear();
                }

                /**
                 * Detect face (before recognition)
                 * @return
                 */
                Exception& detect()  {
                    detection.accurate();

                    if (!detection.run().isOk())
                        return this->exception.propagate(detection);

                    if (detection.notFound())
                        return this->exception.set("No face detected");

                    return this->exception.clear();
                }

                /**
                 * Enroll new face
                 * @param name
                 * @return
                 */
                Exception& enroll(String name)  {
                    if (name == "")
                        return this->exception.set("Cannot enroll empty name");

                    if (name.length() > 16)
                        return this->exception.set("Name too long. Max 16 characters are allowed");

                    this->align();
                    int id = this->recognizer.enroll_id(this->input, detection.keypoints(), name.c_str(), false);
                    this->embedding = recognizer.get_face_emb();

                    // save embedding to file
                    File file = SPIFFS.open(this->filename, "ab");
                    enrolled_face_t enrolled;

                    // fill id
                    enrolled.id = id;

                    // fill name
                    for (uint8_t i = 0; i < name.length(); i++)
                        enrolled.name[i] = name[i];
                    enrolled.name[name.length()] = '\0';

                    // fill embedding
                    for (uint16_t i = 0; i < 512; i++)
                        enrolled.embedding[i] = this->embedding.element[i];

                    // control bytes
                    enrolled.ctrl[0] = 0x14;
                    enrolled.ctrl[1] = 0x08;

                    // write
                    file.write((uint8_t*) &enrolled, sizeof(enrolled_face_t));
                    file.close();
                    ESP_LOGI("FR", "Enrolled %s with id %d", name.c_str(), id);

                    // reload all faces
                    return this->begin();
                }

                /**
                 * Recognize current face
                 * @return
                 */
                Exception& recognize()  {
                    Tensor<uint8_t> tensor;

                    this->benchmark.benchmark([this]() {
                        this->align();
                        face_info_t info = recognizer.recognize(this->input, detection.keypoints());

                        this->match.id = info.id;
                        this->match.name = String(info.name.c_str());
                        this->match.similarity = info.similarity;
                    });

                    return this->exception.clear();
                }

                /**
                 * Dump stored face embeddings
                 */
                void dump() {
                    Serial.println("FACE RECOGNITION DUMP");
                    Serial.println("=====================");

                    File file = SPIFFS.open(this->filename, "rb");

                    while (file.available()) {
                        enrolled_face_t prototype;

                        file.read((uint8_t*) &prototype, sizeof(enrolled_face_t));

                        if (prototype.ctrl[0] != 0x14 || prototype.ctrl[1] != 0x8) {
                            Serial.println("Parse error: aborting...");
                            break;
                        }

                        this->printVector(prototype.name, prototype.embedding);
                    }

                    file.close();
                    Serial.println("=====================");
                }

            protected:
                float thresh;
                const char *filename = "/fr.bin";
                uint8_t aligned[112 * 112 * 3];
                Tensor<uint8_t> input;
                Tensor<float> embedding;

                /**
                 * Align face around detected cx/cy
                 */
                void align() {
                    uint16_t i = 0;
                    uint8_t x1, y1;
                    const uint8_t cx = detection.first.cx;
                    const uint8_t cy = detection.first.cy;

                    if (cx < 56)            x1 = 0;
                    else if (cx > 240 - 56) x1 = 240 - 112;
                    else                    x1 = cx - 56;

                    if (cy < 56)            y1 = 0;
                    else if (cy > 240 - 56) y1 = 240 - 112;
                    else                    y1 = cy - 56;

                    for (uint32_t y = y1; y < y1 + 112; y += 1) {
                        const uint32_t offset_y = y * 240 * 3;

                        for (uint32_t x = x1; x < x1 + 112; x += 1) {
                            const uint32_t offset = offset_y + x * 3;

                            aligned[i++] = detection.image[offset + 0];
                            aligned[i++] = detection.image[offset + 1];
                            aligned[i++] = detection.image[offset + 2];
                        }
                    }
                }

                /**
                 * Print embedding vector
                 * @param v
                 */
                void printVector(String title, float *v) {
                    Serial.print(title);
                    Serial.print(" embedding = [");
                    Serial.print(v[0]);

                    for (uint16_t j = 1; j < 512; j++) {
                        Serial.print(", ");
                        Serial.print(v[j], 4);
                    }

                    Serial.println("]");
                }
            };
        }
    }
}

namespace eloq {
    namespace face {
        static Eloquent::Esp32cam::Face::FaceRecognition recognition;
    }
}

#endif