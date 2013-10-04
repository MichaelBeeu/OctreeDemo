#include <iostream>

#include <SDL2/SDL.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Frustum.h"
#include "Octree.hpp"

#define WIDTH 800
#define HEIGHT 600

typedef glm::vec3 OctreeType;

inline float randRange( float min, float max ){
    return min + ( (max - min) * rand()) * (1.f / RAND_MAX);
}

inline glm::vec3 randVec3( float min, float max ){
    return glm::vec3( randRange(min, max), randRange(min, max), randRange(min, max) );
}

inline glm::vec3 randColor(){
    return glm::vec3( randRange(0.f, 1.f), randRange(0.f, 1.f), randRange(0.f, 1.f) );
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

void renderOctree( const Octree<OctreeType> *octree, bool debug = false ){
    std::vector< Octree<OctreeType>::dataPoint > data;
    std::vector< Octree<OctreeType>::dataPoint >::const_iterator it;
    octree->getData( data );


    glBegin( GL_POINTS );
    glColor4f( 1.f, 0.f, 0.f, 1.f );
    for(it = data.begin();it != data.end(); it++){
        glm::vec3 pos = (*it).first;
        glm::vec3 color = (*it).second;
        glColor3f( color.r, color.g, color.b );
        glVertex3f( pos.x, pos.y, pos.z );
    }
    glEnd();

    if( !octree->isLeafNode() ){
        for(int i=0;i<8;i++){
            renderOctree( octree->getChild(i), debug );
        }
    }
}

void renderVisibleOctree( const std::vector<Octree<OctreeType>*> &octree, bool debug = false ){
    std::vector<Octree<OctreeType>*>::const_iterator it;
    for(it = octree.begin();it != octree.end(); it++){
        if( debug ){
            renderBox( (*it)->getOrigin(), (*it)->getHalfDim(), glm::vec4(1.f,1.f,1.f,1.f) );
        }
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

    SDL_SetRelativeMouseMode( SDL_TRUE );
    SDL_SetWindowGrab( window, SDL_TRUE );

    // Init opengl
    glClearColor( 0.f, 0.f, 0.f, 0.f );
    glClearDepth( 1.f );

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );

    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    glPointSize( 5.f );

    glm::mat4 model, view, projection;
    glm::quat viewRot = glm::angleAxis( 0.f, glm::vec3( 1.f, 0.f, 0.f ) );
    float rotation = 0;

    projection = glm::perspective( 45.f, (float)WIDTH / (float)HEIGHT, 0.01f, 10.f );
    model = glm::mat4();
    view = glm::mat4();

    view = glm::translate( view, glm::vec3( 0.f, 0.f, -8.f ) );

    Octree<OctreeType> octree( glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f,1.f,1.f), 10 );

    float t = 0.f;
    bool debug = true;
    bool autoAddPoints = false;
    bool running = true;
    while( running ){
        SDL_Event evt;
        while( SDL_PollEvent( &evt ) ){
            switch( evt.type ){
                case SDL_QUIT:
                    running = false;
                break;
                case SDL_MOUSEMOTION:{
                    glm::vec3 axis = glm::normalize( glm::vec3( (float)evt.motion.yrel, (float)evt.motion.xrel, 0.f ) ) * viewRot;
                    viewRot = viewRot * glm::angleAxis( 1.f, axis );
                 }break;
                case SDL_KEYDOWN:
                    switch( evt.key.keysym.sym){
                        case SDLK_ESCAPE:
                            running = false;
                        break;
                        case SDLK_q:
                            octree.insert( randVec3(-1.f, 1.f), randColor() );
                        break;
                        case SDLK_s:
                            view = glm::translate( view, glm::vec3( 0.f, 0.f, -0.1f ) * viewRot );
                        break;
                        case SDLK_w:
                            view = glm::translate( view, glm::vec3( 0.f, 0.f, 0.1f ) * viewRot );
                        break;
                        case SDLK_a:
                            view = glm::translate( view, glm::vec3( 0.1f, 0.f, 0.f ) * viewRot );
                        break;
                        case SDLK_d:
                            view = glm::translate( view, glm::vec3( -0.1f, 0.f, 0.f ) * viewRot );
                        break;
                        case SDLK_e:
                            debug ^= true;
                        break;
                        case SDLK_r:
                            autoAddPoints ^= true;
                        break;
                        default:

                        break;
                    }
                break;
                default:
                break;
            }
        }

        // Clear screen
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glm::mat4 camera = glm::mat4_cast( viewRot ) * view;// glm::rotate( view, 30.f, glm::vec3( 1.f, 0.f, 0.f ) );
        //camera = glm::rotate( camera, rotation, glm::vec3( 0.f, 1.f, 0.f ) );
        
        // Set up our matrices
        glm::mat4 modelView = camera * model;
        glm::mat4 mvp = projection * modelView;

        glLoadMatrixf( &mvp[0][0] );

        Frustum frustum( mvp );

        if( autoAddPoints ){
            t += 0.02f;
            float x = sin( t );
            float y = cos( t ) * cos( t * 3.92f );
            float z = sin( t * 3.92f ) * cos(t);
            OctreeType clr(x,y,z);
            clr += 1.f;
            clr /= 2.f;
            octree.insert( glm::vec3( x, y, z ), clr );
        }

        std::vector< Octree<OctreeType>* > visible;
        octree.getOctantsInFrustum( frustum, visible );

        renderVisibleOctree( visible , debug );

        //renderBox( glm::vec3( 0.f, 0.f, 0.f ), glm::vec3( 1.f, 1.f, 1.f ) );

        SDL_GL_SwapWindow( window );
    }

    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );

    std::cout<<"Exit successful!"<<std::endl;
    return 0;
}
