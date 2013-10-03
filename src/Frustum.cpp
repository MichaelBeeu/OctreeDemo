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


void Frustum::calcPlanes( const glm::mat4 &matrix )
{
    // Extract frustum planes from matrix
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
    return /* something */;
}

