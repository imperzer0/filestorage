//
// Created by imper on 8/17/22.
//

#ifndef FILESTORAGE_RESOURCES_HPP
#define FILESTORAGE_RESOURCES_HPP


static constexpr const char* _404_html =
		
		#include "404.html"


static constexpr const char* index_page_html =
		
		#include "index.html"


static constexpr const char* invalid_credentials_page_html =
		
		#include "invalid_credentials.html"


static constexpr const char* explorer_page_html =
		
		#include "explorer.html"


static constexpr const char* explorer_dir_html = R"(
        <form method="POST" action="/explorer/%s" style="display: none;" id="forwardform%s">
            <input type="text" name="login" value="%s"/>
            <input type="password" name="password" value="%s"/>
        </form>

        <li class="folders">
            <a title="EXPLORER/%s" class="folders" onclick="document.getElementById('forwardform%s').submit();"
               oncontextmenu="
	        submit_move_item = (to) => {
	            let formData = new FormData();
	            let xhr = new XMLHttpRequest();
	            xhr.open('POST', '/move/%s', false);
	
	            formData.append('login', '%s');
	            formData.append('password', '%s');
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
            <input type="text" name="login" value="%s"/>
            <input type="password" name="password" value="%s"/>
        </form>

        <li class="files">
            <a title="EXPLORER/%s" class="files" onclick="document.getElementById('forwardform%s').submit();"
               oncontextmenu="
            submit_move_item = (to) => {
                let formData = new FormData();
                let xhr = new XMLHttpRequest();
                xhr.open('POST', '/move/%s', false);
    
                formData.append('login', '%s');
                formData.append('password', '%s');
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


static constexpr const char* deleter_page_html =
		
		#include "deleter.html"


static constexpr const char* deleter_dir_html = R"(
    <li class="folders">
        <a title="EXPLORER/%s" class="folders" onclick="
        submit_deletion = ()=>{
            var xhr = new XMLHttpRequest();
            xhr.open('POST', '/delete/%s', true);
            xhr.send('login=%s&password=%s');
        };

        reload_page = ()=>{
            var xhr = new XMLHttpRequest();
            xhr.open('POST', '/deleter/%s');
            xhr.send('login=%s&password=%s');
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
            xhr.send('login=%s&password=%s');
        };

        reload_page = ()=>{
            var xhr = new XMLHttpRequest();
            xhr.open('POST', '/deleter/%s');
            xhr.send('login=%s&password=%s');
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


static constexpr const char* uploader_page_html =
		
		#include "uploader.html"


#endif //FILESTORAGE_RESOURCES_HPP
