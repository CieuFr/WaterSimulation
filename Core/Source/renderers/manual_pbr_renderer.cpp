#include "manual_pbr_renderer.hpp"


void ManualPBRRenderer::init()
{

	camera = { glm::vec3(0.0f, 0.0f, 3.0f) };

     lightingShader = new Shader{ "Source/shaders/1.colors.vs", "Source/shaders/1.colors.fs" };
     lightCubeShader = new Shader{ "Source/shaders/1.light_cube.vs", "Source/shaders/1.light_cube.fs" };
     skyboxShader = new Shader{ "Source/shaders/skybox.vs", "Source/shaders/skybox.fs" };
     waterShader = new Shader{ "Source/shaders/water.vs", "Source/shaders/water.fs" };
     waterMovement = new Shader{ "Source/shaders/screen_space_quad.vs", "Source/shaders/water_movement.fs" };
     dropOnWater = new Shader{ "Source/shaders/screen_space_quad.vs", "Source/shaders/drop_on_water.fs" };
     debugTextureShader = new Shader{ "Source/shaders/debug_texture.vs", "Source/shaders/debug_texture.fs" };
     wallShader = new Shader{ "Source/shaders/manual_pbr.vs", "Source/shaders/water.fs" };
     normalShader = new Shader{ "Source/shaders/manual_pbr.vs", "Source/shaders/normal_water.fs" };


     cube = new Cube();
     skybox = new Skybox();
     sphere = new Sphere();
    // water = new WaterSurface(10,10,2,0.2);
     water = new WaterSurface();

     //heightfield = new HeightField();
     //heightfield->setup(256,256);

   
     defferedQuad = new Quad();

     back = new Quad();
     right = new Quad();
     left = new Quad();
     top = new Quad();
     bot = new Quad();

     initCornellBox();

     initWater();

     // INIT STATIC UNIFORM PARAMETER 
     skyboxShader->use();
     skyboxShader->setInt("skybox", 0);

     waterShader->use();
     waterShader->setVec3("albedo", 0.5f, 0.0f, 0.0f);
     waterShader->setFloat("ao", 1.0f);
     waterShader->setMat4("projection", projection);
     Vec2f resolution = Vec2f(SCR_WIDTH, SCR_HEIGHT);
     waterShader->setVec2("resolution", resolution);


     wallShader->use();
     wallShader->setVec3("albedo", 0.5f, 0.0f, 0.0f);
     wallShader->setFloat("ao", 1.0f);
     wallShader->setMat4("projection", projection);
     wallShader->setVec2("resolution", resolution);


}

void ManualPBRRenderer::render()
{
    model = glm::mat4(1.0f);
    view = camera.GetViewMatrix();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    renderWater();

    renderCornellBox();

    // render light source (simply re-render sphere at light positions)
     // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
     // keeps the codeprint small.
    for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
    {
        glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
        newPos = lightPositions[i];
        wallShader->setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
        wallShader->setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        model = glm::mat4(1.0f);
        model = glm::translate(model, newPos);
        model = glm::scale(model, glm::vec3(0.5f));
        wallShader->setMat4("model", model);
        wallShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        sphere->render();
        waterShader->use();
        waterShader->setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
        waterShader->setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        wallShader->use();
    }

    // render skybox 
    skyboxShader->use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    skyboxShader->setMat4("view", view);
    skyboxShader->setMat4("projection", projection);
    skybox->render();

}


void ManualPBRRenderer::initCornellBox() {
    Vec3f scale = Vec3f(4.0f, 4.0f, 4.0f);

    // back
    modelBack = glm::translate(modelBack, Vec3f(0.0f, 0.0f, -3.0f));
    modelBack = glm::scale(modelBack, scale);

    // top
    modelTop = glm::translate(modelTop, Vec3f(0.0f, 3.0f, 0.0f));
    modelTop = glm::scale(modelTop, scale);
    modelTop = glm::rotate(modelTop, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // bot
    modelBot = glm::translate(modelBot, Vec3f(0.0f, -3.0f, 0.0f));
    modelBot = glm::scale(modelBot, scale);
    modelBot = glm::rotate(modelBot, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // right
    modelRight = glm::translate(modelRight, Vec3f(3.0f, .0f, 0.0f));
    modelRight = glm::scale(modelRight, scale);
    modelRight = glm::rotate(modelRight, glm::radians(90.0f), glm::vec3(.0f, 1.0f, 0.0f));

    // left
    modelLeft = glm::translate(modelLeft, Vec3f(-3.0f, .0f, 0.0f));
    modelLeft = glm::scale(modelLeft, scale);
    modelLeft = glm::rotate(modelLeft, glm::radians(90.0f), glm::vec3(.0f, 1.0f, 0.0f));

}
void ManualPBRRenderer::renderCornellBox() {
   
    
    wallShader->use();
    wallShader->setMat4("view", view);
    wallShader->setVec3("camPos", camera.Position);
    // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
    // on direct lighting.
    wallShader->setFloat("metallic", metallic);
    wallShader->setFloat("roughness", roughness);
    model = glm::mat4(1.0f);
   
    // BACK
    wallShader->setVec3("albedo", colorBack);
    wallShader->setMat4("model", modelBack);
    wallShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelBack))));
    back->render();

    // TOP   
    wallShader->setVec3("albedo", colorTop);
    wallShader->setMat4("model", modelTop);
    wallShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelTop))));
    top->render();

    //BOT
    wallShader->setVec3("albedo", colorBot);
    wallShader->setMat4("model", modelBot);
    wallShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelBot))));

    bot->render();

    // RIGHT
    wallShader->setVec3("albedo", colorRight);
    waterShader->setMat4("model", modelRight);
    wallShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelRight))));

    right->render();

    // LEFT
    waterShader->setVec3("albedo", colorLeft);
    waterShader->setMat4("model", modelLeft);
    wallShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelLeft))));

    left->render();
    
}


void ManualPBRRenderer::initWater() {

    // REMOVE
    GLfloat inVertices[] = {

       -1.0f, 1.0f, 0.0,
       -1.0f, -1.0f, 0.0,
       1.0f, -1.0f, 0.0,

       -1.0f, 1.0f, 0.0,
       1.0f, -1.0f, 0.0,
       1.0f,  1.0f, 0.0 };

    glGenVertexArrays(1, &inVAO);
    glGenBuffers(1, &inVBO);
    glBindVertexArray(inVAO);
    glBindBuffer(GL_ARRAY_BUFFER, inVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(inVertices), inVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glBindVertexArray(0);
   

    float* randomData = new float[256 * 256 * 3]; // 256x256 pixels, 3 canaux (RGB)

    // Initialiser la graine de rand avec le temps actuel
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (int i = 0; i < 256 * 256 * 3; ++i) {
        randomData[i] = static_cast<float>(std::rand()) / RAND_MAX;
    }
    modelWater = glm::rotate(modelWater, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
   
    dropOnWater->use();
    dropOnWater->setInt("water", 0);
    waterMovement->use();
    dropOnWater->setInt("water", 0);
    
  
    glGenFramebuffers(2, waterHeightFBO);

    // The texture we're going to render to
    glGenTextures(2, waterHeightTexture);

    for (GLuint i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, waterHeightFBO[i]);
        glBindTexture(GL_TEXTURE_2D, waterHeightTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, waterHeightTexture[i], 0);
        GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
        // Always check that our framebuffer is ok
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }


  



    //Texture qui stock la surface d'eau
    // Charger les données des positions des sommets dans la texture
    //glGenFramebuffers(1, &waterHeightFBO);
    //glBindFramebuffer(GL_FRAMEBUFFER, waterHeightFBO);

    //glGenTextures(1, &waterHeightTexture);
    //glBindTexture(GL_TEXTURE_2D, waterHeightTexture);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, randomData);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, waterHeightTexture, 0);

    //unsigned int rbo;
    //glGenRenderbuffers(1, &rbo);
    //glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 256, 256);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    //// attach it to currently bound framebuffer object

    //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //glGenTextures(1, &waterHeightTexture);
    ////glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_FLOAT, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_FLOAT, randomData);
    //// Paramètres de la texture
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ////bind le fbo à la texture
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, waterHeightTexture, 0);
    //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //    std::cout << "Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

  
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

}

void ManualPBRRenderer::renderWater() {

    glViewport(0, 0, 256, 256);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   

    if (userHasClicked) {
        if (isClickOnWater()) {
            dropOnWater->use();
            glBindFramebuffer(GL_FRAMEBUFFER, waterHeightFBO[toggle]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, waterHeightTexture[1- toggle]);
            dropOnWater->setVec2("center", dropX, dropY);
            defferedQuad->render();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            userHasClicked = false;
            toggle = 1 - toggle;
        }
    }

    {
        normalShader->use();
        glBindFramebuffer(GL_FRAMEBUFFER, waterHeightFBO[toggle]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waterHeightTexture[1 - toggle]);
        normalShader->setFloat("delta", TEXELSIZE);
        defferedQuad->render();
        toggle = 1 - toggle;
    }

    {
        waterMovement->use();
        glBindFramebuffer(GL_FRAMEBUFFER, waterHeightFBO[toggle]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waterHeightTexture[1 - toggle]);
        waterMovement->setFloat("delta", TEXELSIZE);
        defferedQuad->render();
        toggle = 1 - toggle;
    }
    // WATER PROCESS
  
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    waterShader->use();
    waterShader->setMat4("view", view);
    waterShader->setVec3("camPos", camera.Position);
    // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
    // on direct lighting.
    waterShader->setFloat("metallic", metallic);
    waterShader->setFloat("roughness", roughness);
    model = glm::mat4(1.0f);
    waterShader->setMat4("model", modelWater);
    waterShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waterHeightTexture[1-toggle]);
    
    water->render();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // DEBUG
    glViewport(0, 0, 256, 256);
    debugTextureShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waterHeightTexture[0]);
   
    defferedQuad->render();

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

}

bool ManualPBRRenderer::isClickOnWater()
{
    dropX = (2.0f * lastX) / SCR_WIDTH - 1.0f;
    dropY = 1.0f - (2.0f * lastY) / SCR_HEIGHT;
    glm::vec4 drop_near = glm::vec4(dropX, dropY, 0, 1);
    glm::vec4 drop_far = glm::vec4(dropX, dropY, 1, 1);

    glm::mat4 inverse_MVP = glm::inverse(projection * view * modelWater);
    drop_near = inverse_MVP * drop_near;
    drop_far = inverse_MVP * drop_far;
    glm::vec3 drop_near_norm = glm::vec3(drop_near / drop_near.w);
    glm::vec3 drop_far_norm = glm::vec3(drop_far / drop_far.w);
    glm::vec3 direction = glm::normalize(glm::vec3(drop_far_norm - drop_near_norm));

    GLfloat distance;
    bool istrue = glm::intersectRayPlane(drop_near_norm, direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), distance);
    if (istrue) {
        glm::vec3 intersection_point = drop_near_norm + distance * direction;
        if (glm::abs(intersection_point.x) <= 1.0 && glm::abs(intersection_point.y) <= 1.0) {
            dropX = intersection_point.x * 0.5 + 0.5;
            dropY = intersection_point.y * 0.5 + 0.5;
            std::cout << dropX << "  " << dropY << std::endl;
            return true;
        }
    }
    return false;
}

void ManualPBRRenderer::displayUI()
{
    ImGui::Begin("ImGui Window");
    ImGui::Text("Text");
    ImGui::Checkbox("DrawTriangle", &drawTriangle);
    ImGui::SliderFloat("Rougness", &roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("Metallic", &metallic, 0.05f, 1.0f);

}

void ManualPBRRenderer::handleEvents(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(ROTATE_RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(ROTATE_RIGHT, deltaTime);
}


void ManualPBRRenderer::handleMouseMoveEvents(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void ManualPBRRenderer::handleMouseClickEvents(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
         userHasClicked = true;
         /*cameraRotEnabled = true; */
    }
    else if (button == GLFW_RELEASE) {
        /* cameraRotEnabled = false;*/
    }
}

void ManualPBRRenderer::handleScrollEvents(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


void ManualPBRRenderer::handleWindowResize(GLFWwindow* window, int width, int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}
