/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLCamera.cpp            +++     +++         **/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 18/09/2025 18:56:40      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "RMDLCamera.hpp"

static simd::float4x4 sInvMatrixLookat( simd::float3 inEye, simd::float3 inTo, simd::float3 inUp )
{
    simd::float3 z = simd::normalize(inTo - inEye);
    simd::float3 x = simd::normalize(simd::cross(inUp, z));
    simd::float3 y = simd::cross(z, x);
    simd::float3 t = (simd::float3) { -simd::dot(x, inEye), -simd::dot(y, inEye), -simd::dot(z, inEye) };
    return ( simd::float4x4( simd::float4 { x.x, y.x, z.x, 0 },
                             simd::float4 { x.y, y.y, z.y, 0 },
                             simd::float4 { x.z, y.z, z.z, 0 },
                             simd::float4 { t.x, t.y, t.z, 1 }) );
}

static simd::float4 sPlaneNormalize( const simd::float4& inPlane )
{
    return (inPlane / simd::length(inPlane.xyz));
}

RMDLCamera::RMDLCamera() : _position{0, 0, 0}, _direction{0, 0, 1}, _up{0, 1, 0}, _viewAngle(0), _aspectRatio(1.0), _nearPlane(0.1f), _farPlane(100.0f), _width(0), _uniformsDirty(true)
{
}

RMDLCamera::~RMDLCamera()
{
}

RMDLCamera& RMDLCamera::initPerspectiveWithPosition( simd::float3 position, simd::float3 direction, simd::float3 up, float viewAngle, float aspectRatio, float nearPlane, float farPlane )
{
    _up = up;
    orthogonalizeFromNewForward(direction);
    _position = position;
    _width = 0;
    _viewAngle = viewAngle;
    _aspectRatio = aspectRatio;
    _nearPlane = nearPlane;
    _farPlane = farPlane;
    _uniformsDirty = true;
    return (*this);
}
    
RMDLCamera& RMDLCamera::initParallelWithPosition( simd::float3 position, simd::float3 direction, simd::float3 up, float width, float height, float nearPlane, float farPlane )
{
    _up = up;
    orthogonalizeFromNewForward(direction);
    _position = position;
    _width = width;
    _viewAngle = 0;
    _aspectRatio = width / height;
    _nearPlane = nearPlane;
    _farPlane = farPlane;
    _uniformsDirty = true;
    return (*this);
}
    
bool RMDLCamera::isPerspective() const
{
    return (_viewAngle != 0.0f);
}

bool RMDLCamera::isParallel() const
{
    return (_viewAngle == 0.0f);
}

void RMDLCamera::updateUniforms()
{
    _uniforms.viewMatrix = sInvMatrixLookat(_position, _position + _direction, _up);
    if (_viewAngle != 0)
    {
        float va_tan = 1.0f / tan(_viewAngle * 0.5f);
        float ys = va_tan;
        float xs = ys / _aspectRatio;
        float zs = _farPlane / (_farPlane - _nearPlane);
        _uniforms.projectionMatrix = simd::float4x4((simd::float4){ xs,  0,  0, 0},
                                                    (simd::float4){  0, ys,  0, 0},
                                                    (simd::float4){  0,  0, zs, 1},
                                                    (simd::float4){  0,  0, -_nearPlane * zs , 0 } );
    }
    else
    {
        float ys = 2.0f / _width;
        float xs = ys / _aspectRatio;
        float zs = 1.0f / (_farPlane - _nearPlane);
        _uniforms.projectionMatrix = simd::float4x4((simd::float4){ xs,  0,  0, 0},
                                                    (simd::float4){  0, ys,  0, 0},
                                                    (simd::float4){  0,  0, zs, 1},
                                                    (simd::float4){  0,  0, -_nearPlane * zs , 1} );
    }
    _uniforms.viewProjectionMatrix = _uniforms.projectionMatrix * _uniforms.viewMatrix;
    _uniforms.invProjectionMatrix = simd_inverse(_uniforms.projectionMatrix);
    _uniforms.invOrientationProjectionMatrix = simd_inverse( _uniforms.projectionMatrix * sInvMatrixLookat( simd::float3{0, 0, 0}, _direction, _up ) );
    _uniforms.invViewProjectionMatrix = simd_inverse(_uniforms.viewProjectionMatrix);
    _uniforms.invViewMatrix = simd_inverse(_uniforms.viewMatrix);
    simd::float4x4 transp_vpm = simd::transpose(_uniforms.viewProjectionMatrix);
    _uniforms.frustumPlanes[0] = sPlaneNormalize(transp_vpm.columns[3] + transp_vpm.columns[0]);
    _uniforms.frustumPlanes[1] = sPlaneNormalize(transp_vpm.columns[3] - transp_vpm.columns[0]);
    _uniforms.frustumPlanes[2] = sPlaneNormalize(transp_vpm.columns[3] + transp_vpm.columns[1]);
    _uniforms.frustumPlanes[3] = sPlaneNormalize(transp_vpm.columns[3] - transp_vpm.columns[1]);
    _uniforms.frustumPlanes[4] = sPlaneNormalize(transp_vpm.columns[3] + transp_vpm.columns[2]);
    _uniforms.frustumPlanes[5] = sPlaneNormalize(transp_vpm.columns[3] - transp_vpm.columns[2]);
    _uniformsDirty = false;
}

simd::float3 RMDLCamera::left() const
{
    return (simd_cross(_direction, _up));
}

simd::float3 RMDLCamera::right() const
{
    return (-left());
}

simd::float3 RMDLCamera::down() const
{
    return (-_up);
}

simd::float3 RMDLCamera::forward() const
{
    return (_direction);
}

simd::float3 RMDLCamera::backward() const
{
    return (-_direction);
}

float RMDLCamera::nearPlane() const
{
    return (_nearPlane);
}

float RMDLCamera::farPlane() const
{
    return (_farPlane);
}

float RMDLCamera::aspectRatio() const
{
    return (_aspectRatio);
}

float RMDLCamera::viewAngle() const
{
    return (_viewAngle);
}

float RMDLCamera::width() const
{
    return (_width);
}

simd::float3 RMDLCamera::up() const
{
    return (_up);
}

simd::float3 RMDLCamera::position() const
{
    return (_position);
}

simd::float3 RMDLCamera::direction() const
{
    return (_direction);
}
    
RMDLCameraUniforms RMDLCamera::uniforms()
{
    if (_uniformsDirty)
        RMDLCamera::updateUniforms();
    return (_uniforms);
}
    
void RMDLCamera::setNearPlane(float newNearPlane)
{
    _nearPlane = newNearPlane;
    _uniformsDirty = true;
}

void RMDLCamera::setFarPlane(float newFarPlane)
{
    _farPlane = newFarPlane;
    _uniformsDirty = true;
}

void RMDLCamera::setAspectRatio(float newAspectRatio)
{
    _aspectRatio = newAspectRatio;
    _uniformsDirty = true;
}

void RMDLCamera::setViewAngle(float newAngle)
{
    _width = 0;
    _viewAngle = newAngle;
    _uniformsDirty = true;
}

void RMDLCamera::setWidth(float newWidth)
{
    _viewAngle = 0;
    _width = newWidth;
    _uniformsDirty = true;
}

void RMDLCamera::setPosition(simd::float3 newPosition)
{
    _position = newPosition;
    _uniformsDirty = true;
}

void RMDLCamera::setUp(simd::float3 newUp)
{
    orthogonalizeFromNewUp(newUp);
    _uniformsDirty = true;
}

void RMDLCamera::setDirection(simd::float3 newDirection)
{
    orthogonalizeFromNewForward(newDirection);
    _uniformsDirty = true;
}

simd::float4x4 RMDLCamera::ViewMatrix()
{
    return (uniforms().viewMatrix);
}

simd::float4x4 RMDLCamera::ProjectionMatrix()
{
    return (uniforms().projectionMatrix);
}

simd::float4x4 RMDLCamera::ViewProjectionMatrix()
{
    return (uniforms().viewProjectionMatrix);
}

simd::float4x4 RMDLCamera::InvOrientationProjectionMatrix()
{
    return (uniforms().invOrientationProjectionMatrix);
}

simd::float4x4 RMDLCamera::InvViewProjectionMatrix()
{
    return (uniforms().invViewProjectionMatrix);
}

simd::float4x4 RMDLCamera::InvProjectionMatrix()
{
    return (uniforms().invProjectionMatrix);
}

simd::float4x4 RMDLCamera::InvViewMatrix()
{
    return (uniforms().invViewMatrix);
}
    
void RMDLCamera::rotateOnAxis( simd::float3 inAxis, float inRadians )
{
    simd::float3 axis = simd::normalize(inAxis);
    float ct = cosf(inRadians);
    float st = sinf(inRadians);
    float ci = 1 - ct;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    simd::float3x3 mat( (simd::float3){ ct + x * x * ci, y * x * ci + z * st, z * x * ci - y * st } ,
                        (simd::float3){ x * y * ci - z * st, ct + y * y * ci, z * y * ci + x * st } ,
                        (simd::float3){ x * z * ci + y * st, y * z * ci - x * st, ct + z * z * ci } );
    _direction = mat * _direction;
    _up = mat * _up;
    _uniformsDirty = true;
}

void RMDLCamera::orthogonalizeFromNewUp( simd::float3 newUp )
{
    _up = simd::normalize(newUp);
    simd::float3 right = simd::normalize( simd::cross(_direction, _up) );
    _direction = simd::cross(_up, right);
}
    
void RMDLCamera::orthogonalizeFromNewForward( simd::float3 newForward )
{
    _direction = simd::normalize(newForward);
    simd::float3 right = simd::normalize(simd::cross(_direction, _up));
    _up = simd::cross(right, _direction);
}
