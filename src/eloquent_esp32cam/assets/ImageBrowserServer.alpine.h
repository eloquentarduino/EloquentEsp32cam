
            #pragma once
            #define IS_ALPINE_SPA 1

            static const char* ALPINE_SPA PROGMEM = R"=====(
                <script src="https://cdn.tailwindcss.com"></script>
                <script src="//unpkg.com/alpinejs" defer></script>
                <div x-cloak x-data="ImageBrowserServer()"><div class="files max-w-xl"><table class="w-full"><thead><tr><th>No</th><th>Filename</th><th>Filesize</th><th></th><th></th><th></th></tr></thead><tbody><template x-if="!files.length"><tr><td colspan="6">No files found</td></tr></template><template x-for="file, i in files"><tr><td x-text="i + 1"></td><td x-text="file.filename"></td><td x-text="file.formattedFilesize"></td><td>View</td><td>Rename</td><td>Download</td><td>Delete</td></tr></template></tbody></table></div></div>
                <script>window.ImageBrowserServer=function(){return{page:0,perPage:30,files:[],errors:[],init(){this.fetchPage(),this.$watch("perPage",()=>{this.page=0})},fetchPage(){fetch(`http://${window.location.host}/api/list?page=${this.page}&perPage=`+this.perPage).then(e=>e.text()).then(e=>e.trim().split("\n")).then(e=>e.map(JSON.parse).filter(e=>e)).then(e=>{this.errors=e.filter(e=>e.error),this.files=this.files.concat(e.filter(e=>e.filename)).sort((e,t)=>e.filename.localeCompare(t.filename)).map(e=>Object.assign(e,{formattedFilesize:this.formatFilesize(e.size)}))})},formatFilesize(e){return e<1e3?e+" bytes":e<1e6?(e/1e3).toFixed(2)+" Kb":(e/1e6).toFixed(2)+" Mb"}}};</script>
            )=====";
    