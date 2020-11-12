module;
#include <tuple>
#include <array>
export module core;

export namespace cla
{
	//define the matrix alias to a c++ array of arrays
	template<typename T, unsigned int N>
	using matrix = std::array<std::array<T, N>, N>;

	//define matrix type aliases
	using int2x2 = matrix<int, 2>;
	using int3x3 = matrix<int, 3>;
	using int4x4 = matrix<int, 4>;

	using float2x2 = matrix<float, 2>;
	using float3x3 = matrix<float, 3>;
	using float4x4 = matrix<float, 4>;

	using double2x2 = matrix<double, 2>;
	using double3x3 = matrix<double, 3>;
	using double4x4 = matrix<double, 4>;

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