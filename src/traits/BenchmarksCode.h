//
// Created by Simone on 09/09/22.
//

#ifndef ELOQUENTSURVEILLANCE_BENCHMARKSCODE_H
#define ELOQUENTSURVEILLANCE_BENCHMARKSCODE_H

namespace EloquentSurveillance {
    class BenchmarksCode {
    public:

        /**
         *
         * @return
         */
        size_t getExecutionTimeInMicros() {
            return _benchmark;
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
        void endBenchmark() {
            _benchmark = micros() - _startOfBenchmark;
        }
    };
}

#endif //ELOQUENTSURVEILLANCE_BENCHMARKSCODE_H
