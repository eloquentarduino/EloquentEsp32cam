const mix = require('laravel-mix');

class Tailwind {
    seesTailwindConfig(path) {
        try {
            require.resolve(path);

            return true;
        } catch (e) {
            return false;
        }
    }

    dependencies() {
        this.requiresReload = `Tailwind CSS has now been installed.`;

        return ['tailwindcss'];
    }

    register(configPath) {
        this.configPath = configPath;

        if (this.configPath !== undefined) {
            return;
        }

        if (this.seesTailwindConfig('./tailwind.config.js')) {
            this.configPath = './tailwind.config.js';

            return;
        }

        if (this.seesTailwindConfig('./tailwind.js')) {
            this.configPath = './tailwind.js';
        }
    }

    boot() {
        if (Mix.components.has('sass')) {
            Config.processCssUrls = false;
        }

        if (Mix.components.has('postCss') && Mix.seesNpmPackage('postcss-import')) {
            Config.postCss.push(require('postcss-import'));
        }

        const tailwindcss = require('tailwindcss');

        Config.postCss.push(tailwindcss(this.configPath));
    }
}

mix.extend('tailwind', new Tailwind());
