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
        xhr.open('POST', '/move/' + encodeURIComponent('%s'), false);

        formData.append('to', to);

        xhr.send(formData);
        window.location.href = '/explorer/' + encodeURIComponent(dirname(to));
    };
    submit_delete_item = () => {
        let xhr = new XMLHttpRequest();
        xhr.open('POST', '/delete/' + encodeURIComponent('%s'), false);
        xhr.send();
    };
    submit_delete_reload = () => {
        window.location.href = '/explorer/' + encodeURIComponent('%s');
    };
    submit_copy_item = (to) => {
        let formData = new FormData();
        let xhr = new XMLHttpRequest();
        xhr.open('POST', '/copy/' + encodeURIComponent('%s'), false);

        formData.append('to', to);

        xhr.send(formData);
        window.location.href = '/explorer/' + encodeURIComponent(dirname(to));
    };
    submit_download_item = () => {
        window.location.href = '/download/' + encodeURIComponent('%s');
    };
    copy_link_item = () => {
        console.log('Copying text to clipboard...');
        navigator.clipboard.writeText(location.protocol + '//' + location.host + '/explorer/' + encodeURIComponent('%s')).then(function () {
            Toastify({
                text: 'Copied URL to clipboard',
                duration: 1000
            }).showToast();
        }, function (err) {
            console.error('Async: Could not copy text: ', err);
        });
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
        xhr.open('POST', '/move/' + encodeURIComponent('%s'), false);

        formData.append('to', to);

        xhr.send(formData);
        window.location.href = '/explorer/' + encodeURIComponent(dirname(to));
    };
    submit_delete_item = () => {
        let xhr = new XMLHttpRequest();
        xhr.open('POST', '/delete/' + encodeURIComponent('%s'), false);
        xhr.send();
    };
    submit_delete_reload = () => {
        window.location.href = '/explorer/' + encodeURIComponent('%s');
    };
    submit_copy_item = (to) => {
        let formData = new FormData();
        let xhr = new XMLHttpRequest();
        xhr.open('POST', '/copy/' + encodeURIComponent('%s'), false);

        formData.append('to', to);

        xhr.send(formData);
        window.location.href = '/explorer/' + encodeURIComponent(dirname(to));
    };
    submit_download_item = () => {
        window.location.href = '/download/' + encodeURIComponent('%s');
    };
    copy_link_item = () => {
        console.log('Copying text to clipboard...');
        navigator.clipboard.writeText(location.protocol + '//' + location.host + '/explorer/' + encodeURIComponent('%s')).then(function () {
            Toastify({
                text: 'Copied URL to clipboard',
                duration: 1000
            }).showToast();
        }, function (err) {
            console.error('Async: Could not copy text: ', err);
        });
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


#endif //FILESTORAGE_TEMPLATES_HPP
