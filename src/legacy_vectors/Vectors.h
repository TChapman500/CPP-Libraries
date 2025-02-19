#pragma once
#include <cmath>

namespace TChapman500
{
	const double PI = 3.14159265358979323846264338327950;

	namespace Vectors
	{
		template<typename T> struct vector2
		{
			T X, Y;

			static inline vector2 Tangent(T theta) { return { std::cos(theta), std::sin(theta) }; }
			static inline vector2 Normal(T theta) { return { -std::sin(theta), std::cos(theta) }; }

			inline vector2() { X = (T)0; Y = (T)0; }
			inline vector2(T x, T y) { X = x; Y = y; }
			inline T SqrMagnitude() { return (X * X) + (Y * Y); }
			inline T Magnitude() { return std::sqrt(SqrMagnitude()); }
			inline vector2 Normalized() { return *this / Magnitude(); }
			inline T Angle()
			{
				vector2<T> unit = Normalized();
				T result = (T)std::cos(unit.X);
				if (unit.Y < (T)0) result = -result + (T)(PI * 2.0);
				return result;
			}

			static inline T Dot(const vector2 &a, const vector2 &b) { return (a.X * b.X) + (a.Y * b.Y); }
			static inline T Distance(vector2 &a, const vector2 &b) { return (a - b).Magnitude(); }

			inline vector2 operator+ (const vector2 &b) { return { X + b.X, Y + b.Y }; }
			inline vector2 operator- (const vector2 &b) { return { X - b.X, Y - b.Y }; }
			inline vector2 operator- () { return { -X, -Y }; }
			inline vector2 operator* (const vector2 &b) { return { X * b.X, Y * b.Y }; }
			inline vector2 operator* (const T b) { return { X * b, Y * b }; }
			inline vector2 operator/ (const vector2 &b) { return { X / b.X, Y / b.Y }; }
			inline vector2 operator/ (const T b) { return { X / b, Y / b }; }
			
			inline vector2 &operator+= (const vector2 &b)
			{
				X += b.X;
				Y += b.Y;
				return *this;
			}

			inline vector2 &operator-= (const vector2 &b)
			{
				X -= b.X;
				Y -= b.Y;
				return *this;
			}

			inline vector2 &operator*= (const vector2 &b)
			{
				X *= b.X;
				Y *= b.Y;
				return *this;
			}

			inline vector2 &operator*= (const T b)
			{
				X *= b;
				Y *= b;
				return *this;
			}

			inline vector2 &operator/= (const vector2 &b)
			{
				X /= b.X;
				Y /= b.Y;
				return *this;
			}

			inline vector2 &operator/= (const T b)
			{
				X /= b;
				Y /= b;
				return *this;
			}

			inline bool operator== (const vector2 &b) { return { X == b.X && Y == b.Y }; }
			inline bool operator!= (const vector2 &b) { return { X != b.X || Y != b.Y }; }
		};
		template<typename T> inline vector2<T> operator* (const T a, const vector2<T> &b) { return { a * b.X, a * b.Y }; }


		template<typename T> struct vector3
		{
			T X, Y, Z;

			inline vector3() { X = (T)0; Y = (T)0; Z = (T)0; }
			inline vector3(T x, T y) { X = x; Y = y; Z = (T)0; }
			inline vector3(vector2<T> &vec2) { X = vec2.X; Y = vec2.Y; Z = (T)0; }
			inline vector3(T x, T y, T z) { X = x; Y = y; Z = z; }
			inline T SqrMagnitude() { return (X * X) + (Y * Y) + (Z * Z); }
			inline T Magnitude() { return (T)sqrt(SqrMagnitude()); }
			inline vector3 Normalized() { return (*this / Magnitude()); }

			static inline T Dot(const vector3 &a, const vector3 &b) { return ((a.X * b.X) + (a.Y * b.Y) + (a.Z * b.Z)); }
			static inline vector3 Cross(const vector3 &a, const vector3 &b)
			{
				return vector3(
					(a.Y * b.Z) - (a.Z * b.Y),
					(a.Z * b.X) - (a.X * b.Z),
					(a.X * b.Y) - (a.Y * b.X)
				);
			}
			static inline T Distance(const vector3 &a, const vector3 &b) { return (a - b).Magnitude(); }

			inline vector3 operator+ (const vector3 &b) { return { X + b.X, Y + b.Y, Z + b.Z }; }
			inline vector3 operator- (const vector3 &b) { return { X - b.X, Y - b.Y, Z - b.Z }; }
			inline vector3 operator- () { return { -X, -Y, -Z }; }
			inline vector3 operator* (const vector3 &b) { return { X * b.X, Y * b.Y, Z * b.Z }; }
			inline vector3 operator* (const T b) { return { X * b, Y * b, Z * b }; }
			inline vector3 operator/ (const vector3 &b) { return { X / b.X, Y / b.Y, Z / b.Z }; }
			inline vector3 operator/ (const double b) { return { X / b, Y / b, Z / b }; }

			inline vector3 &operator+= (const vector3 &b)
			{
				X += b.X;
				Y += b.Y;
				Z += b.Z;
				return *this;
			}
			inline vector3 &operator-= (const vector3 &b)
			{
				X -= b.X;
				Y -= b.Y;
				Z -= b.Z;
				return *this;
			}
			inline vector3 &operator*= (const vector3 &b)
			{
				X *= b.X;
				Y *= b.Y;
				Z *= b.Z;
				return *this;
			}
			inline vector3 &operator*= (const double b)
			{
				X *= b;
				Y *= b;
				Z *= b;
				return *this;
			}
			inline vector3 &operator/= (const vector3 &b)
			{
				X /= b.X;
				Y /= b.Y;
				Z /= b.Z;
				return *this;
			}
			inline vector3 &operator/= (const double b)
			{
				X /= b;
				Y /= b;
				Z /= b;
				return *this;
			}

			inline bool operator== (const vector3 &b) { return X == b.X && Y == b.Y && Z == b.Z; }
			inline bool operator!= (const vector3 &b) { return X != b.X || Y != b.Y || Z != b.Z; }
		};
		template<typename T> inline vector3<T> operator* (const T a, const vector3<T> &b) { return { a * b.X, a * b.Y, a * b.Z }; }


		template<typename T> struct vector4
		{
			T X, Y, Z, W;

			inline vector4() { X = (T)0; Y = (T)0; Z = (T)0; W = (T)0; }
			inline vector4(T x, T y) { X = x; Y = y; Z = (T)0; W = (T)0; }
			inline vector4(vector2<T> &vec2) { X = vec2.X; Y = vec2.Y; Z = (T)0; W = (T)0; }
			inline vector4(vector3<T> &vec3) { X = vec3.X; Y = vec3.Y; Z = vec3.Z; W = (T)0; }
			inline vector4(T x, T y, T z) { X = x; Y = y; Z = z; W = (T)0; }
			inline vector4(T x, T y, T z, T w) { X = x; Y = y; Z = z; W = w; }
			inline T SqrMagnitude() { return (X * X) + (Y * Y) + (Z * Z) + (W * W); }
			inline T Magnitude() { return (T)sqrt(SqrMagnitude()); }
			inline vector4 Normalized() { return *this / Magnitude(); }

			static inline T Dot(const vector4 &a, const vector4 &b) { return (a.X * b.X) + (a.Y * b.Y) + (a.Z * b.Z) + (a.W * b.W); }
			static inline T Distance(const vector4 &a, const vector4 &b) { return (a - b).Magnitude(); }

			inline vector4 operator+ (const vector4 &b) { return { X + b.X, Y + b.Y, Z + b.Z, W + b.W }; }
			inline vector4 operator- (const vector4 &b) { return { X - b.X, Y - b.Y, Z - b.Z, W - b.W }; }
			inline vector4 operator- () { return { -X, -Y, -Z, -W }; }
			inline vector4 operator* (const vector4 &b) { return { X * b.X, Y * b.Y, Z * b.Z, W * b.W }; }
			inline vector4 operator* (const T b) { return { X * b, Y * b, Z * b, W * b }; }
			inline vector4 operator/ (const vector4 &b) { return { X / b.X, Y / b.Y, Z / b.Z, W / b.W }; }
			inline vector4 operator/ (const T b) { return { X / b, Y / b, Z / b, W / b }; }

			inline vector4 &operator+= (const vector4 &b)
			{
				X += b.X;
				Y += b.Y;
				Z += b.Z;
				W += b.W;
				return *this;
			}
			inline vector4 &operator-= (const vector4 &b)
			{
				X -= b.X;
				Y -= b.Y;
				Z -= b.Z;
				W -= b.W;
				return *this;
			}
			inline vector4 &operator*= (const vector4 &b)
			{
				X *= b.X;
				Y *= b.Y;
				Z *= b.Z;
				W *= b.W;
				return *this;
			}
			inline vector4 &operator*= (const T b)
			{
				X *= b;
				Y *= b;
				Z *= b;
				W *= b;
				return *this;
			}
			inline vector4 &operator/= (const vector4 &b)
			{
				X /= b.X;
				Y /= b.Y;
				Z /= b.Z;
				W /= b.W;
				return *this;
			}
			inline vector4 &operator/= (const T b)
			{
				X /= b;
				Y /= b;
				Z /= b;
				W /= b;
				return *this;
			}

			inline bool operator== (const vector4 &b) { return X == b.X && Y == b.Y && Z == b.Z && W == b.W; }
			inline bool operator!= (const vector4 &b) { return X != b.X || Y != b.Y || Z != b.Z || W != b.W; }
		};
		template<typename T> inline vector4<T> operator* (const T a, const vector4<T> &b) { return { a * b.X, a * b.Y, a * b.Z, a * b.W }; }


		template<typename T> struct matrix2
		{
			T M11, M12, M21, M22;

			inline matrix2() { M11 = (T)0; M12 = (T)0; M21 = (T)0; M22 = (T)0; }
			inline matrix2(T m11, T m12, T m21, T m22) { M11 = m11; M12 = m12; M21 = m21; M22 = m22; }
			inline matrix2(T theta)
			{
				T cosTheta = std::cos(theta);
				T sinTheta = std::sin(theta);

				M11 = cosTheta;
				M12 = -sinTheta;
				M21 = sinTheta;
				M22 = cosTheta;
			}

			static inline matrix2 Identity() { return { (T)1, (T)0, (T)0, (T)1 }; }

			inline T Determinant() { return (M11 * M22) - (M12 * M21); }
			inline matrix2 Inverse()
			{
				T d = (T)1 / Determinant();
				return { d * M22, -d * M12, -d * M21, d * M11 };
			}
			inline matrix2 Transposed() { return { M11, M21, M12, M22 }; }

			inline matrix2 operator+ (const matrix2 &b) { return { M11 + b.M11, M12 + b.M12, M21 + b.M21, M22 + b.M22 }; }
			inline matrix2 operator- (const matrix2 &b) { return { M11 - b.M11, M12 - b.M12, M21 - b.M21, M22 - b.M22 }; }
			inline vector2<T> operator* (const vector2<T> &b) { return { (M11 * b.X) + (M12 * b.Y), (M21 * b.X) + (M22 * b.Y) }; }
			inline matrix2 operator* (const matrix2 &b) { return { (M11 * b.M11) + (M12 * b.M21), (M11 * b.M12) + (M12 * b.M22), (M21 * b.M11) + (M22 * b.M21), (M21 * b.M12) + (M22 * b.M22), }; }
			inline matrix2 operator* (const T b) { return { M11 * b, M12 * b, M21 * b, M22 * b }; }

			inline matrix2 &operator+= (const matrix2 &b)
			{
				M11 += b.M11;
				M12 += b.M12;
				M21 += b.M21;
				M22 += b.M22;
				return *this;
			}
			inline matrix2 &operator-= (const matrix2 &b)
			{
				M11 -= b.M11;
				M12 -= b.M12;
				M21 -= b.M21;
				M22 -= b.M22;
				return *this;
			}
			inline matrix2 &operator*= (const T b)
			{
				M11 *= b;
				M12 *= b;
				M21 *= b;
				M22 *= b;
				return *this;
			}

			inline bool operator== (const matrix2 &b) { return M11 == b.M11 && M12 == b.M12 && M21 == b.M21 && M22 == b.M22; }
			inline bool operator!= (const matrix2 &b) { return M11 != b.M11 || M12 != b.M12 || M21 != b.M21 || M22 != b.M22; }
		};
		template<typename T> inline vector2<T> operator* (const vector2<T> &a, const matrix2<T> &b) { return { (a.X * b.M11) + (a.Y * b.M21), (a.X * b.M12) + (a.Y * b.M22) }; }
		template<typename T> inline matrix2<T> operator* (const T a, const matrix2<T> &b) { return { a * b.M11, a * b.M12, a * b.M21, a * b.M22 }; }

		
		template<typename T> struct matrix3
		{
			enum class rotation_axis { X, Y, Z };

			T M11, M12, M13, M21, M22, M23, M31, M32, M33;

			inline matrix3()
			{
				M11 = (T)0;
				M12 = (T)0;
				M13 = (T)0;
				M21 = (T)0;
				M22 = (T)0;
				M23 = (T)0;
				M31 = (T)0;
				M32 = (T)0;
				M33 = (T)0;
			}
			inline matrix3(matrix2<T> &rotation, vector2<T> translation)
			{
				M11 = rotation.M11;
				M12 = rotation.M12;
				M13 = translation.X;
				M21 = rotation.M21;
				M22 = rotation.M22;
				M23 = translation.Y;
				M31 = (T)0;
				M32 = (T)0;
				M33 = (T)1;
			}
			inline matrix3(T theta, vector2<T> delta)
			{
				T cosTheta = std::cos(theta);
				T sinTheta = std::sin(theta);

				M11 = cosTheta;
				M12 = -sinTheta;
				M13 = delta.X;
				M21 = sinTheta;
				M22 = cosTheta;
				M23 = delta.Y;
				M31 = (T)0;
				M32 = (T)0;
				M33 = (T)1;
			}
			inline matrix3(T theta, rotation_axis axis)
			{
				T cosTheta = std::cos(theta);
				T sinTheta = std::sin(theta);

				switch (axis)
				{
				case rotation_axis::X:
					M11 = (T)1;
					M12 = (T)0;
					M13 = (T)0;
					M21 = (T)0;
					M22 = cosTheta;
					M23 = -sinTheta;
					M21 = (T)0;
					M22 = sinTheta;
					M23 = cosTheta;
					break;
				case rotation_axis::Y:
					M11 = cosTheta;
					M12 = (T)0;
					M13 = sinTheta;
					M21 = (T)0;
					M22 = (T)1;
					M23 = (T)0;
					M31 = -sinTheta;
					M32 = (T)0;
					M33 = cosTheta;
					break;
				case rotation_axis::Z:
					M11 = cosTheta;
					M12 = -sinTheta;
					M13 = (T)0;
					M21 = sinTheta;
					M22 = cosTheta;
					M23 = (T)0;
					M21 = (T)0;
					M22 = (T)0;
					M23 = (T)1;
					break;
				}
			}
			inline matrix3(T m11, T m12, T m13, T m21, T m22, T m23, T m31, T m32, T m33)
			{
				M11 = m11;
				M12 = m12;
				M13 = m13;
				M21 = m21;
				M22 = m22;
				M23 = m23;
				M31 = m31;
				M32 = m32;
				M33 = m33;
			}

			static inline matrix3 Identity() { return { (T)1, (T)0, (T)0, (T)0, (T)1, (T)0, (T)0, (T)0, (T)1 }; }

			inline T Determinant() { return { (M11 * M22 * M33) + (M12 * M23 * M31) + (M13 * M21 * M32) - (M13 * M22 * M31) - (M12 * M13 * M33) - (M11 * M13 * M32) }; }
			inline matrix3 Inverse()
			{
				T d = (T)1 / Determinant();
				return
				{
					d * ((M22 * M33) - (M23 * M32)), -d * ((M12 * M33) - (M13 * M32)), d * ((M12 * M23) - (M13 * M22)),
					-d * ((M21 * M33) - (M23 * M31)), d * ((M11 * M33) - (M13 * M31)), -d * ((M11 * M23) - (M13 * M21)),
					d *((M21 * M32) - (M22 * M31)), -d * ((M11 * M32) - (M12 * M31)), d *((M11 * M22) - (M12 * M21)),
				};
			}

			inline matrix3 Transposed() { return { M11, M21, M31, M12, M22, M32, M13, M23, M33 }; }

			inline matrix3 operator+ (const matrix3 &b)
			{
				return
				{
					M11 + b.M11, M12 + b.M12, M13 + b.M13,
					M21 + b.M21, M22 + b.M22, M23 + b.M23,
					M31 + b.M31, M32 + b.M32, M33 + b.M33
				};
			}
			inline matrix3 operator- (const matrix3 &b)
			{
				return
				{
					M11 - b.M11, M12 - b.M12, M13 - b.M13,
					M21 - b.M21, M22 - b.M22, M23 - b.M23,
					M31 - b.M31, M32 - b.M32, M33 - b.M33
				};
			}

			inline vector3<T> operator* (const vector3<T> &b)
			{
				return
				{
					(M11 * b.X) + (M12 * b.Y) + (M13 * b.Z),
					(M21 * b.X) + (M22 * b.Y) + (M23 * b.Z),
					(M31 * b.X) + (M32 * b.Y) + (M33 * b.Z)
				};
			}

			inline matrix3 operator* (const matrix3 &b)
			{
				return
				{
					(M11 * b.M11) + (M12 * b.M21) + (M13 * b.M31), (M11 * b.M12) + (M12 * b.M22) + (M13 * b.M32), (M11 * b.M13) + (M12 * b.M23) + (M13 * b.M33),
					(M21 * b.M11) + (M22 * b.M21) + (M23 * b.M31), (M21 * b.M12) + (M22 * b.M22) + (M23 * b.M32), (M21 * b.M13) + (M22 * b.M23) + (M23 * b.M33),
					(M31 * b.M11) + (M32 * b.M21) + (M33 * b.M31), (M31 * b.M12) + (M32 * b.M22) + (M33 * b.M32), (M31 * b.M13) + (M32 * b.M23) + (M33 * b.M33)
				};
			}

			inline matrix3 operator* (const T b)
			{
				return
				{
					M11 * b, M12 * b, M13 * b,
					M21 * b, M22 * b, M23 * b,
					M31 * b, M32 * b, M33 * b
				};
			}

			inline matrix3 &operator+= (const matrix3 &b)
			{
				M11 += b.M11;
				M12 += b.M12;
				M13 += b.M13;
				M21 += b.M21;
				M22 += b.M22;
				M23 += b.M23;
				M31 += b.M31;
				M32 += b.M32;
				M33 += b.M33;
				return *this;
			}

			inline matrix3 &operator-= (const matrix3 &b)
			{
				M11 -= b.M11;
				M12 -= b.M12;
				M13 -= b.M13;
				M21 -= b.M21;
				M22 -= b.M22;
				M23 -= b.M23;
				M31 -= b.M31;
				M32 -= b.M32;
				M33 -= b.M33;
				return *this;
			}

			inline matrix3 &operator*= (const T b)
			{
				M11 *= b;
				M12 *= b;
				M13 *= b;
				M21 *= b;
				M22 *= b;
				M23 *= b;
				M31 *= b;
				M32 *= b;
				M33 *= b;
				return *this;
			}

			inline bool operator== (const matrix3 &b) { return M11 == b.M11 && M12 == b.M12 && M13 == b.M13 && M21 == b.M21 && M22 == b.M22 && M23 == b.M23 && M31 == b.M31 && M32 == b.M32 && M33 == b.M33; }
			inline bool operator!= (const matrix3 &b) { return M11 != b.M11 || M12 != b.M12 || M13 != b.M13 || M21 != b.M21 || M22 != b.M22 || M23 != b.M23 || M31 != b.M31 || M32 != b.M32 || M33 != b.M33; }
		};
		template<typename T> inline vector3<T> operator* (const vector3<T> &a, const matrix3<T> &b)
		{
			return
			{
				(a.X * b.M11) + (a.Y * b.M21) + (a.Z * b.M31),
				(a.X * b.M12) + (a.Y * b.M22) + (a.Z * b.M32),
				(a.X * b.M13) + (a.Y * b.M23) + (a.Z * b.M33),
			};
		}
		template<typename T> inline matrix3<T> operator* (const T a, const matrix3<T> &b)
		{
			return
			{
				a * b.M11, a * b.M12, a * b.M13,
				a * b.M21, a * b.M22, a * b.M23,
				a * b.M31, a * b.M32, a * b.M33
			};
		}


		template<typename T> struct matrix4
		{
			T M11, M12, M13, M14, M21, M22, M23, M24, M31, M32, M33, M34, M41, M42, M43, M44;

			inline matrix4()
			{
				M11 = (T)0;
				M12 = (T)0;
				M13 = (T)0;
				M14 = (T)0;
				M21 = (T)0;
				M22 = (T)0;
				M23 = (T)0;
				M24 = (T)0;
				M31 = (T)0;
				M32 = (T)0;
				M33 = (T)0;
				M34 = (T)0;
				M41 = (T)0;
				M42 = (T)0;
				M43 = (T)0;
				M44 = (T)0;
			}

			inline matrix4(T m11, T m12, T m13, T m14, T m21, T m22, T m23, T m24, T m31, T m32, T m33, T m34, T m41, T m42, T m43, T m44)
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

			inline matrix4(const matrix3<T> &rotation, const vector3<T> &translation)
			{
				M11 = rotation.M11;
				M12 = rotation.M12;
				M13 = rotation.M13;
				M14 = translation.X;
				M21 = rotation.M21;
				M22 = rotation.M22;
				M23 = rotation.M23;
				M24 = translation.Y;
				M31 = rotation.M31;
				M32 = rotation.M32;
				M33 = rotation.M33;
				M34 = translation.Z;
				M41 = (T)0;
				M42 = (T)0;
				M43 = (T)0;
				M44 = (T)1;
			}

			static inline matrix4 Identity()
			{
				return
				{
					(T)1, (T)0, (T)0, (T)0,
					(T)0, (T)1, (T)0, (T)0,
					(T)0, (T)0, (T)1, (T)0,
					(T)0, (T)0, (T)0, (T)1
				};
			}

			inline matrix4 Transposed()
			{
				return
				{
					M11, M21, M31, M41,
					M12, M22, M32, M42,
					M13, M23, M33, M43,
					M14, M24, M34, M44
				};
			}

			inline matrix4 operator+ (const matrix4 &b)
			{
				return
				{
					M11 + b.M11, M12 + b.M12, M13 + b.M13, M14 + b.M14,
					M21 + b.M21, M22 + b.M22, M23 + b.M23, M24 + b.M24,
					M31 + b.M31, M32 + b.M32, M33 + b.M33, M34 + b.M34,
					M41 + b.M41, M42 + b.M42, M43 + b.M43, M44 + b.M44,
				};
			}
			inline matrix4 operator- (const matrix4 &b)
			{
				return
				{
					M11 - b.M11, M12 - b.M12, M13 - b.M13, M14 - b.M14,
					M21 - b.M21, M22 - b.M22, M23 - b.M23, M24 - b.M24,
					M31 - b.M31, M32 - b.M32, M33 - b.M33, M34 - b.M34,
					M41 - b.M41, M42 - b.M42, M43 - b.M43, M44 - b.M44,
				};
			}

			inline matrix4 operator* (const matrix4 &b)
			{
				return
				{
					(M11 * b.M11) + (M12 * b.M21) + (M13 * b.M31) + (M14 * b.M41),  (M11 * b.M12) + (M12 * b.M22) + (M13 * b.M32) + (M14 * b.M42),  (M11 * b.M13) + (M12 * b.M23) + (M13 * b.M33) + (M14 * b.M43),  (M11 * b.M14) + (M12 * b.M24) + (M13 * b.M34) + (M14 * b.M44),
					(M21 * b.M11) + (M22 * b.M21) + (M23 * b.M31) + (M24 * b.M41),  (M21 * b.M12) + (M22 * b.M22) + (M23 * b.M32) + (M24 * b.M42),  (M21 * b.M13) + (M22 * b.M23) + (M23 * b.M33) + (M24 * b.M43),  (M21 * b.M14) + (M22 * b.M24) + (M23 * b.M34) + (M24 * b.M44),
					(M31 * b.M11) + (M32 * b.M21) + (M33 * b.M31) + (M34 * b.M41),  (M31 * b.M12) + (M32 * b.M22) + (M33 * b.M32) + (M34 * b.M42),  (M31 * b.M13) + (M32 * b.M23) + (M33 * b.M33) + (M34 * b.M43),  (M31 * b.M14) + (M32 * b.M24) + (M33 * b.M34) + (M34 * b.M44),
					(M41 * b.M11) + (M42 * b.M21) + (M43 * b.M31) + (M44 * b.M41),  (M41 * b.M12) + (M42 * b.M22) + (M43 * b.M32) + (M44 * b.M42),  (M41 * b.M13) + (M42 * b.M23) + (M43 * b.M33) + (M44 * b.M43),  (M41 * b.M14) + (M42 * b.M24) + (M43 * b.M34) + (M44 * b.M44)
				};
			}

			inline matrix4 operator* (const T b)
			{
				return
				{
					M11 * b, M12 * b, M13 * b, M14 * b,
					M21 * b, M22 * b, M23 * b, M24 * b,
					M31 * b, M32 * b, M33 * b, M34 * b,
					M41 * b, M42 * b, M43 * b, M44 * b,
				};
			}

			inline vector4<T> operator* (const vector4<T> &b)
			{
				return
				{
					(M11 * b.X) + (M12 * b.Y) + (M13 * b.Z) + (M14 * b.W),
					(M21 * b.X) + (M22 * b.Y) + (M23 * b.Z) + (M24 * b.W),
					(M31 * b.X) + (M32 * b.Y) + (M33 * b.Z) + (M34 * b.W),
					(M41 * b.X) + (M42 * b.Y) + (M43 * b.Z) + (M44 * b.W),
				};
			}

			inline matrix4 &operator+= (const matrix4 &b)
			{
				M11 += b.M11;
				M12 += b.M12;
				M13 += b.M13;
				M14 += b.M14;
				M21 += b.M21;
				M22 += b.M22;
				M23 += b.M23;
				M24 += b.M24;
				M31 += b.M31;
				M32 += b.M32;
				M33 += b.M33;
				M34 += b.M34;
				M41 += b.M41;
				M42 += b.M42;
				M43 += b.M43;
				M44 += b.M44;
				return *this;
			}
			inline matrix4 &operator-= (const matrix4 &b)
			{
				M11 -= b.M11;
				M12 -= b.M12;
				M13 -= b.M13;
				M14 -= b.M14;
				M21 -= b.M21;
				M22 -= b.M22;
				M23 -= b.M23;
				M24 -= b.M24;
				M31 -= b.M31;
				M32 -= b.M32;
				M33 -= b.M33;
				M34 -= b.M34;
				M41 -= b.M41;
				M42 -= b.M42;
				M43 -= b.M43;
				M44 -= b.M44;
				return *this;
			}
			inline matrix4 &operator*= (const T b)
			{
				M11 *= b;
				M12 *= b;
				M13 *= b;
				M14 *= b;
				M21 *= b;
				M22 *= b;
				M23 *= b;
				M24 *= b;
				M31 *= b;
				M32 *= b;
				M33 *= b;
				M34 *= b;
				M41 *= b;
				M42 *= b;
				M43 *= b;
				M44 *= b;
				return *this;
			}
		};
		template<typename T> inline vector4<T> operator* (const vector4<T> &a, const matrix4<T> &b)
		{
			return
			{
				(a.X * b.M11) + (a.Y * b.M21) + (a.Z * b.M31) + (a.W * b.M41),
				(a.X * b.M12) + (a.Y * b.M22) + (a.Z * b.M32) + (a.W * b.M42),
				(a.X * b.M13) + (a.Y * b.M23) + (a.Z * b.M33) + (a.W * b.M43),
				(a.X * b.M14) + (a.Y * b.M24) + (a.Z * b.M34) + (a.W * b.M44),
			};
		}
		template<typename T> inline matrix4<T> operator* (const T a, const matrix4<T> &b)
		{
			return
			{
				a * b.M11, a * b.M12, a * b.M13, a * b.M14,
				a * b.M21, a * b.M22, a * b.M23, a * b.M24,
				a * b.M31, a * b.M32, a * b.M33, a * b.M34,
				a * b.M41, a * b.M42, a * b.M43, a * b.M44,
			};
		}


		template<typename T> struct quaternion
		{
			T W, X, Y, Z;

			inline quaternion()
			{
				W = (T)0;
				X = (T)0;
				Y = (T)0;
				Z = (T)0;
			}
			inline quaternion(T w, T x, T y, T z)
			{
				W = w;
				X = x;
				Y = y;
				Z = z;
			}
			inline quaternion(vector3<T> vec)
			{
				T halfAngle = vec.Magnitude() / (T)2;
				vec = vec.Normalized();

				T cosAngle = std::cos(halfAngle);
				T sinAngle = std::sin(halfAngle);

				W = cosAngle;
				X = sinAngle * vec.X;
				Y = sinAngle * vec.Y;
				Z = sinAngle * vec.Z;
			}
			inline quaternion(T angle, vector3<T> axis)
			{
				T halfAngle = angle / (T)2;
				T cosAngle = std::cos(halfAngle);
				T sinAngle = std::sin(halfAngle);

				W = cosAngle;
				X = sinAngle * axis.X;
				Y = sinAngle * axis.Y;
				Z = sinAngle * axis.Z;
			}
			inline quaternion Identity() { return { (T)1, (T)0, (T)0, (T)0 }; }
			inline T SqrMagnitude() { return (W * W) + (X * X) + (Y * Y) + (Z * Z); }
			inline T Magnitude() { return std::sqrt(SqrMagnitude()); }
			inline quaternion Normalised()
			{
				T mag = Magnitude();
				return
				{
					W / mag,
					X / mag,
					Y / mag,
					Z / mag
				};
			}

			inline quaternion Inverse() { return { W, -X, -Y, -Z }; }

			inline matrix3<T> ToMatrix()
			{
				return
				{
					(T)1 - ((T)2 * Y * Y) - ((T)2 * Z * Z), ((T)2 * X * Y) - ((T)2 * W * Z), ((T)2 * X * Z) + ((T)2 * W * Y),
					((T)2 * X * Y) + ((T)2 * W * Z), (T)1 - ((T)2 * X * X) - ((T)2 * Z * Z), ((T)2 * Y * Z) + ((T)2 * W * X),
					((T)2 * X * Z) - ((T)2 * W * Y), ((T)2 * Y * Z) - ((T)2 * W * X), (T)1 - ((T)2 * X * X) - ((T)2 * Y * Y)
				};
			}

			inline quaternion operator* (const quaternion &b)
			{
				return
				{
					(W * b.W) - (X * b.X) - (Y * b.Y) - (Z * b.Z),
					(W * b.X) + (X * b.W) + (Y * b.Z) - (Z * b.Y),
					(W * b.Y) - (X * b.Z) + (Y * b.W) + (Z * b.X),
					(W * b.Z) + (X * b.Y) - (Y * b.X) + (Z * b.W)
				};
			}
		};
		template<typename T> inline vector3<T> operator *(const vector3<T> &a, const quaternion<T> &b)
		{
			return b.ToMatrix() * a;
		}
	}
}
