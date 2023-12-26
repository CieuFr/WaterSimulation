#include "manual_pbr_renderer.hpp"


void ManualPBRRenderer::init()
{
	 camera = { glm::vec3(0.0f, 0.0f, 3.0f) };

     lightingShader = new Shader{ "Source/shaders/1.colors.vs", "Source/shaders/1.colors.fs" };
     lightCubeShader = new Shader{ "Source/shaders/1.light_cube.vs", "Source/shaders/1.light_cube.fs" };
     skyboxShader = new Shader{ "Source/shaders/skybox.vs", "Source/shaders/skybox.fs" };
     waterShader = new Shader{ "Source/shaders/water.vs", "Source/shaders/water.fs" };
     debugTextureShader = new Shader{ "Source/shaders/debug_texture.vs", "Source/shaders/debug_texture.fs" };
     wallShader = new Shader{ "Source/shaders/manual_pbr.vs", "Source/shaders/manual_pbr.fs" };
     normalShader = new Shader{ "Source/shaders/manual_pbr.vs", "Source/shaders/normal_water.fs" };

     cube = new Cube();
     skybox = new Skybox();
     sphere = new Sphere();
     
     physicScene = new PhysicsScene();
     physicScene->addSphere(1.0,1.0,Vec3f(0.0,0.0,0.1));
   
     defferedQuad = new Quad();

     back = new Quad();
     right = new Quad();
     left = new Quad();
     top = new Quad();
     bot = new Quad(); 

     initCornellBox();

     initRayTracedObjects();

     initWater();

     // INIT STATIC UNIFORM PARAMETER 
     skyboxShader->use();
     skyboxShader->setInt("skybox", 0);

     waterShader->use();
     waterShader->setVec3("albedo", 0.45f, 0.8f, 0.95f);
     waterShader->setFloat("ao", 1.0f);
     waterShader->setMat4("projection", projection);
     Vec2f resolution = Vec2f(SCR_WIDTH, SCR_HEIGHT);
     waterShader->setVec2("resolution", resolution);
     waterShader->setInt("skybox", 0);

     wallShader->use();
     wallShader->setVec3("albedo", 0.5f, 0.0f, 0.0f);
     wallShader->setFloat("ao", 1.0f);
     wallShader->setMat4("projection", projection);
     wallShader->setMat4("model", MAT4F_ID);

     wallShader->setVec2("resolution", resolution);
     wallShader->setBool("isSphere", false);


}

void ManualPBRRenderer::render()
{
    model = glm::mat4(1.0f);
    view = camera.GetViewMatrix();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    if (userHasClicked && !selectionRayLaunched) {
        Vec3f ray = getRayFromClick(lastX, lastY);
        physicScene->selectObject(camera.Position, ray);
        selectionRayLaunched = true;
    }

    // apply physic to water
     physicScene->simulate(deltaTime);

    // render light source (simply re-render sphere at light positions)
     // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
     // keeps the codeprint small.
    for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
    {
        wallShader->use();
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
    }


    renderWater();

    renderCornellBox();

    renderSphere();

    // render skybox 
    skyboxShader->use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    skyboxShader->setMat4("view", view);
    skyboxShader->setMat4("projection", projection);
    skybox->render();


    if (userHasClicked && !clickEnded ) {
        if (physicScene->isObjectSelected) {
            dragObject();
        }
    }

    // RELACHER LE CLICK
    if (userHasClicked && clickEnded) {
        userHasClicked = false;
        clickEnded = false;
        selectionRayLaunched = false;
        physicScene->unSelectObject();
    }

    updateDeltaTime();
}

void ManualPBRRenderer::initRayTracedObjects() {

    // CORNELL BOX
    MeshGPUQuad back;
    back.albedo = colorBack;
    back.metallic = 0.5f;
    back.roughness = 0.5f;
    back.model = modelBack;
    for (int i = 0; i < 4; i++) {
        back.normals[i] = VEC3F_ZERO;
    }
    back.vertices[0] = Vec3f(-1.0f, 1.0f, 0.0f);
    back.vertices[1] = Vec3f(-1.0f, -1.0f, 0.0f);
    back.vertices[2] = Vec3f(1.0f, 1.0f, 0.0f);
    back.vertices[3] = Vec3f(1.0f, -1.0f, 0.0f);

    back.uvs[0] = Vec2f(0.0f, 1.0f);
    back.uvs[1] = Vec2f(0.0f, 0.0f);
    back.uvs[2] = Vec2f(1.0f, 1.0f);
    back.uvs[3] = Vec2f(1.0f, 0.0f);
    back.nb_vertices = 4;

    MeshGPUQuad right;
    right.albedo = colorRight;
    right.metallic = 0.5f;
    right.roughness = 0.5f;
    right.model = modelRight;
    for (int i = 0; i < 4; i++) {
        right.normals[i] = VEC3F_ZERO;
    }
    right.vertices[0] = Vec3f(-1.0f, 1.0f, 0.0f);
    right.vertices[1] = Vec3f(-1.0f, -1.0f, 0.0f);
    right.vertices[2] = Vec3f(1.0f, 1.0f, 0.0f);
    right.vertices[3] = Vec3f(1.0f, -1.0f, 0.0f);

    right.uvs[0] = Vec2f(0.0f, 1.0f);
    right.uvs[1] = Vec2f(0.0f, 0.0f);
    right.uvs[2] = Vec2f(1.0f, 1.0f);
    right.uvs[3] = Vec2f(1.0f, 0.0f);
    right.nb_vertices = 4;


    MeshGPUQuad left;
    left.albedo = colorLeft;
    left.metallic = 0.5f;
    left.roughness = 0.5f;
    left.model = modelLeft;
    for (int i = 0; i < 4; i++) {
        left.normals[i] = VEC3F_ZERO;
    }
    left.vertices[0] = Vec3f(-1.0f, 1.0f, 0.0f);
    left.vertices[1] = Vec3f(-1.0f, -1.0f, 0.0f);
    left.vertices[2] = Vec3f(1.0f, 1.0f, 0.0f);
    left.vertices[3] = Vec3f(1.0f, -1.0f, 0.0f);

    left.uvs[0] = Vec2f(0.0f, 1.0f);
    left.uvs[1] = Vec2f(0.0f, 0.0f);
    left.uvs[2] = Vec2f(1.0f, 1.0f);
    left.uvs[3] = Vec2f(1.0f, 0.0f);
    left.nb_vertices = 4;


    MeshGPUQuad bot;
    bot.albedo = colorBot;
    bot.metallic = 0.5f;
    bot.roughness = 0.5f;
    bot.model = modelBot;
    for (int i = 0; i < 4; i++) {
        bot.normals[i] = VEC3F_ZERO;
    }
    bot.vertices[0] = Vec3f(-1.0f, 1.0f, 0.0f);
    bot.vertices[1] = Vec3f(-1.0f, -1.0f, 0.0f);
    bot.vertices[2] = Vec3f(1.0f, 1.0f, 0.0f);
    bot.vertices[3] = Vec3f(1.0f, -1.0f, 0.0f);

    bot.uvs[0] = Vec2f(0.0f, 1.0f);
    bot.uvs[1] = Vec2f(0.0f, 0.0f);
    bot.uvs[2] = Vec2f(1.0f, 1.0f);
    bot.uvs[3] = Vec2f(1.0f, 0.0f);
    bot.nb_vertices = 4;


    MeshGPUQuad top;
    top.albedo = colorTop;
    top.metallic = 0.5f;
    top.roughness = 0.5f;
    top.model = modelTop;
    for (int i = 0; i < 4; i++) {
        top.normals[i] = VEC3F_ZERO;
    }
    top.vertices[0] = Vec3f(-1.0f, 1.0f, 0.0f);
    top.vertices[1] = Vec3f(-1.0f, -1.0f, 0.0f);
    top.vertices[2] = Vec3f(1.0f, 1.0f, 0.0f);
    top.vertices[3] = Vec3f(1.0f, -1.0f, 0.0f);

    top.uvs[0] = Vec2f(0.0f, 1.0f);
    top.uvs[1] = Vec2f(0.0f, 0.0f);
    top.uvs[2] = Vec2f(1.0f, 1.0f);
    top.uvs[3] = Vec2f(1.0f, 0.0f);
    top.nb_vertices = 4;


    rayTracedMeshes.push_back(back);
    rayTracedMeshes.push_back(top);
    rayTracedMeshes.push_back(bot);
    rayTracedMeshes.push_back(right);
    rayTracedMeshes.push_back(left);


   for (size_t i = 0; i < physicScene->physicalObjects.size(); i++)
    {
        MeshGPUSphere sphere;
        sphere.albedo = colorTop;
        sphere.metallic = 0.5f;
        sphere.roughness = 0.5f;
        sphere.model = physicScene->physicalObjects[i]->model;
        sphere.nb_vertices = physicScene->physicalObjects[i]->positions.size();
        sphere.nb_indices = physicScene->physicalObjects[i]->indices.size();
        std::cout << "aie1  " << sphere.nb_indices << std::endl;
        std::cout << "aie2  " << sphere.nb_vertices << std::endl;
        

        for (size_t ii = 0; ii < physicScene->physicalObjects[i]->positions.size(); ii++) {
            sphere.vertices[ii] = physicScene->physicalObjects[i]->positions[ii];
            sphere.uvs[ii] = physicScene->physicalObjects[i]->uv[ii];
            sphere.normals[ii] = physicScene->physicalObjects[i]->normals[ii];
            
        }
        for (int ii = 0; ii < sphere.nb_indices; ii++) {
            sphere.indices[ii] = physicScene->physicalObjects[i]->indices[ii];
        }
        rayTracedMeshesSphere.push_back(sphere);
    }

    /*nb_meshes_gpu = 5 + physicScene->physicalObjects.size();*/
    
    std::cout << physicScene->physicalObjects[0]->indices.size() << std::endl;

}
void ManualPBRRenderer::initCornellBox() {
    Vec3f scale = Vec3f(4.0f, 4.0f, 4.0f);

    // back
    modelBack = glm::translate(modelBack, Vec3f(0.0f, 0.0f, -3.0f));
    modelBack = glm::scale(modelBack, scale);

    // top
    modelTop = glm::translate(modelTop, Vec3f(0.0f, 3.0f, 0.0f));
    modelTop = glm::scale(modelTop, scale);
    modelTop = glm::rotate(modelTop, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));

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
    wallShader->setBool("isSphere", false);
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
    wallShader->setMat4("model", modelRight);
    wallShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelRight))));

    right->render();

    // LEFT
    wallShader->setVec3("albedo", colorLeft);
    wallShader->setMat4("model", modelLeft);
    wallShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelLeft))));

    left->render();

}


void ManualPBRRenderer::renderSphere() {

    wallShader->use();
    // RENDER SPHERE TODO MOVE 
    wallShader->setVec3("albedo", colorLeft);
    wallShader->setBool("isSphere", true);

    if (physicScene->physicalObjects.size() != 0) {
        wallShader->setMat4("model", physicScene->physicalObjects[0]->model);
        wallShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(physicScene->physicalObjects[0]->model))));
        physicScene->render();
    }


}
Vec3f ManualPBRRenderer::getRayFromClick(float x, float y) {

    float normalizedX = (2.0f * x) / SCR_WIDTH - 1.0f;
    float normalizedY = 1.0f - (2.0f * y) / SCR_HEIGHT;

    glm::mat4 inverseProjection = glm::inverse(projection);
    glm::mat4 inverseView = glm::inverse(view);

    glm::vec4 rayClip(normalizedX, normalizedY, -1.0f, 1.0f);
    
    glm::vec4 rayEye = inverseProjection * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec4 rayWorld = inverseView * rayEye;
    glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorld));

    return rayDir;
}

void ManualPBRRenderer::dragObject() {
    
    GLfloat distance;
    Vec3f rayDir = getRayFromClick(lastX, lastY);
    

    Vec3f objWorld = physicScene->physicalObjects[0]->pos;
    Vec3f vecteurSphereCamera = camera.Position - objWorld;
    bool istrue = glm::intersectRayPlane(camera.Position, rayDir, objWorld, vecteurSphereCamera, distance);
    if (istrue) {
        glm::vec3 intersection_point = camera.Position + distance * rayDir;
        physicScene->moveObject(intersection_point);
    }
    
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    waterShader->use();

    // TODO MODIFY SHADER IF MODIFY HERE
    for (int i = 0; i < 5; ++i)
    {
        using namespace std::literals;
        std::string prefix = "meshQuads["s + std::to_string(i);
        MeshGPUQuad& mesh = rayTracedMeshes[i];
        waterShader->setFloat(std::string(prefix + "].roughness"s), mesh.roughness);
        waterShader->setFloat(std::string(prefix + "].metallic"s), mesh.metallic);
        waterShader->setVec3(std::string(prefix + "].albedo"s), mesh.albedo);
        waterShader->setMat4(std::string(prefix + "].model"s), mesh.model);

        const int nb_mesh = 4;
        for (size_t j = 0; j < nb_mesh; j++)
        {
            waterShader->setVec3(std::string(prefix + "].vertices[" + std::to_string(j) + "]"s), mesh.vertices[j]);
            waterShader->setVec3(std::string(prefix + "].normals[" + std::to_string(j) + "]"s), mesh.normals[j]);
            waterShader->setVec2(std::string(prefix + "].uvs[" + std::to_string(j) + "]"s), mesh.uvs[j]);
        }
    }

    for (int i = 0; i < 1; ++i)
    {
        using namespace std::literals;
        std::string prefix = "meshSphere["s + std::to_string(i);
        MeshGPUSphere& mesh = rayTracedMeshesSphere[i];
        waterShader->setFloat(std::string(prefix + "].roughness"s), mesh.roughness);
        waterShader->setFloat(std::string(prefix + "].metallic"s), mesh.metallic);
        waterShader->setVec3(std::string(prefix + "].albedo"s), mesh.albedo);
        waterShader->setMat4(std::string(prefix + "].model"s), mesh.model);
        waterShader->setInt(std::string(prefix + "].nb_indices"s), mesh.nb_indices);
        waterShader->setInt(std::string(prefix + "].nb_vertices"s), mesh.nb_vertices);

        // TODO MODIFY SHADER IF MODIFY HERE
        //HARCODED 8x8 SPHERE
        for (size_t j = 0; j < mesh.nb_vertices; j++)
        {
            waterShader->setVec3(std::string(prefix + "].vertices[" + std::to_string(j) + "]"s), mesh.vertices[j]);
            waterShader->setVec3(std::string(prefix + "].normals[" + std::to_string(j) + "]"s), mesh.normals[j]);
            waterShader->setVec2(std::string(prefix + "].uvs[" + std::to_string(j) + "]"s), mesh.uvs[j]);
            
        }
        for (size_t j = 0; j < mesh.nb_indices; j++)
        {
            waterShader->setInt(std::string(prefix + "].indices[" + std::to_string(j) + "]"s), mesh.indices[j]);
        }
    }


}

void ManualPBRRenderer::renderWater() {

    glViewport(0, 0, physicScene->water->numX, physicScene->water->numZ);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (userHasClicked && clickEnded) {
        if (isClickOnWater()) {
            physicScene->water->waterDrop(dropX, dropY);
        }
    }

    // WATER PROCESS
    physicScene->water->update(deltaTime);
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
    waterShader->setMat4("model", physicScene->water->modelWater);
    //waterShader->setMat4("modelSphere", physicScene->physicalObjects[0]->model);
    // TODO sphere.model = physicScene->physicalObjects[i]->model;
    for (int i = 0; i < 1; ++i)
    {
        using namespace std::literals;
        std::string prefix = "meshSphere["s + std::to_string(i);
        waterShader->setMat4(std::string(prefix + "].model"s), physicScene->physicalObjects[i]->model);

    }
    waterShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
    waterShader->setFloat("u_dx", physicScene->water->spacing / physicScene->water->sizea);
    waterShader->setFloat("u_dz", physicScene->water->spacing / physicScene->water->sizeb);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, physicScene->water->waterHeightTexture[1-physicScene->water->toggle]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubemapTexture);
   /* glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,physicScene->physicalObjects[physicScene->indexSphereSelected]->geometryTexture[0]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, physicScene->physicalObjects[physicScene->indexSphereSelected]->geometryTexture[1]);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, physicScene->physicalObjects[physicScene->indexSphereSelected]->geometryTexture[2]);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, physicScene->physicalObjects[physicScene->indexSphereSelected]->geometryTexture[3]);*/

    physicScene->water->render();

    // DEBUG
    glViewport(0, 0, physicScene->water->numX, physicScene->water->numZ);
    debugTextureShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, physicScene->water->waterHeightTexture[1-physicScene->water->toggle]);
   
    defferedQuad->render();

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

}

bool ManualPBRRenderer::isClickOnWater()
{
    GLfloat distance;
    Vec3f rayDir = getRayFromClick(lastX, lastY);
    Vec3f rayOrigin = camera.Position;

    Vec3f posPlan = Vec3f(physicScene->water->modelWater * Vec4f(physicScene->water->vertexPositions[0],1));
    // NORMAL VERS LE HAUT ATTENTION ! 
    Vec3f normalPlan = Vec3f(0.0f, 1.0f, 0.0f);

    bool istrue = glm::intersectRayPlane(rayOrigin, rayDir, posPlan, normalPlan, distance);
    if (istrue) {
            Vec3f P = camera.Position + distance * rayDir;
            Vec3f D = Vec3f(-physicScene->tankSize.x / 2, 0, -physicScene->tankSize.z / 2);
            Vec3f C = Vec3f(-physicScene->tankSize.x / 2, 0, physicScene->tankSize.z / 2);
            Vec3f A = Vec3f(physicScene->tankSize.x / 2, 0, -physicScene->tankSize.z / 2);
            Vec3f B = Vec3f(physicScene->tankSize.x / 2, 0, physicScene->tankSize.z / 2);

            Vec3f AB = B - A;
            Vec3f BC = C - B;
            Vec3f CD = D - C;
            Vec3f DA = A - D;

            Vec3f AP = P - A;
            Vec3f BP = P - B;
            Vec3f CP = P - C;
            Vec3f DP = P - D;

            Vec3f crossABAP = glm::cross(AB, AP);
            Vec3f crossBCBP = glm::cross(BC, BP);
            Vec3f crossCDCP = glm::cross(CD, CP);
            Vec3f crossDADP = glm::cross(DA, DP);

             if ((glm::dot(crossABAP, crossBCBP) > 0) && (glm::dot(crossBCBP, crossCDCP) > 0) && (glm::dot(crossCDCP, crossDADP) > 0) && (glm::dot(crossDADP, crossABAP) > 0)) {
                 dropX = P.x / physicScene->tankSize.x + 0.5;
                 dropY = P.z / -physicScene->tankSize.z + 0.5;
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

    if(rightClickPressed)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

void ManualPBRRenderer::handleMouseClickEvents(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !userHasClicked) {
         userHasClicked = true;
    }
    else if (button == GLFW_RELEASE && !clickEnded) {
        clickEnded = true;
       
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        rightClickPressed = true;  
    }
    else {
        rightClickPressed = false;
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

void ManualPBRRenderer::updateDeltaTime() {
    // Measure speed
    double currentTime = glfwGetTime();
    nbFrames++;
    if (currentTime - lastTime >= 1.0) { // If last cout was more than 1 sec ago
        //std::cout << double(nbFrames) << std::endl;
        deltaTime = 1.f / nbFrames;
        nbFrames = 0;
        lastTime += 1.0;
    }
}