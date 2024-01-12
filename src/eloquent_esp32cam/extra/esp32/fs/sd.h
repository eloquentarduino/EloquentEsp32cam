#ifndef ELOQUENT_ESP32CAM_CAMERA_SD
#define ELOQUENT_ESP32CAM_CAMERA_SD

#include <FS.h>
#include <SD.h>
#include "../../exception.h"
#include "./write_session.h"


using Eloquent::Error::Exception;
using namespace eloq;


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Fs {
                /**
                 * Interact with the SD
                 */
                class SPISD {
                public:
                    Exception exception;
                    WriteSession session;

                    /**
                     * 
                     */
                    SPISD() : 
                        exception("SD"),
                        session(&SD),
                        _cs(255) {
                    }

                    /**
                     * 
                     */
                    fs::FS& fs() {
                        return SD;
                    }

                    /**
                     * 
                    */
                    void cs(uint8_t cs) {
                        _cs = cs;
                    }

                    /**
                     * Init SD
                     */
                    Exception& begin() {
                        if (_cs == 255)
                            return exception.set("You must set a CS pin");

                        if (!SD.begin(_cs))
                            return exception.set("Something went wrong with SD.begin()");

                        if (SD.cardType() == CARD_NONE)
                            return exception.set("Cannot detect any SD card");

                        return exception.clear();
                    }

                    /**
                     * See save(data, length)
                     */
                    template<typename BinaryContent>
                    WriteSession& save(BinaryContent& content) {
                        return save(content->buf, content->len);
                    }

                    /**
                     * Write binary content
                     */
                    WriteSession& save(uint8_t *data, size_t length) {
                        session.open(data, length);

                        return session;
                    }

                protected:
                    uint8_t _cs;
                };
            }
        }
    }
}

namespace eloq {
    static Eloquent::Extra::Esp32::Fs::SPISD sd;
}

#endif