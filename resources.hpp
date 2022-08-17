//
// Created by imper on 8/17/22.
//

#ifndef FILESTORAGE_RESOURCES_HPP
#define FILESTORAGE_RESOURCES_HPP


static constexpr const char* error_404_html = R"(
<!doctype html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport"
          content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>ERROR 404</title>
    <style type="text/css">
        @import url(https://fonts.googleapis.com/css?family=Open+Sans:400,700);

        body {
            font-family: 'Open Sans';
            padding: 0;
            margin: 0;
        }

        a,
        a:visited {
            color: #fff;
            outline: none;
            text-decoration: none;
        }

        a:hover,
        a:focus,
        a:visited:hover {
            color: #fff;
            text-decoration: none;
        }

        * {
            paading: 0;
            margin: 0;
        }

        #oopss {
            background: #222;
            text-align: center;
            margin-bottom: 50px;
            font-weight: 400;
            font-size: 20px;
            position: fixed;
            width: 100%;
            height: 100%;
            line-height: 1.5em;
            z-index: 9999;
            left: 0px;
        }

        #error-text {
            top: 30%;
            position: relative;
            font-size: 40px;
            color: #eee;
        }

        #error-text a {
            color: #eeeaaa;
        }

        #error-text a:hover {
            color: #fff;
        }

        #error-text p {
            color: #eee;
            margin: 70px 0 0 0;
        }

        #error-text i {
            margin-left: 10px;
        }

        #error-text p.hmpg {
            margin: 40px 0 0 0;
            font-size: 30px;
        }

        #error-text span {
            position: relative;
            background: #ef4824;
            color: #fff;
            font-size: 300%;
            padding: 0 20px;
            border-radius: 5px;
            font-weight: bolder;
            transition: all .5s;
            cursor: pointer;
            margin: 0 0 40px 0;
        }

        #error-text span:hover {
            background: #d7401f;
            color: #fff;
            -webkit-animation: jelly .5s;
            -moz-animation: jelly .5s;
            -ms-animation: jelly .5s;
            -o-animation: jelly .5s;
            animation: jelly .5s;
        }

        #error-text span:after {
            top: 100%;
            left: 50%;
            border: solid transparent;
            content: '';
            height: 0;
            width: 0;
            position: absolute;
            pointer-events: none;
            border-color: rgba(136, 183, 213, 0);
            border-top-color: #ef4824;
            border-width: 7px;
            margin-left: -7px;
        }

        @-webkit-keyframes jelly {
            from, to {
                -webkit-transform: scale(1, 1);
                transform: scale(1, 1);
            }
            25% {
                -webkit-transform: scale(.9, 1.1);
                transform: scale(.9, 1.1);
            }
            50% {
                -webkit-transform: scale(1.1, .9);
                transform: scale(1.1, .9);
            }
            75% {
                -webkit-transform: scale(.95, 1.05);
                transform: scale(.95, 1.05);
            }
        }

        @keyframes jelly {
            from, to {
                -webkit-transform: scale(1, 1);
                transform: scale(1, 1);
            }
            25% {
                -webkit-transform: scale(.9, 1.1);
                transform: scale(.9, 1.1);
            }
            50% {
                -webkit-transform: scale(1.1, .9);
                transform: scale(1.1, .9);
            }
            75% {
                -webkit-transform: scale(.95, 1.05);
                transform: scale(.95, 1.05);
            }
        }

        /* CSS Error Page Responsive */

        @media only screen and (max-width: 640px) {
            #error-text span {
                font-size: 200%;
            }

            #error-text a:hover {
                color: #fff;
            }
        }

        .back:active {
            -webkit-transform: scale(0.95);
            -moz-transform: scale(0.95);
            transform: scale(0.95);
            background: #f53b3b;
            color: #fff;
        }

        .back:hover {
            background: #4c4c4c;
            color: #fff;
        }

        .back {
            text-decoration: none;
            background: #5b5a5a;
            color: #fff;
            padding: 10px 20px;
            font-size: 20px;
            font-weight: 700;
            line-height: normal;
            text-transform: uppercase;
            border-radius: 3px;
            -webkit-transform: scale(1);
            -moz-transform: scale(1);
            transform: scale(1);
            transition: all 0.5s ease-out;
        }
    </style>
</head>
<body>
<div id='oopss'>
    <div id='error-text'>
        <span>404</span>
        <p>PAGE NOT FOUND</p>
        <p></p>
        <p class='hmpg'><a href='/' class="back">Back To Home</a>, but you also can go <a href='/' class="back">Back To Home</a></p>
    </div>
</div>
</body>
</html>
)";

static constexpr const char* login_page_html = R"(
<!doctype html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport"
          content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Welcome to the storage</title>
    <style type="text/css">
        @import url(https://fonts.googleapis.com/css?family=Inconsolata);

        .login-page {
            width: 360px;
            padding: 8% 0 0;
            margin: auto;
        }

        .form {
            position: relative;
            z-index: 1;
            background: #FFFFFF;
            max-width: 360px;
            margin: 0 auto 100px;
            padding: 45px;
            text-align: center;
            box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.2), 0 5px 5px 0 rgba(0, 0, 0, 0.24);
        }

        .form input {
            font-family: "Inconsolata";
            outline: 0;
            background: #f2f2f2;
            width: 100%;
            border: 0;
            margin: 0 0 15px;
            padding: 15px;
            box-sizing: border-box;
            font-size: 14px;
        }

        .form input[type=submit] {
            font-family: "Inconsolata";
            font-weight: 700;
            text-transform: uppercase;
            outline: 0;
            background: #4CAF50;
            width: 100%;
            border: 0;
            padding: 15px;
            color: #FFFFFF;
            font-size: 14px;
            -webkit-transition: all 0.3 ease;
            transition: all 0.3 ease;
            cursor: pointer;
        }

        .form input[type=submit]:hover, .form input[type=submit]:active, .form input[type=submit]:focus {
            background: #43A049;
        }

        .form .message {
            margin: 15px 0 0;
            color: #b3b3b3;
            font-size: 12px;
        }

        .form .message a {
            color: #4CAF50;
            text-decoration: none;
        }

        .form .register-form {
            display: none;
        }

        .container {
            position: relative;
            z-index: 1;
            max-width: 300px;
            margin: 0 auto;
        }

        .container:before, .container:after {
            content: "";
            display: block;
            clear: both;
        }

        .container .info {
            margin: 50px auto;
            text-align: center;
        }

        .container .info h1 {
            margin: 0 0 15px;
            padding: 0;
            font-size: 36px;
            font-weight: 300;
            color: #1a1a1a;
        }

        .container .info span {
            color: #4d4d4d;
            font-size: 12px;
        }

        .container .info span a {
            color: #000000;
            text-decoration: none;
        }

        .container .info span .fa {
            color: #EF3B3A;
        }

        body {
            background: #6b991f; /* fallback for old browsers */
            background: rgb(107, 153, 31);
            background: linear-gradient(90deg, rgba(121, 174, 91, 1) 10%, rgba(78, 124, 2, 1) 75%);
            font-family: "Inconsolata";
            -webkit-font-smoothing: antialiased;
            -moz-osx-font-smoothing: grayscale;
            margin: 0;
            height: 100%;
            min-height: 100%;
        }

        html {
            margin: 0px;
            height: 100%;
            width: 100%;
        }
    </style>
    <script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/jquery/2.1.3/jquery.min.js"></script>
</head>
<body>
<div class="login-page">
    <div class="form">
        <form class="register-form" method="POST" action="/register">
            <input type="text" placeholder="login" name="login"/>
            <input type="password" placeholder="password" name="password"/>
            <input type="submit" value="register"/>
            <p class="message">Already registered? <a href="#">Sign In</a></p>
        </form>
        <form class="login-form" method="POST" action="/login" autocomplete="on">
            <input type="text" placeholder="login" name="login"/>
            <input type="password" placeholder="password" name="password"/>
            <input type="submit" value="login"/>
            <p class="message">Not registered? <a href='#'>Create an account</a></p>
        </form>
    </div>
</div>
</body>
<script type="text/javascript">
    $('.message a').click(function () {
        $('form').animate({height: "toggle", opacity: "toggle"}, "normal");
    });
</script>
</html>
)";

static constexpr const char* invalid_credentials_page_html = R"(
<!doctype html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport"
        content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0">
  <meta http-equiv="X-UA-Compatible" content="ie=edge">
  <title>Invalid Credentials</title>
  <style type="text/css">
    @import url(https://fonts.googleapis.com/css?family=Inconsolata);

    .login-page {
      width: 360px;
      padding: 8% 0 0;
      margin: auto;
    }

    .form {
      position: relative;
      z-index: 1;
      background: #FFFFFF;
      max-width: 360px;
      margin: 0 auto 100px;
      padding: 45px;
      text-align: center;
      box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.2), 0 5px 5px 0 rgba(0, 0, 0, 0.24);
    }

    .form input {
      font-family: "Inconsolata";
      outline: 0;
      background: #f2f2f2;
      width: 100%;
      border: 0;
      margin: 0 0 15px;
      padding: 15px;
      box-sizing: border-box;
      font-size: 14px;
    }

    .form input[type=submit] {
      font-family: "Inconsolata";
      font-weight: 700;
      text-transform: uppercase;
      outline: 0;
      background: #4CAF50;
      width: 100%;
      border: 0;
      padding: 15px;
      color: #FFFFFF;
      font-size: 14px;
      -webkit-transition: all 0.3 ease;
      transition: all 0.3 ease;
      cursor: pointer;
    }

    .form input[type=submit]:hover, .form input[type=submit]:active, .form input[type=submit]:focus {
      background: #43A049;
    }

    .form .error {
      margin: 15px 15px 15px 15px;
      color: #b30000;
      font-size: 16px;
    }

    .form .message {
      margin: 15px 0 0;
      color: #b3b3b3;
      font-size: 12px;
    }

    .form .message a {
      color: #4CAF50;
      text-decoration: none;
    }

    .form .register-form {
      display: none;
    }

    .container {
      position: relative;
      z-index: 1;
      max-width: 300px;
      margin: 0 auto;
    }

    .container:before, .container:after {
      content: "";
      display: block;
      clear: both;
    }

    .container .info {
      margin: 50px auto;
      text-align: center;
    }

    .container .info h1 {
      margin: 0 0 15px;
      padding: 0;
      font-size: 36px;
      font-weight: 300;
      color: #1a1a1a;
    }

    .container .info span {
      color: #4d4d4d;
      font-size: 12px;
    }

    .container .info span a {
      color: #000000;
      text-decoration: none;
    }

    .container .info span .fa {
      color: #EF3B3A;
    }

    body {
      background: #6b991f; /* fallback for old browsers */
      background: rgb(107, 153, 31);
      background: linear-gradient(90deg, rgba(121, 174, 91, 1) 10%, rgba(78, 124, 2, 1) 75%);
      font-family: "Inconsolata";
      -webkit-font-smoothing: antialiased;
      -moz-osx-font-smoothing: grayscale;
      margin: 0;
      height: 100%;
      min-height: 100%;
    }

    html {
      margin: 0px;
      height: 100%;
      width: 100%;
    }
  </style>
  <script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/jquery/2.1.3/jquery.min.js"></script>
</head>
<body>
<div class="login-page">
  <div class="form">
	<p class="error">%s</p>
    <form class="register-form" method="POST" action="/register">
      <input type="text" placeholder="login" name="login" value="%s"/>
      <input type="password" placeholder="password" name="password" value="%s"/>
      <input type="submit" value="register"/>
      <p class="message">Already registered? <a href="#">Sign In</a></p>
    </form>
    <form class="login-form" method="POST" action="/login" autocomplete="on">
      <input type="text" placeholder="login" name="login" value="%s"/>
      <input type="password" placeholder="password" name="password" value="%s"/>
      <input type="submit" value="login"/>
      <p class="message">Not registered? <a href='#'>Create an account</a></p>
    </form>
  </div>
</div>
</body>
<script type="text/javascript">
  $('.message a').click(function () {
    $('form').animate({height: "toggle", opacity: "toggle"}, "normal");
  });
</script>
</html>
)";

static constexpr const char* explorer_page_html = R"(
<!doctype html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport"
          content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Storage Explorer</title>
    <style type="text/css">
        @charset "utf-8";


        /*-------------------------
            Simple reset
        --------------------------*/

        * {
            margin: 0;
            padding: 0;
        }


        /*-------------------------
            Demo page
        -------------------------*/

        body {
            background-color: #0e3858;
            font: 14px normal Arial, Helvetica, sans-serif;
            z-index: -4;
        }


        /*-------------------------
            File manager
        -------------------------*/


        .filemanager {
            width: 95%;
            max-width: 1340px;
            position: relative;
            margin: 100px auto 50px;
        }

        @media all and (max-width: 965px) {
            .filemanager {
                margin: 30px auto 0;
                padding: 1px;
            }
        }


        /*-------------------------
            Breadcrumps
        -------------------------*/


        .filemanager .breadcrumbs {
            color: #ffffff;
            margin-left: 20px;
            font-family: Inconsolata, monospace;
            font-size: 24px;
            font-weight: 700;
            line-height: 35px;
        }

        .filemanager .breadcrumbs a:link, .breadcrumbs a:visited {
            color: #ffffff;
            text-decoration: none;
        }

        .filemanager .breadcrumbs a:hover {
            color: #00ffff;
            text-decoration: underline;
        }

        .filemanager .breadcrumbs .arrow {
            color: #6a6a72;
            font-size: 24px;
            font-weight: 700;
            line-height: 20px;
        }

        ::-webkit-input-placeholder { /* WebKit browsers */
            color: #4d535e;
        }

        :-moz-placeholder { /* Mozilla Firefox 4 to 18 */
            color: #4d535e;
            opacity: 1;
        }

        ::-moz-placeholder { /* Mozilla Firefox 19+ */
            color: #4d535e;
            opacity: 1;
        }

        :-ms-input-placeholder { /* Internet Explorer 10+ */
            color: #4d535e;
        }


        /*-------------------------
            Content area
        -------------------------*/

        .filemanager .data {
            margin-top: 60px;
            z-index: -3;
        }

        .filemanager .data.animated {
            -webkit-animation: showSlowlyElement 700ms; /* Chrome, Safari, Opera */
            animation: showSlowlyElement 700ms; /* Standard syntax */
        }

        .filemanager .data li {
            border-radius: 3px;
            background-color: #4a7da5;
            width: 307px;
            height: 118px;
            list-style-type: none;
            margin: 10px;
            display: inline-block;
            position: relative;
            overflow: hidden;
            padding: 0.3em;
            z-index: 1;
            cursor: pointer;
            box-sizing: border-box;
            transition: 0.3s background-color;
        }

        .filemanager .data li:hover {
            background-color: #42424E;

        }

        .filemanager .data li a {
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
        }

        .filemanager .data li .name {
            color: #ffffff;
            font-size: 15px;
            font-weight: 700;
            line-height: 20px;
            width: 150px;
            white-space: nowrap;
            display: inline-block;
            position: absolute;
            overflow: hidden;
            text-overflow: ellipsis;
            top: 40px;
        }

        .filemanager .data li .details {
            color: #b6c1c9;
            font-size: 13px;
            font-weight: 400;
            width: 55px;
            height: 10px;
            top: 64px;
            white-space: nowrap;
            position: absolute;
            display: inline-block;
        }

        .filemanager .nothingfound {
            background-color: #072743;
            width: 23em;
            height: 21em;
            margin: 0 auto;
            display: none;
            font-family: Arial;
            -webkit-animation: showSlowlyElement 700ms; /* Chrome, Safari, Opera */
            animation: showSlowlyElement 700ms; /* Standard syntax */
        }

        .filemanager .nothingfound .nofiles {
            margin: 30px auto;
            top: 3em;
            border-radius: 50%;
            position: relative;
            background-color: #d72f6e;
            width: 11em;
            height: 11em;
            line-height: 11.4em;
        }

        .filemanager .nothingfound .nofiles:after {
            content: '×';
            position: absolute;
            color: #ffffff;
            font-size: 14em;
            margin-right: 0.092em;
            right: 0;
        }

        .filemanager .nothingfound span {
            margin: 0 auto auto 6.8em;
            color: #ffffff;
            font-size: 16px;
            font-weight: 700;
            line-height: 20px;
            height: 13px;
            position: relative;
            top: 2em;
        }

        @media all and (max-width: 965px) {

            .filemanager .data li {
                width: 100%;
                margin: 5px 0;
            }

        }

        /* Chrome, Safari, Opera */
        @-webkit-keyframes showSlowlyElement {
            100% {
                transform: scale(1);
                opacity: 1;
            }
            0% {
                transform: scale(1.2);
                opacity: 0;
            }
        }

        /* Standard syntax */
        @keyframes showSlowlyElement {
            100% {
                transform: scale(1);
                opacity: 1;
            }
            0% {
                transform: scale(1.2);
                opacity: 0;
            }
        }


        /*-------------------------
                Icons
        -------------------------*/

        .icon {
            font-size: 23px;
        }

        .icon.folder {
            display: inline-block;
            margin: 1em;
            background-color: transparent;
            overflow: hidden;
        }

        .icon.folder:before {
            content: '';
            float: left;
            background-color: #7ba1ad;

            width: 1.5em;
            height: 0.45em;

            margin-left: 0.07em;
            margin-bottom: -0.07em;

            border-top-left-radius: 0.1em;
            border-top-right-radius: 0.1em;

            box-shadow: 1.25em 0.25em 0 0em #7ba1ad;
        }

        .icon.folder:after {
            content: '';
            float: left;
            clear: left;

            background-color: #a0d4e4;
            width: 3em;
            height: 2.25em;

            border-radius: 0.1em;
        }

        .icon.folder.full:before {
            height: 0.55em;
        }

        .icon.folder.full:after {
            height: 2.15em;
            box-shadow: 0 -0.12em 0 0 #ffffff;
        }

        .icon.file {
            width: 2.5em;
            height: 3em;
            line-height: 3em;
            text-align: center;
            border-radius: 0.25em;
            color: #FFF;
            display: inline-block;
            margin: 0.9em 1.2em 0.8em 1.3em;
            position: relative;
            overflow: hidden;
            box-shadow: 1.74em -2.1em 0 0 #A4A7AC inset;
        }

        .icon.file:first-line {
            font-size: 13px;
            font-weight: 700;
        }

        .icon.file:after {
            content: '';
            position: absolute;
            z-index: -1;
            border-width: 0;
            border-bottom: 2.6em solid #DADDE1;
            border-right: 2.22em solid rgba(0, 0, 0, 0);
            top: -34.5px;
            right: -4px;
        }

        .icon.file.f-avi,
        .icon.file.f-flv,
        .icon.file.f-mkv,
        .icon.file.f-mov,
        .icon.file.f-mpeg,
        .icon.file.f-mpg,
        .icon.file.f-mp4,
        .icon.file.f-m4v,
        .icon.file.f-wmv {
            box-shadow: 1.74em -2.1em 0 0 #7e70ee inset;
        }

        .icon.file.f-avi:after,
        .icon.file.f-flv:after,
        .icon.file.f-mkv:after,
        .icon.file.f-mov:after,
        .icon.file.f-mpeg:after,
        .icon.file.f-mpg:after,
        .icon.file.f-mp4:after,
        .icon.file.f-m4v:after,
        .icon.file.f-wmv:after {
            border-bottom-color: #5649c1;
        }

        .icon.file.f-mp2,
        .icon.file.f-mp3,
        .icon.file.f-m3u,
        .icon.file.f-wma,
        .icon.file.f-xls,
        .icon.file.f-xlsx {
            box-shadow: 1.74em -2.1em 0 0 #5bab6e inset;
        }

        .icon.file.f-mp2:after,
        .icon.file.f-mp3:after,
        .icon.file.f-m3u:after,
        .icon.file.f-wma:after,
        .icon.file.f-xls:after,
        .icon.file.f-xlsx:after {
            border-bottom-color: #448353;
        }

        .icon.file.f-doc,
        .icon.file.f-docx,
        .icon.file.f-psd {
            box-shadow: 1.74em -2.1em 0 0 #03689b inset;
        }

        .icon.file.f-doc:after,
        .icon.file.f-docx:after,
        .icon.file.f-psd:after {
            border-bottom-color: #2980b9;
        }

        .icon.file.f-gif,
        .icon.file.f-jpg,
        .icon.file.f-jpeg,
        .icon.file.f-pdf,
        .icon.file.f-png {
            box-shadow: 1.74em -2.1em 0 0 #e15955 inset;
        }

        .icon.file.f-gif:after,
        .icon.file.f-jpg:after,
        .icon.file.f-jpeg:after,
        .icon.file.f-pdf:after,
        .icon.file.f-png:after {
            border-bottom-color: #c6393f;
        }

        .icon.file.f-deb,
        .icon.file.f-dmg,
        .icon.file.f-gz,
        .icon.file.f-rar,
        .icon.file.f-zip,
        .icon.file.f-7z {
            box-shadow: 1.74em -2.1em 0 0 #867c75 inset;
        }

        .icon.file.f-deb:after,
        .icon.file.f-dmg:after,
        .icon.file.f-gz:after,
        .icon.file.f-rar:after,
        .icon.file.f-zip:after,
        .icon.file.f-7z:after {
            border-bottom-color: #685f58;
        }

        .icon.file.f-html,
        .icon.file.f-rtf,
        .icon.file.f-xml,
        .icon.file.f-xhtml {
            box-shadow: 1.74em -2.1em 0 0 #a94bb7 inset;
        }

        .icon.file.f-html:after,
        .icon.file.f-rtf:after,
        .icon.file.f-xml:after,
        .icon.file.f-xhtml:after {
            border-bottom-color: #d65de8;
        }

        .icon.file.f-js {
            box-shadow: 1.74em -2.1em 0 0 #d0c54d inset;
        }

        .icon.file.f-js:after {
            border-bottom-color: #a69f4e;
        }

        .icon.file.f-css,
        .icon.file.f-saas,
        .icon.file.f-scss {
            box-shadow: 1.74em -2.1em 0 0 #44afa6 inset;
        }

        .icon.file.f-css:after,
        .icon.file.f-saas:after,
        .icon.file.f-scss:after {
            border-bottom-color: #30837c;
        }


        /*----------------------------
            The Demo Footer
        -----------------------------*/


        footer {

            width: 770px;
            font: normal 16px Arial, Helvetica, sans-serif;
            padding: 15px 35px;
            position: fixed;
            bottom: 0;
            left: 50%;
            margin-left: -420px;

            background-color: #1f1f1f;
            background-image: linear-gradient(to bottom, #1f1f1f, #101010);

            border-radius: 2px 2px 0 0;
            box-shadow: 0 -1px 4px rgba(0, 0, 0, 0.4);
            z-index: 1;
        }

        footer a.tz {
            font-weight: normal;
            font-size: 16px !important;
            text-decoration: none !important;
            display: block;
            margin-right: 300px;
            text-overflow: ellipsis;
            white-space: nowrap;
            color: #bfbfbf !important;
            z-index: 1;
        }

        footer a.tz:before {
            content: '';
            background: url('http://cdn.tutorialzine.com/misc/enhance/v2_footer_bg.png') no-repeat 0 -53px;
            width: 138px;
            height: 20px;
            display: inline-block;
            position: relative;
            bottom: -3px;
        }

        footer .close {
            position: absolute;
            cursor: pointer;
            width: 8px;
            height: 8px;
            background: url('http://cdn.tutorialzine.com/misc/enhance/v2_footer_bg.png') no-repeat 0 0px;
            top: 10px;
            right: 10px;
            z-index: 3;
        }

        footer #tzine-actions {
            position: absolute;
            top: 8px;
            width: 500px;
            right: 50%;
            margin-right: -650px;
            text-align: right;
            z-index: 2;
        }

        footer #tzine-actions iframe {
            display: inline-block;
            height: 21px;
            width: 95px;
            position: relative;
            float: left;
            margin-top: 11px;
        }

        @media (max-width: 1024px) {
            #bsaHolder, footer {
                display: none;
            }
        }

        .filemanager .operation {
            position: absolute;
            padding-right: 30px;
            cursor: pointer;
            right: 0;
            font-family: Inconsolata, monospace;
            font-size: 30px;
            font-weight: bold;
            color: #ffffff;
            display: block;
            width: 40px;
            height: 40px;
            text-decoration: none;
        }
    </style>
</head>
<body>
<div class="filemanager">
    <form method="POST" action="/explorer/%s" style="display: none;" id="forwardform%s">
        <input type="text" name="login" value="%s"/>
        <input type="password" name="password" value="%s"/>
    </form>

    <div class="breadcrumbs">
        <span class="folderName">
            <a onclick="document.getElementById('forwardform%s').submit();">ROOT/%s/</a>%s
        </span>
    </div>

    <form method="POST" action="/upload/%s" style="display: none;" id="forwardform-upload%s">
        <input type="text" name="login" value="%s"/>
        <input type="password" name="password" value="%s"/>
    </form>

    <a class="operation" onclick="document.getElementById('forwardform-upload%s').submit();">+</a>

    %s

</div>
</body>
</html>
)";

static constexpr const char* explorer_dir_html = R"(
        <form method="POST" action="/explorer/%s"  style="display: none;" id="forwardform%s">
	        <input type="text" name="login" value="%s"/>
	        <input type="password" name="password" value="%s"/>
	    </form>

	    <li class="folders">
	        <a title="EXPLORER/%s" class="folders" onclick="document.getElementById('forwardform%s').submit();">
	            <span class="icon folder full"></span>
	            <span class="name">%s</span>
	            <span class="details">%zu files, %zu folders</span>
	        </a>
	    </li>
)";

static constexpr const char* explorer_file_html = R"(
        <form method="POST" action="/explorer/%s"  style="display: none;" id="forwardform%s" target="_blank">
	        <input type="text" name="login" value="%s"/>
	        <input type="password" name="password" value="%s"/>
	    </form>

        <li class="files">
            <a title="EXPLORER/%s" class="files" onclick="document.getElementById('forwardform%s').submit();">
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


#endif //FILESTORAGE_RESOURCES_HPP
