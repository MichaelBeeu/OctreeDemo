#include <iostream>

#include <SDL2/SDL.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Frustum.h"
#include "Octree.hpp"

#define WIDTH 800
#define HEIGHT 600

int main( int argc, char **argv ) {
    // Init SDL
    SDL_Init( SDL_INIT_VIDEO );

    SDL_Window *window;
    SDL_Renderer *renderer;
    
    // Create window and renderer
    SDL_CreateWindowAndRenderer( WIDTH, HEIGHT, SDL_WINDOW_OPENGL, &window, &renderer );

    // Init opengl
    glClearColor( 0.f, 0.f, 0.f, 0.f );
    glClearDepth( 1.f );

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );

    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    glm::mat4 model, view, projection;

    projection = glm::perspective( 45.f, (float)WIDTH / (float)HEIGHT, 0.01f, 100.f );

    Octree<float> octree( glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f,1.f,1.f), 10 );

    bool running = true;
    while( running ){
        SDL_Event evt;
        while( SDL_PollEvent( &evt ) ){
            switch( evt.type ){
                case SDL_QUIT:
                    running = false;
                break;
                case SDL_KEYDOWN:
                    switch( evt.key.keysym.sym){
                        case SDLK_ESCAPE:
                            running = false;
                        break;

                    }
                break;
                default:
                break;
            }
        }

        // Clear screen
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // Set up our matrices
        glm::mat4 modelView = view * model;
        glm::mat4 mvp = projection * modelView;

        Frustum frustum( mvp );

        SDL_GL_SwapWindow( window );
    }

    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );

    std::cout<<"Exit successful!"<<std::endl;
    return 0;
}
