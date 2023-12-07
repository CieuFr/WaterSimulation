#include "heightfield.hpp"

HeightField::HeightField()
    :field_fbo(0)
    , tex_u0(0)
    , tex_u1(0)
    , tex_v0(0)
    , tex_v1(0)
    , field_size(128)
    , field_vao(0)
    , state_diffuse(0)
    , win_w(0)
    , win_h(0)
    , process_fbo(0)
    , tex_out_norm(0)
    , tex_out_pos(0)
    , tex_out_texcoord(0)
    , vertices_vbo(0)
    , vertices_vao(0)
{
}

bool HeightField::setup(int w, int h) {

    if (!w || !h) {
        printf("Error: invalid width/height: %d x %d\n", w, h);
        return false;
    }

    win_w = w;
    win_h = h;

    glGenVertexArrays(1, &field_vao);

    if (!setupDiffusing()) {
        printf("Error: cannot set GL state for the diffuse step.\n");
        return false;
    }

    if (!setupProcessing()) {
        printf("Error: cannot setup the GL state for the processing.\n");
        return false;
    }

    if (!setupVertices()) {
        printf("Error: cannot setup the vertices for the height field.\n");
        return false;
    }

    if (!setupDebug()) {
        printf("Error: cannot setup the GL state for debugging.\n");
        return false;
    }

    return true;
}

bool HeightField::setupVertices() {
    glGenVertexArrays(1, &vertices_vao);
    glBindVertexArray(vertices_vao);

    glGenBuffers(1, &vertices_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);

    std::vector<HeightFieldVertex> tmp(field_size * field_size, HeightFieldVertex());
    for (int j = 0; j < field_size; ++j) {
        for (int i = 0; i < field_size; ++i) {
            int dx = j * field_size + i;
            tmp[dx].tex = glm::vec2(i, j);
        }
    }

    for (int j = 0; j < field_size - 1; ++j) {
        for (int i = 0; i < field_size - 1; ++i) {
            int a = (j + 0) * field_size + (i + 0);
            int b = (j + 0) * field_size + (i + 1);
            int c = (j + 1) * field_size + (i + 1);
            int d = (j + 1) * field_size + (i + 0);
            vertices.push_back(tmp[a]);
            vertices.push_back(tmp[b]);
            vertices.push_back(tmp[c]);

            vertices.push_back(tmp[a]);
            vertices.push_back(tmp[c]);
            vertices.push_back(tmp[d]);
        }
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(HeightFieldVertex) * vertices.size(), &vertices[0].tex, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // tex
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(HeightFieldVertex), (GLvoid*)0);
    return true;
}

bool HeightField::setupDebug() {

    //pm.perspective(60.0f, float(win_w) / win_h, 0.01f, 100.0f);
    ////vm.lookAt(vec3(0.0f, 35.0f, 0.0f), vec3(0.0f, 0.0f, 0.1f), vec3(0.0f, 1.0f, 0.0f));
    //vm.translate(0.0f, 0.0f, -30.0f);
    //vm.rotateX(30 * DEG_TO_RAD);

    //const char* atts[] = { "a_tex" };
    //debug_prog.create(GL_VERTEX_SHADER, rx_to_data_path("height_field_debug.vert"));
    //debug_prog.create(GL_FRAGMENT_SHADER, rx_to_data_path("height_field_debug.frag"));
    //debug_prog.link(1, atts);

    //glUseProgram(debug_prog.id);
    //glUniformMatrix4fv(glGetUniformLocation(debug_prog.id, "u_pm"), 1, GL_FALSE, pm.ptr());
    //glUniformMatrix4fv(glGetUniformLocation(debug_prog.id, "u_vm"), 1, GL_FALSE, vm.ptr());
    //glUniform1i(glGetUniformLocation(debug_prog.id, "u_pos_tex"), 0);

    return true;
}

bool HeightField::setupProcessing() {

    glGenTextures(1, &tex_out_norm);
    glBindTexture(GL_TEXTURE_2D, tex_out_norm);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, field_size, field_size, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &tex_out_pos);
    glBindTexture(GL_TEXTURE_2D, tex_out_pos);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, field_size, field_size, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &tex_out_texcoord);
    glBindTexture(GL_TEXTURE_2D, tex_out_texcoord);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, field_size, field_size, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenFramebuffers(1, &process_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, process_fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_out_pos, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex_out_norm, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tex_out_texcoord, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Error: process framebuffer not complete.\n");
        return false;
    }

    // Position processing
    const char* pos_frags[] = { "out_pos" };
    pos_prog = new Shader{ "Source/shaders/heightfield.vs", "Source/shaders/heightfield.fs" };
    pos_prog->use();
    pos_prog->setFloat("u_height_tex", 0);
    pos_prog->setFloat("u_vel_tex", 1);

    // Extra processing
    const char* process_frags[] = { "out_norm", "out_tex" };

    process_prog = new Shader{ "Source/shaders/heightfield.vs", "Source/shaders/heightfield_process.fs" };
    process_prog->use();
    process_prog->setFloat("u_height_tex", 0);
    process_prog->setFloat("u_vel_tex", 1);
    process_prog->setFloat("u_pos_tex", 2);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

bool HeightField::setupDiffusing() {

    // some text data
    float* u = new float[field_size * field_size];
    float* v = new float[field_size * field_size];
    int upper = 50;
    int lower = 30;
    for (int j = 0; j < field_size; ++j) {
        for (int i = 0; i < field_size; ++i) {
            u[j * field_size + i] = 0.0f;
            v[j * field_size + i] = 0.0f;
            if (i > lower && i < upper && j > lower && j < upper) {
                u[j * field_size + i] = 3.5;
            }
        }
    }

    glGenTextures(1, &tex_u0);
    glBindTexture(GL_TEXTURE_2D, tex_u0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, field_size, field_size, 0, GL_RED, GL_FLOAT, u);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &tex_u1);
    glBindTexture(GL_TEXTURE_2D, tex_u1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, field_size, field_size, 0, GL_RED, GL_FLOAT, u);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &tex_v0);
    glBindTexture(GL_TEXTURE_2D, tex_v0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, field_size, field_size, 0, GL_RED, GL_FLOAT, v);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &tex_v1);
    glBindTexture(GL_TEXTURE_2D, tex_v1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, field_size, field_size, 0, GL_RED, GL_FLOAT, v);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    glGenFramebuffers(1, &field_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, field_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_u0, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex_u1, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tex_v0, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, tex_v1, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Error: diffuse framebuffer not complete.\n");
        return false;
    }

    const char* frags[] = { "out_height", "out_vel" };

    field_prog = new Shader{ "Source/shaders/heightfield.vs", "Source/shaders/heightfield.fs" };
    field_prog->use();
    field_prog->setFloat("u_height_tex", 0);
    field_prog->setFloat("u_vel_tex", 1);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    delete[] v;
    delete[] u;
    u = NULL;
    v = NULL;
    return true;
}

void HeightField::update(float dt) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glViewport(0, 0, field_size, field_size);
    glBindFramebuffer(GL_FRAMEBUFFER, field_fbo);
    field_prog->use();
    glBindVertexArray(field_vao);
    glUniform1f(u_dt, dt);

    state_diffuse = 1 - state_diffuse;

    if (state_diffuse == 0) {
        // read from u0, write to u1
        // read from v0, write to v1
        GLenum drawbufs[] = { GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(1, drawbufs);

        glActiveTexture(GL_TEXTURE0);  glBindTexture(GL_TEXTURE_2D, tex_u0);
        glActiveTexture(GL_TEXTURE2);  glBindTexture(GL_TEXTURE_2D, tex_v0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    else {
        // read from u1, write to u0
        // read from v1, write to v0
        GLenum drawbufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(2, drawbufs);

        glActiveTexture(GL_TEXTURE0);  glBindTexture(GL_TEXTURE_2D, tex_u1);
        glActiveTexture(GL_TEXTURE3);  glBindTexture(GL_TEXTURE_2D, tex_v1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, win_w, win_h);
}

void HeightField::process() {
    glBindFramebuffer(GL_FRAMEBUFFER, process_fbo);
    glViewport(0, 0, field_size, field_size);
    glBindVertexArray(field_vao);

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, tex_u0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, tex_v0);

    {
        // Calculate positions.
        pos_prog->use();

        GLenum drawbufs[] = { GL_COLOR_ATTACHMENT0 }; // , GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 } ;
        glDrawBuffers(1, drawbufs);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    {
        // Use positions to calc normals, etc..
        process_prog->use();
        GLenum drawbufs[] = { GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(2, drawbufs);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, tex_out_pos);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HeightField::debugDraw() {
//    glViewport(0, 0, win_w, win_h);
//    glBindFramebuffer(GL_FRAMEBUFFER, 0); // tmp
//    glDisable(GL_DEPTH_TEST);
//
//    glBindVertexArray(vertices_vao);
//    debug_prog->use();
//
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, tex_out_pos);
//
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    //glDrawArrays(GL_TRIANGLES, 0, vertices.size());
//    glDrawArrays(GL_POINTS, 0, vertices.size());
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//#if 1
//    glBindFramebuffer(GL_READ_FRAMEBUFFER, field_fbo);
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//    glReadBuffer(GL_COLOR_ATTACHMENT0);
//    glBlitFramebuffer(0, 0, field_size, field_size, 0, 0, field_size, field_size, GL_COLOR_BUFFER_BIT, GL_LINEAR);
//#endif
}


void HeightField::print() {
    printf("heightfield.tex_u0: %d\n", tex_u0);
    printf("heightfield.tex_u1: %d\n", tex_u1);
    printf("heightfield.tex_v0: %d\n", tex_v0);
    printf("heightfield.tex_v1: %d\n", tex_v1);
    printf("heightfield.tex_out_norm: %d\n", tex_out_norm);
    printf("heightfield.tex_out_texcoord: %d\n", tex_out_texcoord);
    printf("heightfield.tex_out_pos: %d\n", tex_out_pos);
}