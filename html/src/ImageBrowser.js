import Alpine from "alpinejs";
import {onLoad} from "./app";

onLoad("ImageBrowser", () => {
    window.App = {
        ImageBrowser() {
            return {
                isLoading: false,
                files: [],
                page: 0,
                perPage: 30,
                hasNextPage: false,

                get hasPrevPage() {
                    return this.page > 0
                },

                get hasPagination() {
                    return this.hasPrevPage || this.hasNextPage
                },

                /**
                 *
                 */
                init() {
                    this.fetch()
                    this.$watch("page", () => this.fetch())
                    this.$watch("perPage", () => this.fetch())
                },

                /**
                 * Get list of files
                 */
                fetch() {
                    this.request(`/ls?page=${this.page}&perPage=${this.perPage}`)
                        .then(res => res.text())
                        .then(res => {
                            this.hasNextPage = res.indexOf("hasNextPage") > 0

                            return res.split("\n")
                        })
                        .then(lines => {
                            this.files = lines
                                .filter(line => line.startsWith(":file:"))
                                .map(line => {
                                    const [index, name, size] = line.substring(":file:".length).split(";")

                                    return {index, name, size}
                                })
                                .sort((a, b) => a.name > b.name ? -1 : 1)
                        })
                },

                /**
                 * Get next page
                 */
                next() {
                    this.page += 1
                },

                /**
                 *
                 */
                prev() {
                    this.page -= 1
                },

                /**
                 * Format bytes into human readable
                 * @param size
                 * @returns {string}
                 */
                format(size) {
                    if (size < 1024)
                        return `${size}B`;

                    if (size < (1024 * 1024))
                        return `${Math.round(size / 1024)}KB`

                    if (size < (1024 * 1024 * 1024))
                        return `${Math.round(size / 1024 / 1024)}MB`

                    return `${size}B`;
                },

                /**
                 * Delete file
                 * @param filename
                 */
                remove(filename) {
                    if (confirm(`Are you sure you want to delete the image ${filename}?`)) {
                        this.request(`/delete?filename=${filename}`)
                            .then(res => res.text())
                            .then(res => {
                                if (res === "OK") {
                                    this.files = this
                                        .files
                                        .filter(file => file.name !== filename)
                                }

                                alert(res)
                            })
                            .catch(err => alert('An error occurred'))
                    }
                },

                /**
                 * Delete file
                 * @param filename
                 */
                rename(filename) {
                    let rename = prompt(`Rename ${filename} to (must end with .jpg)...`)

                    if (!rename)
                        return alert("Aborted")

                    if (filename.startsWith("/") && !rename.startsWith("/"))
                        rename = `/${rename}`

                    if (!rename.endsWith(".jpg"))
                        rename += ".jpg"

                    this.request(`/rename?filename=${filename}&rename=${rename}`)
                        .then(res => res.text())
                        .then(res => {
                            if (res === "OK") {
                                this
                                    .files
                                    .filter(file => file.name === filename)
                                    .forEach(file => file.name = rename)
                            }

                            alert(res)
                        })
                        .catch(err => alert('An error occurred'))
                },

                /**
                 * Take new photo
                 */
                capture() {
                    this.request("/capture", {method: "POST"})
                        .then(res => res.text())
                        .then(res => {
                            if (res.indexOf("OK")) {
                                this.files.push(JSON.parse(res))
                                alert("OK")
                                return
                            }

                            alert(res)
                        })
                },

                /**
                 * Handle request state
                 * @param url
                 */
                request(url, args = null) {
                    this.isLoading = true

                    return fetch(url, args)
                        .then(res => {
                            this.$nextTick(() => this.isLoading = false)
                            return res
                        })
                }
            }
        }
    }


    Alpine.start()
})()