#pragma once
#include <cmath>
#include <intrin.h>
#include "tc500_math.h"

namespace TChapman500 {
namespace Math {

__declspec(align(16))
union vector4f
{
	struct { float X, Y, Z, W; };
	__m128 Vector;

	inline vector4f() { X = 0.0f; Y = 0.0f; Z = 0.0f; W = 0.0f; }
	inline vector4f(float x, float y) { X = x; Y = y; Z = 0.0f; W = 0.0f; }
	inline vector4f(float x, float y, float z) { X = x; Y = y; Z = z; W = 0.0f; }
	inline vector4f(float x, float y, float z, float w) { X = x; Y = y; Z = z; W = w; }

	static inline vector4f normalXY(float theta)
	{
		__m128 mTheta = { theta, theta, theta, theta };
		__m128 cosTheta = _mm_cos_ps(mTheta);
		__m128 sinTheta = _mm_sin_ps(mTheta);
		vector4f result(-sinTheta.m128_f32[0], cosTheta.m128_f32[0]);
		return result;
	}
	static inline vector4f tangentXY(float theta)
	{
		__m128 mTheta = { theta, theta, theta, theta };
		__m128 cosTheta = _mm_cos_ps(mTheta);
		__m128 sinTheta = _mm_sin_ps(mTheta);
		vector4f result(cosTheta.m128_f32[0], sinTheta.m128_f32[0]);
		return result;
	}

	static inline vector4f normalXZ(float theta)
	{
		__m128 mTheta = { theta, theta, theta, theta };
		__m128 cosTheta = _mm_cos_ps(mTheta);
		__m128 sinTheta = _mm_sin_ps(mTheta);
		vector4f result(sinTheta.m128_f32[0], 0.0f, cosTheta.m128_f32[0]);
		return result;
	}
	static inline vector4f tangentXZ(float theta)
	{
		__m128 mTheta = { theta, theta, theta, theta };
		__m128 cosTheta = _mm_cos_ps(mTheta);
		__m128 sinTheta = _mm_sin_ps(mTheta);
		vector4f result(cosTheta.m128_f32[0], 0.0f, -sinTheta.m128_f32[0]);
		return result;
	}

	static inline vector4f normalYZ(float theta)
	{
		__m128 mTheta = { theta, theta, theta, theta };
		__m128 cosTheta = _mm_cos_ps(mTheta);
		__m128 sinTheta = _mm_sin_ps(mTheta);
		vector4f result(0.0f, -sinTheta.m128_f32[0], cosTheta.m128_f32[0]);
		return result;
	}
	static inline vector4f tangentYZ(float theta)
	{
		__m128 mTheta = { theta, theta, theta, theta };
		__m128 cosTheta = _mm_cos_ps(mTheta);
		__m128 sinTheta = _mm_sin_ps(mTheta);
		vector4f result(0.0f, cosTheta.m128_f32[0], sinTheta.m128_f32[0]);
		return result;
	}



	// Cross product of 2, 3-dimentional vectors (assumes w component is not used).
	static inline vector4f cross(vector4f a, vector4f b)
	{
		vector4f result;

		// Shuffle the elements of a and b to multiply the right elements together
		__m128 a_yzx = _mm_shuffle_ps(a.Vector, a.Vector, _MM_SHUFFLE(3, 0, 2, 1));
		__m128 b_yzx = _mm_shuffle_ps(b.Vector, b.Vector, _MM_SHUFFLE(3, 0, 2, 1));

		// Perform the multiplication for both parts of the cross product
		__m128 c1 = _mm_mul_ps(a_yzx, b.Vector);
		__m128 c2 = _mm_mul_ps(b_yzx, a.Vector);

		// Subtract the two parts of the cross product
		__m128 crossProduct = _mm_sub_ps(c2, c1);

		// Shuffle the result back to the original order and set the 4th element to 0
		result.Vector = _mm_shuffle_ps(crossProduct, crossProduct, _MM_SHUFFLE(3, 0, 2, 1));
		return result;
	}

	static inline float dot(vector4f a, vector4f b)
	{
		__m128 tmp = _mm_dp_ps(a.Vector, b.Vector, 0b11110001);
		return tmp.m128_f32[0];
	}

	static inline float sqr_distance(vector4f a, vector4f b)
	{
		__m128 tmp = _mm_sub_ps(a.Vector, b.Vector);
		tmp = _mm_dp_ps(a.Vector, b.Vector, 0b11110001);
		return tmp.m128_f32[0];
	}
	static inline float distance(vector4f a, vector4f b)
	{
		__m128 tmp = _mm_sub_ps(a.Vector, b.Vector);
		tmp = _mm_dp_ps(a.Vector, b.Vector, 0b11110001);
		tmp = _mm_sqrt_ps(tmp);
		return tmp.m128_f32[0];
	}

	inline float sqr_magnitude()
	{
		__m128 tmp = _mm_dp_ps(Vector, Vector, 0b11110001);
		return tmp.m128_f32[0];
	}

	inline float magnitude()
	{
		__m128 tmp = _mm_dp_ps(Vector, Vector, 0b11110001);
		tmp = _mm_sqrt_ps(tmp);
		return tmp.m128_f32[0];
	}

	inline vector4f normalized()
	{
		// Copy-paste of the magnitude function
		__m128 tmp = _mm_dp_ps(Vector, Vector, 0b11111111);
		tmp = _mm_sqrt_ps(tmp);

		vector4f result;
		result.Vector = _mm_div_ps(Vector, tmp);
		return result;
	}

	inline static vector4f lerp(vector4f a, vector4f b, float t)
	{
		__m128 mT = { t, t, t, t };
		__m128 aLerp = _mm_mul_ps(_mm_sub_ps({ 1.0, 1.0, 1.0, 1.0 }, mT), a.Vector);
		__m128 bLerp = _mm_mul_ps(mT, b.Vector);
		vector4f result;
		result.Vector = _mm_add_ps(aLerp, bLerp);
		return result;
	}

	inline vector4f operator= (vector4f other)
	{
		Vector = other.Vector;
		return *this;
	}

};

inline vector4f operator+ (vector4f a, vector4f b)
{
	vector4f result;
	result.Vector = _mm_add_ps(a.Vector, b.Vector);
	return result;
}
inline vector4f &operator+= (vector4f a, vector4f b)
{
	a.Vector = _mm_add_ps(a.Vector, b.Vector);
	return a;
}

inline vector4f operator- (vector4f a, vector4f b)
{
	vector4f result;
	result.Vector = _mm_sub_ps(a.Vector, b.Vector);
	return result;
}
inline vector4f operator- (vector4f a)
{
	vector4f result;
	result.Vector = _mm_mul_ps(a.Vector, { -1.0, -1.0, -1.0, -1.0 });
	return result;
}
inline vector4f &operator-= (vector4f a, vector4f b)
{
	a.Vector = _mm_sub_ps(a.Vector, b.Vector);
	return a;
}

inline vector4f operator* (vector4f a, vector4f b)
{
	vector4f result;
	result.Vector = _mm_mul_ps(a.Vector, b.Vector);
	return result;
}
inline vector4f &operator*= (vector4f a, vector4f b)
{
	a.Vector = _mm_mul_ps(a.Vector, b.Vector);
	return a;
}

inline vector4f operator* (vector4f a, float b)
{
	__m128 scalar = { b, b, b, b };
	vector4f result;
	result.Vector = _mm_mul_ps(a.Vector, scalar);
	return result;
}
inline vector4f &operator*= (vector4f a, float b)
{
	__m128 scalar = { b, b, b, b };
	a.Vector = _mm_mul_ps(a.Vector, scalar);
	return a;
}
inline vector4f operator* (float a, vector4f b)
{
	__m128 scalar = { a, a, a, a };
	vector4f result;
	result.Vector = _mm_mul_ps(scalar, b.Vector);
	return result;
}

inline vector4f operator/ (vector4f a, vector4f b)
{
	vector4f result;
	result.Vector = _mm_div_ps(a.Vector, b.Vector);
	return result;
}
inline vector4f &operator/= (vector4f a, vector4f b)
{
	a.Vector = _mm_div_ps(a.Vector, b.Vector);
	return a;
}

inline vector4f operator/ (vector4f a, float b)
{
	__m128 scalar = { b, b, b, b };
	vector4f result;
	result.Vector = _mm_div_ps(a.Vector, scalar);
	return result;
}
inline vector4f &operator/= (vector4f a, float b)
{
	__m128 scalar = { b, b, b, b };
	a.Vector = _mm_div_ps(a.Vector, scalar);
	return a;
}

inline bool operator== (vector4f a, vector4f b)
{
	__m128 tmp = _mm_xor_ps(a.Vector, b.Vector);
	tmp = _mm_hadd_ps(tmp, tmp);
	return tmp.m128_f32[0] == 0.0;
}

inline bool operator!= (vector4f a, vector4f b)
{
	__m128 tmp = _mm_xor_ps(a.Vector, b.Vector);
	tmp = _mm_hadd_ps(tmp, tmp);
	return tmp.m128_f32[0] != 0.0;
}


__declspec(align(16))
union matrix2f
{
	struct
	{
		float M11, M12, X1, X2;
		float M21, M22, X3, X4;
	};
	__m128 Matrix[2];
	struct
	{
		vector4f Row1;
		vector4f Row2;
	};

	inline matrix2f()
	{
		M11 = 0.0f;
		M12 = 0.0f;
		X1 = 0.0f;
		X2 = 0.0f;
		M21 = 0.0f;
		M22 = 0.0f;
		X3 = 0.0f;
		X4 = 0.0f;
	}

	inline matrix2f(float theta)
	{
		__m128 mTheta = { theta, theta, theta, theta };
		__m128 cosTheta = _mm_cos_ps(mTheta);
		__m128 sinTheta = _mm_sin_ps(mTheta);

		M11 = cosTheta.m128_f32[0];
		M12 = -sinTheta.m128_f32[0];
		X1 = 0.0f;
		X2 = 0.0f;

		M21 = sinTheta.m128_f32[0];
		M22 = cosTheta.m128_f32[0];
		X3 = 0.0f;
		X4 = 0.0f;
	}

	inline matrix2f(float m11, float m12, float m21, float m22)
	{
		M11 = m11;
		M12 = m12;
		X1 = 0.0f;
		X2 = 0.0f;

		M21 = m21;
		M22 = m22;
		X3 = 0.0f;
		X4 = 0.0f;
	}

	static inline matrix2f identity() { return matrix2f(1.0f, 0.0f, 0.0f, 1.0f); }

	inline matrix2f transposed() { return matrix2f(M11, M21, M12, M22); }

	inline matrix2f operator= (matrix2f other)
	{
		Matrix[0] = other.Matrix[0];
		Matrix[1] = other.Matrix[1];
		return *this;
	}
};

inline matrix2f operator+ (matrix2f mat, float scale)
{
	__m128 mScale = { scale, scale, 0.0f, 0.0f };
	matrix2f result;
	result.Matrix[0] = _mm_add_ps(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_add_ps(mat.Matrix[1], mScale);
	return result;
}

inline matrix2f operator- (matrix2f mat, float scale)
{
	__m128 mScale = { scale, scale, 0.0f, 0.0f };
	matrix2f result;
	result.Matrix[0] = _mm_sub_ps(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_sub_ps(mat.Matrix[1], mScale);
	return result;
}

inline matrix2f operator* (matrix2f mat, float scale)
{
	__m128 mScale = { scale, scale, 0.0f, 0.0f };
	matrix2f result;
	result.Matrix[0] = _mm_mul_ps(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_mul_ps(mat.Matrix[1], mScale);
	return result;
}

inline matrix2f operator/ (matrix2f mat, float scale)
{
	__m128 mScale = { scale, scale, 0.0f, 0.0f };
	matrix2f result;
	result.Matrix[0] = _mm_div_ps(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_div_ps(mat.Matrix[1], mScale);
	return result;
}

inline vector4f operator* (matrix2f mat, vector4f vec)
{
	__m128 dot1 = _mm_dp_ps(mat.Matrix[0], vec.Vector, 0b11110001);
	__m128 dot2 = _mm_dp_ps(mat.Matrix[1], vec.Vector, 0b11110010);
	vector4f result;
	result.Vector = _mm_add_ps(dot1, dot2);
	return result;
}

inline vector4f operator* (vector4f vec, matrix2f mat)
{
	__m128 dot1 = _mm_dp_ps(mat.Matrix[0], vec.Vector, 0b11110001);
	__m128 dot2 = _mm_dp_ps(mat.Matrix[1], vec.Vector, 0b11110010);
	vector4f result;
	result.Vector = _mm_add_ps(dot1, dot2);
	return result;
}

inline matrix2f operator* (matrix2f a, matrix2f b)
{
	__m128 dot1 = _mm_dp_ps(a.Matrix[0], a.Matrix[0], 0b11110001);
	__m128 dot2 = _mm_dp_ps(a.Matrix[0], a.Matrix[1], 0b11110010);
	__m128 dot3 = _mm_dp_ps(a.Matrix[1], a.Matrix[0], 0b11110001);
	__m128 dot4 = _mm_dp_ps(a.Matrix[1], a.Matrix[1], 0b11110010);
	matrix2f result;
	result.Matrix[0] = _mm_add_ps(dot1, dot2);
	result.Matrix[1] = _mm_add_ps(dot3, dot4);
	return result;
}


__declspec(align(16))
union matrix3f
{
	struct
	{
		float M11, M12, M13, X1;
		float M21, M22, M23, X2;
		float M31, M32, M33, X3;
	};
	__m128 Matrix[3];
	struct
	{
		vector4f Row1;
		vector4f Row2;
		vector4f Row3;
	};

	inline matrix3f()
	{
		M11 = 0.0f;
		M12 = 0.0f;
		M13 = 0.0f;
		X1 = 0.0f;
		M21 = 0.0f;
		M22 = 0.0f;
		M23 = 0.0f;
		X2 = 0.0f;
		M31 = 0.0f;
		M32 = 0.0f;
		M33 = 0.0f;
		X3 = 0.0f;
	}

	inline matrix3f(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33)
	{
		M11 = m11;
		M12 = m12;
		M13 = m13;
		X1 = 0.0f;
		M21 = m21;
		M22 = m22;
		M23 = m23;
		X2 = 0.0f;
		M31 = m31;
		M32 = m32;
		M33 = m33;
		X3 = 0.0f;
	}

	static inline matrix3f rotateX(float theta)
	{
		__m128 mTheta = { theta, theta, theta, theta };
		__m128 cosTheta = _mm_cos_ps(mTheta);
		__m128 sinTheta = _mm_sin_ps(mTheta);
		return matrix3f(1.0f, 0.0f, 0.0f, 0.0f, cosTheta.m128_f32[0], -sinTheta.m128_f32[0], 0.0f, sinTheta.m128_f32[0], cosTheta.m128_f32[0]);
	}

	static inline matrix3f rotateY(float theta)
	{
		__m128 mTheta = { theta, theta, theta, theta };
		__m128 cosTheta = _mm_cos_ps(mTheta);
		__m128 sinTheta = _mm_sin_ps(mTheta);
		return matrix3f(cosTheta.m128_f32[0], 0.0f, sinTheta.m128_f32[0], 0.0f, 1.0f, 0.0f, -sinTheta.m128_f32[0], 0.0f, cosTheta.m128_f32[0]);
	}

	static inline matrix3f rotateZ(float theta)
	{
		__m128 mTheta = { theta, theta, theta, theta };
		__m128 cosTheta = _mm_cos_ps(mTheta);
		__m128 sinTheta = _mm_sin_ps(mTheta);
		return matrix3f(cosTheta.m128_f32[0], -sinTheta.m128_f32[0], 0.0f, sinTheta.m128_f32[0], cosTheta.m128_f32[0], 0.0f, 0.0f, 0.0f, 1.0f);
	}

	static inline matrix3f identity() { return matrix3f(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f); }

	inline matrix3f transposed() { return matrix3f(M11, M21, M31, M12, M22, M32, M13, M23, M33); }

	inline matrix3f operator= (matrix3f other)
	{
		Matrix[0] = other.Matrix[0];
		Matrix[1] = other.Matrix[1];
		Matrix[2] = other.Matrix[2];
		return *this;
	}
};

inline matrix3f operator+ (matrix3f mat, float scale)
{
	__m128 mScale = { scale, scale, scale, 0.0f };
	matrix3f result;
	result.Matrix[0] = _mm_add_ps(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_add_ps(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm_add_ps(mat.Matrix[2], mScale);
	return result;
}

inline matrix3f operator- (matrix3f mat, float scale)
{
	__m128 mScale = { scale, scale, scale, 0.0f };
	matrix3f result;
	result.Matrix[0] = _mm_sub_ps(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_sub_ps(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm_sub_ps(mat.Matrix[2], mScale);
	return result;
}

inline matrix3f operator* (matrix3f mat, float scale)
{
	__m128 mScale = { scale, scale, scale, 0.0f };
	matrix3f result;
	result.Matrix[0] = _mm_mul_ps(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_mul_ps(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm_mul_ps(mat.Matrix[2], mScale);
	return result;
}

inline matrix3f operator/ (matrix3f mat, float scale)
{
	__m128 mScale = { scale, scale, scale, 0.0f };
	matrix3f result;
	result.Matrix[0] = _mm_div_ps(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_div_ps(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm_div_ps(mat.Matrix[2], mScale);
	return result;
}

inline vector4f operator* (matrix3f mat, vector4f vec)
{
	__m128 dot1 = _mm_dp_ps(mat.Matrix[0], vec.Vector, 0b11110001);
	__m128 dot2 = _mm_dp_ps(mat.Matrix[1], vec.Vector, 0b11110010);
	__m128 dot3 = _mm_dp_ps(mat.Matrix[2], vec.Vector, 0b11110100);
	vector4f result;
	result.Vector = _mm_add_ps(dot1, dot2);
	result.Vector = _mm_add_ps(result.Vector, dot3);
	return result;
}

inline vector4f operator* (vector4f vec, matrix3f mat)
{
	__m128 dot1 = _mm_dp_ps(mat.Matrix[0], vec.Vector, 0b11110001);
	__m128 dot2 = _mm_dp_ps(mat.Matrix[1], vec.Vector, 0b11110010);
	__m128 dot3 = _mm_dp_ps(mat.Matrix[2], vec.Vector, 0b11110100);
	vector4f result;
	result.Vector = _mm_add_ps(dot1, dot2);
	result.Vector = _mm_add_ps(result.Vector, dot3);
	return result;
}

inline matrix3f operator* (matrix3f a, matrix3f b)
{
	matrix3f result;
	__m128 dot1 = _mm_dp_ps(a.Matrix[0], a.Matrix[0], 0b11110001);
	__m128 dot2 = _mm_dp_ps(a.Matrix[0], a.Matrix[1], 0b11110010);
	__m128 dot3 = _mm_dp_ps(a.Matrix[0], a.Matrix[2], 0b11110100);
	result.Matrix[0] = _mm_add_ps(dot1, dot2);
	result.Matrix[0] = _mm_add_ps(result.Matrix[0], dot3);
	dot1 = _mm_dp_ps(a.Matrix[1], a.Matrix[0], 0b11110001);
	dot2 = _mm_dp_ps(a.Matrix[1], a.Matrix[1], 0b11110010);
	dot3 = _mm_dp_ps(a.Matrix[1], a.Matrix[2], 0b11110100);
	result.Matrix[1] = _mm_add_ps(dot1, dot2);
	result.Matrix[1] = _mm_add_ps(result.Matrix[1], dot3);
	dot1 = _mm_dp_ps(a.Matrix[2], a.Matrix[0], 0b11110001);
	dot2 = _mm_dp_ps(a.Matrix[2], a.Matrix[1], 0b11110010);
	dot3 = _mm_dp_ps(a.Matrix[2], a.Matrix[2], 0b11110100);
	result.Matrix[2] = _mm_add_ps(dot1, dot2);
	result.Matrix[2] = _mm_add_ps(result.Matrix[2], dot3);
	return result;
}


__declspec(align(16))
union matrix4f
{
	struct
	{
		float M11, M12, M13, M14;
		float M21, M22, M23, M24;
		float M31, M32, M33, M34;
		float M41, M42, M43, M44;
	};
	__m128 Matrix[4];
	struct
	{
		vector4f Row1;
		vector4f Row2;
		vector4f Row3;
		vector4f Row4;
	};

	inline matrix4f()
	{
		M11 = 0.0f;
		M12 = 0.0f;
		M13 = 0.0f;
		M14 = 0.0f;
		M21 = 0.0f;
		M22 = 0.0f;
		M23 = 0.0f;
		M24 = 0.0f;
		M31 = 0.0f;
		M32 = 0.0f;
		M33 = 0.0f;
		M34 = 0.0f;
		M41 = 0.0f;
		M42 = 0.0f;
		M43 = 0.0f;
		M44 = 0.0f;
	}

	inline matrix4f(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
	{
		M11 = m11;
		M12 = m12;
		M13 = m13;
		M14 = m14;
		M21 = m21;
		M22 = m22;
		M23 = m23;
		M24 = m24;
		M31 = m31;
		M32 = m32;
		M33 = m33;
		M34 = m34;
		M41 = m41;
		M42 = m42;
		M43 = m43;
		M44 = m44;
	}

	static inline matrix4f rotateX(float theta)
	{
		__m128 mTheta = { theta, theta, theta, theta };
		__m128 cosTheta = _mm_cos_ps(mTheta);
		__m128 sinTheta = _mm_sin_ps(mTheta);
		return matrix4f(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cosTheta.m128_f32[0], -sinTheta.m128_f32[0], 0.0f, 0.0f, sinTheta.m128_f32[0], cosTheta.m128_f32[0], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	static inline matrix4f rotateY(float theta)
	{
		__m128 mTheta = { theta, theta, theta, theta };
		__m128 cosTheta = _mm_cos_ps(mTheta);
		__m128 sinTheta = _mm_sin_ps(mTheta);
		return matrix4f(cosTheta.m128_f32[0], 0.0f, sinTheta.m128_f32[0], 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sinTheta.m128_f32[0], 0.0f, cosTheta.m128_f32[0], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	static inline matrix4f rotateZ(float theta)
	{
		__m128 mTheta = { theta, theta, theta, theta };
		__m128 cosTheta = _mm_cos_ps(mTheta);
		__m128 sinTheta = _mm_sin_ps(mTheta);
		return matrix4f(cosTheta.m128_f32[0], -sinTheta.m128_f32[0], 0.0f, 0.0f, sinTheta.m128_f32[0], cosTheta.m128_f32[0], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	static inline matrix4f translate(matrix4f matrix, vector4f delta)
	{
		matrix4f result(matrix);
		result.M14 = delta.X;
		result.M24 = delta.Y;
		result.M34 = delta.Z;
		return result;
	}

	static inline matrix4f identity() { return matrix4f(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f); }

	inline matrix4f transposed() { return matrix4f(M11, M21, M31, M41, M12, M22, M32, M42, M13, M23, M33, M43, M14, M24, M34, M44); }

	inline matrix4f operator= (matrix4f other)
	{
		Matrix[0] = other.Matrix[0];
		Matrix[1] = other.Matrix[1];
		Matrix[2] = other.Matrix[2];
		Matrix[3] = other.Matrix[3];
		return *this;
	}
};

inline matrix4f operator+ (matrix4f mat, float scale)
{
	__m128 mScale = { scale, scale, scale, scale };
	matrix4f result;
	result.Matrix[0] = _mm_add_ps(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_add_ps(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm_add_ps(mat.Matrix[2], mScale);
	result.Matrix[3] = _mm_add_ps(mat.Matrix[3], mScale);
	return result;
}

inline matrix4f operator- (matrix4f mat, float scale)
{
	__m128 mScale = { scale, scale, scale, scale };
	matrix4f result;
	result.Matrix[0] = _mm_sub_ps(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_sub_ps(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm_sub_ps(mat.Matrix[2], mScale);
	result.Matrix[3] = _mm_sub_ps(mat.Matrix[3], mScale);
	return result;
}

inline matrix4f operator* (matrix4f mat, float scale)
{
	__m128 mScale = { scale, scale, scale, scale };
	matrix4f result;
	result.Matrix[0] = _mm_mul_ps(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_mul_ps(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm_mul_ps(mat.Matrix[2], mScale);
	result.Matrix[3] = _mm_mul_ps(mat.Matrix[3], mScale);
	return result;
}

inline matrix4f operator/ (matrix4f mat, float scale)
{
	__m128 mScale = { scale, scale, scale, scale };
	matrix4f result;
	result.Matrix[0] = _mm_div_ps(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_div_ps(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm_div_ps(mat.Matrix[2], mScale);
	result.Matrix[3] = _mm_div_ps(mat.Matrix[3], mScale);
	return result;
}

inline vector4f operator* (matrix4f mat, vector4f vec)
{
	__m128 dot1 = _mm_dp_ps(mat.Matrix[0], vec.Vector, 0b11110001);
	__m128 dot2 = _mm_dp_ps(mat.Matrix[1], vec.Vector, 0b11110010);
	__m128 dot3 = _mm_dp_ps(mat.Matrix[2], vec.Vector, 0b11110100);
	__m128 dot4 = _mm_dp_ps(mat.Matrix[3], vec.Vector, 0b11111000);
	vector4f result;
	result.Vector = _mm_add_ps(dot1, dot2);
	result.Vector = _mm_add_ps(result.Vector, dot3);
	result.Vector = _mm_add_ps(result.Vector, dot4);
	return result;
}

inline vector4f operator* (vector4f vec, matrix4f mat)
{
	__m128 dot1 = _mm_dp_ps(mat.Matrix[0], vec.Vector, 0b11110001);
	__m128 dot2 = _mm_dp_ps(mat.Matrix[1], vec.Vector, 0b11110010);
	__m128 dot3 = _mm_dp_ps(mat.Matrix[2], vec.Vector, 0b11110100);
	__m128 dot4 = _mm_dp_ps(mat.Matrix[3], vec.Vector, 0b11111000);
	vector4f result;
	result.Vector = _mm_add_ps(dot1, dot2);
	result.Vector = _mm_add_ps(result.Vector, dot3);
	result.Vector = _mm_add_ps(result.Vector, dot4);
	return result;
}

inline matrix4f operator* (matrix4f a, matrix4f b)
{
	matrix4f result;
	__m128 dot1 = _mm_dp_ps(a.Matrix[0], b.Matrix[0], 0b11110001);
	__m128 dot2 = _mm_dp_ps(a.Matrix[0], b.Matrix[1], 0b11110010);
	__m128 dot3 = _mm_dp_ps(a.Matrix[0], b.Matrix[2], 0b11110100);
	__m128 dot4 = _mm_dp_ps(a.Matrix[0], b.Matrix[3], 0b11111000);
	result.Matrix[0] = _mm_add_ps(dot1, dot2);
	result.Matrix[0] = _mm_add_ps(result.Matrix[0], dot3);
	result.Matrix[0] = _mm_add_ps(result.Matrix[0], dot4);
	dot1 = _mm_dp_ps(a.Matrix[1], b.Matrix[0], 0b11110001);
	dot2 = _mm_dp_ps(a.Matrix[1], b.Matrix[1], 0b11110010);
	dot3 = _mm_dp_ps(a.Matrix[1], b.Matrix[2], 0b11110100);
	dot4 = _mm_dp_ps(a.Matrix[1], b.Matrix[3], 0b11111000);
	result.Matrix[1] = _mm_add_ps(dot1, dot2);
	result.Matrix[1] = _mm_add_ps(result.Matrix[1], dot3);
	result.Matrix[1] = _mm_add_ps(result.Matrix[1], dot4);
	dot1 = _mm_dp_ps(a.Matrix[2], b.Matrix[0], 0b11110001);
	dot2 = _mm_dp_ps(a.Matrix[2], b.Matrix[1], 0b11110010);
	dot3 = _mm_dp_ps(a.Matrix[2], b.Matrix[2], 0b11110100);
	dot4 = _mm_dp_ps(a.Matrix[2], b.Matrix[3], 0b11111000);
	result.Matrix[2] = _mm_add_ps(dot1, dot2);
	result.Matrix[2] = _mm_add_ps(result.Matrix[2], dot3);
	result.Matrix[2] = _mm_add_ps(result.Matrix[2], dot4);
	dot1 = _mm_dp_ps(a.Matrix[3], b.Matrix[0], 0b11110001);
	dot2 = _mm_dp_ps(a.Matrix[3], b.Matrix[1], 0b11110010);
	dot3 = _mm_dp_ps(a.Matrix[3], b.Matrix[2], 0b11110100);
	dot4 = _mm_dp_ps(a.Matrix[3], b.Matrix[3], 0b11111000);
	result.Matrix[3] = _mm_add_ps(dot1, dot2);
	result.Matrix[3] = _mm_add_ps(result.Matrix[3], dot3);
	result.Matrix[3] = _mm_add_ps(result.Matrix[3], dot4);
	return result;
}


__declspec(align(16))
union quaternionf
{
	struct { float W, X, Y, Z; };
	__m128 Quaternion;
	vector4f Vector;

	inline quaternionf()
	{
		X = 0.0;
		Y = 0.0;
		Z = 0.0;
		W = 0.0;
	}

	inline quaternionf(float w, float x, float y, float z)
	{
		W = w;
		X = x;
		Y = y;
		Z = z;
	}

	inline quaternionf(vector4f vector, bool euler)	// Yaw, Pitch, Roll
	{
		if (euler)
		{
			__m128 axis = _mm_mul_ps({ 0.0f, vector.Y, vector.X, vector.Z }, { 0.5f, 0.5f, 0.5f, 0.5f });

			__m128 cosAxis = _mm_cos_ps(axis);
			__m128 sinAxis = _mm_sin_ps(axis);

			// Calculate left hand side of the operation
			__m128 yaw = { cosAxis.m128_f32[3], cosAxis.m128_f32[3], sinAxis.m128_f32[3], cosAxis.m128_f32[3] };
			__m128 pitch = { cosAxis.m128_f32[2], sinAxis.m128_f32[2], cosAxis.m128_f32[2], cosAxis.m128_f32[2] };
			__m128 roll = { sinAxis.m128_f32[1], cosAxis.m128_f32[1], cosAxis.m128_f32[1], cosAxis.m128_f32[1] };
			__m128 resultLeft = _mm_mul_ps(yaw, pitch);
			resultLeft = _mm_mul_ps(resultLeft, roll);

			// Calculate right hand side of the operatioin
			yaw = { sinAxis.m128_f32[3], sinAxis.m128_f32[3], cosAxis.m128_f32[3], sinAxis.m128_f32[3] };
			pitch = { sinAxis.m128_f32[2], cosAxis.m128_f32[2], sinAxis.m128_f32[2], sinAxis.m128_f32[2] };
			roll = { cosAxis.m128_f32[1], sinAxis.m128_f32[1], sinAxis.m128_f32[1], sinAxis.m128_f32[1] };
			__m128 resultRight = _mm_mul_ps(yaw, pitch);
			resultRight = _mm_mul_ps(resultRight, roll);

			// Why do you do this, Intel?
			Quaternion = _mm_addsub_ps(resultLeft, resultRight);
			Quaternion = _mm_shuffle_ps(Quaternion, Quaternion, 0b000110011);
		}
		else
		{
			vector.W = 0.0f;
			float angle = vector.magnitude() * 0.5f;

			// Shift the axis to match the location of the quaternion's normal component
			vector4f normal = vector;
			normal.Vector = { 0.0, normal.X, normal.Y, normal.Z };
			normal = normal.normalized();

			// Calculate the the normal and theta components
			__m128 cosTheta = _mm_cos_ps({ angle, 0.0f, 0.0f, 0.0f });
			cosTheta = _mm_mul_ps(cosTheta, { 1.0f, 0.0f, 0.0f, 0.0f });
			__m128 sinTheta = _mm_sin_ps({ 0.0f, angle, angle, angle });

			// Assemble the quaternion
			Quaternion = _mm_mul_ps(normal.Vector, sinTheta);
			Quaternion = _mm_add_ps(Quaternion, cosTheta);
		}


	}

	inline quaternionf(vector4f axis, float angle)
	{
		angle = angle * 0.5f;

		// Shift the axis to match the location of the quaternion's normal component
		vector4f normal = axis;
		normal.Vector = { 0.0f, normal.X, normal.Y, normal.Z };
		normal = normal.normalized();

		// Calculate the the normal and theta components
		__m128 cosTheta = _mm_cos_ps({ angle, 0.0f, 0.0f, 0.0f });
		cosTheta = _mm_mul_ps(cosTheta, { 1.0f, 0.0f, 0.0f, 0.0f });
		__m128 sinTheta = _mm_sin_ps({ 0.0f, angle, angle, angle });

		// Assemble the quaternion
		Quaternion = _mm_mul_ps(normal.Vector, sinTheta);
		Quaternion = _mm_add_ps(Quaternion, cosTheta);
	}


	inline vector4f to_eulerZXY()
	{

	}

	inline matrix3f to_matrix3()
	{
		return
		{
			1.0f - (2.0f * Y * Y) - (2.0f * Z * Z), (2.0f * X * Y) - (2.0f * W * Z), (2.0f * X * Z) + (2.0f * W * Y),
			(2.0f * X * Y) + (2.0f * W * Z), 1.0f - (2.0f * X * X) - (2.0f * Z * Z), (2.0f * Y * Z) + (2.0f * W * X),
			(2.0f * X * Z) - (2.0f * W * Y), (2.0f * Y * Z) - (2.0f * W * X), 1.0f - (2.0f * X * X) - (2.0f * Y * Y)
		};
	}

	inline matrix4f to_matrix4()
	{
		return
		{
			1.0f - (2.0f * Y * Y) - (2.0f * Z * Z), (2.0f * X * Y) - (2.0f * W * Z), (2.0f * X * Z) + (2.0f * W * Y), 0.0f,
			(2.0f * X * Y) + (2.0f * W * Z), 1.0f - (2.0f * X * X) - (2.0f * Z * Z), (2.0f * Y * Z) + (2.0f * W * X), 0.0f,
			(2.0f * X * Z) - (2.0f * W * Y), (2.0f * Y * Z) - (2.0f * W * X), 1.0f - (2.0f * X * X) - (2.0f * Y * Y), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	inline float sqr_magnitude() { return Vector.sqr_magnitude(); }

	inline float magnitude() { return Vector.magnitude(); }

	inline quaternionf normalized()
	{
		quaternionf result;
		result.Vector = Vector.normalized();
		return result;
	}

	static inline quaternionf identity() { return { 1.0f, 0.0f, 0.0f, 0.0f }; }

	inline quaternionf inverve() { return { W, -X, -Y, -Z }; }

	inline quaternionf &operator= (quaternionf other)
	{
		Quaternion = other.Quaternion;
		return *this;
	}
};

inline quaternionf operator* (quaternionf a, quaternionf b)
{
	return
	{
		(a.W * b.W) - (a.X * b.X) - (a.Y * b.Y) - (a.Z * b.Z),
		(a.W * b.X) + (a.X * b.W) + (a.Y * b.Z) - (a.Z * b.Y),
		(a.W * b.Y) - (a.X * b.Z) + (a.Y * b.W) + (a.Z * b.X),
		(a.W * b.Z) + (a.X * b.Y) - (a.Y * b.X) + (a.Z * b.W)
	};
}






__declspec(align(16))
union vector2
{
	struct { double X, Y; };
	__m128d Vector;

	inline vector2()
	{
		X = 0.0;
		Y = 0.0;
	}

	inline vector2(double x, double y)
	{
		X = x;
		Y = y;
	}

	inline static vector2 tangent(double theta)
	{
		__m128d mTheta = { theta, theta };
		vector2 result;
		result.Vector = _mm_shuffle_pd(_mm_cos_pd(mTheta), _mm_sin_pd(mTheta), 0);
		return result;
	}

	inline static vector2 normal(double theta)
	{
		__m128d mTheta = { theta, theta };

		__m128d negate;
		negate.m128d_f64[0] = -1.0;
		negate.m128d_f64[1] = 1.0;

		vector2 result;
		result.Vector = _mm_shuffle_pd(_mm_sin_pd(mTheta), _mm_cos_pd(mTheta), 0);
		result.Vector = _mm_mul_pd(result.Vector, negate);
		return result;
	}

	inline double sqr_magnitude()
	{
		__m128d product = _mm_dp_pd(Vector, Vector, 255);
		return product.m128d_f64[0];
	}

	inline double magnitude()
	{
		__m128d product = _mm_dp_pd(Vector, Vector, 255);
		product = _mm_sqrt_pd(product);
		return product.m128d_f64[0];
	}

	inline vector2 normalized()
	{
		__m128d product = _mm_dp_pd(Vector, Vector, 255);
		product = _mm_sqrt_pd(product);

		vector2 result;
		result.Vector = _mm_div_pd(Vector, product);
		return result;
	}

	inline double angle()
	{
		__m128d unitX = _mm_permute_pd(Vector, 0);
		__m128d unitY = _mm_permute_pd(Vector, 3);
		__m128d aTan = _mm_atan2_pd(unitY, unitX);

		double result = aTan.m128d_f64[0];
		if (Y < 0.0) result = -result + TChapman500::Math::PI;
		return result;
	}

	inline static double dot(vector2 a, vector2 b)
	{
		__m128d product = _mm_dp_pd(a.Vector, b.Vector, 255);
		return product.m128d_f64[0];
	}

	inline static double cross(vector2 a, vector2 b)
	{
		__m128d aShuffle = _mm_shuffle_pd(a.Vector, a.Vector, 2);
		__m128d bShuffle = _mm_shuffle_pd(b.Vector, b.Vector, 1);
		__m128d product = _mm_mul_pd(aShuffle, bShuffle);
		product = _mm_hsub_pd(product, product);
		return product.m128d_f64[0];
	}

	inline static double distance(vector2 a, vector2 b)
	{
		__m128d product = _mm_sub_pd(a.Vector, b.Vector);
		product = _mm_dp_pd(product, product, 255);
		product = _mm_sqrt_pd(product);
		return product.m128d_f64[0];
	}

	inline static vector2 lerp(vector2 a, vector2 b, double t)
	{
		__m128d mT = { t, t };
		__m128d aLerp = _mm_mul_pd(_mm_sub_pd({ 1.0, 1.0 }, mT), a.Vector);
		__m128d bLerp = _mm_mul_pd(mT, b.Vector);
		vector2 result;
		result.Vector = _mm_add_pd(aLerp, bLerp);
		return result;
	}

	inline vector2 operator= (vector2 other)
	{
		Vector = other.Vector;
		return *this;
	}
};

inline vector2 operator+ (vector2 a, vector2 b)
{
	vector2 result;
	result.Vector = _mm_add_pd(a.Vector, b.Vector);
	return result;
}
inline vector2 operator+= (vector2 &a, vector2 b)
{
	a.Vector = _mm_add_pd(a.Vector, b.Vector);
	return a;
}

inline vector2 operator- (vector2 a, vector2 b)
{
	vector2 result;
	result.Vector = _mm_sub_pd(a.Vector, b.Vector);
	return result;
}
inline vector2 operator- (vector2 a)
{
	vector2 result;
	result.Vector = _mm_mul_pd(a.Vector, { -1.0, -1.0 });
	return result;
}
inline vector2 operator-= (vector2 &a, vector2 b)
{
	a.Vector = _mm_sub_pd(a.Vector, b.Vector);
	return a;
}

inline vector2 operator* (vector2 a, vector2 b)
{
	vector2 result;
	result.Vector = _mm_mul_pd(a.Vector, b.Vector);
	return result;
}
inline vector2 operator*= (vector2 &a, vector2 b)
{
	a.Vector = _mm_mul_pd(a.Vector, b.Vector);
	return a;
}

inline vector2 operator* (vector2 a, double b)
{
	vector2 result;
	result.Vector = _mm_mul_pd(a.Vector, _mm_load1_pd(&b));
	return result;
}
inline vector2 operator*= (vector2 &a, double b)
{
	a.Vector = _mm_mul_pd(a.Vector, _mm_load1_pd(&b));
	return a;
}

inline vector2 operator* (double a, vector2 b)
{
	vector2 result;
	result.Vector = _mm_mul_pd(_mm_load1_pd(&a), b.Vector);
	return result;
}

inline vector2 operator/ (vector2 a, vector2 b)
{
	vector2 result;
	result.Vector = _mm_div_pd(a.Vector, b.Vector);
	return result;
}
inline vector2 operator/= (vector2 &a, vector2 b)
{
	a.Vector = _mm_div_pd(a.Vector, b.Vector);
	return a;
}

inline vector2 operator/ (vector2 a, double b)
{
	vector2 result;
	result.Vector = _mm_div_pd(a.Vector, _mm_load1_pd(&b));
	return result;
}
inline vector2 operator/= (vector2 &a, double b)
{
	a.Vector = _mm_div_pd(a.Vector, _mm_load1_pd(&b));
	return a;
}

inline bool operator== (vector2 a, vector2 b) { return (a.X == b.X && a.Y == b.Y); }
inline bool operator!= (vector2 a, vector2 b) { return (a.X != b.X || a.Y != b.Y); }


__declspec(align(32))
union vector4
{
	struct { double X, Y, Z, W; };
	__m256d Vector;

	inline vector4() { X = 0.0; Y = 0.0; Z = 0.0; W = 0.0; }
	inline vector4(double x, double y) { X = x; Y = y; Z = 0.0; W = 0.0; }
	inline vector4(double x, double y, double z) { X = x; Y = y; Z = z; W = 0.0; }
	inline vector4(double x, double y, double z, double w) { X = x; Y = y; Z = z; W = w; }

	// Cross product of 2, 3-dimentional vectors (assumes w component is not used).
	static inline vector4 cross(vector4 a, vector4 b)
	{
		vector4 result;
		result.Vector = _mm256_fmsub_pd(
			_mm256_permute4x64_pd(a.Vector, 0b11001001),
			_mm256_permute4x64_pd(b.Vector, 0b11010010),
			_mm256_mul_pd(
				_mm256_permute4x64_pd(a.Vector, 0b11010010),
				_mm256_permute4x64_pd(b.Vector, 0b11001001)
			)
		);
		return result;
	}

	static inline double dot(vector4 a, vector4 b)
	{
		__m256d tmp = _mm256_mul_pd(a.Vector, b.Vector);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_permute4x64_pd(tmp, 0b11101000);
		tmp = _mm256_hadd_pd(tmp, tmp);
		return tmp.m256d_f64[0];
	}

	static inline double sqr_distance(vector4 a, vector4 b)
	{
		__m256d tmp = _mm256_sub_pd(a.Vector, b.Vector);
		tmp = _mm256_mul_pd(tmp, tmp);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_permute4x64_pd(tmp, 0b11101000);
		tmp = _mm256_hadd_pd(tmp, tmp);
		return tmp.m256d_f64[0];
	}
	static inline double distance(vector4 a, vector4 b)
	{
		__m256d tmp = _mm256_sub_pd(a.Vector, b.Vector);
		tmp = _mm256_mul_pd(tmp, tmp);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_permute4x64_pd(tmp, 0b11101000);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_sqrt_pd(tmp);
		return tmp.m256d_f64[0];
	}

	inline double sqr_magnitude()
	{
		__m256d tmp = _mm256_mul_pd(Vector, Vector);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_permute4x64_pd(tmp, 0b11101000);
		tmp = _mm256_hadd_pd(tmp, tmp);
		return tmp.m256d_f64[0];
	}

	inline double magnitude()
	{
		__m256d tmp = _mm256_mul_pd(Vector, Vector);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_permute4x64_pd(tmp, 0b11101000);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_sqrt_pd(tmp);
		return tmp.m256d_f64[0];
	}

	inline vector4 normalized()
	{
		// Copy-paste of the magnitude function
		__m256d tmp = _mm256_mul_pd(Vector, Vector);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_permute4x64_pd(tmp, 0b11101000);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_sqrt_pd(tmp);
		tmp = _mm256_permute4x64_pd(tmp, 0b00000000);

		vector4 result;
		result.Vector = _mm256_div_pd(Vector, tmp);
		return result;
	}

	inline static vector4 lerp(vector4 a, vector4 b, double t)
	{
		__m256d mT = { t, t, t, t };
		__m256d aLerp = _mm256_mul_pd(_mm256_sub_pd({ 1.0, 1.0, 1.0, 1.0 }, mT), a.Vector);
		__m256d bLerp = _mm256_mul_pd(mT, b.Vector);
		vector4 result;
		result.Vector = _mm256_add_pd(aLerp, bLerp);
		return result;
	}

	inline vector4 operator= (vector4 other)
	{
		Vector = other.Vector;
		return *this;
	}
};

inline vector4 operator+ (vector4 a, vector4 b)
{
	vector4 result;
	result.Vector = _mm256_add_pd(a.Vector, b.Vector);
	return result;
}
inline vector4 &operator+= (vector4 a, vector4 b)
{
	a.Vector = _mm256_add_pd(a.Vector, b.Vector);
	return a;
}

inline vector4 operator- (vector4 a, vector4 b)
{
	vector4 result;
	result.Vector = _mm256_sub_pd(a.Vector, b.Vector);
	return result;
}
inline vector4 operator- (vector4 a)
{
	vector4 result;
	result.Vector = _mm256_mul_pd(a.Vector, { -1.0, -1.0, -1.0, -1.0 });
	return result;
}
inline vector4 &operator-= (vector4 a, vector4 b)
{
	a.Vector = _mm256_sub_pd(a.Vector, b.Vector);
	return a;
}

inline vector4 operator* (vector4 a, vector4 b)
{
	vector4 result;
	result.Vector = _mm256_mul_pd(a.Vector, b.Vector);
	return result;
}
inline vector4 &operator*= (vector4 a, vector4 b)
{
	a.Vector = _mm256_mul_pd(a.Vector, b.Vector);
	return a;
}

inline vector4 operator* (vector4 a, double b)
{
	vector4 scalar = vector4(b, b, b, b);
	vector4 result;
	result.Vector = _mm256_mul_pd(a.Vector, scalar.Vector);
	return result;
}
inline vector4 &operator*= (vector4 a, double b)
{
	vector4 scalar = vector4(b, b, b, b);
	a.Vector = _mm256_mul_pd(a.Vector, scalar.Vector);
	return a;
}
inline vector4 operator* (double a, vector4 b)
{
	vector4 scalar = vector4(a, a, a, a);
	vector4 result;
	result.Vector = _mm256_mul_pd(scalar.Vector, b.Vector);
	return result;
}

inline vector4 operator/ (vector4 a, vector4 b)
{
	vector4 result;
	result.Vector = _mm256_div_pd(a.Vector, b.Vector);
	return result;
}
inline vector4 &operator/= (vector4 a, vector4 b)
{
	a.Vector = _mm256_div_pd(a.Vector, b.Vector);
	return a;
}

inline vector4 operator/ (vector4 a, double b)
{
	vector4 scalar = vector4(b, b, b, b);
	vector4 result;
	result.Vector = _mm256_div_pd(a.Vector, scalar.Vector);
	return result;
}
inline vector4 &operator/= (vector4 a, double b)
{
	vector4 scalar = vector4(b, b, b, b);
	a.Vector = _mm256_div_pd(a.Vector, scalar.Vector);
	return a;
}

inline bool operator== (vector4 a, vector4 b)
{
	__m256d tmp = _mm256_xor_pd(a.Vector, b.Vector);
	tmp = _mm256_hadd_pd(tmp, tmp);
	tmp = _mm256_permute4x64_pd(tmp, 0b11011000);
	tmp = _mm256_hadd_pd(tmp, tmp);
	return tmp.m256d_f64[0] == 0.0;
}

inline bool operator!= (vector4 a, vector4 b)
{
	__m256d tmp = _mm256_xor_pd(a.Vector, b.Vector);
	tmp = _mm256_hadd_pd(tmp, tmp);
	tmp = _mm256_permute4x64_pd(tmp, 0b11011000);
	tmp = _mm256_hadd_pd(tmp, tmp);
	return tmp.m256d_f64[0] != 0.0;
}

__declspec(align(16))
union matrix2
{
	struct
	{
		double M11, M12;
		double M21, M22;
	};
	__m128d Matrix[2];
	struct
	{
		vector2 Row1;
		vector2 Row2;
	};

	inline matrix2()
	{
		M11 = 0.0;
		M12 = 0.0;
		M21 = 0.0;
		M22 = 0.0;
	}

	inline matrix2(double m11, double m12, double m21, double m22)
	{
		M11 = m11;
		M12 = m12;
		M21 = m21;
		M22 = m22;
	}

	inline matrix2(double theta)
	{
		__m128d mTheta = { theta, theta };

		__m128d negate;
		negate.m128d_f64[0] = -1.0;
		negate.m128d_f64[1] = 1.0;

		__m128d cosTheta = _mm_cos_pd(mTheta);
		__m128d sinTheta = _mm_sin_pd(mTheta);
		sinTheta = _mm_mul_pd(sinTheta, negate);

		Matrix[0] = _mm_shuffle_pd(cosTheta, sinTheta, 0);
		Matrix[1] = _mm_shuffle_pd(sinTheta, cosTheta, 1);
	}

	static inline matrix2 identity() { return matrix2(1.0, 0.0, 0.0, 1.0); }

	inline matrix2 transposed() { return matrix2(M11, M21, M12, M22); }


	inline matrix2 operator= (matrix2 &other)
	{
		Matrix[0] = other.Matrix[0];
		Matrix[1] = other.Matrix[1];
		return *this;
	}
};

inline matrix2 operator+ (matrix2 &mat, double scale)
{
	__m128d mScale = { scale, scale };

	matrix2 result;
	result.Matrix[0] = _mm_add_pd(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_add_pd(mat.Matrix[1], mScale);
	return result;
}

inline matrix2 operator- (matrix2 &mat, double scale)
{
	__m128d mScale = { scale, scale };

	matrix2 result;
	result.Matrix[0] = _mm_sub_pd(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_sub_pd(mat.Matrix[1], mScale);
	return result;
}

inline matrix2 operator* (matrix2 &mat, double scale)
{
	__m128d mScale = { scale, scale };

	matrix2 result;
	result.Matrix[0] = _mm_mul_pd(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_mul_pd(mat.Matrix[1], mScale);
	return result;
}

inline matrix2 operator/ (matrix2 &mat, double scale)
{
	__m128d mScale = { scale, scale };

	matrix2 result;
	result.Matrix[0] = _mm_div_pd(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm_div_pd(mat.Matrix[1], mScale);
	return result;
}

inline vector2 operator* (matrix2 &mat, vector2 &vec)
{
	__m128d dot1 = _mm_dp_pd(mat.Matrix[0], vec.Vector, 0xF1);
	__m128d dot2 = _mm_dp_pd(mat.Matrix[1], vec.Vector, 0xF2);
	vector2 result;
	result.Vector = _mm_add_pd(dot1, dot2);
	return result;
}

inline vector2 operator* (vector2 &vec, matrix2 &mat)
{
	__m128d dot1 = _mm_dp_pd(mat.Matrix[0], vec.Vector, 0xF1);
	__m128d dot2 = _mm_dp_pd(mat.Matrix[1], vec.Vector, 0xF2);
	vector2 result;
	result.Vector = _mm_add_pd(dot1, dot2);
	return result;
}

inline matrix2 operator* (matrix2 &a, matrix2 &b)
{
	matrix2 result;
	__m128d dot1 = _mm_dp_pd(a.Matrix[0], b.Matrix[0], 0xF1);
	__m128d dot2 = _mm_dp_pd(a.Matrix[0], b.Matrix[1], 0xF2);
	result.Matrix[0] = _mm_add_pd(dot1, dot2);
	dot1 = _mm_dp_pd(a.Matrix[1], b.Matrix[0], 0xF1);
	dot2 = _mm_dp_pd(a.Matrix[1], b.Matrix[1], 0xF2);
	result.Matrix[1] = _mm_add_pd(dot1, dot2);
	return result;
}

inline __m256d _mm256_dp_pd(__m256d &a, __m256d &b)
{
	__m256d result = _mm256_mul_pd(a, b);
	result = _mm256_hadd_pd(result, result);
	result = _mm256_permute4x64_pd(result, 0b11101000);
	result = _mm256_hadd_pd(result, result);
	result = _mm256_permute4x64_pd(result, 0);
	return result;
}

__declspec(align(32))
union matrix3
{
	struct
	{
		double M11, M12, M13, X1;
		double M21, M22, M23, X2;
		double M31, M32, M33, X3;
	};
	__m256d Matrix[3];
	struct
	{
		vector4 Row1;
		vector4 Row2;
		vector4 Row3;
	};


	inline matrix3()
	{
		M11 = 0.0;
		M12 = 0.0;
		M13 = 0.0;
		X1 = 0.0;
		M21 = 0.0;
		M22 = 0.0;
		M23 = 0.0;
		X2 = 0.0;
		M31 = 0.0;
		M32 = 0.0;
		M33 = 0.0;
		X3 = 0.0;
	}

	inline matrix3(double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33)
	{
		M11 = m11;
		M12 = m12;
		M13 = m13;
		X1 = 0.0;
		M21 = m21;
		M22 = m22;
		M23 = m23;
		X2 = 0.0;
		M31 = m31;
		M32 = m32;
		M33 = m33;
		X3 = 0.0;
	}

	inline matrix3(vector2 delta, double theta)
	{
		__m256d mTheta = { theta, theta, 0.0, 0.0 };

		__m256d negate = { -1.0, 1.0, 0.0, 0.0 };

		__m256d cosTheta = _mm256_cos_pd(mTheta);
		cosTheta = _mm256_mul_pd(cosTheta, { 1.0, 1.0, 0.0, 0.0 });
		__m256d sinTheta = _mm256_sin_pd(mTheta);
		sinTheta = _mm256_mul_pd(sinTheta, negate);

		Matrix[0] = _mm256_shuffle_pd(cosTheta, sinTheta, 0b00000000);
		X1 = 0.0;
		Matrix[1] = _mm256_shuffle_pd(sinTheta, cosTheta, 0b00000001);
		X2 = 0.0;
		Matrix[2] = { 0.0, 0.0, 1.0, 0.0 };
		M13 = delta.X;
		M23 = delta.Y;
	}

	inline static matrix3 rotateX(double theta)
	{
		__m256d mTheta = { 0.0, theta, theta, 0.0 };
		__m256d negate = { 0.0, 1.0, -1.0, 0.0 };
		__m256d cosTheta = _mm256_cos_pd(mTheta);
		cosTheta = _mm256_mul_pd(cosTheta, { 0.0, 1.0, 1.0, 0.0 });
		__m256d sinTheta = _mm256_sin_pd(mTheta);
		sinTheta = _mm256_mul_pd(sinTheta, negate);

		matrix3 result;
		result.Matrix[0] = { 1.0, 0.0, 0.0, 0.0 };
		result.Matrix[1] = _mm256_shuffle_pd(sinTheta, cosTheta, 0b00001010);
		result.X2 = 0.0;
		result.Matrix[2] = _mm256_shuffle_pd(cosTheta, sinTheta, 0b00001010);
		result.X3 = 0.0;
		return result;
	}

	inline static matrix3 rotateY(double theta)
	{
		__m256d mTheta = { theta, theta, 0.0, 0.0 };
		__m256d negate = { -1.0, 1.0, 0.0, 0.0 };
		__m256d cosTheta = _mm256_cos_pd(mTheta);
		cosTheta = _mm256_mul_pd(cosTheta, { 1.0, 1.0, 0.0, 0.0 });
		__m256d sinTheta = _mm256_sin_pd(mTheta);
		sinTheta = _mm256_mul_pd(sinTheta, negate);

		matrix3 result;
		result.Matrix[0] = _mm256_shuffle_pd(sinTheta, cosTheta, 0b00000001);
		result.Matrix[0] = _mm256_permute4x64_pd(result.Matrix[0], 0b11001101);
		result.Matrix[1] = { 0.0, 1.0, 0.0, 0.0 };
		result.Matrix[2] = _mm256_shuffle_pd(cosTheta, sinTheta, 0b00000000);
		result.Matrix[2] = _mm256_permute4x64_pd(result.Matrix[2], 0b11001101);
		return result;
	}

	inline static matrix3 rotateZ(double theta)
	{
		__m256d mTheta = { theta, theta, 0.0, 0.0 };
		__m256d negate = { -1.0, 1.0, 0.0, 0.0 };
		__m256d cosTheta = _mm256_cos_pd(mTheta);
		cosTheta = _mm256_mul_pd(cosTheta, { 1.0, 1.0, 0.0, 0.0 });
		__m256d sinTheta = _mm256_sin_pd(mTheta);
		sinTheta = _mm256_mul_pd(sinTheta, negate);

		matrix3 result;
		result.Matrix[0] = _mm256_shuffle_pd(cosTheta, sinTheta, 0b00000000);
		result.X1 = 0.0;
		result.Matrix[1] = _mm256_shuffle_pd(sinTheta, cosTheta, 0b00000001);
		result.X2 = 0.0;
		result.Matrix[2] = { 0.0, 0.0, 1.0, 0.0 };
		return result;
	}

	inline static matrix3 identity() { return matrix3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0); }

	inline matrix3 transposed() { return matrix3(M11, M21, M31, M12, M22, M32, M13, M23, M33); }

	inline matrix3 operator= (matrix3 other)
	{
		Matrix[0] = other.Matrix[0];
		Matrix[1] = other.Matrix[1];
		Matrix[2] = other.Matrix[2];
		return *this;
	}
};

inline matrix3 operator+ (matrix3 mat, double scale)
{
	__m256d mScale = { scale, scale, scale, 0.0 };
	matrix3 result;
	result.Matrix[0] = _mm256_add_pd(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm256_add_pd(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm256_add_pd(mat.Matrix[2], mScale);
	return result;
}

inline matrix3 operator- (matrix3 mat, double scale)
{
	__m256d mScale = { scale, scale, scale, 0.0 };
	matrix3 result;
	result.Matrix[0] = _mm256_sub_pd(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm256_sub_pd(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm256_sub_pd(mat.Matrix[2], mScale);
	return result;
}

inline matrix3 operator* (matrix3 mat, double scale)
{
	__m256d mScale = { scale, scale, scale, 0.0 };
	matrix3 result;
	result.Matrix[0] = _mm256_mul_pd(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm256_mul_pd(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm256_mul_pd(mat.Matrix[2], mScale);
	return result;
}

inline matrix3 operator/ (matrix3 mat, double scale)
{
	__m256d mScale = { scale, scale, scale, 0.0 };
	matrix3 result;
	result.Matrix[0] = _mm256_div_pd(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm256_div_pd(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm256_div_pd(mat.Matrix[2], mScale);
	return result;
}

inline vector4 operator* (matrix3 mat, vector4 vec)
{
	__m256d dot1 = _mm256_dp_pd(mat.Matrix[0], vec.Vector);
	__m256d dot2 = _mm256_dp_pd(mat.Matrix[1], vec.Vector);
	__m256d dot3 = _mm256_dp_pd(mat.Matrix[2], vec.Vector);
	vector4 result;
	result.Vector = { dot1.m256d_f64[0], dot2.m256d_f64[0], dot3.m256d_f64[0], 0.0 };
	return result;
}

inline vector4 operator* (vector4 vec, matrix3 mat)
{
	__m256d dot1 = _mm256_dp_pd(mat.Matrix[0], vec.Vector);
	__m256d dot2 = _mm256_dp_pd(mat.Matrix[1], vec.Vector);
	__m256d dot3 = _mm256_dp_pd(mat.Matrix[2], vec.Vector);
	vector4 result;
	result.Vector = { dot1.m256d_f64[0], dot2.m256d_f64[0], dot3.m256d_f64[0], 0.0 };
	return result;
}

inline matrix3 operator* (matrix3 a, matrix3 b)
{
	matrix3 result;
	__m256d dot1 = _mm256_dp_pd(a.Matrix[0], b.Matrix[0]);
	__m256d dot2 = _mm256_dp_pd(a.Matrix[0], b.Matrix[1]);
	__m256d dot3 = _mm256_dp_pd(a.Matrix[0], b.Matrix[2]);
	result.Matrix[0] = { dot1.m256d_f64[0], dot2.m256d_f64[0], dot3.m256d_f64[0], 0.0 };
	dot1 = _mm256_dp_pd(a.Matrix[1], b.Matrix[0]);
	dot2 = _mm256_dp_pd(a.Matrix[1], b.Matrix[1]);
	dot3 = _mm256_dp_pd(a.Matrix[1], b.Matrix[2]);
	result.Matrix[1] = { dot1.m256d_f64[0], dot2.m256d_f64[0], dot3.m256d_f64[0], 0.0 };
	dot1 = _mm256_dp_pd(a.Matrix[2], b.Matrix[0]);
	dot2 = _mm256_dp_pd(a.Matrix[2], b.Matrix[1]);
	dot3 = _mm256_dp_pd(a.Matrix[2], b.Matrix[2]);
	result.Matrix[2] = { dot1.m256d_f64[0], dot2.m256d_f64[0], dot3.m256d_f64[0], 0.0 };
	return result;
}

__declspec(align(32))
union matrix4
{
	struct
	{
		double M11, M12, M13, M14;
		double M21, M22, M23, M24;
		double M31, M32, M33, M34;
		double M41, M42, M43, M44;
	};
	__m256d Matrix[4];
	struct
	{
		vector4 Row1;
		vector4 Row2;
		vector4 Row3;
		vector4 Row4;
	};


	inline matrix4()
	{
		M11 = 0.0;
		M12 = 0.0;
		M13 = 0.0;
		M14 = 0.0;
		M21 = 0.0;
		M22 = 0.0;
		M23 = 0.0;
		M24 = 0.0;
		M31 = 0.0;
		M32 = 0.0;
		M33 = 0.0;
		M34 = 0.0;
		M41 = 0.0;
		M42 = 0.0;
		M43 = 0.0;
		M44 = 0.0;
	}

	inline matrix4(double m11, double m12, double m13, double m14, double m21, double m22, double m23, double m24, double m31, double m32, double m33, double m34, double m41, double m42, double m43, double m44)
	{
		M11 = m11;
		M12 = m12;
		M13 = m13;
		M14 = m14;
		M21 = m21;
		M22 = m22;
		M23 = m23;
		M24 = m24;
		M31 = m31;
		M32 = m32;
		M33 = m33;
		M34 = m34;
		M41 = m41;
		M42 = m42;
		M43 = m43;
		M44 = m44;
	}

	inline static matrix4 rotateX(double theta)
	{
		__m256d mTheta = { 0.0, theta, theta, 0.0 };
		__m256d negate = { 0.0, 1.0, -1.0, 0.0 };
		__m256d cosTheta = _mm256_cos_pd(mTheta);
		cosTheta = _mm256_mul_pd(cosTheta, { 1.0, 1.0, 0.0, 0.0 });
		__m256d sinTheta = _mm256_sin_pd(mTheta);
		sinTheta = _mm256_mul_pd(sinTheta, negate);

		matrix4 result;
		result.Matrix[0] = { 1.0, 0.0, 0.0, 0.0 };
		result.Matrix[1] = _mm256_shuffle_pd(sinTheta, cosTheta, 0b00001010);
		result.Matrix[2] = _mm256_shuffle_pd(cosTheta, sinTheta, 0b00001010);
		result.Matrix[3] = { 0.0, 0.0, 0.0, 1.0 };
		return result;
	}

	inline static matrix4 rotateY(double theta)
	{
		__m256d mTheta = { theta, theta, 0.0, 0.0 };
		__m256d negate = { -1.0, 1.0, 0.0, 0.0 };
		__m256d cosTheta = _mm256_cos_pd(mTheta);
		cosTheta = _mm256_mul_pd(cosTheta, { 1.0, 1.0, 0.0, 0.0 });
		__m256d sinTheta = _mm256_sin_pd(mTheta);
		sinTheta = _mm256_mul_pd(sinTheta, negate);

		matrix4 result;
		result.Matrix[0] = _mm256_shuffle_pd(sinTheta, cosTheta, 0b00000001);
		result.Matrix[0] = _mm256_permute4x64_pd(result.Matrix[0], 0b11001101);
		result.Matrix[1] = { 0.0, 1.0, 0.0, 0.0 };
		result.Matrix[2] = _mm256_shuffle_pd(cosTheta, sinTheta, 0b00000000);
		result.Matrix[2] = _mm256_permute4x64_pd(result.Matrix[2], 0b11001101);
		result.Matrix[3] = { 0.0, 0.0, 0.0, 1.0 };
		return result;
	}

	inline static matrix4 rotateZ(double theta)
	{
		__m256d mTheta = { theta, theta, 0.0, 0.0 };
		__m256d negate = { -1.0, 1.0, 0.0, 0.0 };
		__m256d cosTheta = _mm256_cos_pd(mTheta);
		cosTheta = _mm256_mul_pd(cosTheta, { 1.0, 1.0, 0.0, 0.0 });
		__m256d sinTheta = _mm256_sin_pd(mTheta);
		sinTheta = _mm256_mul_pd(sinTheta, negate);

		matrix4 result;
		result.Matrix[0] = _mm256_shuffle_pd(cosTheta, sinTheta, 0b00000000);
		result.Matrix[1] = _mm256_shuffle_pd(sinTheta, cosTheta, 0b00000001);
		result.M13 = 0.0;
		result.M23 = 0.0;
		result.M33 = 1.0;
		result.Matrix[3] = { 0.0, 0.0, 0.0, 1.0 };
		return result;
	}

	inline static matrix4 translate(vector4 delta)
	{
		matrix4 result;
		result.Matrix[0] = { 1.0, 0.0, 0.0, delta.X };
		result.Matrix[1] = { 0.0, 1.0, 0.0, delta.Y };
		result.Matrix[2] = { 0.0, 0.0, 1.0, delta.Z };
		result.Matrix[3] = { 0.0, 0.0, 0.0, 1.0 };
		return result;
	}

	inline static matrix4 translate(matrix4 matrix, vector4 delta)
	{
		matrix4 result;
		result.Matrix[0] = { matrix.M11, matrix.M12, matrix.M13, delta.X };
		result.Matrix[1] = { matrix.M21, matrix.M22, matrix.M23, delta.Y };
		result.Matrix[2] = { matrix.M31, matrix.M32, matrix.M33, delta.Z };
		result.Matrix[3] = { 0.0, 0.0, 0.0, 1.0 };
		return result;
	}

	inline static matrix4 translate(matrix3 matrix, vector4 delta)
	{
		matrix4 result;
		result.Matrix[0] = { matrix.M11, matrix.M12, matrix.M13, delta.X };
		result.Matrix[1] = { matrix.M21, matrix.M22, matrix.M23, delta.Y };
		result.Matrix[2] = { matrix.M31, matrix.M32, matrix.M33, delta.Z };
		result.Matrix[3] = { 0.0, 0.0, 0.0, 1.0 };
		return result;
	}

	inline static matrix4 identity() { return matrix4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0); }

	inline matrix4 transposed() { return matrix4(M11, M21, M31, M41, M12, M22, M32, M42, M13, M23, M33, M43, M14, M24, M34, M44); }

	inline matrix4 operator= (matrix4 other)
	{
		Matrix[0] = other.Matrix[0];
		Matrix[1] = other.Matrix[1];
		Matrix[2] = other.Matrix[2];
		Matrix[3] = other.Matrix[3];
		return *this;
	}
};

inline matrix4 operator+ (matrix4 mat, double scale)
{
	__m256d mScale = { scale, scale, scale, 0.0 };
	matrix4 result;
	result.Matrix[0] = _mm256_add_pd(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm256_add_pd(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm256_add_pd(mat.Matrix[2], mScale);
	result.Matrix[3] = _mm256_add_pd(mat.Matrix[3], mScale);
	return result;
}

inline matrix4 operator- (matrix4 mat, double scale)
{
	__m256d mScale = { scale, scale, scale, 0.0 };
	matrix4 result;
	result.Matrix[0] = _mm256_sub_pd(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm256_sub_pd(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm256_sub_pd(mat.Matrix[2], mScale);
	result.Matrix[3] = _mm256_sub_pd(mat.Matrix[3], mScale);
	return result;
}

inline matrix4 operator* (matrix4 mat, double scale)
{
	__m256d mScale = { scale, scale, scale, 0.0 };
	matrix4 result;
	result.Matrix[0] = _mm256_mul_pd(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm256_mul_pd(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm256_mul_pd(mat.Matrix[2], mScale);
	result.Matrix[3] = _mm256_mul_pd(mat.Matrix[3], mScale);
	return result;
}

inline matrix4 operator/ (matrix4 mat, double scale)
{
	__m256d mScale = { scale, scale, scale, 0.0 };
	matrix4 result;
	result.Matrix[0] = _mm256_div_pd(mat.Matrix[0], mScale);
	result.Matrix[1] = _mm256_div_pd(mat.Matrix[1], mScale);
	result.Matrix[2] = _mm256_div_pd(mat.Matrix[2], mScale);
	result.Matrix[3] = _mm256_div_pd(mat.Matrix[3], mScale);
	return result;
}

inline vector4 operator* (matrix4 mat, vector4 vec)
{
	__m256d dot1 = _mm256_dp_pd(mat.Matrix[0], vec.Vector);
	__m256d dot2 = _mm256_dp_pd(mat.Matrix[1], vec.Vector);
	__m256d dot3 = _mm256_dp_pd(mat.Matrix[2], vec.Vector);
	__m256d dot4 = _mm256_dp_pd(mat.Matrix[3], vec.Vector);
	vector4 result;
	result.Vector = { dot1.m256d_f64[0], dot2.m256d_f64[0], dot3.m256d_f64[0], dot4.m256d_f64[0] };
	return result;
}

inline vector4 operator* (vector4 vec, matrix4 mat)
{
	__m256d dot1 = _mm256_dp_pd(mat.Matrix[0], vec.Vector);
	__m256d dot2 = _mm256_dp_pd(mat.Matrix[1], vec.Vector);
	__m256d dot3 = _mm256_dp_pd(mat.Matrix[2], vec.Vector);
	__m256d dot4 = _mm256_dp_pd(mat.Matrix[3], vec.Vector);
	vector4 result;
	result.Vector = { dot1.m256d_f64[0], dot2.m256d_f64[0], dot3.m256d_f64[0], dot4.m256d_f64[0] };
	return result;
}

inline matrix4 operator* (matrix4 a, matrix4 b)
{
	matrix4 result;
	__m256d dot1 = _mm256_dp_pd(a.Matrix[0], b.Matrix[0]);
	__m256d dot2 = _mm256_dp_pd(a.Matrix[0], b.Matrix[1]);
	__m256d dot3 = _mm256_dp_pd(a.Matrix[0], b.Matrix[2]);
	__m256d dot4 = _mm256_dp_pd(a.Matrix[0], b.Matrix[3]);
	result.Matrix[0] = { dot1.m256d_f64[0], dot2.m256d_f64[0], dot3.m256d_f64[0], dot4.m256d_f64[0] };
	dot1 = _mm256_dp_pd(a.Matrix[1], b.Matrix[0]);
	dot2 = _mm256_dp_pd(a.Matrix[1], b.Matrix[1]);
	dot3 = _mm256_dp_pd(a.Matrix[1], b.Matrix[2]);
	dot4 = _mm256_dp_pd(a.Matrix[1], b.Matrix[3]);
	result.Matrix[1] = { dot1.m256d_f64[0], dot2.m256d_f64[0], dot3.m256d_f64[0], dot4.m256d_f64[0] };
	dot1 = _mm256_dp_pd(a.Matrix[2], b.Matrix[0]);
	dot2 = _mm256_dp_pd(a.Matrix[2], b.Matrix[1]);
	dot3 = _mm256_dp_pd(a.Matrix[2], b.Matrix[2]);
	dot4 = _mm256_dp_pd(a.Matrix[2], b.Matrix[3]);
	result.Matrix[2] = { dot1.m256d_f64[0], dot2.m256d_f64[0], dot3.m256d_f64[0], dot4.m256d_f64[0] };
	dot1 = _mm256_dp_pd(a.Matrix[3], b.Matrix[0]);
	dot2 = _mm256_dp_pd(a.Matrix[3], b.Matrix[1]);
	dot3 = _mm256_dp_pd(a.Matrix[3], b.Matrix[2]);
	dot4 = _mm256_dp_pd(a.Matrix[3], b.Matrix[3]);
	result.Matrix[2] = { dot1.m256d_f64[0], dot2.m256d_f64[0], dot3.m256d_f64[0], dot4.m256d_f64[0] };
	return result;
}


__declspec(align(32))
union quaternion
{
	struct { double W, X, Y, Z; };
	__m256d Quaternion;
	vector4 Vector;

	inline quaternion()
	{
		X = 0.0;
		Y = 0.0;
		Z = 0.0;
		W = 0.0;
	}

	inline quaternion(double w, double x, double y, double z)
	{
		W = w;
		X = x;
		Y = y;
		Z = z;
	}

	inline quaternion(vector4 vector, bool euler)	// Yaw, Pitch, Roll
	{
		if (euler)
		{
			__m256d axis = _mm256_mul_pd({ 0.0, vector.Y, vector.X, vector.Z }, { 0.5, 0.5, 0.5, 0.5 });

			__m256d cosAxis = _mm256_cos_pd(axis);
			__m256d sinAxis = _mm256_sin_pd(axis);

			// Calculate left hand side of the operation
			__m256d yaw = { cosAxis.m256d_f64[3], cosAxis.m256d_f64[3], sinAxis.m256d_f64[3], cosAxis.m256d_f64[3] };
			__m256d pitch = { cosAxis.m256d_f64[2], sinAxis.m256d_f64[2], cosAxis.m256d_f64[2], cosAxis.m256d_f64[2] };
			__m256d roll = { sinAxis.m256d_f64[1], cosAxis.m256d_f64[1], cosAxis.m256d_f64[1], cosAxis.m256d_f64[1] };
			__m256d resultLeft = _mm256_mul_pd(yaw, pitch);
			resultLeft = _mm256_mul_pd(resultLeft, roll);

			// Calculate right hand side of the operatioin
			yaw = { sinAxis.m256d_f64[3], sinAxis.m256d_f64[3], cosAxis.m256d_f64[3], sinAxis.m256d_f64[3] };
			pitch = { sinAxis.m256d_f64[2], cosAxis.m256d_f64[2], sinAxis.m256d_f64[2], sinAxis.m256d_f64[2] };
			roll = { cosAxis.m256d_f64[1], sinAxis.m256d_f64[1], sinAxis.m256d_f64[1], sinAxis.m256d_f64[1] };
			__m256d resultRight = _mm256_mul_pd(yaw, pitch);
			resultRight = _mm256_mul_pd(resultRight, roll);

			// Why do you do this, Intel?
			Quaternion = _mm256_addsub_pd(resultLeft, resultRight);
			Quaternion = _mm256_permute4x64_pd(Quaternion, 0b00011011);
		}
		else
		{
			vector.W = 0.0;
			double angle = vector.magnitude() * 0.5;

			// Shift the axis to match the location of the quaternion's normal component
			vector4 normal = vector;
			normal.Vector = { 0.0, normal.X, normal.Y, normal.Z };
			normal = normal.normalized();

			// Calculate the the normal and theta components
			__m256d cosTheta = _mm256_cos_pd({ angle, 0.0, 0.0, 0.0 });
			cosTheta = _mm256_mul_pd(cosTheta, { 1.0, 0.0, 0.0, 0.0 });
			__m256d sinTheta = _mm256_sin_pd({ 0.0, angle, angle, angle });

			// Assemble the quaternion
			Quaternion = _mm256_mul_pd(normal.Vector, sinTheta);
			Quaternion = _mm256_add_pd(Quaternion, cosTheta);
		}


	}

	inline quaternion(vector4 axis, double angle)
	{
		angle = angle * 0.5f;

		// Shift the axis to match the location of the quaternion's normal component
		vector4 normal = axis;
		normal.Vector = { 0.0, normal.X, normal.Y, normal.Z };
		normal = normal.normalized();

		// Calculate the the normal and theta components
		__m256d cosTheta = _mm256_cos_pd({ angle, 0.0, 0.0, 0.0 });
		cosTheta = _mm256_mul_pd(cosTheta, { 1.0, 0.0, 0.0, 0.0 });
		__m256d sinTheta = _mm256_sin_pd({ 0.0, angle, angle, angle });

		// Assemble the quaternion
		Quaternion = _mm256_mul_pd(normal.Vector, sinTheta);
		Quaternion = _mm256_add_pd(Quaternion, cosTheta);
	}

	inline vector4 to_eulerZXY()
	{

	}

	inline matrix3 to_matrix3()
	{
		return
		{
			1.0 - (2.0 * Y * Y) - (2.0 * Z * Z), (2.0 * X * Y) - (2.0 * W * Z), (2.0 * X * Z) + (2.0 * W * Y),
			(2.0 * X * Y) + (2.0 * W * Z), 1.0 - (2.0 * X * X) - (2.0 * Z * Z), (2.0 * Y * Z) + (2.0 * W * X),
			(2.0 * X * Z) - (2.0 * W * Y), (2.0 * Y * Z) - (2.0 * W * X), 1.0 - (2.0 * X * X) - (2.0 * Y * Y)
		};
	}

	inline matrix4 to_matrix4()
	{
		return
		{
			1.0 - (2.0 * Y * Y) - (2.0 * Z * Z), (2.0 * X * Y) - (2.0 * W * Z), (2.0 * X * Z) + (2.0 * W * Y), 0.0,
			(2.0 * X * Y) + (2.0 * W * Z), 1.0 - (2.0 * X * X) - (2.0 * Z * Z), (2.0 * Y * Z) + (2.0 * W * X), 0.0,
			(2.0 * X * Z) - (2.0 * W * Y), (2.0 * Y * Z) - (2.0 * W * X), 1.0 - (2.0 * X * X) - (2.0 * Y * Y), 0.0,
			0.0, 0.0, 0.0, 1.0
		};
	}

	inline double sqr_magnitude()
	{
		__m256d tmp = _mm256_mul_pd(Quaternion, Quaternion);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_permute4x64_pd(tmp, 0b11101000);
		tmp = _mm256_hadd_pd(tmp, tmp);
		return tmp.m256d_f64[0];
	}

	inline double magnitude()
	{
		__m256d tmp = _mm256_mul_pd(Quaternion, Quaternion);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_permute4x64_pd(tmp, 0b11101000);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_sqrt_pd(tmp);
		return tmp.m256d_f64[0];

	}

	inline quaternion normalized()
	{
		__m256d tmp = _mm256_mul_pd(Quaternion, Quaternion);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_permute4x64_pd(tmp, 0b11101000);
		tmp = _mm256_hadd_pd(tmp, tmp);
		tmp = _mm256_sqrt_pd(tmp);
		tmp = _mm256_permute4x64_pd(tmp, 0b00000000);

		quaternion result;
		result.Quaternion = _mm256_div_pd(Quaternion, tmp);
		return result;
	}

	static inline quaternion identity() { return { 1.0, 0.0, 0.0, 0.0 }; }

	inline quaternion inverve() { return { W, -X, -Y, -Z }; }

	inline quaternion &operator= (quaternion other)
	{
		Quaternion = other.Quaternion;
		return *this;
	}
};

inline quaternion operator* (quaternion a, quaternion b)
{
	return
	{
		(a.W * b.W) - (a.X * b.X) - (a.Y * b.Y) - (a.Z * b.Z),
		(a.W * b.X) + (a.X * b.W) + (a.Y * b.Z) - (a.Z * b.Y),
		(a.W * b.Y) - (a.X * b.Z) + (a.Y * b.W) + (a.Z * b.X),
		(a.W * b.Z) + (a.X * b.Y) - (a.Y * b.X) + (a.Z * b.W)
	};
}

}}
