//
// Created by Simone on 17/11/22.
//

#ifndef ELOQUENTESP32CAM_CONNECTEDCOMPONENT_H
#define ELOQUENTESP32CAM_CONNECTEDCOMPONENT_H

// @from https://weirdo.dev/algorithms/largest-island-in-binary-matrix.html
#include "../../traits/HasErrorMessage.h"
#include "./Detector.h"

namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            class ConnectedComponent : public HasErrorMessage {
            public:
                size_t x;
                size_t y;
                size_t area;

                /**
                 *
                 * @param detector
                 */
                ConnectedComponent(Detector& detector) :
                    area(0),
                    x(0),
                    y(0),
                    _count(0),
                    _detector(&detector) {

                }

                /**
                 * Localize x, y, area of largest foreground object
                 */
                bool localize() {
                    x = 0;
                    y = 0;
                    area = 0;

                    if (!_detector->getWidth() || !_detector->getHeight())
                        return setErrorMessage("Unknown frame size");

                    for(int y = 0; y < _detector->getHeight(); y++) {
                        for(int x = 0; x < _detector->getWidth(); x++) {
                            if(!_visited[y][x] && _detector->isForeground(x, y)) {
                                _count = 1;
                                dfs(x, y, &_count);

                                if(_count > area) {
                                    area = _count;
                                    this->x = x;
                                    this->y = y;
                                    this->area = _count;
                                }
                            }
                        }
                    }

                    return setErrorMessage("");
                }

            protected:
                Detector *_detector;
                size_t _count;
                bool _visited[MAX_RESOLUTION_HEIGHT / 8][MAX_RESOLUTION_WIDTH / 8];

                /**
                 * Depth-fist search
                 * @param row
                 * @param column
                 * @param graph
                 * @return
                 */
                inline void dfs(int x, int y, size_t* count) {
                    _visited[y][x] = true;

                    if (isSafe(x, y + 1)) {
                        (*count) += 1;
                        dfs(x, y + 1, count);
                    }

                    if (isSafe(x + 1, y)) {
                        (*count) += 1;
                        dfs(x + 1, y, count);
                    }

                    if (x > 1 && isSafe(x - 1, y)) {
                        (*count) += 1;
                        dfs(x - 1, y, count);
                    }

                    /*if (y > 1 && isSafe(x, y - 1)) {
                        (*count) += 1;
                        dfs(x, y - 1, count);
                    }*/
                }

                /**
                 *
                 * @param x
                 * @param y
                 * @return
                 */
                inline bool isSafe(int x, int y)
                {
                    return
                        x >= 0
                        && y >= 0
                        && _detector->isForeground(x, y)
                        && !_visited[y][x];
                    ;
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_CONNECTEDCOMPONENT_H
