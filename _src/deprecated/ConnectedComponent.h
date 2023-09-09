//
// Created by Simone on 17/11/22.
// @WIP
//

#ifndef ELOQUENTESP32CAM_CONNECTEDCOMPONENT_H
#define ELOQUENTESP32CAM_CONNECTEDCOMPONENT_H

// @from https://weirdo.dev/algorithms/largest-island-in-binary-matrix.html
#include "../traits/HasErrorMessage.h"

#ifndef MAX_RECURSION_DEPTH
#define MAX_RECURSION_DEPTH 10
#endif


namespace Eloquent {
    namespace Esp32cam {
        template<typename Foreground>
        class ConnectedComponent : public HasErrorMessage {
        public:
            uint16_t x;
            uint16_t y;
            uint16_t top;
            uint16_t bottom;
            uint16_t left;
            uint16_t right;
            size_t area;

            /**
             *
             * @param foreground
             */
            ConnectedComponent(Foreground& foreground) :
                area(0),
                x(0),
                y(0),
                top(0),
                bottom(0),
                left(0),
                right(0),
                _count(0),
                _width(0),
                _height(0),
                _foreground(&foreground) {

            }

            /**
             * Localize x, y, area of largest foreground object
             */
            bool localize() {
                x = 0;
                y = 0;
                area = 0;

                _width = _foreground->getWidth();
                _height = _foreground->getHeight();

                if (!_width || !_height)
                    return setErrorMessage("Unknown frame size");

                top = _height;
                bottom = 0;
                left = _width;
                right = 0;

                // reset visited
                memset(_visited, 0, _width * _height);

                for (int y = 2; y < _height - 2; y++) {
                    for (int x = 2; x < _width - 2; x++) {
                        if (!_visited[y][x] && _foreground->isForeground(x, y)) {
                            _count = 1;
                            _top = _height;
                            _bottom = 0;
                            _left = _width;
                            _right = 0;

                            dfs(x, y);

                            if (_count > area) {
                                area = _count;
                                this->x = x;
                                this->y = y;
                                this->area = _count;
                                top = _top;
                                bottom = _bottom;
                                left = _left;
                                right = _right;
                            }
                        }
                    }
                }

                return setErrorMessage("");
            }

            /**
             * Convert blob to JSON
             *
             * @return
             */
            String toJson() {
                return String("{\"top\":")
                + (top * 8)
                + ", \"bottom\":"
                + (bottom * 8)
                + ", \"left\":"
                + (left * 8)
                + ", \"right\":"
                + (right * 8)
                + "}";
            }

        protected:
            Foreground *_foreground;
            size_t _count;
            uint16_t _width;
            uint16_t _height;
            uint16_t _top;
            uint16_t _bottom;
            uint16_t _left;
            uint16_t _right;

            bool _visited[MAX_RESOLUTION_HEIGHT / 8][MAX_RESOLUTION_WIDTH / 8];

            /**
             * Depth-fist search
             * @param row
             * @param column
             * @param graph
             * @return
             */
            inline void dfs(int x, int y, uint8_t depth = 1) {
                if (depth > MAX_RECURSION_DEPTH)
                    return;

                _visited[y][x] = true;

                if (x < _left) _left = x;
                if (x > _right) _right = x;
                if (y < _top) _top = y;
                if (y > _bottom) _bottom = y;

                // @implemention note
                // using a pure recursive DFS, we get a runtime error
                // due to too deep recursion
                // so we manually "unroll" the recursion

                // 1st degree neighbors
                if (isCandidate(x, y + 1)) {
                    _count++;
                }

                if (isCandidate(x + 1, y)) {
                    _count++;
                }

                if (x > 1 && isCandidate(x - 1, y)) {
                    _count++;
                }

                if (y > 1 && isCandidate(x, y - 1)) {
                    _count++;
                }

                // flattened DFS to limit recursion depth
                const int8_t maxDistance = 12;

                for (int dy = -maxDistance; dy < maxDistance + 1; dy++) {
                    for (int dx = -maxDistance; dx < maxDistance + 1; dx++) {
                        const uint8_t distance = abs(dx) + abs(dy);

                        if (distance > 1 && distance <= maxDistance && isCandidate(x + dx, y + dy)) {
                            _count++;

                            if (distance == maxDistance)
                                dfs(x + dx, y + dy, depth + 1);
                        }
                    }
                }
            }

            /**
             *
             * @param x
             * @param y
             * @return
             */
            inline bool isCandidate(int x, int y)
            {
                return
                    x >= 0
                    && y >= 0
                    && x < _width
                    && y < _height
                    && !_visited[y][x]
                    && _foreground->isForeground(x, y);
                ;
            }
        };
    }
}

#endif //ELOQUENTESP32CAM_CONNECTEDCOMPONENT_H
