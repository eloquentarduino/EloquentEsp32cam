let mix = require('laravel-mix')


function js(filename) {
    mix.js(`src/${filename}.js`, 'dist')
}


require('mix-tailwindcss')
js('ImageBrowser')
mix.sass('src/app.scss', 'dist').tailwind()
mix.copyDirectory('src/templates', 'dist')
