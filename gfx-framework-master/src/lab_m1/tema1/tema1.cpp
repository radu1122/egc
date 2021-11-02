#include "lab_m1/tema1/tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/tema1/transform2D.h"
#include "lab_m1/tema1/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


tema1::tema1()
{
}


tema1::~tema1()
{
}

void tema1::addObstacles() {

    obstacol obs1;
    obs1.x = 3;
    obs1.y = 2;
    obs1.scaleX = .5f;
    obs1.scaleY = .3f;
    obstacoles.push_back(obs1);

    obs1.x = 3;
    obs1.y = 6;
    obs1.scaleX = .5f;
    obs1.scaleY = .3f;
    obstacoles.push_back(obs1);


    obs1.x = 6;
    obs1.y = 6;
    obs1.scaleX = .4f;
    obs1.scaleY = .3f;
    obstacoles.push_back(obs1);

    obs1.x = 6;
    obs1.y = 2;
    obs1.scaleX = .4f;
    obs1.scaleY = .3f;
    obstacoles.push_back(obs1);

    obs1.x = 10;
    obs1.y = 6;
    obs1.scaleX = .2f;
    obs1.scaleY = .6f;
    obstacoles.push_back(obs1);

    obs1.x = 10;
    obs1.y = 2;
    obs1.scaleX = .3f;
    obs1.scaleY = .4f;
    obstacoles.push_back(obs1);
}
void tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, resolutionX, 0, resolutionY, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    logicSpace.x = 0;
    logicSpace.y = 0;
    logicSpace.width = (float)12.8;
    logicSpace.height = (float)7.2;

    float squareSide = 1;

    glm::vec3 corner = glm::vec3(0, 0, 0);

    cx = corner.x + squareSide / 2;
    cy = corner.y + squareSide / 2;

    playerX = logicSpace.width / 2;
    playerY = logicSpace.height / 2;
    playerAngle = 0;


    Mesh* square1 = object2D::CreateSquare("square1", corner, squareSide, glm::vec3(1, 0, 0), true);
    AddMeshToList(square1);

    Mesh* squareEye = object2D::CreateSquare("squareEye", corner, squareSide, glm::vec3(151, 1, 0), true);
    AddMeshToList(squareEye);

    Mesh* mapMargin = object2D::CreateSquare("mapMargin", corner, squareSide, glm::vec3(.115f, .180f, .246f), true);
    AddMeshToList(mapMargin);

    Mesh* obstacolMesh = object2D::CreateSquare("obstacol", corner, squareSide, glm::vec3(0, 1, 0), true);
    AddMeshToList(obstacolMesh);
    addObstacles();

    Mesh* proiectil = object2D::CreateSquare("proiectil", corner, squareSide, glm::vec3(0, 0, 0), true);
    AddMeshToList(proiectil);
    addObstacles();
}




void tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

glm::mat3 tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}

void tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, .2f, .2f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}

void tema1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->GetResolution();


    // seteaza aria de desenare
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0), true);

    // compune matricea 2D de vizualizare
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);


    if (playerDead) {
        glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    }
    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        if (Engine::GetElapsedTime() - it->time > 1) {
            it = projectiles.erase(it);
        } else {
            it->x += cos(it->angle) * deg2rad * 4;
            it->y += sin(it->angle) * deg2rad * 4;
            if (isObjectCollision(it->x, it->y)) {
                it = projectiles.erase(it);
            }
            else {
                ++it;
            }
        }
    }
    for (int i = 0; i < projectiles.size(); i++) {
        projectiles[i].x += cos(projectiles[i].angle) * deg2rad * 4;
        projectiles[i].y += sin(projectiles[i].angle) * deg2rad * 4;
    }
    DrawScene(visMatrix);
}


void tema1::FrameEnd()
{
}


void tema1::DrawScene(glm::mat3 visMatrix)
{

    float scale = .5f;
    modelMatrix = visMatrix * transform2D::Translate(playerX, playerY);
    modelMatrix *= transform2D::Scale(scale, scale);

    modelMatrix *= transform2D::Translate(cx, cy);
    modelMatrix *= transform2D::Rotate(playerAngle);
    modelMatrix *= transform2D::Translate(-cx, -cy);

    RenderMesh2D(meshes["squareEye"], shaders["VertexColor"], modelMatrix * transform2D::Translate(.7f, .0) * transform2D::Scale(.3f, .3f));
    RenderMesh2D(meshes["squareEye"], shaders["VertexColor"], modelMatrix * transform2D::Translate(.7f, .7f) * transform2D::Scale(.3f, .3f));
    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);


    // margini harta
    modelMatrix = visMatrix * transform2D::Translate(0, 0) * transform2D::Scale(.1f, logicSpace.height);
    RenderMesh2D(meshes["mapMargin"], shaders["VertexColor"], modelMatrix);
    modelMatrix = visMatrix * transform2D::Translate(logicSpace.width -.1f, 0) * transform2D::Scale(.1f, logicSpace.height);
    RenderMesh2D(meshes["mapMargin"], shaders["VertexColor"], modelMatrix);
    modelMatrix = visMatrix * transform2D::Translate(0, 0) * transform2D::Scale(logicSpace.width, .1f);
    RenderMesh2D(meshes["mapMargin"], shaders["VertexColor"], modelMatrix);
    modelMatrix = visMatrix * transform2D::Translate(0, 0) * transform2D::Scale(logicSpace.width, .1f);
    RenderMesh2D(meshes["mapMargin"], shaders["VertexColor"], modelMatrix);
    modelMatrix = visMatrix * transform2D::Translate(0, logicSpace.height - .1f) * transform2D::Scale(logicSpace.width, .1f);
    RenderMesh2D(meshes["mapMargin"], shaders["VertexColor"], modelMatrix);

    // obstacole
    for (int i = 0; i < obstacoles.size(); i++) {
        modelMatrix = visMatrix * transform2D::Translate(obstacoles[i].x, obstacoles[i].y) * transform2D::Scale(obstacoles[i].scaleX, obstacoles[i].scaleY);
        RenderMesh2D(meshes["obstacol"], shaders["VertexColor"], modelMatrix);
    }

    // proiectile
    for (int i = 0; i < projectiles.size(); i++) {
        float angle = atan2(playerY - projectiles[i].y, playerX - projectiles[i].x);
        modelMatrix = visMatrix * transform2D::Translate(projectiles[i].x, projectiles[i].y);
        modelMatrix *= transform2D::Rotate(angle);
        RenderMesh2D(meshes["proiectil"], shaders["VertexColor"], modelMatrix * transform2D::Scale(.15f, .15f));
    }

}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void tema1::OnInputUpdate(float deltaTime, int mods)
{
    deltaTime = deltaTime * 2;
    if (window->KeyHold(GLFW_KEY_W)) {
        playerY += deltaTime;
        if (isObjectCollision(playerX, playerY)) {
            playerY -= deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        playerX -= deltaTime;
        if (isObjectCollision(playerX, playerY)) {
            playerX += deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        playerY -= deltaTime;
        if (isObjectCollision(playerX, playerY)) {
            playerY += deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        playerX += deltaTime;
        if (isObjectCollision(playerX, playerY)) {
            playerX -= deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_Z)) {
        logicSpace.height -= deltaTime * 2;
        logicSpace.width -= deltaTime * 2;
        logicSpace.y += deltaTime;
        logicSpace.x += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_X)) {
        logicSpace.height += deltaTime * 2;
        logicSpace.width += deltaTime * 2;
        logicSpace.y -= deltaTime;
        logicSpace.x -= deltaTime;
    }
}


void tema1::OnKeyPress(int key, int mods)
{
}


void tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    float coordinateX = mouseX * logicSpace.width / resolutionX - playerX;
    float coordinateY = (logicSpace.height - mouseY * logicSpace.height / resolutionY) - playerY;

    playerAngle = atan2f(coordinateY, coordinateX);
}

bool tema1::isObjectCollision(float x, float y) {
    bool collisionX = false, collisionY = false, general = false;
    for (int i = 0; i < obstacoles.size(); i++) {
        //collisionX = obstacoles[i].x + collisionOffset + obstacoles[i].scaleX >= playerX &&
        //    playerX + collisionOffset + obstacoles[i].scaleX >= obstacoles[i].x;
        //collisionY = obstacoles[i].y + collisionOffset + obstacoles[i].scaleY >= playerY &&
        //    playerY + collisionOffset + obstacoles[i].scaleY >= obstacoles[i].y;

        collisionX = x + collisionOffset >= obstacoles[i].x &&
            obstacoles[i].x + obstacoles[i].scaleX >= x;
        // collision y-axis?
        collisionY = y + collisionOffset >= obstacoles[i].y &&
            obstacoles[i].y + obstacoles[i].scaleX >= y;
        if (collisionX && collisionY) {
            general = true;
        }
    }

    if (general == true) {
        return true;
    }
    else {
        if (x <= .1f || x >= 12.2f || y <= .1f || y >= 6.6f) {
            return true;
        }
    }

    return false;
}


void tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        projectile proiectil;

        float coordinateX = mouseX * logicSpace.width / resolutionX - playerX;
        float coordinateY = (logicSpace.height - mouseY * logicSpace.height / resolutionY) - playerY;

        float angle = atan2f(coordinateY, coordinateX);

        proiectil.x = playerX + .25f;
        proiectil.y = playerY + .25f;
        proiectil.angle = angle;
        double currTime = Engine::GetElapsedTime();
        proiectil.time = currTime;

        if (currTime - lastTime > .2f) {
            projectiles.push_back(proiectil);
            lastTime = currTime;
        }
    };
}


void tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}
