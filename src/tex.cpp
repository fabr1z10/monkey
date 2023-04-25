#include "tex.h"
#include <iostream>
#include <memory>
#include <yaml-cpp/yaml.h>

//#define cimg_use_jpeg
#define cimg_use_png
#define cimg_display 0

#include "CImg.h"
#include "png.h"
#include "palette.h"

using namespace cimg_library;

Tex::Tex (const std::string& file) : m_palette(false), m_defaultPaletteId(GL_INVALID_VALUE) { //const std::string& filename, TexFilter filter) {
    //std::string file = table.Get<std::string>("file");
    TexFilter filter = nearest;
    auto extension = file.substr(file.size() - 3);
    if (extension == "png") {
        load_png(file);
    } else {
        // default to cimg
        load_generic(file);
    }
}


std::vector<unsigned char> Tex::getRaw(const std::string &file, int& width, int& height) {
    CImg<unsigned char> data (file.c_str());
    width = data.width();
    height = data.height();
    std::cout << "width " << data.width() << "\n";
    std::cout << "height " << data.height() << "\n";
    data.permute_axes ("cxyz");
    std::vector<unsigned char> out;
    for (auto it = data.begin(); it != data.end(); it+=4) {
        out.push_back(*(it+3) == 0 ? 0 : 1);
    }
    return out;

}

void Tex::load_png(const std::string &file) {
    unsigned char sig[8];
    FILE* infile;
    infile = fopen(file.c_str(), "r");

    /* first do a quick check that the file really is a PNG image; could
     * have used slightly more general png_sig_cmp() function instead */
    fread(sig, 1, 8, infile);
    if (!png_check_sig(sig, 8))
        exit(1);   /* bad signature */
    fprintf(stderr, "all ok\n");

    /* could pass pointers to user-defined error handlers instead of NULLs: */

    auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        exit(1);   /* out of memory */

    auto info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        exit(1);   /* out of memory */
    }

//    /* we could create a second info struct here (end_info), but it's only
//     * useful if we want to keep pre- and post-IDAT chunk info separated
//     * (mainly for PNG-aware image editors and converters) */
//
//
//    /* setjmp() must be called in every function that calls a PNG-reading
//     * libpng function */
//
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        exit(1);
    }

    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, 8);  /* we already read the 8 signature bytes */

    png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */
    //png_
//
//
//    /* alternatively, could make separate calls to png_get_image_width(),
//     * etc., but want bit_depth and color_type for later [don't care about
//     * compression_type and filter_type => NULLs] */
//
    auto imgWidth = png_get_image_width(png_ptr, info_ptr);
    auto imgHeight = png_get_image_height(png_ptr, info_ptr);
    auto bitDepth = (int) png_get_bit_depth(png_ptr, info_ptr);
    auto colorType = (int) png_get_color_type(png_ptr, info_ptr);
    m_width = imgWidth;
    m_height = imgHeight;
    /*
       Color    Allowed    Interpretation
       Type    Bit Depths

       0       1,2,4,8,16  Each pixel is a grayscale sample.

       2       8,16        Each pixel is an R,G,B triple.

       3       1,2,4,8     Each pixel is a palette index;
                           a PLTE chunk must appear.

       4       8,16        Each pixel is a grayscale sample,
                           followed by an alpha sample.

       6       8,16        Each pixel is an R,G,B triple,
                           followed by an alpha sample.
    */
    if (colorType == 3) {
        std::cout << " --- indexed PNG found;\n";
        m_palette = true;
    } else if (colorType == 6) {
        std::cout << " --- RGBA fond;\n";
        load_generic(file);
        return;
    } else {
        load_generic(file);
        return;
    }

    auto _row_pointers = new png_bytep[imgHeight];
    unsigned int bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
    auto _pixels = new unsigned char[bytesPerRow * imgHeight];
    auto _data = new unsigned char[imgWidth * imgHeight];

    for (int i=0; i < imgHeight; i++)
    {
        _row_pointers[i] = _pixels + i*bytesPerRow;
    }

    auto pixel_per_byte = 8 / bitDepth;
    unsigned char initSelector;
    switch (bitDepth) {
        case 1:
            initSelector = 0x80;
            break;
        case 2:
            initSelector = 0xC0;
            break;
        case 4:
            initSelector = 0xF0;
            break;
        case 8:
            initSelector = 0xFF;
            break;
    }



    png_read_image(png_ptr, _row_pointers);
    int k = 0;
    int pixel_per_row = 0;
    for (int i = 0; i < bytesPerRow * imgHeight; ++i) {
        // read byte
        unsigned char selector = initSelector;
        //std::cout << " current byte: " << std::hex << (int) _pixels[i] << "\n";
        for (int j = 0; j < pixel_per_byte; ++j) {
            auto pixel = (_pixels[i] & selector) >> (8 - bitDepth * (j + 1));
            _data[k++] = pixel;
            //std::cout << "added pix: " << (int) pixel << "\n";
            pixel_per_row++;
            if (pixel_per_row >= imgWidth) {
                pixel_per_row = 0;
                break;
            }
            selector >>= bitDepth;
        }

    }
    png_colorp pal;
    int colors;
    png_get_PLTE(png_ptr, info_ptr, &pal, &colors);
    std::cout << " number of colors: " << colors << std::endl;
    // store default palette
    std::vector<unsigned char> defaultPalette(1024, 0);
    for (size_t i= 0; i< colors; ++i) {
        std::cout << "color #" << i << ": " << (int) pal[i].red << ", " << (int) pal[i].green << ", " << (int) pal[i].blue << "\n";
        defaultPalette[i * 4] = pal[i].red;
        defaultPalette[i * 4 + 1] = pal[i].green;
        defaultPalette[i * 4 + 2] = pal[i].blue;
        defaultPalette[i * 4 + 3] = i == 0 ? 0 : 255;
    }
    // load extra palettes for this sheet
    try {
        std::string palFile = file.substr(0, file.size()-4) + ".pal";
        auto f = YAML::LoadFile(palFile);
        int nAdditionalPalettes = f["palettes"].size();
        defaultPalette.resize(1024*(nAdditionalPalettes+1), 0);
        int i{1};
        for (const auto& p : f["palettes"]) {
            int offset = 1024 * (i++);
            for (const auto& k : p) {
                int n = k.first.as<int>();
                auto colors = k.second.as<std::vector<int>>();
                std::cout << k.first << std::endl;
                defaultPalette[offset + n * 4] = colors[0];
                defaultPalette[offset + n * 4 + 1] = colors[1];
                defaultPalette[offset + n * 4 + 2] = colors[2];
                defaultPalette[offset + n * 4 + 3] = colors[3];
                std::cout << " cane\n";
            }
        }
    } catch (YAML::BadFile& badFile) {
        std::cout << " --- palette file not found\n";
    }
    _pal = std::make_shared<Palette>(defaultPalette);
    m_defaultPaletteId = _pal->getTexId();

    std::cout << "\n--- raw ---\n";
//    for (int i = 0; i < imgHeight; ++i) {
//        std::cout << "row " << i << "\n";
//        for (int j = 0; j < imgWidth; ++j) {
//            std::cout << (int) _data[i*imgWidth+j] << ",";
//            //_data[i*imgWidth+j] = 2;
//        }
//        std::cout << "\n";
//    }
    std::cout.flush();

    glGenTextures (1, &m_texId);
    glBindTexture (GL_TEXTURE_2D, m_texId);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_R8UI , m_width, m_height, 0,  GL_RED_INTEGER, GL_UNSIGNED_BYTE, _data);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    delete [] _pixels;
    delete [] _row_pointers;
    delete [] _data;


    return;
    //  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
    //             NULL, NULL, NULL);
//    *pWidth = width;
//    *pHeight = height;
//
//
//    /* OK, that's all we need for now; return happy */
//
//    return 0;
}

void Tex::load_generic(const std::string &file) {
    CImg<unsigned char> data (file.c_str());
    m_width = data.width();
    m_height = data.height();
    auto pino = data.depth();
    int channels = data.spectrum();
    data.permute_axes ("cxyz");
	for (int i = 0; i < 100; ++i) {
		std::cout << (int) data._data[i] << ", ";
	}
    glGenTextures (1, &m_texId);
    glBindTexture (GL_TEXTURE_2D, m_texId);
    GLint format;
    switch (channels) {
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            throw std::runtime_error("Unknown number of channels");
    };

    glTexImage2D (GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}


Tex::~Tex() {
    glDeleteTextures (1, &m_texId);
}
