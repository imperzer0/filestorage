function Extensions:index(path)
    local page = "<!DOCTYPE html><html lang=\"en\">" ..
            "<head> <meta charset=\"UTF-8\"> <title>All Extensions</title>" ..
            "<style type=\"text/css\">" ..
            "@import url(https://fonts.googleapis.com/css?family=Consolas);" ..
            "html {" ..
            "    box-sizing: border-box;" ..
            "}" ..
            "*," ..
            "*::before," ..
            "*::after {" ..
            "    box-sizing: inherit;" ..
            "}" ..
            "body {" ..
            "    background: #222222;" ..
            "    font-size: 1.1rem;" ..
            "    line-height: 1.5;" ..
            "    font-family: Consolas, monospace;" ..
            "    color: #d3d3d3;" ..
            "}" ..
            "a {" ..
            "    text-decoration: none;" ..
            "    color: #d3d3d3;" ..
            "}" ..
            "@media (min-width: 900px) {" ..
            "    body {" ..
            "        padding: 1rem;" ..
            "    }" ..
            "}" ..
            "[draggable] {" ..
            "    -webkit-user-select: none;" ..
            "    -moz-user-select: none;" ..
            "    -ms-user-select: none;" ..
            "    user-select: none;" ..
            "    -khtml-user-drag: element;" ..
            "    -webkit-user-drag: element;" ..
            "}" ..
            "h1, h2, h3, h4, h5, h6 {" ..
            "    margin: 0;" ..
            "    font-size: 1.5rem;" ..
            "    font-weight: bold;" ..
            "}" ..
            ".extension-list {" ..
            "    width: 70%;" ..
            "    text-align: left;" ..
            "    margin: 5% 15%;" ..
            "    border: 3px dotted rgba(6, 91, 140, 0.8);" ..
            "    border-radius: 5px;" ..
            "}" ..
            ".extension-list .extension-list__extension:nth-child(odd) {" ..
            "    background: #333333;" ..
            "}" ..
            ".extension-list .extension-list__extension:nth-child(even) {" ..
            "    background: #444444;" ..
            "}" ..
            ".extension-list th {" ..
            "    -webkit-user-select: none;" ..
            "    -moz-user-select: none;" ..
            "    -ms-user-select: none;" ..
            "    user-select: none;" ..
            "    border-bottom: 1px dashed #efefef;" ..
            "    padding: 0.5rem 1rem;" ..
            "}" ..
            ".extension-list th:hover {" ..
            "    cursor: pointer;" ..
            "    color: #ffffff;" ..
            "}" ..
            ".extension-list tr {" ..
            "    padding: 0;" ..
            "}" ..
            ".extension-list td {" ..
            "    padding: 0.3333333333rem 1rem;" ..
            "    margin: 0;" ..
            "}" ..
            ".extension-list td:hover {" ..
            "    cursor: pointer;" ..
            "    color: #000000;" ..
            "}" ..
            ".extension-list__header {" ..
            "    font-size: 1rem;" ..
            "    letter-spacing: 1px;" ..
            "    line-height: 1;" ..
            "}" ..
            ".no_extensions_text {" ..
            "    font-size: 2rem;" ..
            "    color: #ff0058;" ..
            "}" ..
            ".no_extensions_text:hover {" ..
            "    color: #000000;" ..
            "}" ..
            ".back_ref {" ..
            "    font-size: 1.5rem;" ..
            "    color: #ffa058;" ..
            "}" ..
            ".back_ref:hover {" ..
            "    color: #000000;" ..
            "}" ..
            "</style>" ..
            "<style type=\"text/css\">" ..
            "button {" ..
            "    margin: 20px;" ..
            "}" ..
            ".custom-btn {" ..
            "    width: 130px;" ..
            "    height: 40px;" ..
            "    color: #fff;" ..
            "    border-radius: 5px;" ..
            "    padding: 10px 25px;" ..
            "    font-family: 'Lato', sans-serif;" ..
            "    font-weight: 500;" ..
            "    background: transparent;" ..
            "    cursor: pointer;" ..
            "    transition: all 0.3s ease;" ..
            "    position: relative;" ..
            "    display: inline-block;" ..
            "    outline: none;" ..
            "    font-size: 1.1rem;" ..
            "}" ..
            "/* 3 */" ..
            ".btn-3 {" ..
            "    background: rgb(0, 172, 238);" ..
            "    background: linear-gradient(0deg, rgba(0, 172, 238, 1) 0%, rgba(2, 126, 251, 1) 100%);" ..
            "    width: 130px;" ..
            "    height: 40px;" ..
            "    line-height: 42px;" ..
            "    padding: 0;" ..
            "    border: none;" ..
            "}" ..
            ".btn-3 span {" ..
            "    position: relative;" ..
            "    display: block;" ..
            "    width: 100%;" ..
            "    height: 100%;" ..
            "}" ..
            ".btn-3:before," ..
            ".btn-3:after {" ..
            "    position: absolute;" ..
            "    content: \"\";" ..
            "    right: 0;" ..
            "    top: 0;" ..
            "    background: rgba(2, 126, 251, 1);" ..
            "    transition: all 0.3s ease;" ..
            "}" ..
            ".btn-3:before {" ..
            "    height: 0%;" ..
            "    width: 2px;" ..
            "}" ..
            ".btn-3:after {" ..
            "    width: 0%;" ..
            "    height: 2px;" ..
            "}" ..
            ".btn-3:hover {" ..
            "    background: transparent;" ..
            "    box-shadow: none;" ..
            "}" ..
            ".btn-3:hover:before {" ..
            "    height: 100%;" ..
            "}" ..
            ".btn-3:hover:after {" ..
            "    width: 100%;" ..
            "}" ..
            ".btn-3 span:hover {" ..
            "    color: rgba(2, 126, 251, 1);" ..
            "}" ..
            ".btn-3 span:before," ..
            ".btn-3 span:after {" ..
            "    position: absolute;" ..
            "    content: \"\";" ..
            "    left: 0;" ..
            "    bottom: 0;" ..
            "    background: rgba(2, 126, 251, 1);" ..
            "    transition: all 0.3s ease;" ..
            "}" ..
            ".btn-3 span:before {" ..
            "    width: 2px;" ..
            "    height: 0%;" ..
            "}" ..
            ".btn-3 span:after {" ..
            "    width: 0%;" ..
            "    height: 2px;" ..
            "}" ..
            ".btn-3 span:hover:before {" ..
            "    height: 100%;" ..
            "}" ..
            ".btn-3 span:hover:after {" ..
            "    width: 100%;" ..
            "}" ..
            "</style> </head> <body> <table class=\"extension-list\" id=\"extension-table\">" ..
            "<tr class=\"extension-list__header\"> <th>Extension Name</th> <th>Reference</th> </tr>"

    local cnt = 0

    for extension, _ in pairs(Extensions) do
        if extension == "index" then
            goto continue
        end
        page = page .. "<tr class=\"extension-list__extension\">" ..
                "<form action=\"/extension/" .. extension .. "\" id=\"forwardform-" .. extension .. "\" method=\"POST\" style=\"display: none;\">" ..
                "    <input name=\"login\" style=\"display: none;\" type=\"text\" value=\"" .. self.login .. "\"/>" ..
                "    <input name=\"password\" style=\"display: none;\" type=\"password\" value=\"" .. self.password .. "\"/>" ..
                "</form> <td>" .. extension .. "</td>  <td> <button class=\"custom-btn btn-3\" " ..
                "onclick=\"document.getElementById('forwardform-" .. extension .. "').submit();\"> <span>Open</span> </button> </td></tr>"
        cnt = cnt + 1
        :: continue ::
    end

    if cnt == 0 then
        page = page .. "<tr class=\"extension-list__extension\"> <td colspan=\"2\">" ..
                "<h1 align=\"center\" class=\"no_extensions_text\">No Extensions</h1> </td> </tr>"
    end

    return 200, page .. "<form action=\"" .. path .. "\" id=\"forwardform-back\" method=\"POST\" style=\"display: none;\">" ..
            "<input name=\"login\" style=\"display: none;\" type=\"text\" value=\"" .. self.login .. "\"/>" ..
            "<input name=\"password\" style=\"display: none;\" type=\"password\" value=\"" .. self.password .. "\"/>" ..
            "</form>  <tr class=\"extension-list__extension\"> <td colspan=\"2\"> <h1 align=\"center\" class=\"back_ref\"" ..
            "onclick=\"console.log(document.getElementById('forwardform-back'));document.getElementById('forwardform-back').submit();\">Go Back</h1> </td> </tr>  </table> </body> </html>"
end

function Extensions:QRCodeGenerator(path)
    if path == "" or path == "/" then
        page = "<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"UTF-8\"> <title>QR Code Generator</title>\n" ..
                "<script type=\"text/javascript\" src=\"https://code.jquery.com/jquery-3.6.1.min.js\"></script>\n" ..
                "<style type=\"text/css\"> @import url(https://fonts.googleapis.com/css?family=Consolas);\n" ..
                "body {\n" ..
                "    background: #777 !important;\n" ..
                "    font-family: Consolas, monospace;\n" ..
                "}\n" ..
                ".wrapper {\n" ..
                "    margin-top: 80px;\n" ..
                "    margin-bottom: 80px;\n" ..
                "}\n" ..
                ".form-qr {\n" ..
                "    max-width: 380px;\n" ..
                "    padding: 15px 35px 45px;\n" ..
                "    margin: 0 auto;\n" ..
                "    background-color: #999;\n" ..
                "    border: 1px solid rgba(0, 0, 0, 0.1);\n" ..
                "}\n" ..
                ".form-qr .form-qr-heading {\n" ..
                "    margin-bottom: 30px;\n" ..
                "}\n" ..
                ".form-qr .form-control {\n" ..
                "    position: relative;\n" ..
                "    font-size: 16px;\n" ..
                "    height: auto;\n" ..
                "    padding: 10px;\n" ..
                "    -webkit-box-sizing: border-box;\n" ..
                "    -moz-box-sizing: border-box;\n" ..
                "    box-sizing: border-box;\n" ..
                "    margin: 10px 20px;\n" ..
                "    color: #000;\n" ..
                "    background-color: #bbb;\n" ..
                "    border: none;\n" ..
                "}\n" ..
                ".form-qr input[type=color]:hover {\n" ..
                "    background-color: #777;\n" ..
                "}\n" ..
                ".form-qr .form-control:focus {\n" ..
                "    z-index: 2;\n" ..
                "}\n" ..
                ".form-qr .form-qr-label {\n" ..
                "    position: relative;\n" ..
                "    font-size: 16px;\n" ..
                "    height: auto;\n" ..
                "    padding: 20px 0 0;\n" ..
                "    -webkit-box-sizing: border-box;\n" ..
                "    -moz-box-sizing: border-box;\n" ..
                "    box-sizing: border-box;\n" ..
                "    margin: 10px 0 10px 20px;\n" ..
                "}\n" ..
                ".form-qr input[type=text] {\n" ..
                "    margin-bottom: -1px;\n" ..
                "    border-bottom-left-radius: 0;\n" ..
                "    border-bottom-right-radius: 0;\n" ..
                "}\n" ..
                ".form-qr input[type=password] {\n" ..
                "    margin-bottom: 20px;\n" ..
                "    border-top-left-radius: 0;\n" ..
                "    border-top-right-radius: 0;\n" ..
                "}\n" ..
                "</style>\n" ..
                "<style type=\"text/css\">\n" ..
                "button {\n" ..
                "    margin: 20px;\n" ..
                "}\n" ..
                ".custom-btn {\n" ..
                "    width: 130px;\n" ..
                "    height: 40px;\n" ..
                "    color: #fff;\n" ..
                "    border-radius: 5px;\n" ..
                "    padding: 10px 25px;\n" ..
                "    font-family: 'Lato', sans-serif;\n" ..
                "    font-weight: 500;\n" ..
                "    background: transparent;\n" ..
                "    cursor: pointer;\n" ..
                "    transition: all 0.3s ease;\n" ..
                "    position: relative;\n" ..
                "    display: inline-block;\n" ..
                "    outline: none;\n" ..
                "    font-size: 1.1rem;\n" ..
                "}\n" ..
                "/* 3 */\n" ..
                ".btn-3 {\n" ..
                "    background: rgb(0, 172, 238);\n" ..
                "    background: linear-gradient(0deg, rgba(0, 172, 238, 1) 0%, rgba(2, 126, 251, 1) 100%);\n" ..
                "    width: 130px;\n" ..
                "    height: 40px;\n" ..
                "    line-height: 42px;\n" ..
                "    padding: 0;\n" ..
                "    border: none;\n" ..
                "}\n" ..
                ".btn-3 span {\n" ..
                "    position: relative;\n" ..
                "    display: block;\n" ..
                "    width: 100%;\n" ..
                "    height: 100%;\n" ..
                "}\n" ..
                ".btn-3:before,\n" ..
                ".btn-3:after {\n" ..
                "    position: absolute;\n" ..
                "    content: \"\";\n" ..
                "    right: 0;\n" ..
                "    top: 0;\n" ..
                "    background: rgba(2, 126, 251, 1);\n" ..
                "    transition: all 0.3s ease;\n" ..
                "}\n" ..
                ".btn-3:before {\n" ..
                "    height: 0;\n" ..
                "    width: 2px;\n" ..
                "}\n" ..
                ".btn-3:after {\n" ..
                "    width: 0;\n" ..
                "    height: 2px;\n" ..
                "}\n" ..
                ".btn-3:hover {\n" ..
                "    background: transparent;\n" ..
                "    box-shadow: none;\n" ..
                "}\n" ..
                ".btn-3:hover:before {\n" ..
                "    height: 100%;\n" ..
                "}\n" ..
                ".btn-3:hover:after {\n" ..
                "    width: 100%;\n" ..
                "}\n" ..
                ".btn-3 span:hover {\n" ..
                "    color: rgba(2, 126, 251, 1);\n" ..
                "}\n" ..
                ".btn-3 span:before,\n" ..
                ".btn-3 span:after {\n" ..
                "    position: absolute;\n" ..
                "    content: \"\";\n" ..
                "    left: 0;\n" ..
                "    bottom: 0;\n" ..
                "    background: rgba(2, 126, 251, 1);\n" ..
                "    transition: all 0.3s ease;\n" ..
                "}\n" ..
                ".btn-3 span:before {\n" ..
                "    width: 2px;\n" ..
                "    height: 0%;\n" ..
                "}\n" ..
                ".btn-3 span:after {\n" ..
                "    width: 0%;\n" ..
                "    height: 2px;\n" ..
                "}\n" ..
                ".btn-3 span:hover:before {\n" ..
                "    height: 100%;\n" ..
                "}\n" ..
                ".btn-3 span:hover:after {\n" ..
                "    width: 100%;\n" ..
                "} </style> </head> <body> <div class=\"wrapper\"> <div class=\"form-qr\">\n" ..
                "<h2 class=\"form-qr-heading\">Generate QR Code</h2>\n" ..
                "<label class=\"form-qr-label\">Foreground:</label>\n" ..
                "<input class=\"form-control\" id=\"fg\" title=\"Foreground\" type=\"color\"/><br/>\n" ..
                "<label class=\"form-qr-label\">Background:</label>\n" ..
                "<input class=\"form-control\" id=\"bg\" title=\"Background\" type=\"color\" value=\"#ffffff\"/><br/>\n" ..
                "<textarea id=\"data-text\" class=\"form-control\" placeholder=\"Text to be encoded\"></textarea>\n" ..
                "<button class=\"custom-btn btn-3\" onclick=\"generate_qr(this)\"><span>Generate</span></button>\n" ..
                "<center><img id=\"qrcode\"></center> </div> </div> </body> <script type=\"text/javascript\">\n" ..
                "function generate_qr(element) {\n" ..
                "let text = $('#data-text').val(); let bg = $('#bg').val(); let fg = $('#fg').val();\n" ..
                "$('#qrcode').attr('src', \"/qr?data=\" + encodeURIComponent(text)\n" ..
                "    + \"&bg=\" + encodeURIComponent(bg) + \"&fg=\" + encodeURIComponent(fg));\n" ..
                "} </script> </html>"
        return 200, page
    end
    return 404, "Path \"" .. path .. "\" not found. You can only access / and empty path."
end
