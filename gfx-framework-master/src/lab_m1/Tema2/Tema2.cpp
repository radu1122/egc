#include "lab_m1/Tema2/Tema2.h"

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

void Tema2::Init()
{
    renderCameraTarget = false;
    playerRotation = 0;
    isAttacking = false;
    playerDead = false;
    health = 10;
    playerWin = false;

    camera = new implemented::Camera();
    camera->Set(glm::vec3(0, 1.0f, 0.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
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
        Mesh* mesh = new Mesh("teapot");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "teapot.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

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


    glm::mat4 modelMatrix = glm::mat4(1);

    DrawScene(modelMatrix);
}


void Tema2::DrawScene(glm::mat4 visMatrix)
{
    //desenare caracter

    glm::mat4 modelMatrix = glm::mat4(1);
    //modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.3f, 0));
    modelMatrix = glm::rotate(modelMatrix, 0.0f, glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(180.0f), glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f));
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

    //modelMatrix = glm::rotate(modelMatrix, playerRotation, glm::vec3(0, 1, 0));
    //modelMatrix = glm::rotate(modelMatrix, RADIANS(180.0f), glm::vec3(0, 1, 0));
    //modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f));
    //modelMatrix = glm::scale(modelMatrix, glm::vec3(1.3f, 2.0f, 0.5f));

    //RenderMesh(meshes["box"], shaders["Color"], modelMatrix, color);

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
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            // TODO(student): Translate the camera to the left
            camera->TranslateRight(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            // TODO(student): Translate the camera backward
            camera->MoveForward(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // TODO(student): Translate the camera to the right
            camera->TranslateRight(cameraSpeed * deltaTime);
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
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            camera->RotateFirstPerson_OX(sensivityOX * deltaY);
            camera->RotateFirstPerson_OY(sensivityOY * deltaX);
        }
        playerRotation += deltaX * sensivityOX;
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
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
