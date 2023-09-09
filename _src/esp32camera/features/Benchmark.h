//
// Created by Simone on 01/05/23.
//

#ifndef ELOQUENTESP32CAM_BENCHMARK_H
#define ELOQUENTESP32CAM_BENCHMARK_H

namespace Eloquent {
    namespace Esp32cam {
        namespace Features {
            /**
             * Run benchmark on code blocks
             */
            class Benchmark {
            public:

                /**
                 * Start timer
                 */
                void start() {
                    timeStart = micros();
                }

                /**
                 * Stop timer
                 *
                 * @return
                 */
                size_t stop() {
                    elapsedInMicros = micros() - timeStart;

                    return inMillis();
                }

                /**
                 * Get elapsed time in millis
                 *
                 * @return
                 */
                inline size_t inMillis() {
                    return inMicros() / 1000;
                }

                /**
                 * Get elapsed time in micros
                 *
                 * @return
                 */
                inline size_t inMicros() {
                    return elapsedInMicros;
                }

            protected:
                size_t timeStart;
                size_t elapsedInMicros;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_BENCHMARK_H
