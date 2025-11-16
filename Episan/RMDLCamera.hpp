/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLCamera.hpp            +++     +++         **/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 18/09/2025 13:36:07      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef RMDLCAMERA_HPP
# define RMDLCAMERA_HPP

# include <simd/simd.h>

# import "RMDLMainRenderer_shared.h"

class RMDLCamera
{
public:
    RMDLCamera();
    ~RMDLCamera();

    RMDLCamera&     initPerspectiveWithPosition( simd::float3 position, simd::float3 direction, simd::float3 up, float viewAngle, float aspectRatio, float nearPlane, float farPlane );
    RMDLCamera&     initParallelWithPosition( simd::float3 position, simd::float3 direction, simd::float3 up, float width, float height, float nearPlane, float farPlane );
    RMDLCameraUniforms  uniforms();
    void            updateUniforms();
    bool            isPerspective() const;
    bool            isParallel() const;
    simd::float3    left() const;
    simd::float3    right() const;
    simd::float3    down() const;
    simd::float3    forward() const;
    simd::float3    backward() const;
    float           nearPlane() const;
    float           farPlane() const;
    float           aspectRatio() const;
    float           viewAngle() const;
    float           width() const;
    simd::float3    up() const;
    simd::float3    position() const;
    simd::float3    direction() const;
    void            setNearPlane(float newNearPlane);
    void            setFarPlane(float newFarPlane);
    void            setAspectRatio(float newAspectRatio);
    void            setViewAngle(float newAngle);
    void            setWidth(float newWidth);
    void            setPosition(simd::float3 newPosition);
    void            setUp(simd::float3 newUp);
    void            setDirection(simd::float3 newDirection);
    simd::float4x4  ViewMatrix();
    simd::float4x4  ProjectionMatrix();
    simd::float4x4  ViewProjectionMatrix();
    simd::float4x4  InvOrientationProjectionMatrix();
    simd::float4x4  InvViewProjectionMatrix();
    simd::float4x4  InvProjectionMatrix();
    simd::float4x4  InvViewMatrix();
    void            rotateOnAxis( simd::float3 inAxis, float inRadians );
private:
    float           _nearPlane;
    float           _farPlane;
    float           _aspectRatio;
    float           _viewAngle;
    float           _width;
    simd::float3    _up;
    simd::float3    _position;
    simd::float3    _direction;
    void            orthogonalizeFromNewUp( simd::float3 newUp );
    void            orthogonalizeFromNewForward( simd::float3 newForward );
    bool            _uniformsDirty;
    RMDLCameraUniforms  _uniforms;
};

#endif /* RMDLCAMERA_HPP */
