/*MIT License

Copyright (c) 2018 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#ifndef APE_OGREUTILITIES_H
#define APE_OGREUTILITIES_H

#include <sstream>
#include <iostream>
#include <iomanip>
#include "apeOgreRenderPlugin.h"

namespace ape
{
	void SaveVoidBufferToImage(const std::wstring &filename, const void* buffer, int width, int height)
	{
		// Populate the bitmap info header.
		BITMAPINFOHEADER info;
		info.biSize = sizeof(BITMAPINFOHEADER);
		info.biWidth = width;
		info.biHeight = -height;  // minus means top-down bitmap
		info.biPlanes = 1;
		info.biBitCount = 32;
		info.biCompression = BI_RGB;  // no compression
		info.biSizeImage = 0;
		info.biXPelsPerMeter = 1;
		info.biYPelsPerMeter = 1;
		info.biClrUsed = 0;
		info.biClrImportant = 0;

		// Create the bitmap and retrieve the bit buffer.
		HDC screen_dc = GetDC(NULL);
		void* bits = NULL;
		HBITMAP bitmap = CreateDIBSection(screen_dc, reinterpret_cast<BITMAPINFO*>(&info), DIB_RGB_COLORS, &bits, NULL, 0);
		ReleaseDC(NULL, screen_dc);

		// Read the image into the bit buffer.
		if (bitmap == NULL)
			return;

		memcpy(bits, buffer, width * height * 4);

		// Populate the bitmap file header.
		BITMAPFILEHEADER file;
		file.bfType = 0x4d42;
		file.bfSize = sizeof(BITMAPFILEHEADER);
		file.bfReserved1 = 0;
		file.bfReserved2 = 0;
		file.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		// Write the bitmap to file.
		HANDLE file_handle = CreateFileW(filename.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (file_handle != INVALID_HANDLE_VALUE)
		{
			DWORD bytes_written = 0;
			WriteFile(file_handle, &file, sizeof(file), &bytes_written, 0);
			WriteFile(file_handle, &info, sizeof(info), &bytes_written, 0);
			WriteFile(file_handle, bits, width * height * 4, &bytes_written, 0);
			CloseHandle(file_handle);
		}

		DeleteObject(bitmap);
	}
}

#endif
