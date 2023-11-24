#include "manual_pbr_renderer.hpp"


void ManualPBRRenderer::init()
{

	camera = { glm::vec3(0.0f, 0.0f, 3.0f) };

    // build and compile our shader zprogram
    // ------------------------------------
  /*   lightingShader = new Shader{ "Core/Source/shaders/1.colors.vs", "Core/Source/shaders/1.colors.fs" };
     lightCubeShader = new Shader{ "Core/Source/shaders/1.light_cube.vs", "Core/Source/shaders/1.light_cube.fs" };
     skyboxShader = new Shader{ "Core/Source/shaders/skybox.vs", "Core/Source/shaders/skybox.fs" };
     manualPBR = new Shader{ "Core/Source/shaders/water.vs", "Core/Source/shaders/water.fs" };*/

     lightingShader = new Shader{ "Source/shaders/1.colors.vs", "Source/shaders/1.colors.fs" };
     lightCubeShader = new Shader{ "Source/shaders/1.light_cube.vs", "Source/shaders/1.light_cube.fs" };
     skyboxShader = new Shader{ "Source/shaders/skybox.vs", "Source/shaders/skybox.fs" };
     manualPBR = new Shader{ "Source/shaders/water.vs", "Source/shaders/water.fs" };

     cube = new Cube();
     skybox = new Skybox();
     sphere = new Sphere();
     back = new Quad();
     right = new Quad();
     left = new Quad();
     top = new Quad();
     bot = new Quad();

     initCornellBox();

     // INIT STATIC UNIFORM PARAMETER 
     skyboxShader->use();
     skyboxShader->setInt("skybox", 0);

     manualPBR->use();
     manualPBR->setVec3("albedo", 0.5f, 0.0f, 0.0f);
     manualPBR->setFloat("ao", 1.0f);
     manualPBR->use();
     manualPBR->setMat4("projection", projection);


     quadData.push_back({ modelBack,colorBack});
     quadData.push_back({ modelTop,colorTop });
     quadData.push_back({ modelBot,colorBot });
     quadData.push_back({ modelRight,colorRight });
     quadData.push_back({ modelLeft,colorLeft });

     // Création du SSBO
     GLuint ssbo;
     glGenBuffers(1, &ssbo);
     glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
     glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(QuadData) * quadData.size(), quadData.data(), GL_STATIC_DRAW);
     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
     glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void ManualPBRRenderer::render()
{
    model = glm::mat4(1.0f);
    view = camera.GetViewMatrix();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    manualPBR->use();
    manualPBR->setMat4("view", view);
    manualPBR->setVec3("camPos", camera.Position);
    // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
    // on direct lighting.
    manualPBR->setFloat("metallic", metallic);
    manualPBR->setFloat("roughness", roughness);
    model = glm::mat4(1.0f);
    manualPBR->setMat4("model", model);
    manualPBR->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
    // render the cube
    sphere->render();

    renderCornellBox();

    // render light source (simply re-render sphere at light positions)
     // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
     // keeps the codeprint small.
    for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
    {
        glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
        newPos = lightPositions[i];
        manualPBR->setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
        manualPBR->setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

        model = glm::mat4(1.0f);
        model = glm::translate(model, newPos);
        model = glm::scale(model, glm::vec3(0.5f));
        manualPBR->setMat4("model", model);
        manualPBR->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        sphere->render();
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
    
    manualPBR->use();

   
    // BACK
    manualPBR->setVec3("albedo", colorBack);
    manualPBR->setMat4("model", modelBack);
    back->render();

    // TOP   
    manualPBR->setVec3("albedo", colorTop);
    manualPBR->setMat4("model", modelTop);
    top->render();

    //BOT
    manualPBR->setVec3("albedo", colorBot);
    manualPBR->setMat4("model", modelBot);
    bot->render();

    // RIGHT
    manualPBR->setVec3("albedo", colorRight);
    manualPBR->setMat4("model", modelRight);
    right->render();

    // LEFT
    manualPBR->setVec3("albedo", colorLeft);
    manualPBR->setMat4("model", modelLeft);
    left->render();


}


void ManualPBRRenderer::displayUI()
{
    ImGui::Begin("ImGui Window");
    ImGui::Text("Text");
    ImGui::Checkbox("DrawTriangle", &drawTriangle);
    ImGui::SliderFloat("Rougness", &roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("Metallic", &metallic, 0.05f, 1.0f);

}

void ManualPBRRenderer::handleEvents(GLFWwindow* window,float deltaTime)
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


void ManualPBRRenderer::handleMouseEvents(GLFWwindow* window, double xposIn, double yposIn)
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


void ManualPBRRenderer::handleScrollEvents(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}