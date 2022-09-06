// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.


#ifndef FILESTORAGE_GEN_QR_H
#define FILESTORAGE_GEN_QR_H

#define generate_qr_code(text) qrcodegen::QrCode::encodeText(text, qrcodegen::QrCode::Ecc::HIGH)

#include <string>

#include "qrcodegen.hpp"


// Returns a string of SVG code for an image depicting the given QR Code, with the given number
// of border modules. The string always uses Unix newlines (\n), regardless of the platform.
extern std::string To_svg_string(const qrcodegen::QrCode& qr, long long int border, const char* bg, const char* fg);


#endif //FILESTORAGE_GEN_QR_H
