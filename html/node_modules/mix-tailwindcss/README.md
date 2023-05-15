<p align="center">
  <a href="https://laravel-mix.com/extensions/tailwindcss">
      <img src="https://laravel-mix.com/svg/laravel-mix-tailwindcss.svg" alt="Laravel Mix Tailwind CSS" width="200">
  </a>
</p>

# Laravel Mix Tailwind CSS

[![Latest Version on NPM][ico-version]][link-npm]
[![Software License][ico-license]](LICENSE.md)
[![StyleCI][ico-styleci]][link-styleci]
[![Total Downloads][ico-downloads]][link-downloads]

This extension provides instant Tailwind CSS support to your Mix builds.  
Please make sure that you are using `laravel-mix` version 2.1 or higher.

This package works with Tailwind version 1.0 or higher but also with earlier versions.

## Usage

You can install the package with npm or yarn:

```bash
npm install mix-tailwindcss --save-dev
```

```bash
yarn add mix-tailwindcss --dev
```

Then require the extension in your Mix configuration:

```js
const mix = require('laravel-mix');

require('mix-tailwindcss');
...
```

Enable the extension by calling `tailwind()` in your Mix chain:

```js
mix.sass('resources/sass/app.scss', 'public/css')
   .tailwind();
```

If you are using a custom configuration file name, you will need to specify it when calling `tailwind()`:

```js
mix.sass('resources/sass/app.scss', 'public/css')
   .tailwind('./tailwindcss-config.js');
```

And you're done!

Don't forget to [add the Tailwind directives](https://tailwindcss.com/docs/installation/#2-add-tailwind-to-your-css) to your css entry file.

### PostCSS with postcss-import

If you're not integrating any Sass, Less or Stylus stylesheets you can use the `mix.postCss()` method.

When the `postcss-import` plugin is installed, it will be automatically detected and applied to your build.

```js
mix.postCss('resources/css/app.css', 'public/css')
    .tailwind();
```

## Contributing

Please see [CONTRIBUTING](.github/CONTRIBUTING.md) for details.

## Security Vulnerabilities

Please review our [security policy](.github/SECURITY.md) on how to report security vulnerabilities.

## Credits

- [Mark van den Broek](https://github.com/mvdnbrk)
- [Jeffrey Way](https://github.com/JeffreyWay)
- [All Contributors](../../contributors)

## License

The MIT License (MIT). Please see [License File](LICENSE.md) for more information.

[ico-downloads]: https://img.shields.io/npm/dt/mix-tailwindcss.svg?style=flat-square
[ico-license]: https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat-square
[ico-styleci]: https://github.styleci.io/repos/188557285/shield?style=flat-square&branch=master
[ico-version]: https://img.shields.io/npm/v/mix-tailwindcss.svg?style=flat-square

[link-downloads]: https://npmjs.com/package/mix-tailwindcss
[link-npm]: https://npmjs.com/package/mix-tailwindcss
[link-styleci]: https://github.styleci.io/repos/188557285
