#include "test_reflection.hpp"


void TestReflection::init()
{

	camera = { glm::vec3(0.0f, 0.0f, 3.0f) };

    // build and compile our shader zprogram
    // ------------------------------------
     reflectionShader = new Shader{ "Core/Source/shaders/water.vs", "Core/Source/shaders/water.fs" };
     colorShader = new Shader{ "Core/Source/shaders/1.colors.vs", "Core/Source/shaders/1.colors.fs" };
     skyboxShader = new Shader{ "Core/Source/shaders/skybox.vs", "Core/Source/shaders/skybox.fs" };

     sphere = new Sphere();
     back = new Quad();
     right = new Quad();
     left = new Quad();
     top = new Quad();
     bot = new Quad();

     skybox = new Skybox();

     skyboxShader->use();
     skyboxShader->setInt("skybox", 0);
    
}

void TestReflection::render()
{
    model = glm::mat4(1.0f);
    view = camera.GetViewMatrix();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderCornellBox();

    colorShader->use();
    colorShader->setVec3("objectColor", 1.0f, .0f, .0f);
    colorShader->setMat4("projection", projection);
    colorShader->setMat4("view", view);
    colorShader->setMat4("model", model);
    sphere->render();


    // render skybox 
    skyboxShader->use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    skyboxShader->setMat4("view", view);
    skyboxShader->setMat4("projection", projection);
    skybox->render();


}


void TestReflection::renderCornellBox() {
    Vec3f scale = Vec3f(4.0f, 4.0f, 4.0f);

    colorShader->use();
    colorShader->setVec3("objectColor", 1.0f, 1.0f, 1.0f);
    colorShader->setMat4("projection", projection);
    colorShader->setMat4("view", view);
    Mat4f modelTransform = MAT4F_ID;

    // BACK
    modelTransform = glm::translate(modelTransform, Vec3f(0.0f, 0.0f, -3.0f));
    modelTransform = glm::scale(modelTransform, scale);
    //modelTransform = glm::rotate(modelTransform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    colorShader->setMat4("model", modelTransform);
    back->render();

    // TOP
    modelTransform = MAT4F_ID;
    modelTransform = glm::translate(model, Vec3f(0.0f, 3.0f, 0.0f));
    modelTransform = glm::scale(modelTransform, scale);
    modelTransform = glm::rotate(modelTransform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    colorShader->setMat4("model", modelTransform);
    top->render();
   
    //BOT
    modelTransform = MAT4F_ID;
    modelTransform = glm::translate(model, Vec3f(0.0f, -3.0f, 0.0f));
    modelTransform = glm::scale(modelTransform, scale);
    modelTransform = glm::rotate(modelTransform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    colorShader->setMat4("model", modelTransform);
    bot->render();


    // RIGHT
    colorShader->setVec3("objectColor", 0.0f, 1.0f, 0.0f);
    modelTransform = MAT4F_ID;
    modelTransform = glm::translate(model, Vec3f(3.0f, .0f, 0.0f));
    modelTransform = glm::scale(modelTransform, scale);
    modelTransform = glm::rotate(modelTransform, glm::radians(90.0f), glm::vec3(.0f, 1.0f, 0.0f));
    colorShader->setMat4("model", modelTransform);
    right->render();

    // LEFT
    colorShader->setVec3("objectColor", 1.0f, 0.0f, 0.0f);
    modelTransform = MAT4F_ID;
    modelTransform = glm::translate(model, Vec3f(-3.0f, .0f, 0.0f));
    modelTransform = glm::scale(modelTransform, scale);
    modelTransform = glm::rotate(modelTransform, glm::radians(90.0f), glm::vec3(.0f, 1.0f, 0.0f));
    colorShader->setMat4("model", modelTransform);
    left->render();


}


void TestReflection::displayUI()
{
    ImGui::Begin("ImGui Window");
    ImGui::Text("Text");
    ImGui::Checkbox("DrawTriangle", &drawTriangle);
    ImGui::SliderFloat("Rougness", &roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("Metallic", &metallic, 0.05f, 1.0f);

}

void TestReflection::handleEvents(GLFWwindow* window,float deltaTime)
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


void TestReflection::handleMouseEvents(GLFWwindow* window, double xposIn, double yposIn)
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


void TestReflection::handleScrollEvents(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}