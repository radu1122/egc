#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/tema1/object2D.h"
#include "lab_m1/tema1/transform2D.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}

float width = 0.0f;
int gameMatrix[12][12] = {  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,},
                            {-1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1,},
                            { 99,  2,  0, -1, -1, -1, -1,  0,  0,  0, -1, -1,},
                            {-1, -1,  2,  0, -1,  0,  2,  0, -1,  0,  0, -1,},
                            {-1, -1, -1,  0,  0,  0,  0, -1, -1,  0, -1, -1,},
                            {-1, -1, -1,  0,  0,  0,  0, -1, -1,  0, -1, -1,},
                            {-1, -1, -1, -1,  0,  0,  0, -1, -1,  0, -1, -1,},
                            {-1, -1, -1, -1,  0,  0, -1, -1, -1,  0, -1, -1,},
                            {-1, -1, -1, -1, -1,  0,  0,  0, -1,  0, -1, -1,},
                            {-1, -1, -1, -1, -1, -1, -1,  2,  0,  0, -1, -1,},
                            {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,},
                            {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,} };

//int gameMatrix[12][12] = { {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,},
//                            {-1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1,},
//                            { 99,  0,  0, -1, -1, -1, -1,  0,  0,  0, -1, -1,},
//                            {-1, -1,  0,  0, -1,  0,  2,  0, -1,  0,  0, -1,},
//                            {-1, -1, -1,  0,  0,  0,  0, -1, -1,  0, -1, -1,},
//                            {-1, -1, -1,  0,  0,  0,  0, -1, -1,  0, -1, -1,},
//                            {-1, -1, -1, -1,  0,  0,  0, -1, -1,  0, -1, -1,},
//                            {-1, -1, -1, -1,  0,  0, -1, -1, -1,  0, -1, -1,},
//                            {-1, -1, -1, -1, -1,  0,  0,  0, -1,  0, -1, -1,},
//                            {-1, -1, -1, -1, -1, -1, -1,  0,  0,  0, -1, -1,},
//                            {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,},
//                            {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,} };

void Tema2::Init()
{

    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            float x = 0, z = 0;
            if (i <= 5) {
                x = (float)-(5 - i);
            }
            else {
                x = (float)(i - 6);
            }
            if (j <= 5) {
                z = (float)-(5 - j);
            }
            else {
                z = (float)j - 6;
            }

            if (gameMatrix[i][j] == 2) {
                enemy enemy;
                enemy.x = x;
                enemy.z = z;
                enemy.xCurrent = x;
                enemy.zCurrent = z;
                enemy.dirFront = true;
                enemies.push_back(enemy);
            } else if (gameMatrix[i][j] == -1) {
                wall wall;
                wall.x = x;
                wall.z = z;
                walls.push_back(wall);
            }
            else if (gameMatrix[i][j] == 99) {
                WinZone winZoneLocal;
                winZoneLocal.x = x;
                winZoneLocal.z = z;
                winZone = winZoneLocal;
            }


        }
    }

    renderCameraTarget = false;
    playerRotation = 0;
    isAttacking = false;
    playerDead = false;
    health = 2;
    playerWin = false;



    camera = new implemented::Camera();
    camera->Set(glm::vec3(0, 1.5f, 0.3f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    width = window->props.aspectRatio;
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("Archer");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "characters", "archer"), "Archer.fbx");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* square1 = object2D::CreateSquare("square1", glm::vec3(0, 0, 0), 1, glm::vec3(.5f, 0, .5f), true);
        AddMeshToList(square1); 
    }
 
    fov = 80;
    projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);

}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

inline double to_degrees(double radians) {
    return radians * (180.0 / M_PI);
}
void Tema2::Update(float deltaTimeSeconds)
{
    if (playerDead) {
        glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    }

    if (playerWin) {
        glClearColor(0, 1, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    }

    bool collisionXWinZone = camera->GetTargetPosition()[0] * 0.7f >= winZone.x - 0.2f &&
        winZone.x + 0.1f >= camera->GetTargetPosition()[0] * 0.7f;
    bool collisionZWinZone = camera->GetTargetPosition()[2] >= winZone.z - 0.8f &&
        winZone.z + 0.8f >= camera->GetTargetPosition()[2];

    if (collisionXWinZone && collisionZWinZone) {
        playerWin = true;
        return;
    }


    // patrulare inamici
    for (int i = 0; i < enemies.size(); i++) {
        if (enemies[i].dirFront) {
            enemies[i].zCurrent = enemies[i].zCurrent + deltaTimeSeconds * 0.09f;
            if (enemies[i].zCurrent > enemies[i].z + 0.3f) {
                enemies[i].dirFront = false;
            }
        }
        else {
            enemies[i].zCurrent = enemies[i].zCurrent - deltaTimeSeconds * 0.09f;
            if (enemies[i].zCurrent < enemies[i].z - 0.3f) {
                enemies[i].dirFront = true;
            }
        }
    }

    for (auto it = enemies.begin(); it != enemies.end(); ) {
        bool collisionX = camera->GetTargetPosition()[0] * 0.7f >= it->x - 0.2f &&
            it->x + 0.1f >= camera->GetTargetPosition()[0] * 0.7f;
        bool collisionZ = camera->GetTargetPosition()[2] >= it->z - 0.8f &&
            it->z + 0.8f >= camera->GetTargetPosition()[2];
        if (collisionX && collisionZ) {
            it = enemies.erase(it);
            health = health - 1;
            if (health <= 0) {
                playerDead = true;
            }
        }
        else {
            ++it;
        }
    }

    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        if (Engine::GetElapsedTime() - it->time > .7f) {
            it = projectiles.erase(it);
        }
        else {
            it->x += cos(it->angle) * deg2rad * 2;
            it->z += sin(it->angle) * deg2rad * 2;

            bool general = false;

            for (auto it2 = enemies.begin(); it2 != enemies.end(); ) {
                bool collisionX = it->x * 0.7f >= it2->x - 0.2f &&
                    it2->x + 0.1f >= it->x * 0.7f;
                bool collisionZ = it->z >= it2->z - 0.8f &&
                    it2->z + 0.8f >= it->z;
                if (collisionX && collisionZ) {
                    it2 = enemies.erase(it2);
                    if (health <= 0) {
                        general = true;
                        return;
                    }
                }
                else {
                    ++it2;
                }
            }

            if (general) {
                it = projectiles.erase(it);
            }
            else {
                ++it;
            }
        }
    }
    for (int i = 0; i < projectiles.size(); i++) {
        projectiles[i].x += cos(projectiles[i].angle) * deg2rad * 2;
        projectiles[i].z += sin(projectiles[i].angle) * deg2rad * 2;
    }



    glm::mat4 modelMatrix = glm::mat4(1);

    DrawScene(modelMatrix);

}

void Tema2::DrawScene(glm::mat4 visMatrix)
{
    //desenare caracter
    DrawCharacter(visMatrix);
    //desenare scena
    DrawMatrixScena(visMatrix);
   
    // desenare inamici
    for (int i = 0; i < enemies.size(); i++) {
        float colorEnemy[3] = { 1.0f, 0.850f, 0.4f };
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(enemies[i].xCurrent * 2, 0, enemies[i].zCurrent * 2));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.015f));
        RenderMesh(meshes["Archer"], shaders["Simple"], modelMatrix, colorEnemy);
    }
    glm::mat3 modelMatrix;

    // desenare proiectile
    for (int i = 0; i < projectiles.size(); i++) {
        glm::mat4 modelMatrix = glm::mat4(1);

        float angle = atan2(camera->GetTargetPosition()[2] - projectiles[i].z, camera->GetTargetPosition()[0] - projectiles[i].x);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(projectiles[i].x, 1, projectiles[i].z));
        //modelMatrix = glm::rotate(modelMatrix, RADIANS(180.0f), glm::vec3(0, 1, 0));
        //modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
        modelMatrix = glm::rotate(modelMatrix, playerRotation, glm::vec3(0, 1, 0));

        float corlorPoriectil[3] = { 0.658f, 0.345f, 0.203f };
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));

        RenderMesh(meshes["sphere"], shaders["Color"], modelMatrix, corlorPoriectil);
    }

      

}

void Tema2::DrawMatrixScena(glm::mat4 visMatrix)
{

    float colorFreeRoad[3] = { 0.772f, 0.878f, 0.705f };
    float colorWall[3] = { 0.705f, 0.780f, 0.905f };
    float colorEnemy[3] = { 1.0f, 0.850f, 0.4f };
    float finish[3] = { 0.125f, 0.972f, 0.137f };

    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            float x = 0, z = 0;
            if (i <= 5) {
                x = (float)-(5 - i);
            }
            else {
                x = (float)(i - 6);
            }
            if (j <= 5) {
                z = (float)-(5 - j);
            }
            else {
                z = (float)j - 6;
            }


            if (gameMatrix[i][j] == -1) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 6, 2));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(x, 0, z));
                RenderMesh(meshes["box"], shaders["Simple"], modelMatrix, colorWall);
            }
            else if (gameMatrix[i][j] == 2) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 0.03f, 2));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(x, 0, z));
                RenderMesh(meshes["box"], shaders["Color"], modelMatrix, colorEnemy);
            }
            else if (gameMatrix[i][j] == 0) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 0.01f, 2));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(x, 0, z));
                RenderMesh(meshes["box"], shaders["Color"], modelMatrix, colorFreeRoad);
            }
            else if (gameMatrix[i][j] == 99) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 0.01f, 2));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(x, 0, z));
                RenderMesh(meshes["box"], shaders["Color"], modelMatrix, finish);
            }
        }
    }
}

void Tema2::DrawCharacter(glm::mat4 visMatrix)
{

    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(camera->GetTargetPosition()[0], 0.3f, camera->GetTargetPosition()[2]));
    modelMatrix = glm::rotate(modelMatrix, playerRotation, glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(180.0f), glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
    float colorLegs[3] = { 0.219f, 0.254f, 0.584f };
    glm::mat4 posLeftLeg = glm::scale(modelMatrix, glm::vec3(0.8f, 1.5f, 0.6f));
    RenderMesh(meshes["box"], shaders["Color"], posLeftLeg, colorLegs);
    RenderMesh(meshes["box"], shaders["Color"], glm::translate(posLeftLeg, glm::vec3(1.4f, 0, 0)), colorLegs);

    float bodyColor[3] = { 0.278f, 0.458f, 0.454f };
    glm::mat4 posBody = glm::scale(modelMatrix, glm::vec3(1.8f, 1.8f, 0.6f));
    posBody = glm::translate(posBody, glm::vec3(0.3f, 1.0f, 0));

    RenderMesh(meshes["box"], shaders["Color"], posBody, bodyColor);

    glm::mat4 handLeft = glm::scale(modelMatrix, glm::vec3(0.6f, 1.2f, 0.6f));
    handLeft = glm::translate(handLeft, glm::vec3(3.1f, 1.8f, 0));

    RenderMesh(meshes["box"], shaders["Color"], handLeft, bodyColor);

    glm::mat4 handRight = glm::scale(modelMatrix, glm::vec3(0.6f, 1.2f, 0.6f));
    handRight = glm::translate(handRight, glm::vec3(-1.3f, 1.8f, 0));

    RenderMesh(meshes["box"], shaders["Color"], handRight, bodyColor);

    float headColor[3] = { 0.866f, 0.788f, 0.678f };
    glm::mat4 palmLeft = glm::scale(modelMatrix, glm::vec3(0.6f, 0.6f, 0.6f));
    palmLeft = glm::translate(palmLeft, glm::vec3(3.1f, 2.0f, 0));

    RenderMesh(meshes["box"], shaders["Color"], palmLeft, headColor);

    glm::mat4 palmRight = glm::scale(modelMatrix, glm::vec3(0.6f, 0.6f, 0.6f));
    palmRight = glm::translate(palmRight, glm::vec3(-1.3f, 2.0f, 0));

    RenderMesh(meshes["box"], shaders["Color"], palmRight, headColor);

    glm::mat4 head = glm::scale(modelMatrix, glm::vec3(0.6f, 0.6f, 0.6f));
    head = glm::translate(palmRight, glm::vec3(2.25f, 3.2f, 0));

    RenderMesh(meshes["box"], shaders["Color"], head, headColor);

}



void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix, float color[3])
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    int colorLocation = glGetUniformLocation(shader->program, "color");

    glUniform3fv(colorLocation, 1, color);

    mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float cameraSpeed = 2.0f;

        if (window->KeyHold(GLFW_KEY_W)) {
            // TODO(student): Translate the camera forward
            camera->MoveForward(cameraSpeed * deltaTime);
            if (isObjectCollision(camera->GetTargetPosition()[0], camera->GetTargetPosition()[2])) {
                camera->MoveForward(-cameraSpeed * deltaTime);
            }
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            // TODO(student): Translate the camera to the left
            camera->TranslateRight(-cameraSpeed * deltaTime);
            if (isObjectCollision(camera->GetTargetPosition()[0], camera->GetTargetPosition()[2])) {
                camera->TranslateRight(cameraSpeed * deltaTime);
            }
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            // TODO(student): Translate the camera backward
            camera->MoveForward(-cameraSpeed * deltaTime);
            if (isObjectCollision(camera->GetTargetPosition()[0], camera->GetTargetPosition()[2])) {
                camera->MoveForward(cameraSpeed * deltaTime);
            }
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // TODO(student): Translate the camera to the right
            camera->TranslateRight(cameraSpeed * deltaTime);
            if (isObjectCollision(camera->GetTargetPosition()[0], camera->GetTargetPosition()[2])) {
                camera->TranslateRight(-cameraSpeed * deltaTime);
            }
        }

        if (window->KeyHold(GLFW_KEY_Z)) {
            fov += deltaTime;
            projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 100.0f);
        }
        if (window->KeyHold(GLFW_KEY_X)) {
            fov -= deltaTime;
            projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 100.0f);
        }
    }
}


void Tema2::OnKeyPress(int key, int mods)
{


    if (key == GLFW_KEY_LEFT_CONTROL) {
        fov = 30;
        isAttacking = true;
        projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.002f;
        float sensivityOY = 0.002f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            camera->RotateFirstPerson_OX(sensivityOX * deltaY);
            camera->RotateFirstPerson_OY(sensivityOY * deltaX);
        }
        playerRotation += deltaX * sensivityOX;
    }
}

bool Tema2::isObjectCollision(float x, float z) {
    bool collisionX = false, collisionZ = false, general = false;
    for (int i = 0; i < walls.size(); i++) {


        collisionX = camera->GetTargetPosition()[0] * 0.7f >= walls[i].x - 0.2f &&
            walls[i].x + 0.1f >= camera->GetTargetPosition()[0] * 0.7f;
        collisionZ = camera->GetTargetPosition()[2] >= walls[i].z - 0.8f &&
            walls[i].z + 0.8f >= camera->GetTargetPosition()[2];
        if (collisionX && collisionZ) {
            general = true;
        }
    }

    if (general == true) {
        return true;
    }


    return false;
}



void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        projectile proiectil;

        proiectil.x = camera->GetTargetPosition()[0];
        proiectil.z = camera->GetTargetPosition()[2];
        float angle = atan2f(proiectil.z - .1f, proiectil.x - .1f);

        proiectil.angle = angle;
        double currTime = Engine::GetElapsedTime();
        proiectil.time = currTime;

        if (currTime - lastTimeShoot > .1f) {
            projectiles.push_back(proiectil);
            lastTimeShoot = currTime;
        }
    };
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
