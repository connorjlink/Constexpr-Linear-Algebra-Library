module;
#include <tuple>
#include <vector>
#include <cstddef>
export module core;

export namespace core
{
	//define the matrix alias to a c++ array of arrays
	template<std::size_t N, typename T>
	using matrix = std::array<std::array<T, N>, N>;

	//define matrix type aliases
	using int2x2 = matrix<2, int>;
	using int3x3 = matrix<3, int>;
	using int4x4 = matrix<4, int>;

	using float2x2 = matrix<2, float>;
	using float3x3 = matrix<3, float>;
	using float4x4 = matrix<4, float>;

	using double2x2 = matrix<2, double>;
	using double3x3 = matrix<3, double>;
	using double4x4 = matrix<4, double>;

	//define vector type aliases
	using int2 = std::tuple<int, int>;
	using int3 = std::tuple<int, int, int>;
	using int4 = std::tuple<int, int, int, int>;

	using float2 = std::tuple<float, float>;
	using float3 = std::tuple<float, float, float>;
	using float4 = std::tuple<float, float, float, float>;

	using double2 = std::tuple<double, double>;
	using double3 = std::tuple<double, double, double>;
	using double4 = std::tuple<double, double, double, double>;
}