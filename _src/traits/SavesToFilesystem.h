//
// Created by Simone on 11/11/22.
//

#ifndef ELOQUENTESP32CAM_SAVESTOFILESYSTEM_H
#define ELOQUENTESP32CAM_SAVESTOFILESYSTEM_H


#include <FS.h>


namespace Eloquent {
    namespace Esp32cam {

        /**
         * Save image to filesystem
         */
        class SavesToFilesystem {
        protected:
            /**
             *
             * @param fs
             * @param filename
             * @return
             */
            bool saveTo(fs::FS &fs, String filename, uint8_t *buf, size_t length) {
                auto file = fs.open(filename, "wb");

                file.write(buf, length);
                file.close();

                // check that written length equals expected length
                file = fs.open(filename, "rb");
                size_t written = file.size();
                file.close();

                return written == length;
            }
        };
    }
}

#endif //ELOQUENTESP32CAM_SAVESTOFILESYSTEM_H
