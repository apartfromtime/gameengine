#include "entity.h"

//=============================================================================
// constructor
//=============================================================================
Entity::Entity()
{
    // Physics
    center = Vector2();
    curX = entityNS::X;
    curY = entityNS::Y;
    curZ = entityNS::Z;
    curAngle = 0.0f;
    curScale = 1.0f;
    oldX = 0.0f;
    oldY = 0.0f;
    oldZ = 0.0f;
    oldAngle = 0.0f;
    oldScale = 1.0f;
    mass = entityNS::MASS;
    bounciness = 1.0;
    velocity.x = 0.0;
    velocity.y = 0.0;
    deltaV.x = 0.0;
    deltaV.y = 0.0;
    active = true;
    rotation = 0;
    speed = entityNS::SPEED;
    // Collision
    corners[0] = Vector2();
    corners[1] = Vector2();
    corners[2] = Vector2();
    corners[3] = Vector2();
    collisionCenter = Vector2();
    minOverlap = 1.0f;
    radius = (entityNS::W + entityNS::H) / 4;
    edge.min[0] = -(long)(entityNS::W / 2.0f);
    edge.min[1] = -(long)(entityNS::H / 2.0f);
    edge.max[0] =  (long)(entityNS::W / 2.0f);
    edge.max[1] =  (long)(entityNS::H / 2.0f);
    rotatedBoxReady = false;
    intersecting = false;
    collision = false;
    embedded = false;
    collisionType = entityNS::CIRCLE;
    pixelsColliding = 0;
    noBounce = false;
}

//=============================================================================
// destructor
//=============================================================================
Entity::~Entity()
{
}


////////////////////////////////////////
//           Get functions            //
////////////////////////////////////////


//=============================================================================
// Return X position.
//=============================================================================
float Entity::getX()
{
    return curX;
}

//=============================================================================
// Return Y position.
//=============================================================================
float Entity::getY()
{
    return curY;
}

//=============================================================================
// Return Z position.
//=============================================================================
float Entity::getZ()
{
    return curZ;
}

//=============================================================================
// Get angle (in radians)
//=============================================================================
float Entity::getAngle()
{
    return curAngle;
}

//=============================================================================
// Get scale
//=============================================================================
float Entity::getScale()
{
    return curScale;
}

//=============================================================================
// Return center of scaled Entity as screen x,y.
//=============================================================================
const vector2_t Entity::getCenter()
{
    center = Vector2(curX, curY);
    return center;
}

//=============================================================================
// Return collision center
//=============================================================================
const vector2_t Entity::getCollisionCenter()
{
    return collisionCenter;
}

//=============================================================================
// Return radius of collision circle.
//=============================================================================
float Entity::getRadius() const
{
    return radius;
}

//=============================================================================
// Return RECT structure used for BOX and ROTATED_BOX collision detection.
//=============================================================================
const rect_t& Entity::getEdge() const
{
    return edge;
}

//=============================================================================
// Return corner c of ROTATED_BOX
//=============================================================================
const vector2_t Entity::getCorner(unsigned int c) const
{
    if (c >= 4)
    {
        c = 0;
    }

    return corners[c];
}

//=============================================================================
// Return corners array
//=============================================================================
vector2_t* Entity::getCorners()
{
    return corners;
}

//=============================================================================
// Return projection overlaps used in rotated box collision
//=============================================================================
float Entity::getMinOverlap()
{
    return minOverlap;
}

//=============================================================================
// Return velocity vector.
//=============================================================================
const vector2_t Entity::getVelocity() const
{
    return velocity;
}

//=============================================================================
// Return velocity vector.
//=============================================================================
vector2_t Entity::getDeltaV()
{
    return deltaV;
}

//=============================================================================
// Returns rotation rate
//=============================================================================
float Entity::getRotation()
{
    return rotation;
}

//=============================================================================
// Return speed
//=============================================================================
float Entity::getSpeed()
{
    return speed;
}

//=============================================================================
// Return active.
//=============================================================================
bool Entity::getActive() const
{
    return active;
}

//=============================================================================
// Return intersecting. 
// Intersecting is true if this entity is intersecting another entity.
//=============================================================================
bool Entity::getIntersecting() const
{
    return intersecting;
}

//=============================================================================
// Get collision
//=============================================================================
bool Entity::getCollision()
{
    return collision;
}

//=============================================================================
// Return embedded
// embedded is true if this entity is completely contained within the collision
// area of another entity.
//=============================================================================
bool Entity::getEmbedded() const
{
    return embedded;
}

//=============================================================================
// Return mass.
//=============================================================================
float Entity::getMass() const
{
    return mass;
}

//=============================================================================
// Return bounciness
//=============================================================================
float Entity::getBounciness() const
{
    return bounciness;
}

//=============================================================================
// Return noBounce
//=============================================================================
bool Entity::getNoBounce() const
{
    return noBounce;
}

//=============================================================================
// Return collision type (NONE, CIRCLE, BOX, ROTATED_BOX)
//=============================================================================
entityNS::COLLISION_TYPE Entity::getCollisionType()
{
    return collisionType;
}

//=============================================================================
// Return number of pixels colliding in pixel perfect collision
//=============================================================================
unsigned long Entity::getPixelsColliding() const
{
    return pixelsColliding;
}

//=============================================================================
// Return oldX
//=============================================================================
float Entity::getOldX() const
{
    return oldX;
}

//=============================================================================
// Return oldY
//=============================================================================
float Entity::getOldY() const
{
    return oldY;
}

//=============================================================================
// Get rotatedBoxReady.
//=============================================================================
bool Entity::getRotatedBoxReady()
{
    return rotatedBoxReady;
}


////////////////////////////////////////
//           Set functions            //
////////////////////////////////////////


//=============================================================================
// Set corner c of ROTATED_BOX
//=============================================================================
void Entity::setCorner(vector2_t v, unsigned int n)
{
    if (n < 4)
    {
        corners[n] = v;
    }
}

//=============================================================================
// Set minimum overlap
//=============================================================================
void Entity::setMinOverlap(float overlap)
{
    minOverlap = overlap;
}

//=============================================================================
// Set X location.
//=============================================================================
void Entity::setX(float newX)
{
    curX = newX;
}

//=============================================================================
// Set Y location.
//=============================================================================
void Entity::setY(float newY)
{
    curY = newY;
}

//=============================================================================
// Set Z location.
//=============================================================================
void Entity::setZ(float newZ)
{
    curZ = newZ;
}

//=============================================================================
// Set angle (in radians)
//=============================================================================
void Entity::setAngle(float angle)
{
    curAngle = angle;
}

//=============================================================================
// Set scale
//=============================================================================
void Entity::setScale(float scale)
{
    curScale = scale;
}

//=============================================================================
// Set velocity.
//=============================================================================
void Entity::setVelocity(vector2_t v)
{
    velocity = v;
}

//=============================================================================
// Set delta velocity. Added to velocity in update().
//=============================================================================
void Entity::setDeltaV(vector2_t dv)
{
    deltaV = dv;
}

//=============================================================================
// Set rotation rate, radians/second
//=============================================================================
void Entity::setRotation(float r)
{
    rotation = r;
}

//=============================================================================
// Set speed
//=============================================================================
void Entity::setSpeed(float s)
{
    speed = s;
}

//=============================================================================
// Set active.
//=============================================================================
void Entity::setActive(bool a)
{
    active = a;
}

//=============================================================================
// Set intersecting
//=============================================================================
void Entity::setIntersecting(bool i)
{
    intersecting = i;
}

//=============================================================================
// Set collision Boolean
//=============================================================================
void Entity::setCollision(bool c)
{
    collision = c;
}

//=============================================================================
// Set mass.
//=============================================================================
void Entity::setMass(float m)
{
    mass = m;
}

//=============================================================================
// Set bounciness
// 0 = no bounce, 1 = max bounce.
//=============================================================================
void Entity::setBounciness(float b)
{
    if (b < 0)
    {
        b = 0;
    }
    else if (b > 1)
    {
        b = 1;
    }

    bounciness = b;
}

//=============================================================================
// Set noBounce
//=============================================================================
void Entity::setNoBounce(bool no)
{
    noBounce = no;
}

//=============================================================================
// Set radius of collision circle.
//=============================================================================
void Entity::setCollisionRadius(float r)
{
    radius = r;
}

//=============================================================================
// Set collision center
//=============================================================================
void Entity::setCollisionCenter(vector2_t cc)
{
    collisionCenter = cc;
}

//=============================================================================
// Set collision type (NONE, CIRCLE, BOX, ROTATED_BOX)
//=============================================================================
void Entity::setCollisionType(entityNS::COLLISION_TYPE ctype)
{
    collisionType = ctype;
}

//=============================================================================
// Set embedded
//=============================================================================

void Entity::setEmbedded(bool e)
{
    embedded = e;
}

//=============================================================================
// Set RECT structure used for BOX and ROTATED_BOX collision detection.
//=============================================================================
void Entity::setEdge(rect_t e)
{
    edge = e;
}

//=============================================================================
// Set rotatedBoxReady. Set to false to force recalculation.
//=============================================================================
void Entity::setRotatedBoxReady(bool r)
{
    rotatedBoxReady = r;
}

//=============================================================================
// Initialize the Entity.
//=============================================================================
bool Entity::initialize(const vector2_t& position)
{
    center = position;

    return true;
}

//=============================================================================
// activate the entity
//=============================================================================
void Entity::activate()
{
    active = true;
}

////////////////////////////////////////
//          Move functions            //
////////////////////////////////////////

//=============================================================================
// Rotate using specified rotation rate
//=============================================================================
void Entity::rotate(float frameTime, float rotateRate)
{
    curAngle += frameTime * rotateRate;         // apply rotation
}

//=============================================================================
// Rotate using current rotationRate
//=============================================================================
void Entity::rotate(float frameTime)
{
    curAngle += frameTime * rotation;           // apply rotation
}

//=============================================================================
// Change rotationRate (angular momentum)
//=============================================================================
void Entity::turn(float frameTime, float rotateRate)
{
    rotation += frameTime * rotateRate;
}

//=============================================================================
// Angle to target
//=============================================================================
float Entity::angleToTarget(float x, float y)
{
    float distX = x - Entity::getCenter().x;           // Target X - entity X
    float distY = y - Entity::getCenter().y;           // Target Y - entity Y
    float angle = atan2f(distY, distX);         // Angle to target
    float angleDifference = angle - getAngle();         // Angle difference

    // normalize angleDifference to [-PI,PI] range so we turn shortest direction toward target
    if (angleDifference >= M_PI)
    {
        angleDifference -= 2 * (float)M_PI;
    }
    else if (angleDifference < -M_PI)
    {
        angleDifference += 2 * (float)M_PI;
    }

    return angleDifference;         // Angle to target in radians
}

//=============================================================================
// Angle to target
//=============================================================================
float Entity::angleToTarget(vector2_t target)
{
    return angleToTarget(target.x, target.y);
}

//=============================================================================
// Aim at target
//=============================================================================
float Entity::aimAtTarget(float frameTime, float rotationRate, float x, float y)
{
    float angleDifference = angleToTarget(x, y);            // Angle to target
    rotationRate = fabsf(rotationRate);           // Absolute value

    if (angleDifference > 0)            // If aimed right of target
    {
        rotate(frameTime, -rotationRate);           // Turn left
    }
    else if (angleDifference < 0)           // If aimed left of target
    {
        rotate(frameTime, rotationRate);            // Turn right
    }

    return angleDifference;         // Angle to target in radians
}

//=============================================================================
// Aim at target
//=============================================================================
float Entity::aimAtTarget(float frameTime, float rotationRate, vector2_t target)
{
    return aimAtTarget(frameTime, rotationRate, target.x, target.y);
}

//=============================================================================
// Move in X direction using current velocity
//=============================================================================
void Entity::moveX(float frameTime)
{
    curX += frameTime * velocity.x;         // move X 
}

//=============================================================================
// Move in X direction
//=============================================================================
void Entity::moveX(float frameTime, float speed)
{
    curX += frameTime * speed;          // move X 
}

//=============================================================================
// Move in Y direction using current velocity
//=============================================================================
void Entity::moveY(float frameTime)
{
    curY += frameTime * velocity.y;         // move Y
}

//=============================================================================
// Move in Y direction
//=============================================================================
void Entity::moveY(float frameTime, float speed)
{
    curY += frameTime * speed;          // move Y
}

//=============================================================================
// Move in X,Y direction using current velocity
//=============================================================================
void Entity::move(float frameTime)
{
    moveX(frameTime, velocity.x);
    moveY(frameTime, velocity.y);
}

//=============================================================================
// Move in X,Y direction using specified velocity
//=============================================================================
void Entity::move(float frameTime, vector2_t vel)
{
    moveX(frameTime, vel.x);
    moveY(frameTime, vel.y);
}

//=============================================================================
// MoveTo X,Y at speed. Ignores current velocity.
//=============================================================================
void Entity::moveTo(float frameTime, float speed, float x, float y)
{
    float distX = x - Entity::getCenter().x;            // destination X - current X
    float distY = y - Entity::getCenter().y;            // destination Y - current Y
    float distance = sqrtf(distX * distX + distY * distY);
    float moveTime = distance / speed;          // time required to complete move
    moveX(frameTime, distX / moveTime);         // move in X direction
    moveY(frameTime, distY / moveTime);         // move in Y direction
}

//=============================================================================
// Change the velocity
//=============================================================================
void Entity::accelerate(float frameTime, float speed, vector2_t deltaV)
{
    velocity = AddVector2(velocity, ScaleVector2(deltaV, (speed * frameTime)));
}

//=============================================================================
// move forward (constant velocity)
//=============================================================================
void Entity::forward()
{
    velocity.x = (float)cos(curAngle) * speed;
    velocity.y = (float)sin(curAngle) * speed;
}

//=============================================================================
// move in reverse (constant velocity)
//=============================================================================
void Entity::reverse()
{
    velocity.x = -(float)cos(curAngle) * speed;
    velocity.y = -(float)sin(curAngle) * speed;
}

//=============================================================================
// update
//=============================================================================
void Entity::update(float frameTime)
{
    oldX = curX;
    oldY = curY;
    oldZ = curZ;
    oldAngle = curAngle;
    oldScale = curScale;

    velocity = AddVector2(velocity, deltaV);
    deltaV.x = 0;
    deltaV.y = 0;

    rotatedBoxReady = false;            // for rotatedBox collision detection
    intersecting = false;
    embedded = false;
}

//=============================================================================
// ai (artificial intelligence)
//=============================================================================
void Entity::ai(float frameTime, Entity& ent)
{}

//=============================================================================
// Is this Entity outside the specified rectangle
//=============================================================================
bool Entity::outsideRect(rect_t rect)
{
    if (curX + (edge.max[0]) * getScale() < rect.min[0] ||
        curX - (edge.min[0]) * getScale() > rect.max[0] ||
        curY + (edge.max[1]) * getScale() < rect.min[1] ||
        curY - (edge.min[1]) * getScale() > rect.max[1])
    {
        return true;
    }

    return false;
}

//=============================================================================
// Entity bounces after collision with another entity
//=============================================================================
void Entity::bounce(vector2_t& collisionVector, Entity& ent)
{
    float cUVdotVdiff;
    vector2_t Vdiff = SubtractVector2(ent.getVelocity(), getVelocity());
    vector2_t cUV = NormalizeVector2(collisionVector);          // collision unit vector
    cUVdotVdiff = DotVector2(cUV, Vdiff);          // use velocity difference
    float massRatio = 2.0f;

    if (getMass() != 0)
    {
        massRatio *= (ent.getMass() / (getMass() + ent.getMass()));
    }

    if (massRatio < 0.1f)
    {
        massRatio = 0.1f;
    }

    // bounce
    setDeltaV(AddVector2(getDeltaV(), ScaleVector2(cUV, (massRatio * cUVdotVdiff))));
    setDeltaV(ScaleVector2(getDeltaV(), bounciness));          // how bouncy is this entity

    // if this entity is embedded inside the other entity or the other entity does not
    // react to collision
    if (getEmbedded() || ent.getNoBounce())
    {
        // Move this entity out of collision along the collision vector.
        // The collision vector contains the embedded or overlap distance
        curX += collisionVector.x;
        curY += collisionVector.y;
    }
    else            // the other entity does bounce
    {
        // Move this entity along collision vector using massRatio
        // The collision vector contains the embedded distance.
        curX += collisionVector.x * massRatio;
        curY += collisionVector.y * massRatio;
    }
}

//=============================================================================
// Move this entity to its oldX, oldY, oldZ
//=============================================================================
void Entity::moveToOldXYZ()
{
    curX = oldX;
    curY = oldY;
    curZ = oldZ;
}

//=============================================================================
// move ship out of collision
//=============================================================================
void Entity::toOldPosition()
{
    curX = oldX;
    curY = oldY;
    curZ = oldZ;
    curAngle = oldAngle;
    curScale = oldScale;
    rotation = 0.0f;
}

//=============================================================================
// Force of gravity on this entity from other entity
//=============================================================================
void Entity::gravityForce(Entity* ent, float frameTime)
{
    // if either entity is not active then no gravity effect
    if (!getActive() || !ent->getActive())
    {
        return;
    }

    float rr = powf((ent->getCenter().x - getCenter().x), 2) +
        powf((ent->getCenter().y - getCenter().y), 2);          // Radius squared variable
    float force = (entityNS::GRAVITY * ent->getMass() * mass) / rr;         // Force of gravity

    // --- Using vector math to create gravity vector ---
    // Create vector between entities
    vector2_t gravityV = Vector2(ent->getCenter().x - getCenter().x,
        ent->getCenter().y - getCenter().y);

    // NormalizeVector2 the vector
    gravityV = NormalizeVector2(gravityV);
    // Multipy by force of gravity to create gravity vector
    gravityV = ScaleVector2(gravityV, (force * frameTime));
    // Add gravity vector to moving velocity vector to change direction
    deltaV = AddVector2(deltaV, gravityV);
}


//-----------------------------------------------------------------------------
// 
// COLLISION
// 
//-----------------------------------------------------------------------------


//=============================================================================
// Compute corners of rotated box, projection edges and min and max projections
// 0---1  corner numbers
// |   |
// 3---2
//=============================================================================
void computeRotatedBox(Entity& ent)
{
    if (ent.getRotatedBoxReady() == true)
    {
        return;
    }

    const float angle = ent.getAngle();
    const float scale = ent.getScale();
    float projection = 0.0f;

    vector2_t rotatedX = Vector2(cosf(angle), sinf(angle));
    vector2_t rotatedY = Vector2(-sinf(angle), cosf(angle));

    const rect_t bbox = ent.getEdge();            // bounding box
    const vector2_t center = ent.getCenter();
    vector2_t corner[4] = { 0 };          // for ROTATED_BOX collision detection

    corner[0] = AddVector2(center, AddVector2(ScaleVector2(rotatedX, ((float)bbox.min[0] * scale)),
        ScaleVector2(rotatedY, ((float)bbox.min[1] * scale))));
    corner[1] = AddVector2(center, AddVector2(ScaleVector2(rotatedX, ((float)bbox.max[0] * scale)),
        ScaleVector2(rotatedY, ((float)bbox.min[1] * scale))));
    corner[2] = AddVector2(center, AddVector2(ScaleVector2(rotatedX, ((float)bbox.max[0] * scale)),
        ScaleVector2(rotatedY, ((float)bbox.max[1] * scale))));
    corner[3] = AddVector2(center, AddVector2(ScaleVector2(rotatedX, ((float)bbox.min[0] * scale)),
        ScaleVector2(rotatedY, ((float)bbox.max[1] * scale))));

    ent.setCorner(corner[0], 0);
    ent.setCorner(corner[1], 1);
    ent.setCorner(corner[2], 2);
    ent.setCorner(corner[3], 3);

    ent.setRotatedBoxReady(true);
}

//=============================================================================
// Circular collision detection method.
// Called by collision(), default collision detection method.
// Returns true if collision, false otherwise.
// Sets collisionVector if collision. The collisionVector points in the
// direction of force that would be applied to this entity as a result
// of the collision. The magnitude of the collision vector is the
// distance the entities are overlapping.
//=============================================================================
bool collideCircle(Entity& ent0, Entity& ent1, vector2_t& collisionVector)
{
    // difference between centers
    vector2_t distSquared = SubtractVector2(ent0.getCenter(), ent1.getCenter());
    distSquared.x = distSquared.x * distSquared.x;          // difference squared
    distSquared.y = distSquared.y * distSquared.y;

    // Calculate the sum of the radii (adjusted for scale)
    float sumRadiiSquared = (ent0.getRadius() * ent0.getScale()) + (ent1.getRadius() * ent1.getScale());
    sumRadiiSquared *= sumRadiiSquared;         // square it

    // if entities are colliding
    if (distSquared.x + distSquared.y <= sumRadiiSquared)
    {
        // set collision vector
        collisionVector = SubtractVector2(ent0.getCenter(), ent1.getCenter());
        collisionVector = NormalizeVector2(collisionVector);           // set vector length to 1

        // Calculate overlap
        float overlap = sqrtf(sumRadiiSquared - (distSquared.x + distSquared.y));
        collisionVector = ScaleVector2(collisionVector, overlap);          // Include overlap distance
        ent0.setCollisionCenter(ent0.getCenter());
        ent1.setCollisionCenter(ent1.getCenter());

        return true;
    }

    return false;           // not colliding
}

//=============================================================================
// Axis aligned bounding box collision detection method
// Called by collision()
// Returns true if collision, false otherwise.
// Sets collisionVector if collision. The collisionVector points in the
// direction of force that would be applied to this entity as a result
// of the collision. The magnitude of the collision vector is the
// distance the entities are overlapping.
//=============================================================================
bool collideBox(Entity& ent0, Entity& ent1, vector2_t& collisionVector)
{
    vector2_t ent0Center = ent0.getCenter();
    vector2_t ent1Center = ent1.getCenter();
    rect_t ent0Edge = ent0.getEdge();
    rect_t ent1Edge = ent1.getEdge();
    float ent0Scale = ent0.getScale();
    float ent1Scale = ent1.getScale();

    // Check for collision using Axis Aligned Bounding Box.
    if ((ent0Center.x + ent0Edge.max[0] * ent0Scale >= ent1Center.x + ent1Edge.min[0] * ent1Scale) &&
        (ent0Center.x + ent0Edge.min[0] * ent0Scale <= ent1Center.x + ent1Edge.max[0] * ent1Scale) &&
        (ent0Center.y + ent0Edge.max[1] * ent0Scale >= ent1Center.y + ent1Edge.min[1] * ent1Scale) &&
        (ent0Center.y + ent0Edge.min[1] * ent0Scale <= ent1Center.y + ent1Edge.max[1] * ent1Scale))
    {
        // If we get to here the entities are colliding. The edge with the smallest
        // overlapping section is the edge where the collision is occurring.
        // The collision vector is created perpendicular to the collision edge.

        // Calculate amount of overlap between entities on each edge of box
        float overlapX = 0.0f;
        float overlapY = 0.0f;

        if (ent0Center.x < ent1Center.x)          // if this entity left of other entity
        {
            overlapX = (ent0Center.x + ent0Edge.max[0] * ent0Scale) -
                (ent1Center.x + ent1Edge.min[0] * ent1Scale);
            collisionVector = Vector2(-overlapX, 0);            // collison vector points left
        }
        else            // this entity right of other entity
        {
            overlapX = (ent1Center.x + ent1Edge.max[0] * ent1Scale) -
                (ent0Center.x + ent0Edge.min[0] * ent0Scale);
            collisionVector = Vector2(overlapX, 0);         // collison vector points right
        }

        if (ent0Center.y < ent1Center.y)          // if this entity above other entity
        {
            overlapY = (ent0Center.y + ent0Edge.max[1] * ent0Scale) -
                (ent1Center.y + ent1Edge.min[1] * ent1Scale);

            if (overlapY < overlapX)            // if Y overlap is smaller
            {
                collisionVector = Vector2(0, -overlapY);            // collison vector points up
            }
        }
        else            // this entity below other entity
        {
            overlapY = (ent1Center.y + ent1Edge.max[1] * ent1Scale) -
                (ent0Center.y + ent0Edge.min[1] * ent0Scale);

            if (overlapY < overlapX)            // if Y overlap is smaller
            {
                collisionVector = Vector2(0, overlapY);         // collison vector points down
            }
        }

        ent0.setCollisionCenter(ent0Center);
        ent1.setCollisionCenter(ent1Center);

        return true;            // entities are colliding
    }

    return false;           // entities are not colliding
}

//=============================================================================
// This function projects the other entity onto this entity's edges 01 and 03.
// The projection edges 01 and 03 are created for each entity A and B.
// In this diagram the A and B entities are both aligned with the 01
// and 03 axes but that may not be the case with actual entities.
//
// Projections as they appear from this entity
//
//                    entA01min
//                   /     entB01min
//                  /     / entA01max 
//                 /     / /  entB01max
//                /     / /  /
//            0--------------------1
// entB03min..|          ____  
// entA03min..|    _____|_ B |   A = this entity
//            |   | A   | |  |   B = entB entity
// entA03max..|   |_____|_|  |
// entB03max..|         |____|
//            | 
//            |   
//            3
//            
// Pre: The current entity's projections have been calculated in the computeRotatedBox function.
// Post: returns true if projections overlap, false otherwise
//       if projections overlap
//           minOverlap contains minimum overlap distance
//           collisionVector is set
//=============================================================================
bool projectionsOverlap(Entity& entA, Entity& entB, vector2_t& collisionVector)
{
    vector2_t edge01, edge03;         // edges used for projection
    vector2_t* corner = entA.getCorners();          // for ROTATED_BOX collision detection
    // min and max projections for this entity
    float entA01min, entA01max, entA03min, entA03max;
    // min and max projections for other entity
    float entB01min, entB01max, entB03min, entB03max;
    float projection;
    float overlap01, overlap03;

    // corners[0] is used as origin
    // The two edges connected to corners[0] are used as the projection lines
    edge01 = Vector2(corner[1].x - corner[0].x, corner[1].y - corner[0].y);
    edge01 = NormalizeVector2(edge01);
    edge03 = Vector2(corner[3].x - corner[0].x, corner[3].y - corner[0].y);
    edge03 = NormalizeVector2(edge03);

    // this entities min and max projection onto edges
    projection = DotVector2(edge01, corner[0]);
    entA01min = projection;
    entA01max = projection;
    // project onto edge01
    projection = DotVector2(edge01, corner[1]);

    if (projection < entA01min)
    {
        entA01min = projection;
    }
    else if (projection > entA01max)
    {
        entA01max = projection;
    }

    // project onto edge03
    projection = DotVector2(edge03, corner[0]);
    entA03min = projection;
    entA03max = projection;
    projection = DotVector2(edge03, corner[3]);

    if (projection < entA03min)
    {
        entA03min = projection;
    }
    else if (projection > entA03max)
    {
        entA03max = projection;
    }

    // project other box onto edge01
    projection = DotVector2(edge01, entB.getCorner(0));            // project corner 0
    entB01min = projection;
    entB01max = projection;

    // for each remaining corner
    for (int c = 1; c < 4; c++)
    {
        // project corner onto edge01
        projection = DotVector2(edge01, entB.getCorner(c));

        if (projection < entB01min)
        {
            entB01min = projection;
        }
        else if (projection > entB01max)
        {
            entB01max = projection;
        }
    }

    // if projections do not overlap
    if (entB01min > entA01max || entB01max < entA01min)
    {
        return false;           // no collision is possible
    }

    // project other box onto edge03
    projection = DotVector2(edge03, entB.getCorner(0));            // project corner 0
    entB03min = projection;
    entB03max = projection;

    // for each remaining corner
    for (int c = 1; c < 4; c++)
    {
        // project corner onto edge03
        projection = DotVector2(edge03, entB.getCorner(c));

        if (projection < entB03min)
        {
            entB03min = projection;
        }
        else if (projection > entB03max)
        {
            entB03max = projection;
        }
    }

    if (entB03min > entA03max || entB03max < entA03min)         // if projections do not overlap
    {
        return false;           // no collision is possible
    }

    // The projections overlap. 
    // Calculate the amount of overlap along edge01 and edge03.
    // Sets a possible collision vector.
    if (entA01min < entB01min)          // if A left of B
    {
        overlap01 = entA01max - entB01min;
        collisionVector = SubtractVector2(corner[0], corner[1]);
        entA.setCollisionCenter(corner[0]);
        entB.setCollisionCenter(corner[1]);
    }
    else            // else, A right of B
    {
        overlap01 = entB01max - entA01min;
        collisionVector = SubtractVector2(corner[1], corner[0]);
        entA.setCollisionCenter(corner[1]);
        entB.setCollisionCenter(corner[0]);
    }

    entA.setMinOverlap(overlap01);         // set minimum overlap

    if (entA03min < entB03min)          // if A above B
    {
        overlap03 = entA03max - entB03min;

        if (overlap03 < overlap01)
        {
            collisionVector = SubtractVector2(corner[0], corner[3]);
            entA.setMinOverlap(overlap03);         // minimum overlap
            entA.setCollisionCenter(corner[0]);
            entB.setCollisionCenter(corner[3]);
        }
    }
    else            // else, A below B
    {
        overlap03 = entB03max - entA03min;

        if (overlap03 < overlap01)
        {
            collisionVector = SubtractVector2(corner[3], corner[0]);
            entA.setMinOverlap(overlap03);         // minimum overlap
            entA.setCollisionCenter(corner[3]);
            entB.setCollisionCenter(corner[0]);
        }
    }

    return true;            // projections overlap
}

//=============================================================================
// Rotated Box collision detection method
// Called by collision()
// Uses Separating Axis Test to detect collision. 
// The separating axis test:
// Two boxes are not colliding if their projections onto a line do not overlap.
// The current entity is A the other entity is B
// Returns true if collision, false otherwise
// Sets collisionVector if collision. The collisionVector points in the
// direction of force that would be applied to this entity as a result
// of the collision. The magnitude of the collision vector is the
// distance the entities are overlapping.
//=============================================================================
bool collideRotatedBox(Entity& ent0, Entity& ent1, vector2_t& collisionVector)
{
    vector2_t collisionVect1, collisionVect2;         // temp collision vectors

    computeRotatedBox(ent0);            // prepare rotated box
    computeRotatedBox(ent1);            // prepare rotated box

    if (projectionsOverlap(ent0, ent1, collisionVect1) && projectionsOverlap(ent1, ent0, collisionVect2))
    {
        // If we get here the entities are colliding. The edge with the
        // smallest overlapping section is the edge where the collision is
        // occurring. The collision vector is created perpendicular to the
        // collision edge. 

        if (ent0.getMinOverlap() < ent1.getMinOverlap())           // if this entity has the smallest overlap
        {
            collisionVector = collisionVect1;           // use collisionVect1
            collisionVector = NormalizeVector2(collisionVector);           // normalize the collision vector
            collisionVector = ScaleVector2(collisionVector, ent0.getMinOverlap());           // collision vector contains the overlap distance
        }
        else
        {
            // Use inverted collisionVect2 so the direction is correct for *this entity.
            collisionVector = ScaleVector2(collisionVect2, -1.0f);
            collisionVector = NormalizeVector2(collisionVector);           // normalize the collision vector
            collisionVector = ScaleVector2(collisionVector, ent1.getMinOverlap());          // collision vector contains the overlap distance
        }

        return true;
    }

    return false;
}

//=============================================================================
// The box corner is checked for collision with circle using a distance check.
// Called by collideRotatedBoxCircle().  This entity is rotated box. The other
// entity is the circle.
// Returns true if collision, false otherwise
// Sets collisionVector if collision. The collisionVector points in the
// direction of force that would be applied to this entity as a result
// of the collision. The magnitude of the collision vector is the
// distance the entities are overlapping.
//=============================================================================
bool collideCornerCircle(int32_t corner, Entity& ent0, Entity& ent1, vector2_t& collisionVector)
{
    vector2_t distSquared = SubtractVector2(ent0.getCorner(corner), ent1.getCenter());            // corner - circle
    distSquared.x = distSquared.x * distSquared.x;          // difference squared
    distSquared.y = distSquared.y * distSquared.y;

    // Calculate the sum of the radii, then square it
    float sumRadiiSquared = ent1.getRadius() * ent1.getScale();           // (0 + circleR)
    sumRadiiSquared *= sumRadiiSquared;         // square it

    // if corner and circle are colliding
    if (distSquared.x + distSquared.y <= sumRadiiSquared)
    {
        // Set collision vector between corner and center
        collisionVector = SubtractVector2(ent0.getCorner(corner), ent1.getCenter());
        collisionVector = NormalizeVector2(collisionVector);           // set vector length to 1

        // Calculate overlap
        float overlap = sqrtf(sumRadiiSquared - (distSquared.x + distSquared.y));
        collisionVector = ScaleVector2(collisionVector, overlap);          // Include overlap distance
        ent0.setCollisionCenter(ent0.getCorner(corner));
        ent1.setCollisionCenter(ent1.getCenter());

        return true;
    }

    return false;
}

//=============================================================================
// Rotated Box and Circle collision detection method
// Called by collision()
// Uses separating axis test on edges of box and radius of circle.
// If the circle center is outside the lines extended from the collision box
// edges (also known as the Voronoi region) then the nearest box corner is checked
// for collision using a distance check.
// The nearest corner is determined from the overlap tests.
//
//   Voronoi0 |   | Voronoi1
//         ---0---1---
//            |   |
//         ---3---2---
//   Voronoi3 |   | Voronoi2
//
// This entity (entA) must be rotated box and other entity (entB) must be circle.
// Returns true if collision, false otherwise.
// Sets collisionVector if collision. The collisionVector points in the
// direction of force that would be applied to this entity as a result
// of the collision. The magnitude of the collision vector is the
// distance the entities are overlapping.
//=============================================================================
bool collideRotatedBoxCircle(Entity& entA, Entity& entB, vector2_t& collisionVector)
{
    vector2_t edge01, edge03;         // edges used for projection
    vector2_t* corner = entA.getCorners();         // for ROTATED_BOX collision detection

    // min and max projections for this entity
    float entA01min, entA01max, entA03min, entA03max;
    // min and max projections for other entity
    float entB01min, entB01max, entB03min, entB03max;
    float center01, center03, overlap01, overlap03;
    float projection;

    computeRotatedBox(entA);            // prepare rotated box

    // corners[0] is used as origin
    // The two edges connected to corners[0] are used as the projection lines
    edge01 = Vector2(corner[1].x - corner[0].x, corner[1].y - corner[0].y);
    edge01 = NormalizeVector2(edge01);
    edge03 = Vector2(corner[3].x - corner[0].x, corner[3].y - corner[0].y);
    edge03 = NormalizeVector2(edge03);

    // this entities min and max projection onto edges
    projection = DotVector2(edge01, corner[0]);
    entA01min = projection;
    entA01max = projection;

    // project onto edge01
    projection = DotVector2(edge01, corner[1]);

    if (projection < entA01min)
    {
        entA01min = projection;
    }
    else if (projection > entA01max)
    {
        entA01max = projection;
    }

    // project onto edge03
    projection = DotVector2(edge03, corner[0]);
    entA03min = projection;
    entA03max = projection;
    projection = DotVector2(edge03, corner[3]);

    if (projection < entA03min)
    {
        entA03min = projection;
    }
    else if (projection > entA03max)
    {
        entA03max = projection;
    }

    // project circle center onto edge01
    center01 = DotVector2(edge01, entB.getCenter());
    entB01min = center01 - entB.getRadius() * entB.getScale();          // min and max are Radius from center
    entB01max = center01 + entB.getRadius() * entB.getScale();

    if (entB01min > entA01max || entB01max < entA01min)         // if projections do not overlap
    {
        return false;           // no collision is possible
    }

    // project circle center onto edge03
    center03 = DotVector2(edge03, entB.getCenter());
    entB03min = center03 - entB.getRadius() * entB.getScale();          // min and max are Radius from center
    entB03max = center03 + entB.getRadius() * entB.getScale();

    if (entB03min > entA03max || entB03max < entA03min)         // if projections do not overlap
    {
        return false;           // no collision is possible
    }

    // circle projection overlaps box projection
    // check to see if circle is in voronoi region of collision box
    if (center01 < entA01min && center03 < entA03min)           // if circle in Voronoi0
    {
        return collideCornerCircle(0, entA, entB, collisionVector);
    }

    if (center01 > entA01max && center03 < entA03min)           // if circle in Voronoi1
    {
        return collideCornerCircle(1, entA, entB, collisionVector);
    }

    if (center01 > entA01max && center03 > entA03max)           // if circle in Voronoi2
    {
        return collideCornerCircle(2, entA, entB, collisionVector);
    }

    if (center01 < entA01min && center03 > entA03max)           // if circle in Voronoi3
    {
        return collideCornerCircle(3, entA, entB, collisionVector);
    }

    // If we reach this point the circle and the rotated box are colliding

    // Special case 1, the circle is inside the box (this is called tunneling). 
    // High closing velocities, an unusually long frameTime or very small entities may all contribute to this.
    // This creates two problems:
    //    1. How to determine the collision vector.
    //    2. How to make sure the entities are no longer colliding after bounce function is called.
    //       The bounce function uses the embedded flag.
    //
    // A is this entity (rotated box), B is other entity (circle).
    // A' and B' are entites at previous frame's X,Y
    //                    entA01min
    //                   /   entB01min
    //                  /   /    entB01max 
    //                 /   /    /  entA01max 
    //                /   /    /  /         
    //            0--------------------1    
    //            |                   ___        ___________ 
    //            |                  / B'\      | A'        |
    //            |                  \___/      |           |
    //            |                             |           |
    // entA03min..|    ___________              |___________|
    // entB03min..|   | A  ___    |    
    //            |   |   / B \   |    
    // entB03max..|   |   \___/   |    
    // entA03max..|   |___________|    
    //            3
    //                 case 1    
    //
    // Special case 2, the box is inside the circle (this is called tunneling).
    // A is this entity (rotated box), B is other entity (circle).
    // A' and B' are entites at previous frame's X,Y
    //                   ___
    //                  | A'|
    //                  |___|
    //       ______              ______
    //      /B ___ \            /B'    \
    //     /  | A | \          /        \
    //     \  |___| /          \        /
    //      \______/            \______/
    //                 case 2
    // if box A is inside circle B

    // if circle B is inside box A OR if box A is inside circle B
    if ((entB01min > entA01min && entB01max < entA01max && entB03min > entA03min && entB03max < entA03max) ||
        (entA01min > entB01min && entA01max < entB01max && entA03min > entB03min && entA03max < entB03max))
    {
        // Treat B as stationary and calculate the velocity of A relative to B.
        vector2_t Vdiff = SubtractVector2(entA.getVelocity(), entB.getVelocity());          // velocity difference

        // Use the largest component (X or Y) of Vdiff to determine which edge of box A the collision occurred on.
        // This is a bit of a hack because it is not 100% accurate but that is OK. It's quick and we don't need to
        // be perfect.
        if (fabsf(Vdiff.x) > fabsf(Vdiff.y))            // if X velocity > Y velocity
        {
            if (Vdiff.x > 0)            // if A moving right relative to B
            {
                overlap01 = entA01max - entB01min;          // embedded distance
                collisionVector = SubtractVector2(entA.getCorner(0), entA.getCorner(1));
            }
            else            // else, A moving left relative to B
            {
                overlap01 = entB01max - entA01min;          // embedded distance
                collisionVector = SubtractVector2(entA.getCorner(1), entA.getCorner(0));
            }

            collisionVector = NormalizeVector2(collisionVector);           // normalize the collision vector
            collisionVector = ScaleVector2(collisionVector, overlap01);            // collision vector contains the embedded distance
        }
        else            // Y velocity >= X velocity
        {
            if (Vdiff.y > 0)            // if A moving down relative to B
            {
                overlap03 = entA03max - entB03min;          // embedded distance
                collisionVector = SubtractVector2(entA.getCorner(0), entA.getCorner(3));
            }
            else            // else, B moving up relative to A
            {
                overlap03 = entB03max - entA03min;          // embedded distance
                collisionVector = SubtractVector2(entA.getCorner(3), entA.getCorner(0));
            }

            collisionVector = NormalizeVector2(collisionVector);           // normalize the collision vector
            collisionVector = ScaleVector2(collisionVector, overlap03);            // collision vector contains the embedded distance
        }

        entB.setEmbedded(true);         // entity B is completely contained within the collision area of A

        return true;
    }
    else
    {
        entB.setEmbedded(false);
    }

    // Normal case
    // Circle not in voronoi region so it is colliding with edge of box.
    // The edge with the smallest overlapping section is the edge where the
    // collision is occurring. The collision vector is created perpendicular
    // to the collision edge. The projection edges are 01 and 03.
    // A is this entity (rotated box), B is other entity (circle).
    //                    entA01min
    //                   /   entB01min
    //                  /   /    entB01max 
    //                 /   /    /  entA01max 
    //                /   /    /  /         
    //            0--------------------1    
    // entB03min..|        ___              
    // entA03min..|    ___/ B \__           
    // entB03max..|   |   \___/  |          
    //            |   |          |          
    // entA03max..|   |  A(this) |          
    //            |   |__________|          
    //            |             
    //            |    normal case  
    //            3
    //            
    if (entA01min < entB01min)          // if A left of B
    {
        overlap01 = entA01max - entB01min;          // Overlap along 0..1 axis
        collisionVector = SubtractVector2(entA.getCorner(0), entA.getCorner(1));
        entA.setCollisionCenter(entA.getCorner(0));
        entB.setCollisionCenter(entA.getCorner(1));
    }
    else            // else, A right of B
    {
        overlap01 = entB01max - entA01min;          // Overlap along 0..1 axis
        collisionVector = SubtractVector2(entA.getCorner(1), entA.getCorner(0));

        entA.setCollisionCenter(entA.getCorner(1));
        entB.setCollisionCenter(entA.getCorner(0));
    }

    if (entA03min < entB03min)          // if A above B
    {
        overlap03 = entA03max - entB03min;          // Overlap along 0..3 axis

        if (overlap03 < overlap01)
        {
            collisionVector = SubtractVector2(entA.getCorner(0), entA.getCorner(3));
            entA.setCollisionCenter(entA.getCorner(0));
            entB.setCollisionCenter(entA.getCorner(3));
        }
    }
    else            // else, A below B
    {
        overlap03 = entB03max - entA03min;          // Overlap along 0..3 axis

        if (overlap03 < overlap01)
        {
            collisionVector = SubtractVector2(entA.getCorner(3), entA.getCorner(0));
            entA.setCollisionCenter(entA.getCorner(3));
            entB.setCollisionCenter(entA.getCorner(0));
        }
    }

    collisionVector = NormalizeVector2(collisionVector);           // normalize the collision vector

    if (overlap03 < overlap01)
    {
        collisionVector = ScaleVector2(collisionVector, overlap03);
    }
    else
    {
        collisionVector = ScaleVector2(collisionVector, overlap01);            // collision vector contains the overlap distance
    }

    return true;
}

//=============================================================================
// Does this entity intersect with ent?
// Each entity must use a single collision type. Complex shapes that require
// multiple collision types may be done by treating each part as a separate
// entity or by using PIXEL_PERFECT collision. PIXEL_PERFECT collision detection
// is slower and returns a less accurate collision vector so it is not good for
// realistic physics.
// Returns true if the entities are intersecting.
// Sets the collision vector if intersecting.
// Sets intersecting true if this entity is intersecting another entity.
// The collisionVector points in the direction of force that would be applied
// to this entity as a result of the collision. (e.g. If this entity is a ball
// that is dropped onto a box, the collision vector would point up (-Y).
//=============================================================================
bool intersects(Entity& ent0, Entity& ent1, vector2_t& collisionVector)
{
    bool result = false;

    // If either entity is using PIXEL_PERFECT collision
    // If both entities are using CIRCLE collision
    if (ent0.getCollisionType() == entityNS::CIRCLE && ent1.getCollisionType() == entityNS::CIRCLE)
    {
        result = collideCircle(ent0, ent1, collisionVector);
    }
    // If both entities are using BOX collision
    else if (ent0.getCollisionType() == entityNS::BOX && ent1.getCollisionType() == entityNS::BOX)
    {
        result = collideBox(ent0, ent1, collisionVector);
    }
    // All other combinations use separating axis test.
    else
    {
        // If neither entity uses CIRCLE collision.
        if (ent0.getCollisionType() != entityNS::CIRCLE && ent1.getCollisionType() != entityNS::CIRCLE)
        {
            result = collideRotatedBox(ent0, ent1, collisionVector);
        }
        else    // one of the entities is a circle
        {
            if (ent0.getCollisionType() == entityNS::CIRCLE)  // if this entity uses CIRCLE collision
            {
                // Check for collision from other box with our circle
                result = collideRotatedBoxCircle(ent1, ent0, collisionVector);
                collisionVector = ScaleVector2(collisionVector, -1);  // put collisionVector in proper direction
            }
            else    // the other entity uses CIRCLE collision
            {
                result = collideRotatedBoxCircle(ent1, ent0, collisionVector);
            }
        }
    }

    ent0.setIntersecting(result);
    ent0.setCollision(result);
    ent1.setIntersecting(result);
    ent1.setCollision(result);

    return result;
}

//=============================================================================
// Perform collision detection between this entity and the other Entity.
// Calls intersects function.
// Each entity must use a single collision type. Complex shapes that require
// multiple collision types may be done by treating each part as a separate
// entity or by using PIXEL_PERFECT collision.  PIXEL_PERFECT collision detection
// is slower and returns a less accurate collision vector so it is not good for
// realistic physics. 
// Typically called once per frame.
// The collision types: CIRCLE, BOX, ROTATED_BOX or PIXEL_PERFECT.
// 
// Returns true if collision, false otherwise. A collision is occurring if
// the entities are intersecting and moving toward each other. If the
// entities are intersecting and moving apart it is assumed they are
// intersecting as the result of a previous collision; false is returned.
// Sets intersecting true if collision.
// Sets collisionVector if collision. The collisionVector points in the
// direction of force that would be applied to this entity as a result
// of the collision. (e.g. If this entity is a ball that is dropped
// onto a box, the collision vector would point up (-Y).
//=============================================================================
bool collidesWith(Entity& ent0, Entity& ent1, vector2_t& collisionVector)
{
    // if either entity is not active then no collision may occcur
    if (!ent0.getActive() || !ent1.getActive())
    {
        return false;
    }

    vector2_t cv;
    // Are the entities intersecting? Sets intersecting bool.
    bool collide = intersects(ent0, ent1, cv);

    if (collide == false)
    {
        return false;           // not colliding
    }

    vector2_t Vdiff = SubtractVector2(ent0.getVelocity(), ent1.getVelocity());          // velocity difference
    vector2_t cUV = NormalizeVector2(cv);           // collision unit vector
    float cUVdotVdiff = DotVector2(cUV, Vdiff);

    // If cUVdotVdiff > 0 it indicates the entities are moving apart. This may indicate
    // that the entities are still intersecting from an earlier collision.
    if (cUVdotVdiff > 0)            // Entities are moving apart.
    {
        return false;           // Not a new collision.
    }

    collisionVector = cv;           // Set collisionVector.

    return true;            // Entites are colliding.
}
