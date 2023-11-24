#include "ibl_pbr_renderer.hpp"


void IBLPBRRenderer::init()
{

	camera = { glm::vec3(0.0f, 0.0f, 3.0f) };

    // build and compile our shader zprogram
    // ------------------------------------
     lightingShader = new Shader{ "Core/Source/shaders/1.colors.vs", "Core/Source/shaders/1.colors.fs" };
     lightCubeShader = new Shader{ "Core/Source/shaders/1.light_cube.vs", "Core/Source/shaders/1.light_cube.fs" };
     skyboxShader = new Shader{ "Core/Source/shaders/skybox.vs", "Core/Source/shaders/skybox.fs" };
     manualPBR = new Shader{ "Core/Source/shaders/manual_pbr.vs", "Core/Source/shaders/manual_pbr.fs" };

     cube = new Cube();
     skybox = new Skybox();
     sphere = new Sphere();
     // INIT STATIC UNIFORM PARAMETER 
     skyboxShader->use();
     skyboxShader->setInt("skybox", 0);

     manualPBR->use();
     manualPBR->setVec3("albedo", 0.5f, 0.0f, 0.0f);
     manualPBR->setFloat("ao", 1.0f);
     manualPBR->use();
     manualPBR->setMat4("projection", projection);

}

void IBLPBRRenderer::render()
{
    model = glm::mat4(1.0f);
    view = camera.GetViewMatrix();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //// be sure to activate shader when setting uniforms/drawing objects
        //lightingShader.use();
        //lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        //lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

        //// view/projection transformation
        //lightingShader.setMat4("projection", projection);
        //lightingShader.setMat4("view", view);
        //// world transformation
        //lightingShader.setMat4("model", model);
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


void IBLPBRRenderer::displayUI()
{
    ImGui::Begin("ImGui Window");
    ImGui::Text("Text");
    ImGui::Checkbox("DrawTriangle", &drawTriangle);
    ImGui::SliderFloat("Rougness", &roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("Metallic", &metallic, 0.05f, 1.0f);

}

void IBLPBRRenderer::handleEvents(GLFWwindow* window,float deltaTime)
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


void IBLPBRRenderer::handleMouseEvents(GLFWwindow* window, double xposIn, double yposIn)
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


void IBLPBRRenderer::handleScrollEvents(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}