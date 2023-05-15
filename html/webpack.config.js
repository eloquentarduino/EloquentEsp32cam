const path = require('path');

module.exports = {
    entry: './src/ImageBrowser.js',
    output: {
        filename: 'ImageBrowser.js',
        path: path.resolve(__dirname, 'dist'),
    },
};