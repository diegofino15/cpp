#include <raylib.h>
#include <stdio.h>

#include <iostream>
#include <cmath>
#include <cstdlib>
using namespace std;


class VertletObject
{
    public:

    float x, y;
    float lastX, lastY;
    float radius;

    float accX, accY;
    float r, g, b;

    bool isStatic;

    VertletObject(float x, float y, float radius, bool isStatic) {
        this->x = x;
        this->y = y;
        this->radius = radius;

        this->lastX = x;
        this->lastY = y;

        this->isStatic = isStatic;
    }
    VertletObject() {
        this->x = GetScreenWidth() / 2.0f + 1;
        this->y = GetScreenHeight() / 2.0f + 1;

        this->radius = (float) (rand() % 5 + 5);

        this->lastX = x;
        this->lastY = y;

        this->r = 50;
        this->g = 50;
        this->b = (float) (rand() % 50 + 100);

        this->r = (float) (rand() % 128 + 128);
        this->g = (float) (rand() % 128 + 128);
        this->b = (float) (rand() % 128 + 128);


        this->isStatic = false;
    }

    void updatePosition(float dt)
    {   
        if (!isStatic)
        {
            float velX, velY;
            velX = x - lastX;
            velY = y - lastY;

            lastX = x;
            lastY = y;

            x += velX + accX * dt * dt;
            y += velY + accY * dt * dt;

            accX = 0.0f;
            accY = 0.0f;
        }
    }

    void accelerate(float x, float y)
    {
        accX += x;
        accY += y;
    }

    void stayInBounds(float posX, float posY, float rad)
    {
        float disX = x - posX;
        float disY = y - posY;
        float dis = sqrt(disX * disX + disY * disY);

        if (dis > (rad - radius))
        {
            float targetDis = rad - radius;
            float ratio = dis / targetDis;

            float targetDisX = disX / ratio;
            float targetDisY = disY / ratio;

            x = (posX + targetDisX);
            y = (posY + targetDisY);
        }
    }

    void Draw()
    {
        Color color{(unsigned char) r, (unsigned char) g, (unsigned char) b, 255};
        DrawCircle((int) x, (int) y, radius, color);
    }
};


class Solver
{
    public:

    float gravX, gravY;
    VertletObject* vertletObjects;

    int ballsNumber;

    float posX, posY, radius;

    Solver(float gravX, float gravY, int ballsNumber, float posX, float posY, float radius)
    {
        this->gravX = gravX;
        this->gravY = gravY;

        this->ballsNumber = ballsNumber;

        this->posX = posX;
        this->posY = posY;
        this->radius = radius;

        // VertletObject vertletObjects[ballsNumber];
        vertletObjects = new VertletObject[this->ballsNumber];
    } 
    
    void update(float dt)
    {
        int subSteps = 10;
        float subDT = dt / subSteps;

        for (int _ = 0; _ < subSteps; _++)
        {
            solveCollisions();
            for (int i = 0; i < ballsNumber; i++)
            {
                vertletObjects[i].accelerate(gravX, gravY);
                vertletObjects[i].stayInBounds(posX, posY, radius);
                vertletObjects[i].updatePosition(subDT);
            }
        }
    }
    
    void solveCollisions()
    {
        for (int i = 0; i < ballsNumber; i++)
        {
            for (int k = (i + 1); k < ballsNumber; k++)
            {
                float collisionAxisX = vertletObjects[i].x - vertletObjects[k].x;
                float collisionAxisY = vertletObjects[i].y - vertletObjects[k].y;

                float dist = sqrt(collisionAxisX * collisionAxisX + collisionAxisY * collisionAxisY);

                float minDist = vertletObjects[i].radius + vertletObjects[k].radius;

                if (dist < minDist)
                {
                    float nX = collisionAxisX / dist;
                    float nY = collisionAxisY / dist;
                    float delta = minDist - dist;

                    if (!vertletObjects[i].isStatic)
                    {
                        vertletObjects[i].x += 0.5 * delta * nX;
                        vertletObjects[i].y += 0.5 * delta * nY;
                    }
                    if (!vertletObjects[k].isStatic)
                    {
                        vertletObjects[k].x -= 0.5 * delta * nX;
                        vertletObjects[k].y -= 0.5 * delta * nY;
                    }
                }
            }
        }
    }

    void DrawAll()
    {
        for (int i = 0; i < ballsNumber; i++)
        {
            vertletObjects[i].Draw();
        }
    }

};



int main()
{
    InitWindow(800, 720, "Physics simulation");
    SetWindowState(FLAG_VSYNC_HINT);

    int ballsNumber = 500;
    Vector2 mousePos;

    Solver solver = Solver(0.0f, 1000.0f, ballsNumber, GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, GetScreenHeight() / 2.0f);
    solver.vertletObjects[1].x = GetScreenWidth() / 2 - 1;

    solver.vertletObjects[50].x = GetScreenWidth() / 2.0f;
    solver.vertletObjects[50].y = GetScreenHeight() / 2.0f;
    solver.vertletObjects[50].radius = 50;
    solver.vertletObjects[50].isStatic = true;

    while (!WindowShouldClose())
    {
        solver.update(1/60.0f);

        mousePos = GetMousePosition();

        if (IsKeyDown(KEY_SPACE))
        {
            for (int i = 0; i < ballsNumber; i++)
            {
                float disX = solver.vertletObjects[i].x - mousePos.x;
                float disY = solver.vertletObjects[i].y - mousePos.y;

                solver.vertletObjects[i].accelerate(-disX * 100, -disY * 100);
            }
        }

        BeginDrawing();
            ClearBackground(BLACK);

            DrawCircle(solver.posX, solver.posY, solver.radius, GRAY);
            solver.DrawAll();

            DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}







