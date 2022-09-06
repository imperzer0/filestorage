// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.


#include <climits>
#include <sstream>

#include "Gen_QR.h"
#include "mongoose.h"


// Returns a string of SVG code for an image depicting the given QR Code, with the given number
// of border modules. The string always uses Unix newlines (\n), regardless of the platform.
std::string To_svg_string(const qrcodegen::QrCode& qr, long long int border, const char* bg, const char* fg)
{
	if (border < 0)
	{
		#ifndef EXTERNAL_TEST
		MG_ERROR(("[QRCode] Border must be non-negative."));
		#endif
		return "";
	}
	
	if (border > INT_MAX / 2 || border * 2 > INT_MAX - qr.getSize())
	{
		#ifndef EXTERNAL_TEST
		MG_ERROR(("[QRCode] Border is too large."));
		#endif
		return "";
	}
	
	std::ostringstream sb;
	sb << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	      "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
	      R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1" viewBox="0 0 )";
	sb << (qr.getSize() + border * 2) << " " << (qr.getSize() + border * 2) << "\" stroke=\"none\">\n";
	sb << "\t<rect width=\"100%\" height=\"100%\" fill=\"" << bg << "\"/>\n"
	                                                                "\t<path d=\"";
	for (int y = 0; y < qr.getSize(); y++)
	{
		for (int x = 0; x < qr.getSize(); x++)
		{
			if (qr.getModule(x, y))
			{
				if (x != 0 || y != 0)
					sb << " ";
				sb << "M" << (x + border) << "," << (y + border) << "h1v1h-1z";
			}
		}
	}
	sb << "\" fill=\"" << fg << "\"/>\n"
	                            "</svg>\n";
	return sb.str();
}