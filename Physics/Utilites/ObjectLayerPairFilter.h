#ifndef __OBJECT_LAYER_PAIR_FILTER__
#define __OBJECT_LAYER_PAIR_FILTER__

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

class ObjectLayerPairFilterImpl: public JPH::ObjectLayerPairFilter
{
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override;
};

#endif