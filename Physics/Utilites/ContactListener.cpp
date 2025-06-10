#include "ContactListener.h"
#include "../../Common/Console.h"

JPH::ValidateResult MyContactListener::OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult)
{
    SysPrint("Contact vaildate callback",2);
    return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void MyContactListener::OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings)
{
    SysPrint("Contact Added (Object Collided)",2);
}

void MyContactListener::OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings)
{
    SysPrint("Contact Persisted (Object Stayed)",2);
}

void MyContactListener::OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair)
{
    SysPrint("Contact Removed (Object Stopped Colliding)",2);
}