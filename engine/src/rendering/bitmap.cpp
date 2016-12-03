#include "bitmap.h"
#include "../staticLibs/stb_image.h"
#include <iostream>
#include "../core/util.h"

Bitmap::Bitmap(std::string fileName)
{
	//ctor

	int x, y, bytesPerPixel;
	unsigned char* data = stbi_load((Util::ResourcePath() + "bitmaps/" + fileName).c_str(), &x, &y, &bytesPerPixel, 4);

	if (data == NULL)
	{
		std::cerr << "Unable to load bitmap: " << fileName << std::endl;
	}

	m_pixels = (unsigned int*)data;
	m_width = x;
	m_height = y;
	m_bpp = bytesPerPixel;
}

Bitmap::Bitmap(int width, int height)
	:
	m_width(width),
	m_height(height)
{
	//ctor

	m_pixels = new unsigned int[width * height];
}

Bitmap::~Bitmap()
{
	//dtor

	delete m_pixels;
}

Bitmap* Bitmap::FlipX()
{
	unsigned int* temp = new unsigned int[m_width * m_height * m_bpp];

	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			temp[i + j * m_width] = m_pixels[(m_width - i - 1) + j * m_width];
		}
	}

	delete m_pixels;
	m_pixels = temp;

	return this;
}

Bitmap* Bitmap::FlipY()
{
	unsigned int* temp = new unsigned int[m_width * m_height * m_bpp];

	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			temp[i + j * m_width] = m_pixels[i + (m_height - j - 1) * m_width];
		}
	}

	delete m_pixels;
	m_pixels = temp;

	return this;
}
