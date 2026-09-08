/**
 * \file      SLRay.cpp
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <atomic>
#include <cmath>
#include <ctime>
#include <random>

#include <SLRay.h>
#include <SLSceneView.h>
#include <SLSkybox.h>

// init static variables
SLint   SLRay::maxDepth         = 0;
SLfloat SLRay::minContrib       = 1.0 / 256.0;
SLuint  SLRay::primaryRays      = 0;
SLuint  SLRay::reflectedRays    = 0;
SLuint  SLRay::refractedRays    = 0;
SLuint  SLRay::ignoredRays      = 0;
SLuint  SLRay::shadowRays       = 0;
SLuint  SLRay::subsampledRays   = 0;
SLuint  SLRay::subsampledPixels = 0;
SLuint  SLRay::tirRays          = 0;
SLuint  SLRay::tests            = 0;
SLuint  SLRay::intersections    = 0;
SLint   SLRay::depthReached     = 1;
SLint   SLRay::maxDepthReached  = 0;
SLfloat SLRay::avgDepth         = 0;

//-----------------------------------------------------------------------------
/*! Uniform random number generator for numbers between 0 and 1 that is used in
SLRay, SLLightRect and SLPathtracer.
\remarks The engine state is thread_local and must stay that way. The ray
tracer and the path tracer call rnd01 concurrently from all worker threads (see
SLPathtracer::render). A single shared std::mt19937 would be a data race on its
624 word state plus its position index. That is undefined behaviour, and in
practice the racing threads hand each other torn and repeated values, so their
samples are no longer independent and the noise no longer averages out with
1/sqrt(N).
Each thread seeds its own engine from the current time mixed with a shared
atomic counter, so that threads created within the same second still get
different sequences.
*/
SLfloat rnd01()
{
    static std::atomic<SLuint> seedCounter{0};

    thread_local std::mt19937 engine((SLuint)std::time(nullptr) * 2654435761u +
                                     seedCounter.fetch_add(1u) * 40503u + 1u);

    thread_local std::uniform_real_distribution<SLfloat> dist(0.0f, 1.0f);

    return dist(engine);
}
//-----------------------------------------------------------------------------
/*!
SLRay::SLRay default constructor
*/
SLRay::SLRay(SLSceneView* sceneView)
{
    origin = SLVec3f::ZERO;
    setDir(SLVec3f::ZERO);
    type           = PRIMARY;
    length         = FLT_MAX;
    depth          = 1;
    hitTriangle    = -1;
    hitPoint       = SLVec3f::ZERO;
    hitNormal      = SLVec3f::ZERO;
    hitTexColor    = SLCol4f::WHITE;
    hitAO          = 1.0f;
    hitNode        = nullptr;
    hitMesh        = nullptr;
    srcNode        = nullptr;
    srcMesh        = nullptr;
    srcTriangle    = -1;
    x              = -1;
    y              = -1;
    contrib        = 1.0f;
    isOutside      = true;
    isInsideVolume = false;
    sv             = sceneView;
}
//-----------------------------------------------------------------------------
/*!
SLRay::SLRay constructor for primary rays
*/
SLRay::SLRay(const SLVec3f& Origin,
             const SLVec3f& Dir,
             SLfloat        X,
             SLfloat        Y,
             const SLCol4f& backColor,
             SLSceneView*   sceneView)
{
    origin = Origin;
    setDir(Dir);
    type            = PRIMARY;
    length          = FLT_MAX;
    depth           = 1;
    hitTriangle     = -1;
    hitPoint        = SLVec3f::ZERO;
    hitNormal       = SLVec3f::ZERO;
    hitTexColor     = SLCol4f::WHITE;
    hitAO           = 1.0f;
    hitNode         = nullptr;
    hitMesh         = nullptr;
    srcNode         = nullptr;
    srcMesh         = nullptr;
    srcTriangle     = -1;
    x               = (SLfloat)X;
    y               = (SLfloat)Y;
    contrib         = 1.0f;
    isOutside       = true;
    isInsideVolume  = false;
    backgroundColor = backColor;
    sv              = sceneView;
}
//-----------------------------------------------------------------------------
/*!
SLRay::SLRay constructor for shadow rays
*/
SLRay::SLRay(SLfloat        distToLight,
             const SLVec3f& dirToLight,
             SLRay*         rayFromHitPoint)
{
    origin = rayFromHitPoint->hitPoint;
    setDir(dirToLight);
    type            = SHADOW;
    length          = distToLight;
    lightDist       = distToLight;
    depth           = rayFromHitPoint->depth;
    hitPoint        = SLVec3f::ZERO;
    hitNormal       = SLVec3f::ZERO;
    hitTexColor     = SLCol4f::WHITE;
    hitAO           = 1.0f;
    hitTriangle     = -1;
    hitNode         = nullptr;
    hitMesh         = nullptr;
    srcNode         = rayFromHitPoint->hitNode;
    srcMesh         = rayFromHitPoint->hitMesh;
    srcTriangle     = rayFromHitPoint->hitTriangle;
    x               = rayFromHitPoint->x;
    y               = rayFromHitPoint->y;
    backgroundColor = rayFromHitPoint->backgroundColor;
    sv              = rayFromHitPoint->sv;
    contrib         = 0.0f;
    isOutside       = rayFromHitPoint->isOutside;
    shadowRays++;
}
//-----------------------------------------------------------------------------
/*!
SLRay::prints prints the rays origin (O), direction (D) and the length to the
intersection (L)
*/
void SLRay::print() const
{
    SL_LOG("Ray: O(%.2f, %.2f, %.2f), D(%.2f, %.2f, %.2f), L: %.2f",
           origin.x,
           origin.y,
           origin.z,
           dir.x,
           dir.y,
           dir.z,
           length);
}
//-----------------------------------------------------------------------------
/*!
SLRay::reflect calculates a secondary ray reflected at the normal, starting at
the intersection point. All vectors must be normalized vectors.
R = 2(-I*N) N + I
*/
void SLRay::reflect(SLRay* reflected) const
{
#ifdef DEBUG_RAY
    for (SLint i = 0; i < depth; ++i)
        cout << " ";
    cout << "Reflect: " << hitMesh->name() << endl;
#endif

    SLVec3f R(dir - 2.0f * (dir * hitNormal) * hitNormal);

    reflected->setDir(R);
    reflected->origin.set(hitPoint);
    reflected->depth       = depth + 1;
    reflected->length      = FLT_MAX;
    reflected->contrib     = contrib * hitMesh->mat()->kr();
    reflected->srcNode     = hitNode;
    reflected->srcMesh     = hitMesh;
    reflected->srcTriangle = hitTriangle;
    reflected->type        = REFLECTED;
    reflected->isOutside   = isOutside;
    reflected->x           = x;
    reflected->y           = y;
    reflected->sv          = sv;
    if (sv->s()->skybox())
        reflected->backgroundColor = sv->s()->skybox()->colorAtDir(reflected->dir);
    else
        reflected->backgroundColor = backgroundColor;

    depthReached = reflected->depth;
    ++reflectedRays;
}
//-----------------------------------------------------------------------------
/*!
SLRay::refract calculates a secondary refracted ray, starting at the
intersection point. All vectors must be normalized vectors, so the refracted
vector T will be a unit vector too. If total internal refraction occurs a
reflected ray is calculated instead.
Index of refraction eta = Kn_Source/Kn_Destination (Kn_Air = 1.0)
We are using a formula by Xavier Bec that is a little faster:
http://www.realtimerendering.com/resources/RTNews/html/rtnv10n1.html#art3
*/
void SLRay::refract(SLRay* refracted)
{
    assert(hitMesh && "hitMesh is null");

    SLVec3f T;   // refracted direction
    SLfloat eta; // refraction coefficient

    SLfloat c1           = hitNormal.dot(-dir);
    SLbool  hitFrontSide = c1 > 0.0f;

    SLMaterial* srcMat    = srcMesh ? srcMesh->mat() : nullptr;
    SLMaterial* hitMat    = hitMesh ? hitMesh->mat() : nullptr;
    SLMaterial* hitMatOut = hitMesh ? hitMesh->matOut() : nullptr;

#ifdef DEBUG_RAY
    for (SLint i = 0; i < depth; ++i)
        cout << " ";
    cout << "Refract: ";
#endif

    // Calculate index of refraction eta = Kn_Source/Kn_Destination
    // Case 1: From air into a mesh
    if (isOutside)
    {
        eta = 1.0f / hitMat->kn();
    }
    else
    { // Case 2: From inside the same mesh
        if (hitMesh == srcMesh)
        {
            if (hitMatOut)          // Case 2a: into another material
                eta = hitMat->kn() / hitMatOut->kn();
            else                    // Case 2b: into air
                eta = hitMat->kn(); // = hitMat / 1.0
        }
        else
        { // Case 3: We hit inside another material from the front
            if (hitFrontSide)
            {
                if (hitMatOut)
                    eta = hitMatOut->kn() / hitMat->kn();
                else
                { // Mesh hit without outside material before leaving another mesh.
                    // This should not happen, but can due to float inaccuracies
                    eta = srcMat->kn() / hitMat->kn();
                }
            }
            else                        // Case 4: We hit inside another material from behind
            {
                if (hitMatOut)          // Case 4a: into another material
                    eta = hitMat->kn() / hitMatOut->kn();
                else                    // Case 4b: into air
                    eta = hitMat->kn(); // = hitMat / 1.0
            }
        }
    }

    // Invert the hit normal if ray hit backside for correct refraction
    if (!hitFrontSide)
    {
        c1 *= -1.0f;
        hitNormal *= -1.0f;
    }

    SLfloat w  = eta * c1;
    SLfloat c2 = 1.0f + (w - eta) * (w + eta);

    if (c2 >= 0.0f)
    {
        T                  = eta * dir + (w - sqrt(c2)) * hitNormal;
        refracted->contrib = contrib * hitMat->kt();
        refracted->type    = REFRACTED;

        if (isOutside)
            refracted->isOutside = false;
        else // inside
        {
            if (srcMesh == hitMesh)
                refracted->isOutside = !hitMatOut;
            else
                refracted->isOutside = !hitFrontSide;
        }

        ++refractedRays;
    }
    else // total internal refraction results in a internal reflected ray
    {
        T                    = 2.0f * (-dir * hitNormal) * hitNormal + dir;
        refracted->contrib   = 1.0f;
        refracted->type      = REFLECTED;
        refracted->isOutside = isOutside; // remain inside
        ++tirRays;
    }

    refracted->setDir(T);
    refracted->origin.set(hitPoint);
    refracted->length      = FLT_MAX;
    refracted->srcNode     = hitNode;
    refracted->srcMesh     = hitMesh;
    refracted->srcTriangle = hitTriangle;
    refracted->depth       = depth + 1;
    refracted->x           = x;
    refracted->y           = y;
    refracted->sv          = sv;
    if (sv->s()->skybox())
        refracted->backgroundColor = sv->s()->skybox()->colorAtDir(refracted->dir);
    else
        refracted->backgroundColor = backgroundColor;
    depthReached = refracted->depth;

#ifdef DEBUG_RAY
    cout << hitMesh->name();
    if (isOutside)
        cout << ",out";
    else
        cout << ",in";
    if (refracted->isOutside)
        cout << ">out";
    else
        cout << ">in";
    cout << ", dir: " << refracted->dir.toString();
    cout << ", contrib: " << Utils::toString(refracted->contrib, 2);
    cout << endl;
#endif
}
//-----------------------------------------------------------------------------
/*!
SLRay::lobeToWorld returns the rotation matrix that maps a direction sampled
around the +z axis onto lobeAxis. Its columns are an orthonormal basis whose
third vector is lobeAxis, so that rotMat * v = v.x*t + v.y*b + v.z*lobeAxis.

This replaces the axis-angle construction that the scattering functions used to
do themselves:

    SLVec3f rotAxis((SLVec3f(0,0,1) ^ dir).normalize());
    rotMat.rotation(acos(dir.z) * 180 * ONEOVERPI, rotAxis);

which fails exactly where it is used most. The cross product is
(-dir.y, dir.x, 0) and its length is the sine of the angle, so it vanishes for
a lobe axis along +-z. SLVec3::normalize guards with if (L > 0) and therefore
returns the zero vector rather than a NaN, and the matrix built from a zero
axis is diag(cos a, cos a, cos a) - a uniform scale, not a rotation. It happens
to be usable at exactly +-z (identity and -I, and -I is fine for an
azimuthally symmetric lobe) but the axis loses its precision continuously as
the lobe approaches either pole, and acos(dir.z) additionally returns a NaN as
soon as rounding pushes |dir.z| past 1. In the Muttenzer Box the box is axis
aligned and the front and back walls face +-z, so this is the common case and
not a corner one.

The basis is the branchless construction of Duff et al., "Building an
Orthonormal Basis, Revisited", JCGT 6(1), 2017. It is exact and orthonormal for
every unit vector including both poles, needs no trigonometry, and has no
branch on a tolerance that has to be tuned.
*/
SLMat3f SLRay::lobeToWorld(const SLVec3f& lobeAxis)
{
    SLVec3f n(lobeAxis);
    n.normalize();

    SLfloat sign = std::copysign(1.0f, n.z);
    SLfloat a    = -1.0f / (sign + n.z);
    SLfloat b    = n.x * n.y * a;

    SLVec3f t(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
    SLVec3f u(b, sign + n.y * n.y * a, -n.y);

    // The SLMat3 constructor takes the components in row order and stores them
    // column wise, so this sets the columns to t, u and n.
    return SLMat3f(t.x, u.x, n.x, t.y, u.y, n.y, t.z, u.z, n.z);
}
//-----------------------------------------------------------------------------
/*!
SLRay::reflectMC scatters a ray around perfect specular direction according to
shininess (for higher shininess the ray is less scattered). This is used for
path tracing and distributed ray tracing as well as for photon scattering.
The direction is calculated according to MCCABE. The created direction is
along z-axis and then transformed to lie along specular direction with
rotationMatrix rotMat, which SLRay::lobeToWorld builds from the perfect
specular direction. The rotation matrix must be precalculated (stays the same
for each ray sample, needs to be calculated only once).

reflected->dir must hold the perfect specular direction on entry, i.e. the
caller must have run SLRay::reflect first, because it is the reference against
which the sampled direction is tested.

\return false if the sample landed on the far side of the surface. The Phong
lobe is a cone around the mirror direction and is not clipped to the
hemisphere, so a wide lobe at a grazing angle puts part of its samples below
the horizon. The normalised Phong BRDF is zero there, so such a sample carries
no energy and the caller must not trace it. Note that the test is a comparison
of signs and not "points along the normal": SLMesh::preShade does not flip the
hit normal towards the ray, so a back face hit has a perfect specular direction
with a negative dot product and every sample around it would otherwise be
rejected.
*/
bool SLRay::reflectMC(SLRay* reflected, const SLMat3f& rotMat) const
{
    SLfloat eta1, eta2;
    SLVec3f randVec;
    SLfloat shininess = hitMesh->mat()->shininess();

    // The side of the surface the perfect specular direction leaves on
    SLfloat perfectCos = hitNormal.dot(reflected->dir);

    // scatter within specular lobe
    eta1       = rnd01();
    eta2       = Utils::TWOPI * rnd01();
    SLfloat f1 = sqrt(1.0f - pow(eta1, 2.0f / (shininess + 1.0f)));

    // tranform to cartesian
    randVec.set(f1 * cos(eta2),
                f1 * sin(eta2),
                pow(eta1, 1.0f / (shininess + 1.0f)));

    // ray needs to be reset if already hit a scene node
    if (reflected->hitNode)
    {
        reflected->length    = FLT_MAX;
        reflected->hitNode   = nullptr;
        reflected->hitMesh   = nullptr;
        reflected->hitPoint  = SLVec3f::ZERO;
        reflected->hitNormal = SLVec3f::ZERO;
    }

    // apply rotation
    reflected->setDir(rotMat * randVec);

    // Set pixel and background
    reflected->x  = x;
    reflected->y  = y;
    reflected->sv = sv;
    if (sv->s()->skybox())
        reflected->backgroundColor = sv->s()->skybox()->colorAtDir(reflected->dir);
    else
        reflected->backgroundColor = backgroundColor;

    // true if the sample stayed on the same side of the surface as the
    // perfect specular direction it was scattered around
    return (perfectCos * hitNormal.dot(reflected->dir) > 0.0f);
}
//-----------------------------------------------------------------------------
/*!
SLRay::refractMC scatters a ray around perfect transmissive direction according
to translucency (for higher translucency the ray is less scattered).
This is used for path tracing and distributed ray tracing as well as for photon
scattering. The direction is calculated the same as with specular scattering
(see reflectMC). The created direction is along z-axis and then transformed to
lie along transmissive direction with rotationMatrix rotMat, which
SLRay::lobeToWorld builds from the perfect transmissive direction. The rotation
matrix must be precalculated (stays the same for each ray sample, needs to be
calculated only once).

refracted->dir must hold the perfect transmissive direction on entry, i.e. the
caller must have run SLRay::refract first.

\return false if the sample landed on the near side of the surface, for the
same reason as in reflectMC. Testing against the sign of the perfect direction
rather than against the normal is what makes this work for total internal
reflection too, where SLRay::refract returns a direction on the incident side.
*/
bool SLRay::refractMC(SLRay* refracted, const SLMat3f& rotMat) const
{
    SLfloat eta1, eta2;
    SLVec3f randVec;
    SLfloat translucency = hitMesh->mat()->translucency();

    // The side of the surface the perfect transmissive direction leaves on
    SLfloat perfectCos = hitNormal.dot(refracted->dir);

    // scatter within transmissive lobe
    eta1       = rnd01();
    eta2       = Utils::TWOPI * rnd01();
    SLfloat f1 = sqrt(1.0f - pow(eta1, 2.0f / (translucency + 1.0f)));

    // transform to cartesian
    randVec.set(f1 * cos(eta2),
                f1 * sin(eta2),
                pow(eta1, 1.0f / (translucency + 1.0f)));

    // ray needs to be reset if already hit a scene node
    if (refracted->hitNode)
    {
        refracted->length    = FLT_MAX;
        refracted->hitNode   = nullptr;
        refracted->hitMesh   = nullptr;
        refracted->hitPoint  = SLVec3f::ZERO;
        refracted->hitNormal = SLVec3f::ZERO;
    }

    // Apply rotation
    refracted->setDir(rotMat * randVec);

    // Set pixel and background
    refracted->x  = x;
    refracted->y  = y;
    refracted->sv = sv;
    if (sv->s()->skybox())
        refracted->backgroundColor = sv->s()->skybox()->colorAtDir(refracted->dir);
    else
        refracted->backgroundColor = backgroundColor;

    // true if the sample stayed on the same side of the surface as the
    // perfect transmissive direction it was scattered around
    return (perfectCos * hitNormal.dot(refracted->dir) > 0.0f);
}
//-----------------------------------------------------------------------------
/*!
SLRay::diffuseMC scatters a ray around the hit normal with a cosine
distribution, which is the importance sampling of the Lambertian BRDF: the
density is cos(theta)/PI, so it cancels the cosine of the rendering equation
and every sample carries the same weight. SLPathtracer::trace is its only
caller in this repository.

The random direction lies around the z-Axis and is then transformed by a
rotation matrix to lie along the normal. The direction is calculated according
to MCCABE.

\remarks The comment here used to read "This is only used for
photonmapping(russian roulette)". Both halves were wrong. Cosine distributed
scattering is importance sampling and has nothing to do with Russian roulette,
which is the unrelated technique that terminates the recursion in
SLPathtracer::trace (see plan point 13), and there is no photon mapper in this
repository.
*/
void SLRay::diffuseMC(SLRay* scattered) const
{
    SLVec3f randVec;
    SLfloat eta1, eta2, eta1sqrt;

    scattered->setDir(hitNormal);
    scattered->origin = hitPoint;
    scattered->depth  = depth + 1;
    depthReached      = scattered->depth;

    // for reflectance the start material stays the same
    scattered->srcNode = hitNode;
    scattered->srcMesh = hitMesh;
    scattered->type    = REFLECTED;

    // Rotation matrix that takes the +z lobe onto the hit normal. See
    // SLRay::lobeToWorld for why this is not built from an axis and an angle.
    SLMat3f rotMat = lobeToWorld(scattered->dir);

    // cosine distribution
    eta1     = rnd01();
    eta2     = Utils::TWOPI * rnd01();
    eta1sqrt = sqrt(1 - eta1);

    // transform to cartesian
    randVec.set(eta1sqrt * cos(eta2),
                eta1sqrt * sin(eta2),
                sqrt(eta1));

    // Apply rotation
    scattered->setDir(rotMat * randVec);

    // Set pixel and background
    scattered->x  = x;
    scattered->y  = y;
    scattered->sv = sv;
    if (sv->s()->skybox())
        scattered->backgroundColor = sv->s()->skybox()->colorAtDir(scattered->dir);
    else
        scattered->backgroundColor = backgroundColor;
}
//-----------------------------------------------------------------------------
