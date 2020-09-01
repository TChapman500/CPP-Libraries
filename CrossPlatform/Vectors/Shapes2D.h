#pragma once
#include "Vectors.h"

namespace TChapman500
{
	namespace Shapes2D
	{
		template<typename T> union line
		{
			Vectors::vector3<T> Vector;
			struct
			{
				Vectors::vector2<T> Normal;
				T Distance;
			};
			struct
			{
				T X;
				T Y;
				T Z;
			};

			inline line()
			{
				Normal = Vectors::vector2<T>();
				Distance = (T)0;
			}

			inline line(T normalX, T normalY, T distance)
			{
				Normal = Vectors::vector2<T>(normalX, normalY).Normalized();
				Distance = distance;
			}

			inline line(Vectors::vector2<T> normal, T distance)
			{
				Normal = normal;
				Distance = distance;
			}

			inline line(Vectors::vector2<T> pointA, Vectors::vector2<T> pointB, bool pointBIsNormal)
			{
				if (pointBIsNormal)
				{
					Normal = pointB.Normalized();
					Distance = line(pointB, (T)0).DistanceToPoint(pointA);
				}
				else
				{
					Normal = (pointB - pointA).Normalized();
					Normal = Vectors::vector2<T>(-Normal.Y, Normal.X);
					Distance = line(Normal, (T)0).DistanceToPoint(pointA);
				}
			}

			inline T DistanceToPoint(Vectors::vector2<T> point) { return Vectors::vector2<T>::Dot(Normal, point) - Distance; }

			static inline Vectors::vector2<T> Intersection(const line &a, const line &b)
			{
				Vectors::vector2<T> result;

				// Solve for X assuming that the lines are not parallel along the axis (initiate self-destruct if they are)
				result.X = ((b.Y * a.Z) - (a.Y * b.Z)) / ((a.X * b.Y) - (a.Y * b.X));

				// Solve for Y assuming we survived solving for X
				if (a.Y == 0.0) result.Y = -((b.X * result.X) - b.Z) / b.Y;
				else result.Y = -((a.X * result.X) - a.Z) / a.Y;

				return result;
			}
		};

		template<typename T> struct circle
		{
			Vectors::vector2<T> Position;
			T Radius;

			inline circle()
			{
				Position = Vectors::vector2<T>();
				Radius = (T)0;
			}

			inline circle(Vectors::vector2<T> pos, T radius)
			{
				Position = pos;
				Radius = radius;
			}

			inline T DistanceToPoint(Vectors::vector2<T> &point)
			{
				return Vectors::vector2<T>::Distance(point, Position) - Radius;
			}

			static inline Vectors::vector2<T> Intersection(const circle<T> &a, const line<T> &b, bool min)
			{
				Vectors::vector2<T> result;
				
				// NOTE:  aRes = b.X^2 + b.Y^2 = 1

				T bRes, cRes;

				if (b.Y == (T)0)
				{
					bRes = -((T)2 * b.Y * b.Z) + ((T)2 * b.X * b.Y * a.Position.X) - ((T)2 * (b.X * b.X) * a.Position.Y);
					cRes = (b.Z * b.Z) - ((T)2 * b.X * b.Z * a.Position.X) - ((b.X * b.X) * ((a.Radius * a.Radius) - (a.Position.X * a.Position.X) - (a.Position.Y * a.Position.Y)));
				}
				else
				{
					bRes = -((T)2 * b.X * b.Z) + ((T)2 * b.X * b.Y * a.Position.Y) - ((T)2 * (b.Y * b.Y) * a.Position.X);
					cRes = (b.Z * b.Z) - ((T)2 * b.Y * b.Z * a.Position.Y) - ((b.Y * b.Y) * ((a.Radius * a.Radius) - (a.Position.X * a.Position.X) - (a.Position.Y * a.Position.Y)));
				}
				T det = (bRes * bRes) - ((T)4 * cRes);

				if (det < (T)0) result = { (T)INFINITY, (T)INFINITY };
				else
				{
					if (b.Y == (T)0)
					{
						if (min) result.Y = -(bRes + std::sqrt(det)) / (T)2;
						else result.Y = -(bRes - std::sqrt(det)) / (T)2;
						result.X = -((b.Y * result.Y) - b.Z) / b.X;
					}
					else
					{
						if (min) result.X = -(bRes + std::sqrt(det)) / (T)2;
						else result.X = -(bRes - std::sqrt(det)) / (T)2;
						result.Y = -((b.X * result.X) - b.Z) / b.Y;
					}
				}

				return result;
			}

			static inline Vectors::vector2<T> Intersection(const circle &a, const circle &b, bool min)
			{
				Vectors::vector2<T> result;

				return result;
			}

			static inline T Penetration(const circle &a, const circle &b)
			{
				T dist = (a.Radius + b.Radius) - Vectors::vector2<T>::Distance(a.Position, b.Position);
			}

			static inline Vectors::vector2<T> PenetrationNormal(const circle &a, const circle &b)
			{
				return (a.Position - b.Position).Normalized();
			}
		};

		template<typename T> struct polygon
		{
			int VertexCount;
			Vectors::vector2<T> *VertexList;
			line<T> *LineList;

			inline polygon()
			{
				VertexCount = 0;
				VertexList = nullptr;
				LineList = nullptr;
			}

			inline polygon(Vectors::vector2<T> *vertexList, int vertexCount)
			{
				VertexCount = vertexCount;
				VertexList = new Vectors::vector2<T>[VertexCount];
				LineList = new line<T>[VertexCount];

				for (int i = 0; i < VertexCount; i++) VertexList[i] = vertexList[i];
				for (int i = 0; i < VertexCount; i++)
				{
					if (i == VertexCount - 1) LineList[i] = line<T>(VertexList[i], VertexList[0], false);
					else LineList[i] = line<T>(VertexList[i], VertexList[i + 1], false);
				}
			}
			
			inline ~polygon()
			{
				delete[] VertexList;
				delete[] LineList;
			}

			inline bool TestPoint(Vectors::vector2<T> point)
			{
				bool result = false;
				int negativeResults = 0;
				for (int i = 0; i < VertexCount; i++)
				{
					if (LineList[i].DistanceToPoint(point) < (T)0) negativeResults++;
				}
				if (negativeResults == VertexCount) result = true;
				return result;
			}
		};
	}
}
