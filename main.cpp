#include "SDL.h"
#include "GMath/GMath.h"
#include "GUtils/GUtil.h"
#include "GUtils/GConfig.h"
#include "GGraphics/GModelLoader.h"
#include "GGraphics/GGraphics.h"

int main(int argc, char* argv[])
{
    Granite::GGraphics::InitGraphics();

    SDL_Event e;

    float milisecondsPassed = 0;
    float deltaTime = 0;
    float startTime = 0;
    float endTime = 0;
    float milisecondsPerFrame = 16.f;
    float fps = 0;

    float fTheta = .0f;
    Granite::GMath::Mesh mesh;
    Granite::GMath::FMatrix4x4 projMatrix = Granite::GGraphics::GetProjectionMatrix();
    Granite::GMath::FVector3 camera;

    mesh = Granite::ModelLoader::Load();

    while (true)
    {
        endTime = SDL_GetTicks();
        milisecondsPassed = endTime - startTime;
        deltaTime = milisecondsPassed / 1000.f;
        startTime = endTime;

        if (milisecondsPassed < milisecondsPerFrame)
        {
            SDL_Delay(milisecondsPerFrame - milisecondsPassed);
        }

        if (milisecondsPassed > milisecondsPerFrame) {
            printf("FPS is: %f \n", 1000.f / milisecondsPassed);
        }

        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT) 
        {
            SDL_Log("Program quit after %i ticks", e.quit.timestamp);
            break;
        }

        Granite::GGraphics::ClearScreen(Granite::GGraphics::Color::Black);

        // Set up rotation matrices
        Granite::GMath::FMatrix4x4 matRotZ, matRotX;
        fTheta += deltaTime;

        // Rotation Z
        matRotZ.matrix[0][0] = cosf(fTheta);
        matRotZ.matrix[0][1] = sinf(fTheta);
        matRotZ.matrix[1][0] = -sinf(fTheta);
        matRotZ.matrix[1][1] = cosf(fTheta);
        matRotZ.matrix[2][2] = 1;
        matRotZ.matrix[3][3] = 1;

        // Rotation X
        matRotX.matrix[0][0] = 1;
        matRotX.matrix[1][1] = cosf(fTheta * 0.5f);
        matRotX.matrix[1][2] = sinf(fTheta * 0.5f);
        matRotX.matrix[2][1] = -sinf(fTheta * 0.5f);
        matRotX.matrix[2][2] = cosf(fTheta * 0.5f);
        matRotX.matrix[3][3] = 1;

        for (auto &polygon : mesh.polygons)
        {
            Granite::GMath::Polygon triTranslated;

            Granite::GMath::MultiplyMatrixPolygon(polygon, triTranslated, matRotZ);
            Granite::GMath::MultiplyMatrixPolygon(triTranslated, matRotX);
            Granite::GMath::OffsetPolygonDepth(triTranslated, 1800.f);

            Granite::GMath::FVector3 normal, line1, line2, cameraToPoint;

            line1 = triTranslated.vertices[1] - triTranslated.vertices[0];
            line2 = triTranslated.vertices[2] - triTranslated.vertices[0];

            normal = line1.CrossProduct(line2);
            normal.Normalize();

            cameraToPoint = triTranslated.vertices[0] - camera;
            cameraToPoint.Normalize();

            // dot product
            if (normal.DotProduct(cameraToPoint) < .0f)
            {
                // Project triangles from 3D --> 2D
                Granite::GMath::MultiplyMatrixPolygon(triTranslated, projMatrix);

                triTranslated.UniformMove(1.0f);
                triTranslated.UniformScale(0.5f * (float)Granite::GConfig::WINDOW_WIDTH);

                if (Granite::GConfig::WIREFRAME_MODE)
                {
                    Granite::GGraphics::DrawLine(
                        Granite::GMath::IPoint(triTranslated.vertices[0].x, triTranslated.vertices[0].y),
                        Granite::GMath::IPoint(triTranslated.vertices[1].x, triTranslated.vertices[1].y),
                        Granite::GGraphics::Color::Red);

                    Granite::GGraphics::DrawLine(
                        Granite::GMath::IPoint(triTranslated.vertices[1].x, triTranslated.vertices[1].y),
                        Granite::GMath::IPoint(triTranslated.vertices[2].x, triTranslated.vertices[2].y),
                        Granite::GGraphics::Color::Red);

                    Granite::GGraphics::DrawLine(
                        Granite::GMath::IPoint(triTranslated.vertices[2].x, triTranslated.vertices[2].y),
                        Granite::GMath::IPoint(triTranslated.vertices[0].x, triTranslated.vertices[0].y),
                        Granite::GGraphics::Color::Red);
                }

                if (Granite::GConfig::RASTERIZE)
                {
                    //Granite::GGraphics::RasterizeTriangle(triTranslated, Granite::GGraphics::Color::Blue);
                    Granite::GGraphics::RasterizeTriangle(triTranslated, Granite::GGraphics::GetRandomColor());
                }
            }
        }

        Granite::GGraphics::UpdateScreen();
    }

    Granite::GGraphics::DestroyGraphics();
    return 0;
}