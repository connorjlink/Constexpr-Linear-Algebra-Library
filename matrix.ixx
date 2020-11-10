module;
#include <algorithm>
#include <functional>
#include <array>
#include <tuple>
#include "gcem.hpp"
export module matrix;

import core;
import trig;
import vector;

export namespace cla::mat
{
	template<typename binaryop = std::multiplies<void>, typename T, std::size_t S>
	constexpr auto apply(const std::array<std::array<T, S>, S>& mat1, const T& operand)
	{
		cla::core::matrix<T, S> mat;

		for (auto i = 0; i < S; ++i)
		{
			for (auto j = 0; j < S; ++j)
			{
				mat[i][j] = binaryop{}(mat1[i][j], operand);
			}
		}

		return mat;
	}

	template<typename binaryop = std::multiplies<void>, typename T, std::size_t S>
	constexpr auto apply(const std::array<std::array<T, S>, S>& mat1, const std::array<std::array<T, S>, S>& mat2)
	{
		std::array<std::array<T, S>, S> mat;

		for (auto i = 0; i < S; ++i)
		{
			for (auto j = 0; j < S; ++j)
			{
				mat[i][j] = binaryop{}(mat1[i][j], mat2[i][j]);
			}
		}

		return mat;
	}

	template<typename T, std::size_t S>
	constexpr auto compose(const std::array<std::array<T, S>, S>& mat1, const std::array<std::array<T, S>, S>& mat2) noexcept
	{
		std::array<std::array<T, S>, S> mat;

		for (auto i = 0; i < S; i++)
		{
			for (auto j = 0; j < S; j++)
			{
				T sum = static_cast<T>(0);

				for (auto k = 0; k < S; k++)
				{
					sum += mat1[i][k] * mat2[k][j];
				}

				mat[i][j] = sum;
			}
		}

		return mat;
	}

	template<std::size_t S, typename T>
	constexpr auto identity() noexcept
	{
		std::array<std::array<T, S>, S> mat;

		for (auto i = 0; i < S; ++i)
		{
			mat[i][i] = static_cast<T>(1);
		}

		return mat;
	}

	template<typename T>
	constexpr auto rotationX(const T& angle) noexcept
	{
		return std::array<std::array<T, 4>, 4>
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
		return std::array<std::array<T, 4>, 4>
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
		return std::array<std::array<T, 4>, 4>
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
		return std::array<std::array<T, 4>, 4>
		{
			(T)1.0L, (T)0.0L, (T)0.0L, (T)0.0L,
			(T)0.0L, (T)1.0L, (T)0.0L, (T)0.0L,
			(T)0.0L, (T)0.0L, (T)1.0L, (T)0.0L,
			x, y, z, (T)1.0L
		};
	}

	template<typename T>
	constexpr auto projection(const T& fov, const T& aspectRatio, const T& near, const T& far) noexcept
	{
		T fovRad = static_cast<T>(1) / gcem::tan(cla::trig::radians(fov * static_cast<T>(0.5L)));

		return std::array<std::array<T, 4>, 4>
		{
			(aspectRatio* fovRad), (T)0.0L, (T)0.0L, (T)0.0L,
			(T)0.0L, fovRad, (T)0.0L, (T)0.0L,
			(T)0.0L, (T)0.0L, (far / (far - near)), (T)1.0L,
			(T)0.0L, (T)0.0L, ((-far * near) / (far - near)), (T)0.0L
		};
	}

	template<typename T, typename... Ts>
	constexpr auto pointAt(const std::tuple<Ts...>& pos, const std::tuple<Ts...>& target, const std::tuple<Ts...>& up) noexcept requires (sizeof...(Ts) == 3) or (sizeof...(Ts) == 4)
	{
		auto newForward = cla::vec::normalize(cla::vec::apply<std::minus<>>(target, pos));

		auto newUp = cla::vec::normalize(cla::vec::apply<std::minus<>>(up, cla::vec::apply<std::multiplies<>>(newForward, cla::vec::dot(up, newForward))));

		auto newRight = cla::vec::cross(newUp, newForward);

		return std::array<std::array<T, 4>, 4>
		{
			std::get<0>(newRight),   std::get<1>(newRight),   std::get<2>(newRight),   0.0f,
			std::get<0>(newUp),      std::get<1>(newUp),      std::get<2>(newUp),      0.0f,
			std::get<0>(newForward), std::get<1>(newForward), std::get<2>(newForward), 0.0f,
			std::get<0>(pos),        std::get<1>(pos),        std::get<2>(pos),        1.0f
		};
	}
}