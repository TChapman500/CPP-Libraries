// Dependent structure
union vector4
{
	struct { double X, Y, Z, W; };
	__m256d Vector;
};

// Structure Layout
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
};

// Function to optimize
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
	result.Matrix[3] = { dot1.m256d_f64[0], dot2.m256d_f64[0], dot3.m256d_f64[0], dot4.m256d_f64[0] };
	return result;
}
