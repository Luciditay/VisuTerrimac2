#include "SDL2/SDL.h"
#include "GL/glu.h"
#include "stdlib.h"
#include "stdio.h"
#include "iostream"
#include "quadtree.h"
#include "getFile.h"
#include <fstream>
#include <string>
#include "terrain.h"
#include "loadSDL.h"
#include "camera.h"
#include "geometry.h"
#include "light.h"


using namespace std;

#define GL_TEXTURE_CUBE_MAP_ARB             0x8513
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB  0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB  0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB  0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB  0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB  0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB  0x851A
#define STB_IMAGE_IMPLEMENTATION


bool drawFil = false; //True ==> Dessiner en mode fil de fer
bool drawQuadtree = false; //Dessiner le quadTree

static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

int main(int argc, char const *argv[])
{
    const string fichierParams = (string) argv[1]; //Fichiers parametre.timac renommé pour tester 
    const string texturePath1 = "texture/sable.jpg";
    const string texturePath2 = "texture/texture_Bleu_Chelou.jpg";
    const string texturePath3 = "texture/texture_beige.jpg";

    const string textureArbrePath = "texture/palmier.png";

    srand (time(NULL));

    //Pour changer la texture renommer la texture désiré en texturePath1
    string heightMapFile;
    int xSize, ySize, zMin, zMax;
    float zNear, zFar, fov;
    
    loadParams(fichierParams, &heightMapFile, &xSize, &ySize, &zMin, &zMax, &zNear, &zFar, &fov);

    int xArbre[nbreArbres];
    int yArbre[nbreArbres];

    for (int i=0; i<nbreArbres ; i++){
        xArbre[i] =  rand() % xSize + 20;
        yArbre[i] =  rand() % ySize + 20;
    }
    
    
    float imageTab[xSize * ySize];
    Image* image = (Image*) malloc(sizeof(Image));

    image->pixels = new float *[xSize];
    for (int i = 0; i < xSize; i++)
    {
        image->pixels[i] = new float[ySize];
    }

    loadHeightMap(heightMapFile, imageTab, xSize * ySize, zMin, zMax, image);
    tabToMatrice(imageTab, image->pixels, xSize, ySize);  
    Node quadTree;
    initialiserQuadTree(&quadTree, image->pixels, xSize, ySize);
    //parcourirQuad(&quadTree);
    loadSDL();
    int IDText = 0;
    SDL_Window* window = createWindow(800, 600);
    SDL_GLContext context=createContext(window);

    SDL_Surface* texture1 = loadTexture(texturePath1);
    GLuint idTexture = textureCarteGraphique(texture1, &IDText);

    SDL_Surface* textureArbre = loadTexture(textureArbrePath);
    GLuint idTextArbre = textureCarteGraphique(textureArbre, &IDText);

    //Pour loader plsuieurs textures : later !
    // GLuint idTexture = textureCarteGraphique(texture2, &IDText);
    
    // SDL_Surface* texture3 = loadTexture(texturePath3);
    // GLuint idTexture = textureCarteGraphique(texture3, &IDText);
    

    /* PARTIE SKYBOX */


    // Chargement des textures
    GLenum texture_faces[6] = {           
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB
        };


    SDL_Surface texture_image[6]; 
    const char* images_paths[6] = {
        "./Skybox/left.jpg",
        "./Skybox/right.jpg",
        "./Skybox/top.jpg",
        "./Skybox/bottom.jpg",
        "./Skybox/front.jpg",
        "./Skybox/back.jpg",
    };
    

    for (int i = 0; i < 6; ++i) {
        texture_image[i] = loadTexture(image_paths[i]);
        }
    }


    // Configuration de la texture
    GLuint cubemap_text_ID;
    glGenTextures(1, &cubemap_text_ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cubemap_text_ID);

    for (int i = 0; i < 6; i++)
    {
        glTexImage2D(cubemap_faces[i], 0, 3, texture_faces[i]->sizeX, texture_faces[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_faces[i]->pixels);

        if (texture_faces[i])                
        {
            if (texture_faces[i]->pixels)    
            {
                free(texture_faces[i]->pixels);    
            }
            free(texture_faces[i]);    
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);    
        /* FIN PARTIE SKYBOX */


    onWindowResized(800, 600, zFar, zNear, fov);    

    Point3D camPos = {xSize, ySize, zMax}; //Position de la camera
    Point3D lookAtDirection = {10, 10, -0.8}; //Vecteur directeur (dans quelle direction regarde la camera)
    Point3D vectCamera;
    Triangle* champCamera = (Triangle*) malloc(sizeof(Triangle));
    float angleCam;

    int loop=1;
    while(loop){
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
       // lookAtDirection =scale3DVect(lookAtDirection, 1/(norme3D(lookAtDirection.x, lookAtDirection.y, lookAtDirection.z)));
        vectCamera = add3DVect(camPos, lookAtDirection);
       // vectCamera = scale3DVect(vectCamera, 1/(norme3D(vectCamera.x, vectCamera.y, vectCamera.z)));
        champCamera = triangleCamera(sub3DVect({(float) xSize/2,(float) ySize/2}, camPos ), lookAtDirection, fov, zFar);  
        gluLookAt(camPos.x, camPos.y, camPos.z, vectCamera.x, vectCamera.y, vectCamera.z, 0, 0, 1);        
        glTranslatef(image->w / 2.f, image->h / 2.f, 0);
        glScalef((float) xSize / image->w, (float) ySize / image->h, 1.);        
                 
        if (drawFil){
            if (drawQuadtree)
                drawQuadTreeFil(&quadTree);
            else
                drawTerrainFil(&quadTree, champCamera);
        }     

        else{
            if (drawQuadtree)
                drawQuadTreeTexture(&quadTree, idTexture);
            else
                drawTerrainTexture(&quadTree, idTexture, champCamera, zMax, zMin);
        }

        for (int i=0; i<nbreArbres; i++){
            if (isInTriangle(champCamera, {(float) xArbre[i],(float)  yArbre[i]}))
                drawObjet(idTextArbre, xArbre[i], yArbre[i], image->pixels, 0, 5, zMin);
        }
        
        glDisable(GL_DEPTH_TEST);

        //glScalef((float) xSize / image->w, (float) ySize / image->h, 1.);
       // glTranslatef(image->w / 2.f, image->h / 2.f, 0);
        afficherTriangle(champCamera);

        Uint32 startTime = SDL_GetTicks();

        Uint32 elapsedTime = SDL_GetTicks() - startTime;
        /* Si trop peu de temps s'est ecoule, on met en pause le programme */
        if(elapsedTime < FRAMERATE_MILLISECONDS) 
        {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }

        SDL_Event e;
        while(SDL_PollEvent(&e)) 
        {           
            switch(e.type) 
            {

                case SDL_QUIT:
                    loop=0;
                    break;
                /* Clic souris */
                case SDL_MOUSEBUTTONUP:
                    //printf("clic en (%d, %d)\n", e.button.x, e.button.y);
                    break;
                
                /* Touche clavier */
                case SDL_KEYDOWN:                    
                    //printf("touche pressee (code = %d)\n", e.key.keysym.sym);

                    /* Check the SDLKey values and move change the coords */
                    switch(e.key.keysym.sym ){
                        //Gestion Camera (position de la camera et changement du vecteur directeur de la camera)
                        case SDLK_z:
                            camPos.x++;
                            break;
                        case SDLK_q:
                            camPos.y++;
                            break;
                        case SDLK_s:
                            camPos.x--;
                            break;
                        case SDLK_d:
                            camPos.y--;
                            break;

                        case SDLK_SPACE:
                            camPos.z++;
                            break;

                        case SDLK_x:
                            camPos.z--;
                            break;

                        case SDLK_m:
                            lookAtDirection.x+=0.3;
                            break;

                        case SDLK_l:
                            lookAtDirection.x-=0.3;
                            break;

                        case SDLK_LEFT:
                            lookAtDirection.y-=0.3;
                            break;

                        case SDLK_RIGHT:
                            lookAtDirection.y+=0.3;
                            break;

                        case SDLK_UP:
                            lookAtDirection.z+=0.3;
                            break;

                        case SDLK_DOWN:
                            lookAtDirection.z-=0.3;
                            break;
                        
                        case SDLK_f:
                            drawFil=!drawFil; //Active/Désactive le mode draw fil en appuyant sur f
                            break;

                        case SDLK_n:
                            drawQuadtree=!drawQuadtree;
                            break;
                    }
                    break;
                
                case SDL_WINDOWEVENT:
                    switch (e.window.event) 
                    {
                        /* Redimensionnement fenetre */
                        case SDL_WINDOWEVENT_RESIZED:
                            onWindowResized(e.window.data1, e.window.data2, zFar, zNear, fov);                
                            break;

                        default:
                            break; 
                    }
                    break;

                default:
                    break;
            }
        }

        SDL_GL_SwapWindow(window);
     }

    /* Liberation des ressources associees a la SDL */
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(texture1);
    glDeleteTextures(1, &idTexture);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
