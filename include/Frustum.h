#ifndef __FRUSTUM_H__
#     define __FRUSTUM_H__

#include <glm/glm.hpp>

class Frustum
{
public:
    Frustum( /* c'tor Params */ );
    Frustum( const glm::mat4 &matrix );
    virtual ~Frustum();

    enum Plane {
        Right = 0,
        Left,
        Bottom,
        Top,
        Far,
        Near
    };

    glm::vec4 getPlane( Plane p ) const;
    void calcPlanes( const glm::mat4 &matrix );

    int halfPlaneTest(  const glm::vec3 &p, const glm::vec3 &normal, float offset ){
        float dist = glm::dot( p, normal ) + offset;
        if( dist >= 0.02 ) // Point is in front of plane
            return 1;
        else if( dist < -0.02 ) // Point is behind plane
        return 2; // Point is on plane
    }

    /* Public data */
protected:
    /* Protected data */
private:
    /* Private data */
    glm::vec4 planes[6];
};
#endif /* __FRUSTUM_H__ */
