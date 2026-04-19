/* nuklear - v1.05 - public domain */
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <climits>
#include <cstring>
#include <vector>
#include <filesystem>

#include <utils.hpp>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#include <nk/nuklear.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef __APPLE__
  #define NK_SHADER_VERSION "#version 150\n"
#else
  #define NK_SHADER_VERSION "#version 300 es\n"
#endif

#ifndef _WIN32
#include <pwd.h>
#endif

/* ===============================================================
 *
 *                          GUI
 *
 * ===============================================================*/
struct icons {
    nk::image desktop;
    nk::image home;
    nk::image computer;
    nk::image directory;

    nk::image default_file;
    nk::image text_file;
    nk::image music_file;
    nk::image font_file;
    nk::image img_file;
    nk::image movie_file;
};

enum file_groups {
    FILE_GROUP_DEFAULT,
    FILE_GROUP_TEXT,
    FILE_GROUP_MUSIC,
    FILE_GROUP_FONT,
    FILE_GROUP_IMAGE,
    FILE_GROUP_MOVIE,
    FILE_GROUP_MAX
};

enum file_types {
    FILE_DEFAULT,
    FILE_TEXT,
    FILE_C_SOURCE,
    FILE_CPP_SOURCE,
    FILE_HEADER,
    FILE_CPP_HEADER,
    FILE_MP3,
    FILE_WAV,
    FILE_OGG,
    FILE_TTF,
    FILE_BMP,
    FILE_PNG,
    FILE_JPEG,
    FILE_PCX,
    FILE_TGA,
    FILE_GIF,
    FILE_MAX
};

struct file_group {
    file_groups group;
    const char *name;
    nk::image *icon;
};

struct file {
    file_types type;
    const char *suffix;
    file_groups group;
};

static file_group
FILE_GROUP(const file_groups group, const char *name, nk::image *icon)
{
  file_group fg{};
  fg.group = group;
  fg.name = name;
  fg.icon = icon;
  return fg;
}

static file
FILE_DEF(const file_types type, const char *suffix, const file_groups group)
{
  file fd{};
  fd.type = type;
  fd.suffix = suffix;
  fd.group = group;
  return fd;
}

static std::vector<std::string>
dir_list(const std::filesystem::path& dir, const bool files_only = false) {
  namespace fs = std::filesystem;
  std::vector<std::string> results;

  if (fs::exists(dir) && fs::is_directory(dir)) {

    if (!files_only) { results.emplace_back(".."); }

    for (const auto& data : fs::recursive_directory_iterator(dir)) {
      const auto d_name = data.path().filename();

      if (!files_only) {
        results.push_back(d_name.string());
      } else if (!data.is_directory()) {
        results.push_back(d_name.string());
      }
    }
  }

  return std::vector{ std::move(results) };
}

struct browser_media {
  browser_media() {
    /* file groups */
    group[FILE_GROUP_DEFAULT] = FILE_GROUP(FILE_GROUP_DEFAULT,"default",&m_icons.default_file);
    group[FILE_GROUP_TEXT] = FILE_GROUP(FILE_GROUP_TEXT, "textual", &m_icons.text_file);
    group[FILE_GROUP_MUSIC] = FILE_GROUP(FILE_GROUP_MUSIC, "music", &m_icons.music_file);
    group[FILE_GROUP_FONT] = FILE_GROUP(FILE_GROUP_FONT, "font", &m_icons.font_file);
    group[FILE_GROUP_IMAGE] = FILE_GROUP(FILE_GROUP_IMAGE, "image", &m_icons.img_file);
    group[FILE_GROUP_MOVIE] = FILE_GROUP(FILE_GROUP_MOVIE, "movie", &m_icons.movie_file);

    /* files */
    files[FILE_DEFAULT] = FILE_DEF(FILE_DEFAULT, nullptr, FILE_GROUP_DEFAULT);
    files[FILE_TEXT] = FILE_DEF(FILE_TEXT, "txt", FILE_GROUP_TEXT);
    files[FILE_C_SOURCE] = FILE_DEF(FILE_C_SOURCE, "c", FILE_GROUP_TEXT);
    files[FILE_CPP_SOURCE] = FILE_DEF(FILE_CPP_SOURCE, "cpp", FILE_GROUP_TEXT);
    files[FILE_HEADER] = FILE_DEF(FILE_HEADER, "h", FILE_GROUP_TEXT);
    files[FILE_CPP_HEADER] = FILE_DEF(FILE_HEADER, "hpp", FILE_GROUP_TEXT);
    files[FILE_MP3] = FILE_DEF(FILE_MP3, "mp3", FILE_GROUP_MUSIC);
    files[FILE_WAV] = FILE_DEF(FILE_WAV, "wav", FILE_GROUP_MUSIC);
    files[FILE_OGG] = FILE_DEF(FILE_OGG, "ogg", FILE_GROUP_MUSIC);
    files[FILE_TTF] = FILE_DEF(FILE_TTF, "ttf", FILE_GROUP_FONT);
    files[FILE_BMP] = FILE_DEF(FILE_BMP, "bmp", FILE_GROUP_IMAGE);
    files[FILE_PNG] = FILE_DEF(FILE_PNG, "png", FILE_GROUP_IMAGE);
    files[FILE_JPEG] = FILE_DEF(FILE_JPEG, "jpg", FILE_GROUP_IMAGE);
    files[FILE_PCX] = FILE_DEF(FILE_PCX, "pcx", FILE_GROUP_IMAGE);
    files[FILE_TGA] = FILE_DEF(FILE_TGA, "tga", FILE_GROUP_IMAGE);
    files[FILE_GIF] = FILE_DEF(FILE_GIF, "gif", FILE_GROUP_IMAGE);
  }

  auto icon_for_file(const char *file) -> nk::image* {
    int i = 0;
    const char *s = file;
    char suffix[4];
    int found = 0;
    memset(suffix, 0, sizeof(suffix));

    /* extract suffix .xxx from file */
    while (*s++ != '\0') {
      if (found && i < 3)
        suffix[i++] = *s;

      if (*s == '.') {
        if (found){
          found = 0;
          break;
        }
        found = 1;
      }
    }

    /* check for all file definition of all groups for fitting suffix*/
    for (i = 0; i < FILE_MAX && found; ++i) {
      struct file *d = &files[i];
      {
        const char *f = d->suffix;
        s = suffix;
        while (f && *f && *s && *s == *f) {
          s++; f++;
        }

        /* found correct file definition so */
        if (f && *s == '\0' && *f == '\0')
          return group[d->group].icon;
      }
    }
    return &m_icons.default_file;
  }

  int font{};
  int icon_sheet{};
  icons m_icons{};
  file_group group[FILE_GROUP_MAX]{};
  file files[FILE_MAX]{};
};

constexpr auto MAX_PATH_LEN = 512u;

struct file_browser {
  explicit file_browser(browser_media *media) : media(media) {
    /* load files and subdirectory list */
    std::array<char, MAX_PATH_LEN> home_dir{};
    auto* buffer = home_dir.data(); // Because we need a pointer to a pointer.
    auto size = 0uz;

    auto err = _dupenv_s(&buffer, &size, "HOME");
#ifdef _WIN32
    if (err != 0) { err = _dupenv_s(&buffer, &size, "USERPROFILE"); }
#else
    if (err != 0) { home_dir = getpwuid(getuid())->pw_dir; }
#endif

    if (err == 0) {
      home = std::filesystem::path { home_dir.data() };
      desktop = home / "desktop";

      files = dir_list(directory, true);
      directories = dir_list(directory);
    }
  }

  auto reload_directory_content(const std::filesystem::path& path) -> void {
    files = dir_list(path, true);
    directories = dir_list(path);
  }

  auto run(nk::context* ctx) -> int {
    int ret = 0;

    if (nk::begin(ctx, "File Browser", nk::rect(50, 50, 800, 600),
        nk::panel_flags::WINDOW_BORDER
        | nk::panel_flags::WINDOW_NO_SCROLLBAR
        | nk::panel_flags::WINDOW_MOVABLE))
    {
      nk::rectf total_space{};
      static float ratio[] = { 0.25f, 1.0f };
      const float spacing_x = ctx->style.window.spacing.x;

      /* output path directory selector in the menubar */
      ctx->style.window.spacing.x = 0;
      nk::menubar_begin(ctx);
      nk::layout_row_dynamic(ctx, 25, 6);

      for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (nk::button_label(ctx, entry.path().filename().string().c_str())) {
          reload_directory_content(directory);
          break;
        }
      }

      nk::menubar_end(ctx);
      ctx->style.window.spacing.x = spacing_x;

      /* window layout */
      total_space = nk::window_get_content_region(ctx);
      nk::layout_row(ctx, nk::layout_format::DYNAMIC, total_space.h, 2, ratio);
      nk::group_begin(ctx, "Special", nk::panel_flags::WINDOW_NO_SCROLLBAR);

      nk::layout_row_dynamic(ctx, 40, 1);

      if (nk::button_image_label(ctx, media->m_icons.home, "home", nk::NK_TEXT_CENTERED)) {
          reload_directory_content(home);
      }
      if (nk::button_image_label(ctx, media->m_icons.desktop, "desktop", nk::NK_TEXT_CENTERED)) {
          reload_directory_content(desktop);
      }
      if (nk::button_image_label(ctx, media->m_icons.computer, "computer", nk::NK_TEXT_CENTERED)) {
        reload_directory_content("/");
      }
      nk::group_end(ctx);

      /* output directory content window */
      nk::group_begin(ctx, "Content", 0);
      {
          int index = -1;
          size_t i = 0, j = 0, k = 0;
          size_t rows = 0, cols = 0;
          const size_t count = directories.size() + files.size();

          cols = 4;
          rows = count / cols;
          for (i = 0; i <= rows; i += 1) {
            const size_t nRows = j + cols;
            nk::layout_row_dynamic(ctx, 135, (int)cols);
            for (; j < count && j < nRows; ++j) {
              /* draw one row of icons */
              if (j < directories.size()) {
                /* draw and execute directory buttons */
                if (nk::button_image(ctx,media->m_icons.directory))
                    index = (int)j;
              } else {
                /* draw and execute files buttons */
                const auto fileIndex = j - directories.size();
                const nk::image* icon = media->icon_for_file(files[fileIndex].c_str());
                if (nk::button_image(ctx, *icon)) {
                    file = directory / files[fileIndex];
                    ret = 1;
                }
              }
            }
            const size_t nCols = k + cols;
            nk::layout_row_dynamic(ctx, 20, (int)cols);
            for (; k < count && k < nRows; k++) {
              /* draw one row of labels */
              if (k < directories.size()) {
                  nk::label(ctx, directories[k].c_str(), nk::NK_TEXT_CENTERED);
              } else {
                  size_t t = k - directories.size();
                  nk::label(ctx, files[t].c_str(), nk::NK_TEXT_CENTERED);
              }
            }
          }

          reload_directory_content(directory / directories[static_cast<std::size_t>(index)]);
          nk::group_end(ctx);
      }
    }
    nk::end(ctx);
    return ret;
  }

  /* path */
  std::filesystem::path file{};
  std::filesystem::path home{};
  std::filesystem::path desktop{};
  std::filesystem::path directory{};

  /* directory content */
  std::vector<std::string> files;
  std::vector<std::string> directories;
  browser_media *media{}; // file_browser does not own this pointer.
};

static void
die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputs("\n", stderr);
    exit(EXIT_FAILURE);
}

static nk::image icon_load(const char *filename) {
  const auto high_bit_set = [](const unsigned int val) { return (val & 0x80000000) > 0; };

  unsigned int x,y,n;
  GLuint tex;
  unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
  if (!data || high_bit_set(x) || high_bit_set(y)) {
    die("[SDL]: failed to load image: %s", filename);
  }

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLint>(x), static_cast<GLint>(y), 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
  return nk::image_id((int)tex);
}

/* glfw callbacks (I don't know if there is a easier way to access text and scroll )*/
static void error_callback(int e, const char *d){printf("Error %d: %s\n", e, d);}
static void text_input(GLFWwindow *win, unsigned int codepoint)
{nk::input_unicode((nk::context*)glfwGetWindowUserPointer(win), codepoint);}
static void scroll_input(GLFWwindow *win, double _, double yoff)
{;nk::input_scroll((nk::context*)glfwGetWindowUserPointer(win), nk::vec2_from_floats(0, (float)yoff));}

int main(const int argc, char *argv[])
{
    /* Platform */
    static GLFWwindow *win;
    int width = 0, height = 0;
    int display_width = 0, display_height = 0;

    /* GUI */
    nk::context ctx{};
    nk::font *font{};
    nk::font_atlas atlas{};

    /* GLFW */
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        fprintf(stdout, "[GFLW] failed to init!\n");
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Demo", NULL, NULL);
    glfwMakeContextCurrent(win);
    glfwSetWindowUserPointer(win, &ctx);
    glfwSetCharCallback(win, text_input);
    glfwSetScrollCallback(win, scroll_input);

    /* OpenGL */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glewExperimental = 1;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    device device;

    {/* GUI */
    {const void *image; unsigned int w, h;
    const char *font_path = (argc > 1) ? argv[1]: 0;
    nk::font_atlas_init_default(&atlas);
    nk::font_atlas_begin(&atlas);
    if (font_path) font = nk::font_atlas_add_from_file(&atlas, font_path, 13.0f, NULL);
    else font = nk::font_atlas_add_default(&atlas, 13.0f, NULL);
    image = nk::font_atlas_bake(&atlas, &w, &h, nk::NK_FONT_ATLAS_RGBA32);
    device.upload_atlas(image, w, h);
    nk::font_atlas_end(&atlas, nk::handle_id((int)device.font_tex), &device.tex_null);}
    nk::init_default(&ctx, &font->handle);}

    /* icons */
    glEnable(GL_TEXTURE_2D);

    browser_media media;
    media.m_icons.home = icon_load("../icon/home.png");
    media.m_icons.directory = icon_load("../icon/directory.png");
    media.m_icons.computer = icon_load("../icon/computer.png");
    media.m_icons.desktop = icon_load("../icon/desktop.png");
    media.m_icons.default_file = icon_load("../icon/default.png");
    media.m_icons.text_file = icon_load("../icon/text.png");
    media.m_icons.music_file = icon_load("../icon/music.png");
    media.m_icons.font_file =  icon_load("../icon/font.png");
    media.m_icons.img_file = icon_load("../icon/img.png");
    media.m_icons.movie_file = icon_load("../icon/movie.png");

    file_browser browser(&media);
    while (!glfwWindowShouldClose(win))
    {
        /* High DPI displays */
        nk::vec2f scale;
        glfwGetWindowSize(win, &width, &height);
        glfwGetFramebufferSize(win, &display_width, &display_height);
        scale.x = (float)display_width/(float)width;
        scale.y = (float)display_height/(float)height;

        /* Input */
        {double x, y;
        nk::input_begin(&ctx);
        glfwPollEvents();
        nk::input_key(&ctx, nk::NK_KEY_DEL, glfwGetKey(win, GLFW_KEY_DELETE) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_ENTER, glfwGetKey(win, GLFW_KEY_ENTER) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_TAB, glfwGetKey(win, GLFW_KEY_TAB) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_BACKSPACE, glfwGetKey(win, GLFW_KEY_BACKSPACE) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_LEFT, glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_RIGHT, glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_UP, glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_DOWN, glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS);
        if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
            glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
            nk::input_key(&ctx, nk::NK_KEY_COPY, glfwGetKey(win, GLFW_KEY_C) == GLFW_PRESS);
            nk::input_key(&ctx, nk::NK_KEY_PASTE, glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS);
            nk::input_key(&ctx, nk::NK_KEY_CUT, glfwGetKey(win, GLFW_KEY_X) == GLFW_PRESS);
            nk::input_key(&ctx, nk::NK_KEY_CUT, glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS);
            nk::input_key(&ctx, nk::NK_KEY_SHIFT, 1);
        } else {
            nk::input_key(&ctx, nk::NK_KEY_COPY, 0);
            nk::input_key(&ctx, nk::NK_KEY_PASTE, 0);
            nk::input_key(&ctx, nk::NK_KEY_CUT, 0);
            nk::input_key(&ctx, nk::NK_KEY_SHIFT, 0);
        }
        glfwGetCursorPos(win, &x, &y);
        nk::input_motion(&ctx, (int)x, (int)y);
        nk::input_button(&ctx, nk::NK_BUTTON_LEFT, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
        nk::input_button(&ctx, nk::NK_BUTTON_MIDDLE, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
        nk::input_button(&ctx, nk::NK_BUTTON_RIGHT, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
        nk::input_end(&ctx);}

        /* GUI */
        browser.run(&ctx);

        /* Draw */
        glViewport(0, 0, display_width, display_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        device.draw(&ctx, width, height, nk::NK_ANTI_ALIASING_ON);
        glfwSwapBuffers(win);
    }

    glDeleteTextures(1,(const GLuint*)&media.m_icons.home.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.m_icons.directory.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.m_icons.computer.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.m_icons.desktop.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.m_icons.default_file.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.m_icons.text_file.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.m_icons.music_file.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.m_icons.font_file.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.m_icons.img_file.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.m_icons.movie_file.handle.id);

    nk::font_atlas_clear(&atlas);
    nk::free(&ctx);
    glfwTerminate();
    return 0;
}


