#include <iostream>
#include <tuple>
#include <utility>
#include <functional>
#include <algorithm>
#include <type_traits>
#include <array>
#include <cmath>
#include <fstream>
#include <string>

#ifndef __cpp_lib_concepts
#define __cpp_lib_concepts
#include <concepts>
#endif

#include "gcem.hpp"



namespace std
{
	using int_t = int;
	using float_t = float;
	using double_t = double;
	using long_double_t = long double;
	using signed_t = signed;
	using unsigned_t = unsigned;
	using long_t = long;
	using long_long_t = long long;
	using short_t = short;

	constexpr auto pi_t = 3.14159265f;
	constexpr auto half_pi_t = pi_t / 2.0f;
	constexpr auto two_pi_t = pi_t * 2.0f;

	template<typename V, typename... Ts>
	constexpr auto make_array(Ts&&... t) noexcept -> std::array<V, sizeof...(Ts)>
	{
		return { { std::forward<Ts>(t)... } };
	}

	template<class... Formats, std::size_t N, std::size_t... Is, typename T>
	constexpr std::tuple<Formats...> as_tuple(const std::array<T, N>& arr, std::index_sequence<Is...>) noexcept
	{
		return std::make_tuple(Formats{ arr[Is] }...);
	}

	template<class... Formats, typename T, std::size_t N>
	constexpr std::tuple<Formats...> as_tuple(const std::array<T, N>& arr) noexcept requires (N == sizeof...(Formats))
	{
		return as_tuple<Formats...>(arr, std::make_index_sequence<N>{});
	}

	template<typename tuple_t>
	constexpr auto as_array(tuple_t&& tuple) noexcept
	{
		constexpr auto get_array = [](auto&& ... x) { return std::array{ std::forward<decltype(x)>(x) ... }; };
		return std::apply(get_array, std::forward<tuple_t>(tuple));
	}

	template<typename x_Tuple, std::size_t... x_index>
	constexpr auto make_tuple_helper(x_Tuple const& other, std::index_sequence<x_index...>)
	{
		return std::make_tuple(std::get<x_index>(other)...);
	}

	template<typename... x_Field>
	constexpr auto flatten(std::tuple<x_Field...> const& other)
	{
		return std::make_tuple_helper(other, std::make_index_sequence < sizeof...(x_Field) - std::size_t{ 1 } > {});
	}

	template<>
	constexpr auto flatten(std::tuple<> const& other)
	{
		return other;
	}

	auto count_lines(std::string&& filepath) noexcept
	{
		std::size_t numLines = static_cast<std::size_t>(0);
		std::ifstream inFile(std::move(filepath));

		std::string lineStr;

		while (std::getline(inFile, lineStr))
		{
			++numLines;
		}

		return numLines;
	}
	

	template <typename T, typename ...Ts>
	concept are_same = std::conjunction_v<std::is_same<T, Ts>...>;
}

namespace core
{
	//define the matrix alias to a c++ array
	template<std::size_t N, typename T>
	using matrix = std::array<std::array<T, N>, N>;

	//define matrix type aliases
	using int2x2 = matrix<2, std::int_t>;
	using int3x3 = matrix<3, std::int_t>;
	using int4x4 = matrix<4, std::int_t>;

	using float2x2 = matrix<2, std::float_t>;
	using float3x3 = matrix<3, std::float_t>;
	using float4x4 = matrix<4, std::float_t>;

	using double2x2 = matrix<2, std::double_t>;
	using double3x3 = matrix<3, std::double_t>;
	using double4x4 = matrix<4, std::double_t>;

	//define vector type aliases
	using int2 = std::tuple<std::int_t, std::int_t>;
	using int3 = std::tuple<std::int_t, std::int_t, std::int_t>;
	using int4 = std::tuple<std::int_t, std::int_t, std::int_t, std::int_t>;

	using float2 = std::tuple<std::float_t, std::float_t>;
	using float3 = std::tuple<std::float_t, std::float_t, std::float_t>;
	using float4 = std::tuple<std::float_t, std::float_t, std::float_t, std::float_t>;

	using double2 = std::tuple<std::double_t, std::double_t>;
	using double3 = std::tuple<std::double_t, std::double_t, std::double_t>;
	using double4 = std::tuple<std::double_t, std::double_t, std::double_t, std::double_t>;
	
	//define primitive type aliases
	template<typename T>
	struct Triangle
	{
		T p1, p2, p3;
	};

	template<typename T>
	struct Quad
	{
		T p1, p2, p3, p4;
	};

	//define mesh type aliases
	template<std::size_t S>
	using mesh3 = std::array<Triangle, S>;

	template<std::size_t S>
	using mesh4 = std::array<Quad, S>;
	

	enum class mod
	{
		add, remove
	};

	namespace trig
	{
		template<typename T = std::float_t>
		constexpr auto radians(T&& degrees) noexcept requires std::floating_point<T>
		{
			return (std::move(degrees) / (T)57.295779513082320876798154814105);
		}

		template<typename T = std::float_t>
		constexpr auto radians(const T& degrees) noexcept requires std::floating_point<T>
		{
			return (degrees / (T)57.295779513082320876798154814105);
		}

		template<typename T = std::float_t>
		constexpr auto degrees(T&& radians) noexcept requires std::floating_point<T>
		{
			return (std::move(radians) * (T)57.295779513082320876798154814105);
		}

		template<typename T = std::float_t>
		constexpr auto degrees(const T& radians) noexcept requires std::floating_point<T>
		{
			return (radians * (T)57.295779513082320876798154814105);
		}
	}

	namespace vec
	{
		template<mod op, typename T = std::int_t, typename... Ts>
		constexpr auto modify(const std::tuple<Ts...>& vec, T&& val = 0) noexcept requires std::are_same<T, Ts...>
		{
			if constexpr (op == mod::add)
			{
				return std::tuple_cat(vec, std::make_tuple(val));
			}

			if constexpr (op == mod::remove)
			{
				return std::flatten(vec);
			}
		}
		
		template<typename binaryop = std::multiplies<void>, typename T, typename... Ts>
		constexpr auto apply(const std::tuple<Ts...>& vec, T&& operand) noexcept
		{
			std::tuple<Ts...> outVec;
			
			[&]<std::size_t... i>(std::index_sequence<i...>)
			{
				((std::get<i>(outVec) = binaryop{}(std::get<i>(vec), operand)) , ...);
			}
			(std::make_index_sequence<sizeof...(Ts)>{});
			
			return outVec;
		}

		template<typename binaryop = std::multiplies<void>, typename... Ts>
		constexpr auto apply(const std::tuple<Ts...>& vec1, const std::tuple<Ts...>& vec2) noexcept
		{
			std::tuple<Ts...> outVec;

			[&] <std::size_t... i>(std::index_sequence<i...>)
			{
				((std::get<i>(outVec) = binaryop{}(std::get<i>(vec1), std::get<i>(vec2))) , ...);
			}
			(std::make_index_sequence<sizeof...(Ts)>{});

			return outVec;
		}

		template<typename... Ts>
		constexpr auto sum(const std::tuple<Ts...>& vec) noexcept
		{
			return std::apply([](auto&&... v) { return (v + ...); }, vec);
		}

		template<typename... Ts>
		constexpr auto lengthSquared(const std::tuple<Ts...>& vec) noexcept
		{
			return sum(vec::apply<std::multiplies<>>(vec, vec));
		}

		template<typename... Ts>
		constexpr auto length(const std::tuple<Ts...>& vec) noexcept
		{
			return gcem::sqrt(lengthSquared(vec));
		}

		template<typename... Ts>
		constexpr auto normalize(const std::tuple<Ts...>& vec) noexcept
		{
			return vec::apply<std::divides<>>(vec, (length(vec)));
		}

		template<bool Normalize = false, typename... Ts>
		constexpr auto dot(const std::tuple<Ts...>& vec1, const std::tuple<Ts...>& vec2) noexcept
		{
			if constexpr (Normalize) return sum(std::tuple<Ts...>{ vec::apply<std::multiplies<>>(normalize(vec1), normalize(vec2)) });
			else return sum(std::tuple<Ts...>{ vec::apply<std::multiplies<>>(vec1, vec2) });
		}

		template<typename... Ts>
		constexpr auto cross(const std::tuple<Ts...>& vec1, const std::tuple<Ts...>& vec2) noexcept requires (sizeof...(Ts) == 3)
		{
			return std::make_tuple
			(
				((std::get<1>(vec1) * std::get<2>(vec2)) - (std::get<2>(vec1) * std::get<1>(vec2))),
				((std::get<2>(vec1) * std::get<0>(vec2)) - (std::get<0>(vec1) * std::get<2>(vec2))),
				((std::get<0>(vec1) * std::get<1>(vec2)) - (std::get<1>(vec1) * std::get<0>(vec2)))
			);
		}

		template<typename T, std::size_t S, typename... Ts>
		constexpr auto mul(const std::tuple<Ts...>& vec, const matrix<S, T>& mat) noexcept requires (S == sizeof...(Ts))
		{
			std::array<T, S> outVec;

			std::array<T, S> inVec = std::as_array(vec);

			for (auto i = 0; i < S; ++i)
			{
				outVec[i] = 0;

				for (auto j = 0; j < S; ++j)
				{
					outVec[i] += mat[i][j] * inVec[j];
				}
			}

			return std::as_tuple<Ts...>(outVec);
		}
	}

	namespace mat
	{
		template<typename binaryop = std::multiplies<void>, typename T, std::size_t S>
		constexpr auto apply(const matrix<S, T>& mat, T&& operand)
		{
			matrix<S, T> outMat;

			for (auto i = 0; i < S; ++i)
			{
				for (auto j = 0; j < S; ++j)
				{
					outMat[i][j] = binaryop{}(mat[i][j], operand);
				}
			}

			return outMat;
		}

		template<typename binaryop = std::multiplies<void>, typename T, std::size_t S>
		constexpr auto apply(const matrix<S, T>& mat1, const matrix<S, T>& mat2)
		{
			matrix<S, T> outMat;

			for (auto i = 0; i < S; ++i)
			{
				for (auto j = 0; j < S; ++j)
				{
					outMat[i][j] = binaryop{}(mat1[i][j], mat2[i][j]);
				}
			}

			return outMat;
		}

		template<typename T, std::size_t S>
		constexpr auto compose(const matrix<S, T>& m1, const matrix<S, T>& m2) noexcept
		{
			matrix<S, T> mat = {};

			for (auto i = 0; i < S; i++)
			{
				for (auto j = 0; j < S; j++)
				{
					T sum = static_cast<T>(0);

					for (auto k = 0; k < S; k++)
					{
						sum += m1[i][k] * m2[k][j];
					}

					mat[i][j] = sum;
				}
			}

			return mat;
		}

		template<std::size_t S, typename T>
		constexpr auto identity() noexcept
		{
			matrix<S, T> mat = {};

			for (auto i = 0; i < S; ++i)
			{
				mat[i][i] = static_cast<T>(1);
			}

			return mat;
		}

		template<typename T>
		constexpr auto rotationX(const T& angle) noexcept
		{
			return matrix<4, T>
			{
				(T)1.0L, (T)0.0L, (T)0.0L, (T)0.0L,
				(T)0.0L, gcem::cos(angle), -gcem::sin(angle), (T)0.0L,
				(T)0.0L, gcem::sin(angle), gcem::cos(angle), (T)0.0L,
				(T)0.0L, (T)0.0L, (T)0.0L, (T)1.0L
			};
		}

		template<typename T>
		constexpr auto rotationY(const T& angle) noexcept
		{
			return matrix<4, T>
			{
				gcem::cos(angle), (T)0.0L, gcem::sin(angle), (T)0.0L,
				(T)0.0L, (T)1.0L, (T)0.0L, (T)0.0L,
				-gcem::sin(angle), (T)0.0L, gcem::cos(angle), (T)0.0L,
				(T)0.0L, (T)0.0L, (T)0.0L, (T)1.0L
			};
		}

		template<typename T>
		constexpr auto rotationZ(const T& angle) noexcept
		{
			return matrix<4, T>
			{
				gcem::cos(angle), gcem::sin(angle), (T)0.0L, (T)0.0L,
				gcem::sin(angle), gcem::cos(angle), (T)0.0L, (T)0.0L,
				(T)0.0L, (T)0.0L, (T)1.0L, (T)0.0L,
				(T)0.0L, (T)0.0L, (T)0.0L, (T)1.0L
			};
		}

		template<typename T>
		constexpr auto rotationXYZ(const T& x, const T& y, const T& z) noexcept
		{
			return mat::compose(mat::rotationX(x), mat::compose(mat::rotationY(y), mat::rotationZ(y)));
		}

		template<typename T>
		constexpr auto translation(T&& x, T&& y, T&& z) noexcept
		{
			return matrix<4, T>
			{
				(T)1.0L, (T)0.0L, (T)0.0L, (T)0.0L,
				(T)0.0L, (T)1.0L, (T)0.0L, (T)0.0L,
				(T)0.0L, (T)0.0L, (T)1.0L, (T)0.0L,
				x, y, z, (T)1.0L
			};
		}

		template<typename T>
		constexpr auto projection(T&& fov, T&& aspectRatio, T&& near, T&& far) noexcept requires std::floating_point<T>
		{
			T fovRad = static_cast<T>(1) / gcem::tan(trig::radians(fov * static_cast<T>(0.5L)));

			return matrix<4, T>
			{
				(aspectRatio * fovRad), (T)0.0L, (T)0.0L, (T)0.0L,
				(T)0.0L, fovRad, (T)0.0L, (T)0.0L,
				(T)0.0L, (T)0.0L, (far / (far - near)), (T)1.0L,
				(T)0.0L, (T)0.0L, ((-far * near) / (far - near)), (T)0.0L
			};
		}

		template<typename T, typename... Ts>
		constexpr auto pointAt(const std::tuple<Ts...>& pos, const std::tuple<Ts...>& target, const std::tuple<Ts...>& up) noexcept requires (sizeof...(Ts) == 3) or (sizeof...(Ts) == 4)
		{
			auto newForward = vec::normalize(vec::apply<std::minus<>>(target, pos));

			auto newUp = vec::normalize(vec::apply<std::minus<>>(up, vec::apply<std::multiplies<>>(newForward, vec::dot(up, newForward))));

			auto newRight = vec::cross(newUp, newForward);

			return matrix<4, T>
			{
				std::get<0>(newRight),   std::get<1>(newRight),   std::get<2>(newRight),   0.0f,
				std::get<0>(newUp),      std::get<1>(newUp),      std::get<2>(newUp),      0.0f,
				std::get<0>(newForward), std::get<1>(newForward), std::get<2>(newForward), 0.0f,
				std::get<0>(pos),        std::get<1>(pos),        std::get<2>(pos),        1.0f
			};
		}
	}

	namespace mesh
	{
		
		
		template<typename T>
		constexpr auto make_mesh3(std::string&& filepath) noexcept
		{
			std::ifstream f(std::move(filepath));
			

			if (!f.is_open()) 
		}
	}
}

template<typename T, std::size_t S>
std::ostream& operator<<(std::ostream& stream, const core::matrix<S, T>& mat) noexcept
{
	stream << "[ ";

	for (auto i = 0; i < S; ++i)
	{
		for (auto j = 0; j < S; ++j)
		{
			stream << mat[i][j];

			if (j != (S - 1)) stream << ' ';
		}

		stream << " ]" << '\n';

		if (i != (S - 1)) stream << "[ ";
	}

	return stream;
}

template<typename... Ts>
std::ostream& operator<<(std::ostream& stream, const std::tuple<Ts...>& vec) noexcept
{
	if constexpr (bool(sizeof...(Ts)))
	{
		stream << "[ ";

		[&] <std::size_t... i>(std::index_sequence<i...>)
		{
			((stream << std::get<i>(vec) << ", "), ...);
		}
		(std::make_index_sequence<(sizeof...(Ts) - 1)>{});

		stream << std::get<(sizeof...(Ts) - 1)>(vec) << " ]";

		return stream;
	}

	else
	{
		stream << "[ ]";

		return stream;
	}
}


int main()
{
	using namespace core;
	using enum mod;

	//constexpr auto x = std::pi_t;
	//constexpr auto y = std::half_pi_t;
	//constexpr auto z = 0.0f;
	//
	//
	//constexpr float4x4 a = mat::rotationXYZ(x, y, z);
	//
	//std::cout << a << std::endl;

	constexpr int4 a = std::make_tuple(4, 5, 6, 1);

	std::cout << a << std::endl;

	constexpr auto b = vec::modify<remove>(a);

	std::cout << b << std::endl;

	constexpr auto c = vec::modify<remove>(b);

	std::cout << c << std::endl;

	constexpr auto d = vec::modify<remove>(c);
	
	std::cout << d << std::endl;

	constexpr auto e = vec::modify<remove>(d);

	std::cout << e << std::endl;

	return 0;
}