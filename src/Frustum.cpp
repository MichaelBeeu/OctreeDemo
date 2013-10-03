#include <iostream>
#include "Frustum.h"

Frustum::Frustum( /* c'tor Params */ )
{
}

Frustum::Frustum( const glm::mat4 &matrix )
{
    calcPlanes( matrix );
}

Frustum::~Frustum()
{
}


glm::vec4 Frustum::getPlane( Plane p ) const
{
    return planes[ p ];
}

int Frustum::halfPlaneTest(  const glm::vec3 &p, const glm::vec3 &normal, float offset ) const {
    float dist = glm::dot( p, normal ) + offset;
    if( dist > 0.02 ) // Point is in front of plane
        return 1;
    else if( dist < -0.02 ) // Point is behind plane
        return 0;
    return 2; // Point is on plane
}

int Frustum::isBoxInFrustum( const glm::vec3 &origin, const glm::vec3 &halfDim ) const {
    static const glm::vec3 cornerOffsets[] = {
                                 glm::vec3(-1.f,-1.f,-1.f),
                                 glm::vec3(-1.f,-1.f, 1.f),
                                 glm::vec3(-1.f, 1.f,-1.f),
                                 glm::vec3(-1.f, 1.f, 1.f),
                                 glm::vec3( 1.f,-1.f,-1.f),
                                 glm::vec3( 1.f,-1.f, 1.f),
                                 glm::vec3( 1.f, 1.f,-1.f),
                                 glm::vec3( 1.f, 1.f, 1.f) 
                                            };
    int c2 = 0;
    // TODO: This can probably be sped up, or at least unrolled,
    // but works for now.
    // Test each plane
    for(int i=0;i<6;i++){
        int c = 0;
        for(int j=0;j<8;j++){
            glm::vec3 testPoint = origin + halfDim * cornerOffsets[j];

            int t = halfPlaneTest( testPoint, glm::vec3( planes[i] ), planes[i].w );
            if( t > 0 )
                c++;
        }
        if( c == 0 ){
            c2 = 0;
            break;
        } else if( c == 8 ){
            c2 ++;
        }
    }
    if( c2 == 6 ) return 1;
    if( c2 > 0 ) return 3;
    return 0;
    //return (inside?1:0) | (partial?2:0);
}

void Frustum::calcPlanes( const glm::mat4 &matrix )
{
    // Extract frustum planes from matrix
    // Planes are in format: normal(xyz), offset(w)
    planes[Right ] = glm::vec4( matrix[0][3] - matrix[0][0],
                                matrix[1][3] - matrix[1][0],
                                matrix[2][3] - matrix[2][0],
                                matrix[3][3] - matrix[3][0] );

    planes[Left  ] = glm::vec4( matrix[0][3] + matrix[0][0],
                                matrix[1][3] + matrix[1][0],
                                matrix[2][3] + matrix[2][0],
                                matrix[3][3] + matrix[3][0] );

    planes[Bottom] = glm::vec4( matrix[0][3] + matrix[0][1],
                                matrix[1][3] + matrix[1][1],
                                matrix[2][3] + matrix[2][1],
                                matrix[3][3] + matrix[3][1] );

    planes[Top   ] = glm::vec4( matrix[0][3] - matrix[0][1],
                                matrix[1][3] - matrix[1][1],
                                matrix[2][3] - matrix[2][1],
                                matrix[3][3] - matrix[3][1] );

    planes[Far   ] = glm::vec4( matrix[0][3] - matrix[0][2],
                                matrix[1][3] - matrix[1][2],
                                matrix[2][3] - matrix[2][2],
                                matrix[3][3] - matrix[3][2] );

    planes[Near  ] = glm::vec4( matrix[0][3] + matrix[0][2],
                                matrix[1][3] + matrix[1][2],
                                matrix[2][3] + matrix[2][2],
                                matrix[3][3] + matrix[3][2] );
    // Normalize them
    for(int i=0;i<6;i++){
        float invl = sqrt( planes[i].x * planes[i].x +
                           planes[i].y * planes[i].y +
                           planes[i].z * planes[i].z );
        planes[i] /= invl;
    }
}

