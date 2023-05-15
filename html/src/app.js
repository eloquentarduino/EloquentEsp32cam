/**
 *
 * @returns {string}
 */
export function getRemoteURL() {
    const scripts = document.getElementsByTagName("script");

    return scripts[scripts.length - 1].src.split("/").slice(0, -1).join("/")
}


/**
 *
 * @returns {string}
 */
export function getSelfName() {
    const scripts = document.getElementsByTagName("script");

    return scripts[scripts.length - 1].src.split("/").slice(-1)[0].replace(".js", "")
}

/**
 * Run function on load
 *
 * @param title
 * @param callback
 * @returns {function(): void}
 */
export function onLoad(title, callback) {
    return function() {
        document.addEventListener("DOMContentLoaded", () => {
            const remoteURL = getRemoteURL()
            const selfName = getSelfName()

            // add custom class for styling
            document.body.classList.add(title)
            document.getElementsByTagName("html")[0].classList.add(title)

            // inject CSS
            fetch(`${remoteURL}/app.css`)
                .then(res => res.text())
                .then(res => {
                    const css = document.createElement("style")

                    css.innerHTML = res
                    document.body.appendChild(css)
                })

            // inject HTML
            fetch(`${remoteURL}/${selfName}.html`)
                .then(res => res.text())
                .then(res => {
                    const app = document.createElement('div')

                    app.id = "app"
                    app.innerHTML = res
                    document.body.appendChild(app)

                    // run code
                    setTimeout(() => callback(), 100)
                })
        })
    }
}