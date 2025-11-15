#pragma once
#include <cmath>

namespace cradle::engine
{
    struct vector2
    {
        float X{0.0f}, Y{0.0f};

        vector2() = default;
        vector2(float x, float y) : X(x), Y(y) {}

        vector2 operator+(const vector2 &rhs) const { return {X + rhs.X, Y + rhs.Y}; }
        vector2 operator-(const vector2 &rhs) const { return {X - rhs.X, Y - rhs.Y}; }
        vector2 operator*(float scalar) const { return {X * scalar, Y * scalar}; }

        vector2 &operator+=(const vector2 &rhs)
        {
            X += rhs.X;
            Y += rhs.Y;
            return *this;
        }
        vector2 &operator-=(const vector2 &rhs)
        {
            X -= rhs.X;
            Y -= rhs.Y;
            return *this;
        }

        float magnitude() const { return std::sqrt(X * X + Y * Y); }
        vector2 normalize() const
        {
            float mag = magnitude();
            return {X / mag, Y / mag};
        }
    };

    struct vector3
    {
        float X{0.0f}, Y{0.0f}, Z{0.0f};

        vector3() = default;
        vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

        vector3 operator+(const vector3 &rhs) const { return {X + rhs.X, Y + rhs.Y, Z + rhs.Z}; }
        vector3 operator-(const vector3 &rhs) const { return {X - rhs.X, Y - rhs.Y, Z - rhs.Z}; }
        vector3 operator*(float scalar) const { return {X * scalar, Y * scalar, Z * scalar}; }
        vector3 operator*(const vector3 &rhs) const { return {X * rhs.X, Y * rhs.Y, Z * rhs.Z}; }
        vector3 operator/(float scalar) const { return {X / scalar, Y / scalar, Z / scalar}; }

        vector3 &operator+=(const vector3 &rhs)
        {
            X += rhs.X;
            Y += rhs.Y;
            Z += rhs.Z;
            return *this;
        }
        vector3 &operator-=(const vector3 &rhs)
        {
            X -= rhs.X;
            Y -= rhs.Y;
            Z -= rhs.Z;
            return *this;
        }

        bool operator==(const vector3 &rhs) const { return X == rhs.X && Y == rhs.Y && Z == rhs.Z; }

        float magnitude() const { return std::sqrt(X * X + Y * Y + Z * Z); }
        vector3 normalize() const
        {
            float mag = magnitude();
            return {X / mag, Y / mag, Z / mag};
        }
        float distance(const vector3 &v) const { return (*this - v).magnitude(); }

        vector3 cross(const vector3 &rhs) const
        {
            return vector3(
                Y * rhs.Z - Z * rhs.Y,
                Z * rhs.X - X * rhs.Z,
                X * rhs.Y - Y * rhs.X);
        }
    };

    struct matrix3
    {
        float data[9];

        matrix3()
        {
            for (int i = 0; i < 9; i++)
                data[i] = 0.0f;
        }
        matrix3(float m[9])
        {
            for (int i = 0; i < 9; i++)
                data[i] = m[i];
        }

        matrix3 transpose() const
        {
            matrix3 result;
            result.data[0] = data[0];
            result.data[1] = data[3];
            result.data[2] = data[6];
            result.data[3] = data[1];
            result.data[4] = data[4];
            result.data[5] = data[7];
            result.data[6] = data[2];
            result.data[7] = data[5];
            result.data[8] = data[8];
            return result;
        }

        vector3 multiply(const vector3 &v) const
        {
            return vector3(
                data[0] * v.X + data[1] * v.Y + data[2] * v.Z,
                data[3] * v.X + data[4] * v.Y + data[5] * v.Z,
                data[6] * v.X + data[7] * v.Y + data[8] * v.Z);
        }
    };

    struct cframe
    {
        matrix3 rotation;
        vector3 position;

        cframe() = default;
        cframe(const matrix3 &rot, const vector3 &pos) : rotation(rot), position(pos) {}

        vector3 get_right_vector() const { return vector3(rotation.data[0], rotation.data[3], rotation.data[6]); }
        vector3 get_up_vector() const { return vector3(rotation.data[1], rotation.data[4], rotation.data[7]); }
        vector3 get_look_vector() const { return vector3(-rotation.data[2], -rotation.data[5], -rotation.data[8]); }
    };

    struct vector4
    {
        float x, y, z, w;

        vector4() : x(0), y(0), z(0), w(1) {}
        vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

        static vector4 from_matrix(const matrix3 &m)
        {
            vector4 q;
            float trace = m.data[0] + m.data[4] + m.data[8];

            if (trace > 0.0f)
            {
                float s = std::sqrt(trace + 1.0f) * 2.0f;
                q.w = 0.25f * s;
                q.x = (m.data[7] - m.data[5]) / s;
                q.y = (m.data[2] - m.data[6]) / s;
                q.z = (m.data[3] - m.data[1]) / s;
            }
            else if (m.data[0] > m.data[4] && m.data[0] > m.data[8])
            {
                float s = std::sqrt(1.0f + m.data[0] - m.data[4] - m.data[8]) * 2.0f;
                q.w = (m.data[7] - m.data[5]) / s;
                q.x = 0.25f * s;
                q.y = (m.data[1] + m.data[3]) / s;
                q.z = (m.data[2] + m.data[6]) / s;
            }
            else if (m.data[4] > m.data[8])
            {
                float s = std::sqrt(1.0f + m.data[4] - m.data[0] - m.data[8]) * 2.0f;
                q.w = (m.data[2] - m.data[6]) / s;
                q.x = (m.data[1] + m.data[3]) / s;
                q.y = 0.25f * s;
                q.z = (m.data[5] + m.data[7]) / s;
            }
            else
            {
                float s = std::sqrt(1.0f + m.data[8] - m.data[0] - m.data[4]) * 2.0f;
                q.w = (m.data[3] - m.data[1]) / s;
                q.x = (m.data[2] + m.data[6]) / s;
                q.y = (m.data[5] + m.data[7]) / s;
                q.z = 0.25f * s;
            }
            return q;
        }

        static vector4 slerp(const vector4 &a, const vector4 &b, float t)
        {
            float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;

            vector4 b_copy = b;
            if (dot < 0.0f)
            {
                b_copy.x = -b.x;
                b_copy.y = -b.y;
                b_copy.z = -b.z;
                b_copy.w = -b.w;
                dot = -dot;
            }

            if (dot > 0.9995f)
            {
                return vector4(
                    a.x + t * (b_copy.x - a.x),
                    a.y + t * (b_copy.y - a.y),
                    a.z + t * (b_copy.z - a.z),
                    a.w + t * (b_copy.w - a.w));
            }

            float theta = std::acos(dot);
            float sin_theta = std::sin(theta);
            float wa = std::sin((1.0f - t) * theta) / sin_theta;
            float wb = std::sin(t * theta) / sin_theta;

            return vector4(
                a.x * wa + b_copy.x * wb,
                a.y * wa + b_copy.y * wb,
                a.z * wa + b_copy.z * wb,
                a.w * wa + b_copy.w * wb);
        }

        matrix3 to_matrix() const
        {
            matrix3 m;
            float xx = x * x, yy = y * y, zz = z * z;
            float xy = x * y, xz = x * z, yz = y * z;
            float wx = w * x, wy = w * y, wz = w * z;

            m.data[0] = 1.0f - 2.0f * (yy + zz);
            m.data[1] = 2.0f * (xy + wz);
            m.data[2] = 2.0f * (xz - wy);

            m.data[3] = 2.0f * (xy - wz);
            m.data[4] = 1.0f - 2.0f * (xx + zz);
            m.data[5] = 2.0f * (yz + wx);

            m.data[6] = 2.0f * (xz + wy);
            m.data[7] = 2.0f * (yz - wx);
            m.data[8] = 1.0f - 2.0f * (xx + yy);

            return m;
        }
    };

    inline cframe look_at(const vector3 &from, const vector3 &to, const vector3 &up = vector3(0, 1, 0))
    {
        vector3 forward = (to - from).normalize();
        vector3 right = forward.cross(up).normalize();
        vector3 real_up = right.cross(forward);

        matrix3 rot;
        rot.data[0] = right.X;
        rot.data[1] = real_up.X;
        rot.data[2] = forward.X;

        rot.data[3] = right.Y;
        rot.data[4] = real_up.Y;
        rot.data[5] = forward.Y;

        rot.data[6] = right.Z;
        rot.data[7] = real_up.Z;
        rot.data[8] = forward.Z;

        return cframe(rot, from);
    }

    struct matrix4
    {
        float data[16];
    };
}
