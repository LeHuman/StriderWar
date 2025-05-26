#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef void (*LineFunc)(int x0, int y0, int x1, int y1, int color);

class Graphics {
public:
    static void init();
    static void pixel(int x, int y, unsigned char color);
    static void clear();

    static void text(int x, int y, char *string, unsigned char color);
    static void line(int x0, int y0, int x1, int y1, unsigned char color);
};

#endif // GRAPHICS_H
