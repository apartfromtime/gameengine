#pragma once
#include <GEUL\g_math.h>

namespace entityNS
{
    const float W = 16.0f;          // x-size
    const float H = 16.0f;          // y-size
    const float D = 2.0f;           // z-size
    const float X = W / 2.0f;           // origin
    const float Y = H / 2.0f;
    const float Z = D / 2.0f;
    enum COLLISION_TYPE { NONE, CIRCLE, BOX, ROTATED_BOX, PIXEL_PERFECT };
    const float COLLISION_RADIUS = W / 2.0f;            // for circular collision
    const float GRAVITY = 6.67428e-11f;         // gravitational constant
    const float ROTATION_RATE = (float)M_PI / 3;            // radians per second
    const float SPEED = 100.0f;         // 100 pixels per second
    const float MASS = 1.0f;            // mass
}

class Entity
{
    // Entity properties
private:
    // Collision
    vector2_t corners[4];           // for ROTATED_BOX collision detection
    vector2_t collisionCenter;          // center of collision
    float   minOverlap;         // projection overlaps
    float   radius;         // radius of collision circle
    // edge specifies the collision box relative to the center of the entity.
    // left and top are typically negative numbers
    rect_t edge;            // for BOX and ROTATED_BOX collision detection
    entityNS::COLLISION_TYPE collisionType;
    bool    rotatedBoxReady;            // true when rotated collision box is ready
    bool    intersecting;           // true when this entity is intersecting another entity
    bool    collision;          // true when ship is colliding
    // The bounce function will perform an extra move of the entity when embedded is true.
    bool    embedded;           // true when this entity is completely contained within the collision area of another entity.
    unsigned long pixelsColliding;          // number of pixels colliding in pixel perfect collision
    bool    noBounce;           // true indicates this entity does not move as a result of a collision
    // Physics
    vector2_t velocity;         // velocity
    vector2_t deltaV;           // added to velocity during next call to update()
    vector2_t center;           // center of entity
    float   curX, curY, curZ, curAngle, curScale;           // the location of the entity in the current frame
    float   oldX, oldY, oldZ, oldAngle, oldScale;           // the location of the entity in the previous frame
    float   rotation;           // current rotation rate (radians/second)
    float   speed;          // pixels per second
    float   mass;           // mass of entity
    float   bounciness;         // how bouncy is this entity 0 (none) through 1 (max)
    bool    active;         // only active entities may collide

public:

    // Constructor
    Entity();
    // Destructor
    ~Entity();

    ////////////////////////////////////////
    //           Get functions            //
    ////////////////////////////////////////

    // Return X position.
    float getX();

    // Return Y position.
    float getY();

    // Return Z position.
    float getZ();

    // Return angle (in radians)
    float getAngle();

    // Return scale
    float getScale();

    // Return center of scaled Entity as screen x,y.
    const vector2_t getCenter();

    // Return collision center
    const vector2_t getCollisionCenter();

    // Return radius of collision circle.
    float getRadius() const;

    // Return RECT structure used for BOX and ROTATED_BOX collision detection.
    const rect_t& getEdge() const;

    // Return corner c of ROTATED_BOX
    const vector2_t getCorner(unsigned int c) const;

    // Return corners array
    vector2_t* getCorners();

    // Return projection overlaps used in rotated box collision
    float getMinOverlap();

    // Return velocity vector.
    const vector2_t getVelocity() const;

    // Get delta velocity. Added to velocity in update().
    vector2_t getDeltaV();

    // Returns rotation rate
    float getRotation();

    // Get speed
    float getSpeed();

    // Return active.
    bool  getActive() const;

    // Return intersecting. 
    // Intersecting is true if this entity is intersecting another entity.
    bool getIntersecting() const;

    // Get collision
    bool getCollision();

    // Return embedded
    // embedded is true if this entity is completely contained within the collision area of another entity.
    bool getEmbedded() const;

    // Return mass.
    float getMass() const;

    // Return bounciness
    float getBounciness() const;

    // Return noBounce
    bool getNoBounce() const;

    // Return collision type (NONE, CIRCLE, BOX, ROTATED_BOX, PIXEL_PERFECT)
    entityNS::COLLISION_TYPE getCollisionType();

    // Return number of pixels colliding in pixel perfect collision
    unsigned long getPixelsColliding() const;

    // Return oldX
    float getOldX() const;

    // Return oldY
    float getOldY() const;

    // Set rotatedBoxReady. Set to false to force recalculation.
    bool getRotatedBoxReady();


    ////////////////////////////////////////
    //           Set functions            //
    ////////////////////////////////////////

    // Set corner c of ROTATED_BOX
    void setCorner(vector2_t v, unsigned int n);

    // Set minimum overlap
    void setMinOverlap(float overlap);

    // Set X location.
    void setX(float newX);

    // Set Y location.
    void setY(float newY);

    // Set Z location.
    void setZ(float newZ);

    // Set angle (in radians)
    void setAngle(float angle);

    // Set scale
    void setScale(float scale);

    // Set velocity.
    void  setVelocity(vector2_t v);

    // Set delta velocity. Added to velocity in update().
    void  setDeltaV(vector2_t dv);

    // Set rotation rate, radians/second
    void setRotation(float r);

    // Set speed
    void setSpeed(float s);

    // Set active.
    void  setActive(bool a);

    void setIntersecting(bool i);

    // Set collision Boolean
    void setCollision(bool c);

    // Set mass.
    void  setMass(float m);

    // Set bounciness
    // 0 = no bounce, 1 = max bounce.
    void setBounciness(float b);

    // Set noBounce
    // true indicates this entity does not move as a result of a collision
    void setNoBounce(bool no);

    // Set radius of collision circle.
    void setCollisionRadius(float r);

    // Set collision center
    void setCollisionCenter(vector2_t cc);

    // Set collision type (NONE, CIRCLE, BOX, ROTATED_BOX, PIXEL_PERFECT)
    void setCollisionType(entityNS::COLLISION_TYPE ctype);

    // Set embedded
    // embedded is true if this entity is completely contained within the collision area of another entity.
    void setEmbedded(bool e);

    // Set RECT structure used for BOX and ROTATED_BOX collision detection.
    // edge specifies the collision box relative to the center of the entity.
    // left and top are typically negative numbers.
    void setEdge(rect_t e);

    // Set rotatedBoxReady. Set to false to force recalculation.
    void setRotatedBoxReady(bool r);

    ////////////////////////////////////////
    //          Move functions            //
    ////////////////////////////////////////

    //=============================================================================
    // Rotate using specified rotation rate
    // Pre: frameTime = Elapsed time in seconds
    //      rotateRate = radians/second, + clockwise, - counterclockwise
    // Post: Current rotation rate is unchanged (no change in momentum)
    //=============================================================================
    void rotate(float frameTime, float rotateRate);

    //=============================================================================
    // Rotate using current rotationRate
    // Pre: frameTime = Elapsed time in seconds
    // Post: Current rotation rate is unchanged (no change in momentum)
    // Call this from the game's update method to give the Entity momentum.
    //=============================================================================
    void rotate(float frameTime);

    //=============================================================================
    // Change rotationRate (angular momentum)
    // Pre: frameTime = Elapsed time in seconds
    //      rotateRate = radians/second, + clockwise, - counterclockwise
    // Post: Current rotation rate is changed (change in momentum)
    // The Entity does not turn until the rotate method is called.
    //=============================================================================
    void turn(float frameTime, float rotateRate);

    //=============================================================================
    // Angle to target
    // Pre:  x,y = Target center
    // Returns: Angle to target in radians. + target is clockwise, - target is counterclockwise
    //=============================================================================
    float angleToTarget(float x, float y);

    //=============================================================================
    // Angle to target
    // Pre:  target = Target center
    // Returns: Angle to target in radians. + target is clockwise, - target is counterclockwise
    //=============================================================================
    float angleToTarget(vector2_t target);

    //=============================================================================
    // Aim at target
    // Pre: frameTime = Elapsed time in seconds
    //      rotationRate = radians/second
    //      x,y = Target center
    // Post: Current rotation rate is unchanged (no change in momentum)
    // Returns: Angle to target in radians. + target is clockwise, - target is counterclockwise
    //=============================================================================
    float aimAtTarget(float frameTime, float rotationRate, float x, float y);

    //=============================================================================
    // Aim at target
    // Pre: frameTime = Elapsed time in seconds
    //      rotationRate = radians/second
    //      target = Target center
    // Post: Current rotation rate is unchanged (no change in momentum)
    // Returns: Angle to target in radians. + target is clockwise, - target is counterclockwise
    //=============================================================================
    float aimAtTarget(float frameTime, float rotationRate, vector2_t target);

    //=============================================================================
    // Move in X direction using current velocity
    // Pre: frameTime = Elapsed time in seconds
    // Post: Current velocity vector is unchanged (no change in momentum)
    //=============================================================================
    void moveX(float frameTime);

    //=============================================================================
    // Move in X direction
    // Pre: frameTime = Elapsed time in seconds
    //      speed = Pixels/second (+ right, - left)
    // Post: Current velocity vector is unchanged (no change in momentum)
    //=============================================================================
    void moveX(float frameTime, float speed);

    //=============================================================================
    // Move in Y direction using current velocity
    // Pre: frameTime = Elapsed time in seconds
    // Post: Current velocity vector is unchanged (no change in momentum)
    //=============================================================================
    void moveY(float frameTime);

    //=============================================================================
    // Move in Y direction
    // Pre: frameTime = Elapsed time in seconds
    //      speed = Pixels/second (+ down, - up)
    // Post: Current velocity vector is unchanged (no change in momentum)
    //=============================================================================
    void moveY(float frameTime, float speed);

    //=============================================================================
    // Move in X,Y direction using current velocity
    // Pre: frameTime = Elapsed time in seconds
    // Post: Current velocity vector is unchanged (no change in momentum)
    // Call this from the game's update method to give the Entity momentum.
    //=============================================================================
    void move(float frameTime);

    //=============================================================================
    // Move in X,Y direction using specified velocity
    // Pre: frameTime = Elapsed time in seconds
    //      vel = Velocity vector
    // Post: Current velocity vector is unchanged (no change in momentum)
    //=============================================================================
    void move(float frameTime, vector2_t vel);

    //=============================================================================
    // MoveTo X,Y at speed. Ignores current velocity.
    // Pre: frameTime = Elapsed time in seconds
    //      speed = Movement speed in pixels/second
    //      x,y = Destination location (center)
    // Post: Current velocity vector is unchanged (no change in momentum)
    //=============================================================================
    void moveTo(float frameTime, float speed, float x, float y);

    //=============================================================================
    // Change the velocity
    // Pre: frameTime = Elapsed time in seconds
    //      speed = Movement speed in pixels/second
    //      vel = Velocity vector
    // Post: Current velocity vector is changed (change in momentum)
    //=============================================================================
    void accelerate(float frameTime, float speed, vector2_t deltaV);

    // move forward
    void forward();

    // move in reverse
    void reverse();

    ////////////////////////////////////////
    //         Other functions            //
    ////////////////////////////////////////

    //=============================================================================
    // update
    // Typically called once per frame.
    // Pre: frameTime = Elapsed time of the prevous frame in seconds.
    //                  It is used to regulate the speed of movement and animation.
    //=============================================================================
    void update(float frameTime);

    //=============================================================================
    // Initialize Entity
    //=============================================================================
    bool initialize(const vector2_t& position);

    //=============================================================================
    // Activate Entity. Only active etities may collide.
    //=============================================================================
    void activate();

    //=============================================================================
    // ai (artificial intelligence)
    // Empty ai function to allow Entity objects to be instantiated.
    // typically called once per frame
    // performs ai calculations, ent is passed for interaction
    //=============================================================================
    void ai(float frameTime, Entity& ent);

    //=============================================================================
    // Is this Entity outside the specified rectangle
    // Post: returns true if outside rect, false otherwise
    //=============================================================================
    bool outsideRect(rect_t rect);

    //=============================================================================
    // Entity bounces after collision with another entity
    // Pre: ent is the other entity.
    //      The collisionVector has been set with direction of force that should be
    //      applied to this entity. The magnitude of collisionVector is the
    //      distance the two entities are overlapping.
    // Post: The deltaV of the current entity is set. The deltaV will be applied
    //      to this entity in the update function.
    //      This entity is moved in the direction of the collison vector by a
    //      distance proportional to the mass ratio with the other entity.
    //=============================================================================
    void bounce(vector2_t& collisionVector, Entity& ent);

    //=============================================================================
    // Move this entity to its oldX, oldY
    //=============================================================================
    void moveToOldXYZ();

    //=============================================================================
    // Move entity out of collision
    //=============================================================================
    void toOldPosition();

    //=============================================================================
    // Force of gravity on this entity from other entity
    // Adds the gravitational force to the velocity vector of this entity
    // force = (GRAVITY * m1 * m2) / (r * r)
    //
    //  (r * r)  = (Ax - Bx) ^ 2 + (Ay - By) ^ 2
    //=============================================================================
    void gravityForce(Entity* other, float frameTime);
};

//=============================================================================
// Perform collision detection between this entity and the other Entity.
//=============================================================================
bool collidesWith(Entity& ent0, Entity& ent1, vector2_t& collisionVector);

