//
// Created by Simone on 09/09/22.
//

#ifndef ELOQUENTSURVEILLANCE_BENCHMARKSCODE_H
#define ELOQUENTSURVEILLANCE_BENCHMARKSCODE_H

namespace Eloquent {
    namespace Esp32cam {
        /**
         * Measure how long a piece of code takes
         */
        class BenchmarksCode {
        public:

            /**
             *
             * @return
             */
            size_t getExecutionTimeInMicros() {
                return _benchmark;
            }

            /**
             *
             * @return
             */
            size_t getExecutionTimeInMillis() {
                return getExecutionTimeInMicros() / 1000;
            }

        protected:
            size_t _benchmark;
            size_t _startOfBenchmark;

            /**
             *
             */
            void startBenchmark() {
                _startOfBenchmark = micros();
            }

            /**
             *
             */
            void stopBenchmark() {
                _benchmark = micros() - _startOfBenchmark;
            }
        };
    }
}

#endif //ELOQUENTSURVEILLANCE_BENCHMARKSCODE_H
