#ifndef EXAMPLES_UTILS_HPP_
#define EXAMPLES_UTILS_HPP_
#include <glew/include/GL/glew.h>
#include <gl/GL.h>
#include <GLFW/glfw3.h>
#include <cassert>

#include <nk/nuklear.hpp>

constexpr inline auto WINDOW_WIDTH = 1200;
constexpr inline auto WINDOW_HEIGHT = 800;

constexpr inline auto MAX_VERTEX_MEMORY = 512 * 1024;
constexpr inline auto MAX_ELEMENT_MEMORY = 128 * 1024;

/* ===============================================================
 *
 *                          DEVICE
 *
 * ===============================================================*/
struct nk_glfw_vertex {
  float position[2];
  float uv[2];
  std::byte col[4];
};

struct device {
  device()
    : prog(glCreateProgram()), vert_shdr(glCreateShader(GL_VERTEX_SHADER))
    , frag_shdr(glCreateShader(GL_FRAGMENT_SHADER))
  {

    GLint status = 0;
    static constexpr auto vertex_shader =
        "#version 150\n"
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 TexCoord;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main() {\n"
        "   Frag_UV = TexCoord;\n"
        "   Frag_Color = Color;\n"
        "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";

    static constexpr auto fragment_shader =
        "#version 150\n"
        "precision mediump float;\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main(){\n"
        "   Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
        "}\n";

    nk::buffer_init_default(&cmds);



    glShaderSource(vert_shdr, 1, &vertex_shader, 0);
    glShaderSource(frag_shdr, 1, &fragment_shader, 0);
    glCompileShader(vert_shdr);
    glCompileShader(frag_shdr);
    glGetShaderiv(vert_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glGetShaderiv(frag_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glAttachShader(prog, vert_shdr);
    glAttachShader(prog, frag_shdr);
    glLinkProgram(prog);
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    assert(status == GL_TRUE);

    uniform_tex = glGetUniformLocation(prog, "Texture");
    uniform_proj = glGetUniformLocation(prog, "ProjMtx");
    attrib_pos = glGetAttribLocation(prog, "Position");
    attrib_uv = glGetAttribLocation(prog, "TexCoord");
    attrib_col = glGetAttribLocation(prog, "Color");

    {
        /* buffer setup */
        GLsizei vs = sizeof(struct nk_glfw_vertex);
        size_t vp = offsetof(struct nk_glfw_vertex, position);
        size_t vt = offsetof(struct nk_glfw_vertex, uv);
        size_t vc = offsetof(struct nk_glfw_vertex, col);

        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        glEnableVertexAttribArray((GLuint)attrib_pos);
        glEnableVertexAttribArray((GLuint)attrib_uv);
        glEnableVertexAttribArray((GLuint)attrib_col);

        glVertexAttribPointer((GLuint)attrib_pos, 2, GL_FLOAT, GL_FALSE, vs, (void*)vp);
        glVertexAttribPointer((GLuint)attrib_uv, 2, GL_FLOAT, GL_FALSE, vs, (void*)vt);
        glVertexAttribPointer((GLuint)attrib_col, 4, GL_UNSIGNED_BYTE, GL_TRUE, vs, (void*)vc);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  ~device() {
    glDetachShader(prog, vert_shdr);
    glDetachShader(prog, frag_shdr);
    glDeleteShader(vert_shdr);
    glDeleteShader(frag_shdr);
    glDeleteProgram(prog);
    glDeleteTextures(1, &font_tex);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    nk::buffer_free(&cmds);
  }

  device(const device&) = delete;
  device(device&&) = delete;
  auto operator=(const device&) = delete;
  auto operator=(device&&) = delete;

  auto draw(nk::context *ctx, const int width, const int height,
            const nk::anti_aliasing AA = nk::NK_ANTI_ALIASING_OFF) -> void {
    GLfloat ortho[4][4] = {
        {2.0f, 0.0f, 0.0f, 0.0f},
        {0.0f,-2.0f, 0.0f, 0.0f},
        {0.0f, 0.0f,-1.0f, 0.0f},
        {-1.0f,1.0f, 0.0f, 1.0f},
    };
    ortho[0][0] /= (GLfloat)width;
    ortho[1][1] /= (GLfloat)height;

    /* setup global state */
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    /* setup program */
    glUseProgram(prog);
    glUniform1i(uniform_tex, 0);
    glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, &ortho[0][0]);
    {
        /* convert from command queue into draw list and draw to screen */
        const nk::draw_command *cmd;
        const nk::draw_index *offset = nullptr;

        /* allocate vertex and element buffer */
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        glBufferData(GL_ARRAY_BUFFER, MAX_VERTEX_MEMORY, NULL, GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_ELEMENT_MEMORY, NULL, GL_STREAM_DRAW);

        /* load draw vertices & elements directly into vertex + element buffer */
        void* vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        void* elements = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        {
            /* fill convert configuration */
            nk::convert_config config{};
            static const nk::draw_vertex_layout_element vertex_layout[] = {
                {nk::NK_VERTEX_POSITION, nk::NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_glfw_vertex, position)},
                {nk::NK_VERTEX_TEXCOORD, nk::NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_glfw_vertex, uv)},
                {nk::NK_VERTEX_COLOR, nk::NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_glfw_vertex, col)},
                {nk::NK_VERTEX_ATTRIBUTE_COUNT, nk::NK_FORMAT_COUNT, 0}
            };

            config.vertex_layout = vertex_layout;
            config.vertex_size = sizeof(nk_glfw_vertex);
            config.vertex_alignment = alignof(nk_glfw_vertex);
            config.tex_null = tex_null;
            config.circle_segment_count = 22;
            config.curve_segment_count = 22;
            config.arc_segment_count = 22;
            config.global_alpha = 1.0f;
            config.shape_AA = AA;
            config.line_AA = AA;

            /* setup buffers to load vertices and elements */
            {
              nk::memory_buffer vbuf{}, ebuf{};
              nk::buffer_init_fixed(&vbuf, vertices, MAX_VERTEX_MEMORY);
              nk::buffer_init_fixed(&ebuf, elements, MAX_ELEMENT_MEMORY);
              nk::convert(ctx, &cmds, &vbuf, &ebuf, &config);
            }
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

      /* iterate over and execute each draw command */
        draw_foreach(cmd, ctx, &cmds)
        {
            if (!cmd->elem_count) {
              continue;
            }

            glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
            glScissor(
                (GLint)(cmd->clip_rect.x),
                (GLint)((height - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h))),
                (GLint)(cmd->clip_rect.w),
                (GLint)(cmd->clip_rect.h));
            glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT, offset);
            offset += cmd->elem_count;
        }

        nk::clear(ctx);
        nk::buffer_clear(&cmds);
    }

    /* default OpenGL state */
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
  }

  auto upload_atlas(const void* image, const unsigned int width, const unsigned int height) -> void {
    glGenTextures(1, &font_tex);
    glBindTexture(GL_TEXTURE_2D, font_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, image);
  }

  nk::memory_buffer cmds{};
  nk::draw_null_texture tex_null{};
  GLuint vbo{}, vao{}, ebo{};
  GLuint prog;
  GLuint vert_shdr;
  GLuint frag_shdr;
  GLint attrib_pos;
  GLint attrib_uv;
  GLint attrib_col;
  GLint uniform_tex;
  GLint uniform_proj;
  GLuint font_tex{};
};
#endif

