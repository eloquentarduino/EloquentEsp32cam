//
// Created by Simone on 05/11/22.
//

#ifndef ELOQUENTESP32CAM_IMAGE_BROWSER_H
#define ELOQUENTESP32CAM_IMAGE_BROWSER_H


#include <FS.h>
#include "../../traits/HasErrorMessage.h"
#include "../../traits/IsHttpServer.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {

            /**
             * ESP32 camera image browser web server
             */
            class ImageBrowser : public Trait::IsHttpServer {
            public:

                /**
                 * Constructor
                 */
                ImageBrowser() {
                    setMaxNumFilesPerPage(30);
                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    on("/", [this]() { onIndex(); });
                    on("/ls", [this]() { onList(); });
                    on("/view", [this]() { onView(); });
                    on("/delete", [this]() { onDelete(); });
                    on("/rename", [this]() { onRename(); });
                    onPOST("/capture", [this]() { onCapture(); });

                    return startServer();
                }

                /**
                 *
                 * @param value
                 */
                void setMaxNumFilesPerPage(uint16_t value) {
                    _maxNumFiles = value;
                }


            protected:
                uint16_t _maxNumFiles;

                /**
                 *
                 * @return
                 */
                const char* getServerName() {
                    return "ImageBrowser";
                }

                /**
                 * Return list of files
                 */
                void onList() {
                    bool hasNextPage = false;
                    uint16_t i = 0;
                    fs::FS *fs = camera.storage.getFS();

                    if (!fs) {
                        serverError("No filesystem set");
                        return;
                    }

                    File root = fs->open("/");
                    File file = root.openNextFile();
                    const uint16_t page = argInt("page", 0);
                    const uint16_t perPage = argInt("perPage", _maxNumFiles);
                    const uint16_t offset = page * perPage;

                    // empty list
                    if (!file)
                        server.send(200, "text/plain", "");

                    while (file) {
                        // only list jpeg files
                        String filename = file.name();

                        if (filename.indexOf(".jpg") < 0) {
                            file = root.openNextFile();
                            continue;
                        }

                        if (i++ < offset) {
                            file = root.openNextFile();
                            continue;
                        }

                        if (i >= offset + perPage) {
                            hasNextPage = true;
                            break;
                        }

                        server.sendContent(F(":file:"));
                        server.sendContent(String(i));
                        server.sendContent(F(";"));
                        server.sendContent(filename);
                        server.sendContent(F(";"));
                        server.sendContent(String(file.size()));
                        server.sendContent(F("\n"));

                        file = root.openNextFile();
                    }

                    if (hasNextPage)
                        server.sendContent(":hasNextPage:");
                }


                /**
                 * Return image
                 */
                void onView() {
                    if (!assertFileExists())
                        return;

                    String path = arg("filename");
                    fs::FS *fs = camera.storage.getFS();
                    File file = fs->open(path, "rb");

                    if (!file.size()) {
                        serverError("File is empty");
                        return;
                    }

                    server.streamFile(file, "image/jpeg");
                    file.close();
                }

                /**
                 * Delete file
                 */
                void onDelete() {
                    if (!assertFileExists())
                        return;

                    String path = arg("filename");

                    camera.storage.getFS()->remove(path);
                    ok();
                }

                /**
                 * Rename file
                 */
                void onRename() {
                    if (!assertFileExists())
                        return;

                    String path = arg("filename");
                    String rename = arg("rename");

                    if (!rename.length()) {
                        clientError("No rename provided");
                        return;
                    }

                    camera.storage.getFS()->rename(path, rename);
                    ok();
                }

                /**
                 * Capture new photo
                 */
                void onCapture() {
                    fs::FS *fs = camera.storage.getFS();

                    if (!fs) {
                        serverError("No filesystem set");
                        return;
                    }

                    if (!camera.capture()) {
                        serverError(camera.getErrorMessage());
                        return;
                    }

                    if (!camera.storage.save()) {
                        serverError(camera.storage.getErrorMessage());
                        return;
                    }

                    File file = camera.storage.getFS()->open(camera.storage.lastFilename);
                    size_t size = file.size();
                    file.close();

                    sendChunks(
                        "{\"status\":\"OK\", \"name\":\"",
                        camera.storage.lastFilename,
                        "\", \"size\":",
                        size,
                        "}"
                    );
                }

                /**
                 * Assert that request file exists
                 * @return
                 */
                bool assertFileExists() {
                    String path = arg("filename");
                    fs::FS *fs = camera.storage.getFS();

                    if (!fs) {
                        serverError("No filesystem set");
                        return false;
                    }

                    if (!path.length()) {
                        clientError("Empty filename");
                        return false;
                    }

                    if (!fs->exists(path)) {
                        clientError("File not found");
                        return false;
                    }

                    return true;
                }
            };
        }
    }
}

static Eloquent::Esp32cam::Http::ImageBrowser imageBrowser;

#endif //ELOQUENTESP32CAM_IMAGE_BROWSER_H
