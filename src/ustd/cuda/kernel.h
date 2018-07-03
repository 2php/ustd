#pragma once

#define let const auto
#define mut       auto
#define fn        auto
#define pub       __declspec(dllexport)

namespace ustd
{

using i8  = __int8;     using u8  = unsigned __int8;
using i16 = __int16;    using u16 = unsigned __int16;
using i32 = __int32;    using u32 = unsigned __int32;
using i64 = __int64;    using u64 = unsigned __int64;

using f32 = float;
using f64 = double;

struct u32x2 {
    u32     x;
    u32     y;
};

struct u32x3 {
    u32     x;
    u32     y;
    u32     z;
};

struct f32x2 {
    f32     x;
    f32     y;
};

struct f32x3 {
    f32     x;
    f32     y;
    f32     z;
};

inline __device__ fn operator+(f32x3 a, f32x3 b) -> f32x3 { return f32x3{ a.x + b.x, a.y + b.y, a.z + b.z }; }
inline __device__ fn operator-(f32x3 a, f32x3 b) -> f32x3 { return f32x3{ a.x - b.x, a.y - b.y, a.z - b.z }; }
inline __device__ fn operator*(f32x3 a, f32x3 b) -> f32x3 { return f32x3{ a.x * b.x, a.y * b.y, a.z * b.z }; }
inline __device__ fn operator/(f32x3 a, f32x3 b) -> f32x3 { return f32x3{ a.x / b.x, a.y / b.y, a.z / b.z }; }

inline __device__ fn operator+=(f32x3& a, f32x3 b) -> f32x3& { a.x += b.x, a.y += b.y, a.z += b.z; return a; }
inline __device__ fn operator-=(f32x3& a, f32x3 b) -> f32x3& { a.x -= b.x, a.y -= b.y, a.z -= b.z; return a; }
inline __device__ fn operator*=(f32x3& a, f32x3 b) -> f32x3& { a.x *= b.x, a.y *= b.y, a.z *= b.z; return a; }
inline __device__ fn operator/=(f32x3& a, f32x3 b) -> f32x3& { a.x /= b.x, a.y /= b.y, a.z /= b.z; return a; }

inline __device__ fn operator*(f32 k, f32x3 v) -> f32x3 {
    let w = f32x3{ k*v.x, k*v.y, k*v.z };
    return w;
}

inline __device__ fn vnorm(f32x3 v) -> f32 {
    let w = v.x*v.x + v.y*v.y + v.z*v.z;
    return sqrtf(w);
}

namespace cuda
{
using tex_t = unsigned long long;
}

}
