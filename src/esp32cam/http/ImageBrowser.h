//
// Created by Simone on 05/11/22.
//

#ifndef ELOQUENTESP32CAM_IMAGE_BROWSER_H
#define ELOQUENTESP32CAM_IMAGE_BROWSER_H


#include <WiFi.h>
#include <WebServer.h>
#include "../../traits/HasErrorMessage.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {

            /**
             * ESP32 camera image browser web server
             */
            class ImageBrowser :
                    public HasErrorMessage {

            public:
                /**
                 *
                 * @param port
                 */
                ImageBrowser(fs::FS& fs, uint16_t httpPort = 80) :
                _fs (fs),
                _port(httpPort),
                _server(httpPort)  {

                    setMaxNumFilesPerPage(30);
                    setErrorMessage("");
                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    // index
                    _server.on("/", HTTP_GET, [this]() {
                        _server.setContentLength(CONTENT_LENGTH_UNKNOWN);
                        _server.send(200, "text/html", "");
                        _server.sendContent(F(R"===(
<table border="1">
    <thead>
        <tr>
            <th>Idx</th>
            <th>Filename</th>
            <th>Size</th>
            <th></th>
            <th></th>
        </tr>
    </thead>
    <tbody>
)==="));

                        bool hasNextPage = false;
                        uint16_t i = 0;
                        File root = this->_fs.open("/");
                        File file = root.openNextFile();

                        String pageArg = this->_server.arg("page");
                        const uint8_t page = pageArg == "" ? 0 : pageArg.toInt();
                        const uint16_t offset = page * _maxNumFiles;

                        while (file) {
                            // only list jpeg files
                            String filename = file.name();
                            size_t filesize = file.size();

                            if (filename.indexOf(".jpeg") < 0 && filename.indexOf(".jpg") < 0) {
                                file = root.openNextFile();
                                continue;
                            }

                            if (++i <= offset) {
                                file = root.openNextFile();
                                continue;
                            }

                            if (i > offset + this->_maxNumFiles) {
                                hasNextPage = true;
                                break;
                            }

                            _server.sendContent(F("<tr><td>"));
                            _server.sendContent(String(i));
                            _server.sendContent(F("</td><td>"));
                            _server.sendContent(filename);
                            _server.sendContent(F("</td><td>"));
                            _server.sendContent(this->formatBytes(filesize));
                            _server.sendContent(F("</td><td></td><td></td></tr>"));

                            file = root.openNextFile();
                        }

                        _server.sendContent(F("</tbody></table>"));
                        _server.sendContent(F("<div id=\"nav\"></div>"));

                        if (file)
                            file.close();

                        if (hasNextPage)
                            _server.sendContent(F("<script>var hasNextPage = true</script>"));

                        _server.sendContent(F(R"===(
<style>
form { margin: 0 }
#nav { display: flex; }
#nav > a { margin: 10px; }
</style>
<script>
;[].slice.call(document.querySelectorAll('tbody tr')).forEach(tr => {
    const tdName = tr.children[1];
    const tdRename = tr.children[3];
    const tdDelete = tr.children[4];
    const name = tdName.innerText;

    tdName.innerHTML = `<a href="/view?name=${name}" target="_blank">${name}</a>`
    tdDelete.innerHTML = `<form action="/delete?name=${name}" method="POST"><button type="submit">Delete</button></form>`
    tdRename.innerHTML = `<form action="/rename?name=${name}" method="POST"><input name="rename" required /><button type="submit">Rename</button></form>`
});
</script>
<script>
const nav = document.getElementById('nav')
const params = new URLSearchParams(window.location.search)
const page = parseInt(params.get('page') || 0)

if (page > 0) {
    const prev = document.createElement('a')
    prev.innerText = "Prev"
    prev.href = `/?page=${page-1}`
    nav.appendChild(prev)
}

if (hasNextPage) {
    const next = document.createElement('a')
    next.innerText = "Next"
    next.href = `/?page=${page+1}`
    nav.appendChild(next)
}
</script>
)==="));
                        _server.sendContent(F(""));
                        _server.client().stop();

                        return true;
                    });

                    // send raw image
                    _server.on("/view", HTTP_GET, [this]() {
                        String path = this->_server.arg("name");

                        if (!this->_fs.exists(path)) {
                            this->_server.send(404, "text/plain", path + " not found");
                            return false;
                        }

                        File file = this->_fs.open(path, "rb");

                        if (!file.size()) {
                            this->_server.send(500, "text/plain", path + " is empty");
                            return false;
                        }

                        this->_server.streamFile(file, "image/jpeg");
                        file.close();
                        return true;
                    });

                    // delete file
                    _server.on("/delete", HTTP_POST, [this]() {
                        String path = this->_server.arg("name");

                        if (!this->_fs.exists(path)) {
                            _server.send(404, "text/plain", path + " not found");
                            return false;
                        }

                        this->_fs.remove(path);
                        this->_server.sendHeader("Location", "/");
                        this->_server.send(302, "text/plain", path + " deleted!");

                        return true;
                    });

                    // rename file
                    _server.on("/rename", HTTP_POST, [this]() {
                        String path = this->_server.arg("name");
                        String rename = this->_server.arg("rename");

                        if (!this->_fs.exists(path)) {
                            _server.send(404, "text/plain", path + " not found");
                            return false;
                        }

                        if (rename == "") {
                            _server.send(400, "text/plain", F("new name cannot be empty"));
                            return false;
                        }

                        if (!rename.endsWith(".jpg") && !rename.endsWith(".jpeg")) {
                            _server.send(400, "text/plain", F("new name must end with .jpg or .jpeg"));
                            return false;
                        }

                        if (rename[0] != '/')
                            rename = String('/') + rename;


                        this->_fs.rename(path, rename);
                        this->_server.sendHeader("Location", "/");
                        this->_server.send(302, "text/plain", path + " renamed to " + rename);

                        return true;
                    });

                    _server.begin();

                    return true;
                }

                /**
                 *
                 * @param value
                 */
                void setMaxNumFilesPerPage(uint16_t value) {
                    _maxNumFiles = value;
                }

                /**
                 *
                 */
                void handle() {
                    _server.handleClient();
                }

                /**
                 *
                 * @return
                 */
                String getWelcomeMessage() {
                    IPAddress ipAddr = WiFi.localIP();
                    String ip = String(ipAddr[0]) + '.' + ipAddr[1] + '.' + ipAddr[2] + '.' + ipAddr[3];

                    return
                            String(F("Image Browser available at http://"))
                            + ip
                            + (_port != 80 ? String(':') + _port : "");
                }

            protected:
                fs::FS _fs;
                uint16_t _port;
                uint16_t _maxNumFiles;
                WebServer _server;

                /**
                 *
                 * @param bytes
                 * @return
                 */
                String formatBytes(size_t bytes) {
                    if (bytes < 1024) {
                        return String(bytes) + "B";
                    } else if (bytes < (1024 * 1024)) {
                        return String(bytes / 1024.0) + "KB";
                    } else if (bytes < (1024 * 1024 * 1024)) {
                        return String(bytes / 1024.0 / 1024.0) + "MB";
                    } else {
                        return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
                    }
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_IMAGE_BROWSER_H
