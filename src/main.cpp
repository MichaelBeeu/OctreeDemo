#include <iostream>

#include <SDL2/SDL.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Frustum.h"
#include "Octree.hpp"

#define WIDTH 800
#define HEIGHT 600

inline float randRange( float min, float max ){
    return min + ( (max - min) * rand()) * (1.f / RAND_MAX);
}

inline glm::vec3 randVec3( float min, float max ){
    return glm::vec3( randRange(min, max), randRange(min, max), randRange(min, max) );
}

void renderBox( const glm::vec3 &center, const glm::vec3 &dim, const glm::vec4 &clr = glm::vec4(1.f,1.f,1.f,1.f)){
    // Back "face"
    glBegin( GL_LINE_STRIP );
        glColor4f( clr.r, clr.g, clr.b, clr.a );
        glVertex3f( center.x - dim.x, center.y - dim.y, center.z - dim.z );
        glVertex3f( center.x + dim.x, center.y - dim.y, center.z - dim.z );
        glVertex3f( center.x + dim.x, center.y + dim.y, center.z - dim.z );
        glVertex3f( center.x - dim.x, center.y + dim.y, center.z - dim.z );
        glVertex3f( center.x - dim.x, center.y - dim.y, center.z - dim.z );
    glEnd();

    // Front "face"
    glBegin( GL_LINE_STRIP );
        glColor4f( clr.r, clr.g, clr.b, clr.a );
        glVertex3f( center.x - dim.x, center.y - dim.y, center.z + dim.z );
        glVertex3f( center.x + dim.x, center.y - dim.y, center.z + dim.z );
        glVertex3f( center.x + dim.x, center.y + dim.y, center.z + dim.z );
        glVertex3f( center.x - dim.x, center.y + dim.y, center.z + dim.z );
        glVertex3f( center.x - dim.x, center.y - dim.y, center.z + dim.z );
    glEnd();

    glBegin( GL_LINES );
        glColor4f( clr.r, clr.g, clr.b, clr.a );
        glVertex3f( center.x - dim.x, center.y - dim.y, center.z + dim.z );
        glVertex3f( center.x - dim.x, center.y - dim.y, center.z - dim.z );

        glVertex3f( center.x + dim.x, center.y - dim.y, center.z + dim.z );
        glVertex3f( center.x + dim.x, center.y - dim.y, center.z - dim.z );

        glVertex3f( center.x + dim.x, center.y + dim.y, center.z + dim.z );
        glVertex3f( center.x + dim.x, center.y + dim.y, center.z - dim.z );

        glVertex3f( center.x - dim.x, center.y + dim.y, center.z + dim.z );
        glVertex3f( center.x - dim.x, center.y + dim.y, center.z - dim.z );
    glEnd();

}

void renderOctree( const Octree<float> *octree, bool debug = false ){
    std::vector< Octree<float>::dataPoint > data;
    std::vector< Octree<float>::dataPoint >::const_iterator it;
    octree->getData( data );

    if( debug ){
        renderBox( octree->getOrigin(), octree->getHalfDim(), glm::vec4(1.f,1.f,1.f,1.f) );
    }

    glBegin( GL_POINTS );
    glColor4f( 1.f, 0.f, 0.f, 1.f );
    for(it = data.begin();it != data.end(); it++){
        glm::vec3 pos = (*it).first;
        glVertex3f( pos.x, pos.y, pos.z );
    }
    glEnd();

    if( !octree->isLeafNode() ){
        for(int i=0;i<8;i++){
            renderOctree( octree->getChild(i), debug );
        }
    }
}

void renderVisibleOctree( const std::vector<Octree<float>*> &octree, bool debug = false ){
    std::vector<Octree<float>*>::const_iterator it;
    for(it = octree.begin();it != octree.end(); it++){
        renderOctree( (*it), debug );
    }
}

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
    model = glm::mat4();
    view = glm::mat4();

    view = glm::translate( view, glm::vec3( 0.f, 0.f, -8.f ) );

    Octree<float> octree( glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f,1.f,1.f), 1 );

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
                        case SDLK_q:
                            octree.insert( randVec3(-1.f, 1.f), randRange(0.f,1.f) );
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

        glLoadMatrixf( &mvp[0][0] );

        Frustum frustum( mvp );

        std::vector< Octree<float>* > visible;
        octree.getOctantsInFrustum( frustum, visible );

        renderVisibleOctree( visible , true);

        //renderBox( glm::vec3( 0.f, 0.f, 0.f ), glm::vec3( 1.f, 1.f, 1.f ) );

        SDL_GL_SwapWindow( window );
    }

    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );

    std::cout<<"Exit successful!"<<std::endl;
    return 0;
}
