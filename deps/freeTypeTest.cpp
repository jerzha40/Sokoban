#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>

int main()
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "❌ FreeType init failed!" << std::endl;
        return -1;
    }
    std::cout << "✅ FreeType init succeed!" << std::endl;
    FT_Done_FreeType(ft);
    return 0;
}
