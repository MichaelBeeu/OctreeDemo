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

    int halfPlaneTest(  const glm::vec3 &p, const glm::vec3 &normal, float offset );
    int isBoxInFrustum( const glm::vec3 &origin, const glm::vec3 &halfDim );
    /* Public data */
protected:
    /* Protected data */
private:
    /* Private data */
    glm::vec4 planes[6];

    inline int vectorToIndex( const glm::vec3 &v ) const {
        int idx = 0;
        if( v.z >= 0 ) idx |= 1;
        if( v.y >= 0 ) idx |= 2;
        if( v.z >= 0 ) idx |= 4;
        return idx;
    }
};
#endif /* __FRUSTUM_H__ */
