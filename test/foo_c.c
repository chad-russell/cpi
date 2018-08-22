#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ot.h>

#include <cairo/cairo.h>
#include <cairo/cairo-ft.h>

#define FONT_SIZE 18
#define WIDTH     200
#define HEIGHT    100

double toDouble(int n) {
    return (double) n;
}

double toInt(double n) {
    return (int) n;
}

long i32ToI64(int n) {
    return (long) n;
}

void printDouble(double n) {
    printf("%f\n", n);
}

struct context_t {
    hb_font_t *hb_font;

    cairo_font_face_t *cairo_font;
    cairo_glyph_t *cairo_glyphs;
    cairo_surface_t *cairo_surface;
    cairo_t *cairo_context;

    const char *font;
    const char *text;

    int done;

    SDL_Window *window;
    SDL_Surface *sdl_surface;
    SDL_Renderer *renderer;
};

void c_initCairo(struct context_t *context) {
    context->renderer = SDL_CreateRenderer(context->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Compute screen resolution
    // For instance, on a retina screen, renderer size is twice as window size
    int window_width;
    int window_height;
    SDL_GetWindowSize(context->window, &window_width, &window_height);

    int renderer_width;
    int renderer_height;
    SDL_GetRendererOutputSize(context->renderer, &renderer_width, &renderer_height);

    // Create a SDL surface for Cairo to render onto
    context->sdl_surface = SDL_CreateRGBSurface (
            0,
            renderer_width,
            renderer_height,
            32,
            0x00ff0000,
            0x0000ff00,
            0x000000ff,
            0
    );

    // Get Cairo surface form SDL2 surface
    context->cairo_surface = cairo_image_surface_create_for_data (
            (unsigned char *)context->sdl_surface->pixels,
            CAIRO_FORMAT_RGB24,
            context->sdl_surface->w,
            context->sdl_surface->h,
            context->sdl_surface->pitch);

    int cairo_x_multiplier = renderer_width / window_width;
    int cairo_y_multiplier = renderer_height / window_height;

    // Scale cairo to use screen resolution
    cairo_surface_set_device_scale(context->cairo_surface, cairo_x_multiplier, cairo_y_multiplier);

    // Get Cairo context from Cairo surface
    context->cairo_context = cairo_create(context->cairo_surface);
    cairo_set_source_rgba (context->cairo_context, 0, 0, 0, 1.0);
    cairo_set_font_face(context->cairo_context, context->cairo_font);
    cairo_set_font_size(context->cairo_context, FONT_SIZE);
}

void c_loadFonts(struct context_t *context) {
//    context->font = "/System/Library/Fonts/Apple Color Emoji.ttc";
//    context->font = "/Users/chadrussell/Projects/text/Noto-unhinted/NotoColorEmoji.ttf";
//    context->text = "🙈 🇹🇩";

//    const char *font = "/Users/chadrussell/Library/Fonts/Roboto-Light.ttf";
    context->font = "/Library/Fonts/Inconsolata.otf";
    context->text = "";

    // For CAIRO, load using FreeType
    FT_Library ft_library;
    assert(FT_Init_FreeType(&ft_library) == 0);
    FT_Face ft_face;
    assert(FT_New_Face(ft_library, context->font, 0, &ft_face) == 0);
    context->cairo_font = cairo_ft_font_face_create_for_ft_face(ft_face, 0);

    // For Harfbuzz, load using OpenType (HarfBuzz FT does not support bitmap font)
    hb_blob_t *blob = hb_blob_create_from_file(context->font);
    hb_face_t *face = hb_face_create (blob, 0);
    context->hb_font = hb_font_create (face);
    hb_ot_font_set_funcs(context->hb_font);
    hb_font_set_scale(context->hb_font, FONT_SIZE*64, FONT_SIZE*64);
}

void c_drawText(struct context_t *context) {
    // Create  HarfBuzz buffer
    hb_buffer_t *buf = hb_buffer_create();

    // Set buffer to LTR direction, common script and default language
    hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
    hb_buffer_set_script(buf, HB_SCRIPT_COMMON);
    hb_buffer_set_language(buf, hb_language_get_default());

    // Add text and lay it out
    hb_buffer_add_utf8(buf, context->text, -1, 0, -1);
    hb_shape(context->hb_font, buf, NULL, 0);

    // Get buffer data
    unsigned int        glyph_count = hb_buffer_get_length (buf);
    hb_glyph_info_t     *glyph_info   = hb_buffer_get_glyph_infos(buf, NULL);
    hb_glyph_position_t *glyph_pos    = hb_buffer_get_glyph_positions(buf, NULL);

    unsigned int string_width_in_pixels = 0;
    for (int i = 0; i < glyph_count; ++i) {
        string_width_in_pixels += glyph_pos[i].x_advance / 64.0;
    }

    /*printf("glyph_count=%d\n", glyph_count);*/
    /*printf("string_width=%d \n", string_width_in_pixels);*/

    // Shape glyph for Cairo
    context->cairo_glyphs = cairo_glyph_allocate(glyph_count);
    int x = 0;
    int y = 0;
    for (int i = 0 ; i < glyph_count ; ++i) {
        context->cairo_glyphs[i].index = glyph_info[i].codepoint;
        context->cairo_glyphs[i].x = x + (glyph_pos[i].x_offset / (64.0));
        context->cairo_glyphs[i].y = -(y + glyph_pos[i].y_offset / (64.0));
        x += glyph_pos[i].x_advance / (64.0);
        y += glyph_pos[i].y_advance / (64.0);

        /*printf("glyph_codepoint=%lu size=(%g, %g) advance=(%g, %g)\n", context->cairo_glyphs[i].index, glyph_pos[i].x_advance/64.0, glyph_pos[i].y_advance/64.0, glyph_pos[i].x_advance/64.0, glyph_pos[i].y_advance/64.0);*/
    }

    // Move glyph to be on window middle
    for (int i = 0 ; i < glyph_count ; ++i) {
        context->cairo_glyphs[i].x += WIDTH / 2 - string_width_in_pixels / 2;
        context->cairo_glyphs[i].y += HEIGHT / 2;
    }

    // =======================
    // Fill background in white
    SDL_FillRect(context->sdl_surface, NULL, SDL_MapRGB(context->sdl_surface->format, 255, 255, 255));

    // Render glyph onto cairo context (which render onto SDL2 surface)
    cairo_show_glyphs(context->cairo_context, context->cairo_glyphs, glyph_count);

    // Render SDL2 surface onto SDL2 renderer
    SDL_Texture* texture = SDL_CreateTextureFromSurface(context->renderer, context->sdl_surface);
    SDL_RenderCopy(context->renderer, texture, 0, 0);
    SDL_RenderPresent(context->renderer);
}

int c_main() {
    // Draw text in SDL2 with Cairo
    SDL_Window* window = NULL;

    assert(SDL_Init(SDL_INIT_VIDEO) == 0);

    window = SDL_CreateWindow("harfbuzz + sdl2 + cairo + freetype",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WIDTH, HEIGHT,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    struct context_t context;
    context.done = 0;
    context.window = window;

    c_loadFonts(&context);

    c_initCairo(&context);

    while (context.done == 0) {
        // Quit app on close event
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    context.done = 1;
                } break;
                case SDL_KEYDOWN: {
                    char foo[50];
                    sprintf(foo, "%s%s", context.text, "e");
                    context.text = foo;
                } break;
                default:break;
            }

            c_drawText(&context);
        }
    }

    // Free stuff
    free(context.cairo_glyphs);
    cairo_surface_destroy(context.cairo_surface);
    cairo_destroy(context.cairo_context);
    cairo_font_face_destroy(context.cairo_font);
    hb_font_destroy(context.hb_font);
    SDL_FreeSurface(context.sdl_surface);

    SDL_Quit();
    return 0;
}

