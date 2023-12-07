/*

  HeightField
  -----------

  This class implements a basic height field diffusion as described in
  http://www.matthiasmueller.info/talks/gdc2008.pdf

  We perform the following steps:

  - Diffuse the current height values according to some
    velocity. We use u0 and u1 as the height values. We need two
    because we're ping/ponging the values. The current velocity
    at which the height field diffuses is stored in v0 and v1.
    After each time step we change `state_diffuse` which toggles
    the reading/writing from u0/v0 and u1/v1.

  - Once we've done the diffuse step, we perform a processing step
    where we calculate the world position of the heightfield. These
    height values are stored in `tex_out_pos`.

  - When we have the positions in `tex_out_pos` we perform on more
    step where we calculate the normals, and values we might need for
    water rendering.

  - To render the height field you can use `vertices_vbo` and `vertices_vao`.
    They are setup in such a way that you'll get one in attribute in your shader
    called `tex` which contains the row/column into the position texture. Use this
    to set gl_Position.

  Things to know / improve:

  - We're not reusing the attribute less GL_VERTEX_SHADER;

  - You can use the vertices_vao/vbo to render the height field, see the
    debug shaders.

 */
#ifndef HEIGHTFIELD
#define HEIGHTFIELD

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "utils/shader.hpp"
#include "base_object.hpp"


struct HeightFieldVertex {
    HeightFieldVertex(glm::vec2 texcoord) :tex(texcoord) {}
    HeightFieldVertex() {}
    glm::vec2 tex;
};

class HeightField {

public:
    HeightField();
    bool setup(int w, int h);
    void update(float dt);     /* diffuses the height field */
    void process();            /* processes the current values, calculates normals, create position texture etc.. */
    void debugDraw();

    void print();               /* print some debug info */

private:
    bool setupDiffusing();     /* setup GL state for the diffusion step */
    bool setupProcessing();    /* setup GL state for the processing step; calculates normals, positions, texcoord etc.. using the current field values */
    bool setupDebug();         /* setup GL state for debugging */
    bool setupVertices();      /* create the triangle mesh (or the order of vertices, position is extracted from the position texture) */

public:

    /* diffusion of height field */
    GLuint field_fbo;
    GLuint tex_u0;              /* height value */
    GLuint tex_u1;              /* height value */
    GLuint tex_v0;              /* velocity at which u diffuses */
    GLuint tex_v1;              /* velocity at which u diffuses */
    GLint u_dt;                 /* reference to our dt uniform */
    GLuint field_vao;           /* we need a vao to render attribute less vertices */
    Shader* field_prog;         /* this program does the diffuse step */
    int state_diffuse;          /* toggles between 0 and 1 to ping/pong the diffuse/vel textures */

    /* general info */
    int field_size;             /* the size of our rectangular height field */
    int win_w;                  /* window width, use to reset the viewport */
    int win_h;                  /* window height, used to reset the viewport */

    /* used to process the height field and extract some usefull data */
    GLuint process_fbo;         /* we use a separate FBO to perform the processing step so we have some space for extra attachments */
    GLuint tex_out_norm;        /* the GL_RGB32F texture that will keep our calculated normals */
    GLuint tex_out_pos;         /* the GL_RGB32F texture that will keep our positions */
    GLuint tex_out_texcoord;    /* the GL_RG32F texture that will keep our texcoords */
    Shader* process_prog;       /* the program we use to calculate things like normals, etc.. */
    Shader* pos_prog;           /* the program we use to calculate the positions */

    /* used to debug draw */
    Shader* debug_prog;         /* debug shaders, shows how to set gl_Position */
    glm::mat4 pm;                    /* projection matrix */
    glm::mat4 vm;                    /* view matrix */

    /* vertices */
    std::vector<HeightFieldVertex> vertices;    /* The vertices that you can use to render a triangular height field, see the debug shader */
    GLuint vertices_vbo;                        /* VBO that holds the HeightFieldVertex data that forms the height field triangular grid */
    GLuint vertices_vao;                        /* The VAO to draw the height field vertices */
};

#endif