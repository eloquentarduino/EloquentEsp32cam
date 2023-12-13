#ifndef ELOQUENT_ESP32CAM_FACE_DAEMON_H
#define ELOQUENT_ESP32CAM_FACE_DAEMON_H

#include <functional>
#include "../camera/camera.h"
#include "../extra/esp32/multiprocessing/thread.h"
#include "./face_t.h"

using eloq::camera;
using eloq::face_t;
using Eloquent::Extra::Esp32::Multiprocessing::Thread;
using OnFaceCallback = std::function<void(face_t&)>;
using OnMultipleFacesCallback = std::function<void(uint8_t, face_t&)>;


namespace Eloquent {
    namespace Esp32cam {
        namespace Face {
            /**
             * Run face detection in a task
             * 
             * @class Daemon
             * @author Simone
             * @date 13/12/2023
             * @file daemon.h
             * @brief 
             */
             template<typename T>
            class Daemon {
            public:
                Thread thread;
                
                /**
                 * Constructor
                 *
                 * @brief 
                 */
                Daemon(T* detection) : 
                    thread("FaceDetection"),
                    _detection(detection) {
                    
                }
            
                /**
                 * Run function when a face is detected
                 * 
                 * @brief 
                 * @param callback
                 */
                void onFace(OnFaceCallback callback) {
                    _onFace = callback;
                }
                
                /**
                 * Run function when multiple faces are detected
                 * 
                 * @brief 
                 * @param callback
                 */
                void onMultipleFaces(OnMultipleFacesCallback callback) {
                    _onMultipleFaces = callback;
                }
                
                /**
                 * Start face detection in background
                 * 
                 * @brief 
                 */
                void start() {
                    thread
                        .withArgs((void*) this)
                        .withStackSize(4000)
                        .run([](void *args) {
                            Daemon *self = (Daemon*) args;
                            
                            delay(3000);
                            
                            while (true) {
                                yield();
                                delay(1);
                                
                                if (!camera.capture().isOk())
                                    continue;

                                if (!self->_detection->run().isOk())
                                    continue;
                                    
                                if (self->_detection->notFound())
                                    continue;
                                    
                                if (self->_detection->count() == 1) {
                                    self->_onFace(self->_detection->first);
                                    continue;
                                }
                                    
                                self->_detection->forEach([&self](int i, face_t& face) {
                                    self->_onMultipleFaces(i, face);
                                });
                            }
                        });
                }
                
            protected:
                T *_detection;
                OnFaceCallback _onFace;
                OnMultipleFacesCallback _onMultipleFaces;
                
            };
        }
    }
}


#endif