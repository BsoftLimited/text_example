#ifndef UTILS_H
	#define UTILS_H

namespace grafx{
    struct Vector2{ int x; int y; };
    struct Size{ float width; float height; };

    struct Character {
        unsigned int TextureID; // ID handle of the glyph texture
        Vector2   Size;      // Size of glyph
        Vector2   Bearing;   // Offset from baseline to left/top of glyph
        unsigned int Advance;   // Horizontal offset to advance to next glyph
    };

    struct Color{
        float red; float green; float blue; float alpha;
    };

    void OrthogonalMatrix(float matrix[], float top, float bottom, float left, float right){
        for(int i = 0; i < 16; i++){ matrix[i] = 0; }
        matrix[0] = 2.0f/(right - left);      matrix[12] =  -(right + left) / (right - left);
        matrix[5] = 2.0f/(top - bottom);      matrix[13] = -(top + bottom) / (top - bottom);
        matrix[10] = -2.0f/(100.0f - 0.1f);
        matrix[15] = 1.0f;
    }
}

#endif