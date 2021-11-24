#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/lab_camera.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
         Tema2();
        ~Tema2();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, float color[3]);
        void DrawScene(glm::mat4 visMatrix);
        void DrawCharacter(glm::mat4 visMatrix);
        void DrawMatrixScena(glm::mat4 visMatrix);
        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        bool isObjectCollision(float x, float z);
    struct enemy {
        float x;
        float z;
        float xCurrent;
        float zCurrent;
        bool dirFront;
    };
    struct wall {
        float x;
        float z;
    };

    struct projectile {
        float x;
        float z;
        double time;
        float angle;
    };

    struct WinZone {
        float x;
        float z;
    };
     protected:
        implemented::Camera *camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;
        float playerRotation;
        int fov, health;
        bool isAttacking, playerDead, playerWin;
        float collisionOffset = 0.1f;
        WinZone winZone;
        double lastTimeShoot = 0;
        const float PI = (float)3.14159265;
        const float deg2rad = PI / 180;


        std::vector<enemy> enemies;
        std::vector<wall> walls;
        std::vector<projectile> projectiles;


    };
}   // namespace m1
