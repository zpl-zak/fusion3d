#ifndef BITMAP_H
#define BITMAP_H

#include <string>

class Bitmap
{
    public:
        Bitmap(std::string fileName);
        Bitmap(int width, int height);
        virtual ~Bitmap();

        Bitmap* FlipX();
        Bitmap* FlipY();

        inline int GetWidth() { return m_width; }
        inline int GetHeight() { return m_height; }
        inline unsigned int* GetPixels() const { return m_pixels; }
        inline unsigned int  GetPixel(int x, int y) { return m_pixels[x + y * m_width]; }
    protected:
    private:
        int m_width;
        int m_height;
        int m_bpp;
        unsigned int* m_pixels;
};

#endif // BITMAP_H
