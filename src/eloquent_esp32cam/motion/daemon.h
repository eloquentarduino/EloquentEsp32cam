#ifndef ELOQUENT_ESP32CAM_MOTION_DAEMON_H
#define ELOQUENT_ESP32CAM_MOTION_DAEMON_H

#include <functional>
#include "../camera/camera.h"
#include "../extra/esp32/multiprocessing/thread.h"

using eloq::camera;
using Eloquent::Extra::Esp32::Multiprocessing::Thread;
using OnMotionCallback = std::function<void(void)>;


namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            /**
             * Run motion detection in a task
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
                    thread("MotionDetection"),
                    _detection(detection) {
                    
                }
            
                /**
                 * Run function when a face is detected
                 * 
                 * @brief 
                 * @param callback
                 */
                void onMotion(OnMotionCallback callback) {
                    _onMotion = callback;
                }
                
                /**
                 * Start motion detection in background
                 * 
                 * @brief 
                 */
                void start() {
                    thread
                        .withArgs((void*) this)
                        .withStackSize(5000)
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
                                    
                                if (!self->_detection->triggered())
                                    continue;
                                    
                                self->_onMotion();
                            }
                        });
                }
                
            protected:
                T *_detection;
                OnMotionCallback _onMotion;
            };
        }
    }
}


#endif