#include "transform3d.hpp"

namespace jam::components
{
    Transform3D::Transform3D()
        : position({ 0,0,0 })
        , size({ 1,1,1 })
        , orientation({ 0,0,0,1 })
        , scale(1.0f)
    {
    }

    Transform3D::Transform3D(Vector3 Position)
        : position(Position)
        , size({ 1,1,1 })
        , orientation({ 0,0,0,1 })
        , scale(1.0f)
    {
    }

    Transform3D::Transform3D(Vector3 Position, Vector3 Size)
        : position(Position)
        , size(Size)
        , orientation({ 0,0,0,1 })
        , scale(1.0f)
    {
    }

    Transform3D::Transform3D(Vector3 Position, Vector3 Size, Quaternion Orientation)
        : position(Position)
        , size(Size)
        , orientation(Orientation)
        , scale(1.0f)
    {
    }

    Matrix Transform3D::toMatrix() const
    {
        return MatrixMultiply(MatrixMultiply(MatrixScale(size.x * scale, size.y * scale, size.z * scale),
            QuaternionToMatrix(orientation)),
            MatrixTranslate(position.x, position.y, position.z));
    }

    Vector3 Transform3D::GetEuler() const
    {
        return QuaternionToEuler(orientation);
    }

    void Transform3D::SetEuler(Vector3 euler)
    {
        orientation = QuaternionFromEuler(euler.x, euler.y, euler.z);
    }

    Vector3 Transform3D::Front() const
    {
        float x2 = 2 * orientation.x;
        float y2 = 2 * orientation.y;
        float z2 = 2 * orientation.z;

        float x2w = x2 * orientation.w;
        float y2w = y2 * orientation.w;

        float x2x = x2 * orientation.x;
        float z2x = z2 * orientation.x;

        float y2y = y2 * orientation.y;
        float z2y = z2 * orientation.y;

        return Vector3{ z2x + y2w, z2y - x2w, 1.0f - (x2x + y2y) };
    }

    Vector3 Transform3D::Right() const
    {
        Matrix mat = toMatrix();

        return Vector3{ mat.m0, mat.m1, mat.m2 };
    }

    Vector3 Transform3D::Up() const
    {
        Matrix mat = toMatrix();
        return Vector3{ mat.m4, mat.m5, mat.m6 };
    }

    Transform Transform3D::toTransform() const
    {
        return Transform{ .translation = position, .rotation = orientation, .scale = size * scale };
    }

    void Transform3D::LookAt(Vector3 target)
    {
        Matrix matLookAt = MatrixLookAt(position, target, Vector3{0,1,0});

        Matrix mat = MatrixMultiply(matLookAt, toMatrix());
        orientation = QuaternionNormalize(QuaternionFromMatrix(mat));
    }

}