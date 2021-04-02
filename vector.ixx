module;
#include <tuple>
#include <type_traits>
#include <functional>
#include <concepts>
export module vector;

import stdex;
import core;

export namespace cla
{
	template<typename binaryop = std::multiplies<void>, typename T>
	constexpr auto apply(const cla::v3d_generic<T>& vec, const T& operand) noexcept
	{
		cla::vf3d outVec;

		outVec.x = binaryop{}(vec.x, operand);
		outVec.y = binaryop{}(vec.y, operand);
		outVec.z = binaryop{}(vec.z, operand);

		return outVec;
	}

	template<typename binaryop = std::multiplies<void>, typename T>
	constexpr auto reduce(const cla::v3d_generic<T>& vec1, const cla::v3d_generic<T>& vec2) noexcept
	{
		cla::vf3d outVec;

		outVec.x = binaryop{}(vec1.x, vec2.x);
		outVec.y = binaryop{}(vec1.y, vec2.y);
		outVec.z = binaryop{}(vec1.z, vec2.z);

		return outVec;
	}

	template<typename T>
	constexpr auto sum(const cla::v3d_generic<T>& vec) noexcept
	{
		return (vec.x + vec.y + vec.z);
	}

	template<typename T>
	constexpr auto length2(const cla::v3d_generic<T>& vec) noexcept
	{
		return cla::sum(cla::reduce<std::multiplies<>>(vec, vec));
	}

	template<typename T>
	constexpr auto length(const cla::v3d_generic<T>& vec) noexcept
	{
		return std::my_sqrt(cla::length2(vec));
	}

	template<typename T>
	constexpr auto normalize(const cla::v3d_generic<T>& vec) noexcept
	{
		return cla::apply<std::divides<>>(vec, (cla::length(vec)));
	}

	template<bool Normalize = false, typename T>
	constexpr auto dot(const cla::v3d_generic<T>& vec1, const cla::v3d_generic<T>& vec2) noexcept
	{
		if constexpr (Normalize) return cla::sum(cla::reduce<std::multiplies<>>(cla::normalize(vec1), cla::normalize(vec2)));
		else return cla::sum(cla::reduce<std::multiplies<>>(vec1, vec2));
	}

	template<typename T>
	constexpr auto cross(const cla::v3d_generic<T>& vec1, const cla::v3d_generic<T>& vec2) noexcept
	{
		cla::v3d_generic<T> outVec;

		outVec.x = ((vec1.y * vec2.z) - (vec1.z * vec2.y));
		outVec.y = ((vec1.z * vec2.x) - (vec1.x * vec2.z));
		outVec.z = ((vec1.x * vec2.y) - (vec1.y * vec2.x));
		outVec.w = vec1.w;

		return outVec;
	}

	template<typename T, std::size_t S>
	constexpr auto mul(const cla::v3d_generic<T>& vec, const cla::matrix<T, S>& mat) noexcept
	{
		cla::v3d_generic<T> outVec;

		outVec.x = vec.x * mat[0][0] + vec.y * mat[1][0] + vec.z * mat[2][0] + mat[3][0];
		outVec.y = vec.x * mat[0][1] + vec.y * mat[1][1] + vec.z * mat[2][1] + mat[3][1];
		outVec.z = vec.x * mat[0][2] + vec.y * mat[1][2] + vec.z * mat[2][2] + mat[3][2];
		outVec.w = vec.x * mat[0][3] + vec.y * mat[1][3] + vec.z * mat[2][3] + mat[3][3];

		return outVec;
	}
}