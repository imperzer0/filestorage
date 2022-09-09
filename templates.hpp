//
// Created by imper on 8/29/22.
//

#ifndef FILESTORAGE_TEMPLATES_HPP
#define FILESTORAGE_TEMPLATES_HPP


static constexpr const char* explorer_dir_html = R"(
        <form method="POST" action="/explorer/%s" style="display: none;" id="forwardform%s">
        </form>

        <li class="folders">
            <a title="EXPLORER/%s" class="folders" onclick="document.getElementById('forwardform%s').submit();"
               oncontextmenu="
	        submit_move_item = (to) => {
	            let formData = new FormData();
	            let xhr = new XMLHttpRequest();
	            xhr.open('POST', '/move/%s', false);
	
	            formData.append('to', to);
	
	            xhr.send(formData);
	        };">
                <span class="icon folder full"></span>
                <span class="name">%s</span>
                <span class="details">%zu files, %zu folders</span>
            </a>
        </li>
)";

static constexpr const char* explorer_file_html = R"(
        <form method="POST" action="/explorer/%s" style="display: none;" id="forwardform%s" target="_blank">
        </form>

        <li class="files">
            <a title="EXPLORER/%s" class="files" onclick="document.getElementById('forwardform%s').submit();"
               oncontextmenu="
            submit_move_item = (to) => {
                let formData = new FormData();
                let xhr = new XMLHttpRequest();
                xhr.open('POST', '/move/%s', false);
    
                formData.append('to', to);
    
                xhr.send(formData);
            };">
                <span class="icon file f-%s">.%s</span>
                <span class="name">%s</span>
                <span class="details">%zu bytes</span>
            </a>
        </li>
)";

static constexpr const char* explorer_dir_content_html = R"(
    <ul class="data animated" style="">
    %s
    </ul>
)";

static constexpr const char* explorer_dir_empty_html = R"(
    <div class="nothingfound" style="display: block;">
        <div class="nofiles"></div>
        <span>Folder is empty</span>
    </div>
)";


static constexpr const char* deleter_dir_html = R"(
    <li class="folders">
        <a title="EXPLORER/%s" class="folders" onclick="
        submit_deletion = ()=>{
            var xhr = new XMLHttpRequest();
            xhr.open('POST', '/delete/%s', true);
            xhr.send();
        };

        reload_page = ()=>{
            var xhr = new XMLHttpRequest();
            xhr.open('POST', '/deleter/%s');
            xhr.send();
            xhr.addEventListener('load', function () {
                document.open();
                document.write(this.responseText);
                document.close();
            });
        };

        JSalert();">
            <span class="icon folder full"></span>
            <span class="name">%s</span>
            <span class="details">%zu files, %zu folders</span>
        </a>
    </li>
)";

static constexpr const char* deleter_file_html = R"(
        <li class="files">
            <a title="EXPLORER/%s" class="files"  onclick="
        submit_deletion = ()=>{
            var xhr = new XMLHttpRequest();
            xhr.open('POST', '/delete/%s', true);
            xhr.send();
        };

        reload_page = ()=>{
            var xhr = new XMLHttpRequest();
            xhr.open('POST', '/deleter/%s');
            xhr.send();
            xhr.addEventListener('load', function () {
                document.open();
                document.write(this.responseText);
                document.close();
            });
        };

        JSalert();">
                <span class="icon file f-%s">.%s</span>
                <span class="name">%s</span>
                <span class="details">%zu bytes</span>
            </a>
        </li>
)";


#endif //FILESTORAGE_TEMPLATES_HPP
