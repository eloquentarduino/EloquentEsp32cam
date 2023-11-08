#ifndef ELOQUENT_ESP32CAM_EDGEIMPULSE_CLASSIFIER_H
#define ELOQUENT_ESP32CAM_EDGEIMPULSE_CLASSIFIER_H

#include "../extra/exception.h"
#include "../extra/time/benchmark.h"

using Eloquent::Extra::Exception;
using Eloquent::Extra::Time::Benchmark;


namespace Eloquent {
    namespace Esp32cam {
        namespace EdgeImpulse {
            /**
             * Base class for Edge Impulse classifiers
             */
            class Classifier {
                public:
                    ei::signal_t signal;
                    ei_impulse_result_t result;
                    EI_IMPULSE_ERROR error;
                    Exception exception;
                    Benchmark benchmark;
                    struct {
                        size_t dsp;
                        size_t anomaly;
                        size_t classification;
                        size_t total;
                    } timing;

                    /**
                     * 
                     */
                    Classifier(): 
                        exception("EI " EI_CLASSIFIER_PROJECT_NAME),
                        _isDebugEnabled(false) {
                        signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
                    }

                    /**
                     *
                     * @param enabled
                     */
                    void debug(bool enabled = true) {
                        _isDebugEnabled = enabled;
                    }

                    /**
                     * Run model
                     */
                    Exception& run() {
                        if (!beforeClassification())
                            return exception;
                            
                        benchmark.start();
                        error = run_classifier(&signal, &result, _isDebugEnabled);
                        benchmark.stop();

                        if (error != EI_IMPULSE_OK)
                            return exception.set(String("Failed to run classifier with error code ") + error);

                        afterClassification();

                        timing.dsp = result.timing.dsp;
                        timing.classification = result.timing.classification;
                        timing.anomaly = result.timing.anomaly;
                        timing.total = timing.dsp + timing.classification + timing.anomaly;

                        return exception.clear();
                    }

                protected:
                    bool _isDebugEnabled;

                    /**
                     * Run checks before classification.
                     * If false is returned, don't run
                     */
                    virtual bool beforeClassification() = 0;

                    /**
                     * Run actions after classification results
                     */
                    virtual void afterClassification() = 0;
            };
        }
    }
}

#endif