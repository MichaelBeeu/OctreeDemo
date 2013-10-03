#ifndef __OCTREE_HPP__
#     define __OCTREE_HPP__

#include <vector>
#include <glm/glm.hpp>

#include <Frustum.h>

template <typename T>
class Octree
{
public:
    typedef std::pair<glm::vec3, T> dataPoint;
private:
    glm::vec3 origin; // The origin of our octant
    glm::vec3 halfDim; // Half dimension of the octant

    Octree<T> *children[8];
    std::vector<dataPoint> data; // The data contained in this octant
    unsigned int maxData; // Max data per octant
public:
    Octree( const glm::vec3 &orig, const glm::vec3 &hDim, unsigned int max) :
        origin( orig ),
        halfDim( hDim ),
        maxData( max )
    {
        // Clear child array
        //std::fill( children, children+sizeof(children), reinterpret_cast<Octree<T>*>(NULL) );
        for(int i=0;i<8;i++){
            children[i] = NULL;
        }
    }

    virtual ~Octree(){
        // Delete children
        if( !isLeafNode() ){
            for(int i=0;i<8;i++){
                delete children[i];
                children[i] = NULL;
            }
        }
    }

    bool isLeafNode() const {
        return children[0] == NULL;
    }

    void insert( const dataPoint &p ){
        if( isLeafNode() ){
            // We are a leaf node, and there is still room left
            if( data.size() < maxData ){
                data.push_back( p );
            } else { // No room left, create some children
                // TODO: Implement a max depth?
                for(int i=0;i<8;i++){
                    glm::vec3 nOrigin = origin;
                    nOrigin.x += halfDim.x * (i&4 ? 0.5f : -0.5f );
                    nOrigin.y += halfDim.y * (i&2 ? 0.5f : -0.5f );
                    nOrigin.z += halfDim.z * (i&1 ? 0.5f : -0.5f );

                    children[i] = new Octree( nOrigin, halfDim * 0.5f, maxData );
                }

                // Reorganize our data into child nodes
                typename std::vector< dataPoint >::iterator it;
                for( it = data.begin(); it != data.end(); it++){
                    children[ getOctantFromPoint( (*it).first ) ]->insert( (*it) );
                }

                data.clear();

                children[ getOctantFromPoint( p.first ) ]->insert( p );
            }
        } else { // We are not a leaf node
            findBestChild( p.first )->insert( p );
        }
    }

    void insert( glm::vec3 pos, T d ){
        insert( dataPoint( pos, d ) );
    }

    void getOctantsInFrustum( const Frustum &frustum, std::vector< Octree<T>*> &out ){
        int inFrustum = frustum.isBoxInFrustum( origin, halfDim );
        if( inFrustum & 1 ){ // We are inside frustum
            if( inFrustum & 2  && !isLeafNode() ){ // But only partially:
                for(int i=0;i<8;i++){
                    children[i]->getOctantsInFrustum( frustum, out );
                }
            } else { // Completely
                out.push_back( this );
            }
        }
    }

    // Define some simple getters
    // (mostly for debug rendering)
    glm::vec3 getOrigin() const {
        return origin;
    }

    glm::vec3 getHalfDim() const {
        return halfDim;
    }

    Octree<T> *getChild( unsigned int i ) const {
        //if( i >= 8 ) return NULL;
        return children[i];
    }

    void getData( std::vector<dataPoint> &out ) const {
        out = data;
    }
    
    /* Public data */
protected:
    /* Protected data */
private:
    /* Private data */

    // This function finds the first
    // non-leaf octant for a data point
    Octree<T> *findBestChild( const glm::vec3 &pos ){
        int octant = 0;
        Octree<T> *ret = this;
        do {
            octant = ret->getOctantFromPoint( pos );
            ret = ret->children[ octant ];
        } while( !ret->isLeafNode() );
        return ret;
    }

    int getOctantFromPoint( const glm::vec3 &point ) const {
        int index = 0;
        if( point.x >= origin.x ) index |= 4;
        if( point.y >= origin.y ) index |= 2;
        if( point.z >= origin.z ) index |= 1;

        return index;
    }
};
#endif /* __OCTREE_HPP__ */
