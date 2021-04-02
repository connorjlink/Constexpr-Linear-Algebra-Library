module;
#include <algorithm>
#include <functional>
#include <array>
#include <tuple>
#include <cmath>
export module matrix;

import core;
import trig;
import vector;

export namespace cla
{
	template<typename binaryop = std::multiplies<void>, typename T, std::size_t S>
	constexpr auto apply(const cla::matrix<T, S>& mat1, const T& operand)
	{
		std::array<std::array<T, S>, S> mat;

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
	constexpr auto apply(const cla::matrix<T, S>& mat1, const cla::matrix<T, S>& mat2)
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
	constexpr auto compose(const cla::matrix<T, S>& mat1, const cla::matrix<T, S>& mat2) noexcept
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

	const auto rotationX(const float& theta) noexcept
	{
		cla::float4x4 mat;

		for (auto i = 0; i < 4; ++i)
		{
			for (auto j = 0; j < 4; ++j)
			{
				mat[i][j] = 0.0f;
			}
		}

		mat[0][0] = 1.0f;
		mat[1][1] = std::cosf(theta);
		mat[1][2] = std::sinf(theta);
		mat[2][1] = -std::sinf(theta);
		mat[2][2] = std::cosf(theta);
		mat[3][3] = 1.0f;

		return mat;
	}

	const auto rotationY(const float& theta) noexcept
	{
		cla::float4x4 mat;

		for (auto i = 0; i < 4; ++i)
		{
			for (auto j = 0; j < 4; ++j)
			{
				mat[i][j] = 0.0f;
			}
		}

		mat[0][0] = std::cosf(theta);
		mat[1][1] = 1.0f;
		mat[0][2] = std::sinf(theta);
		mat[2][0] = -std::sinf(theta);
		mat[2][2] = std::cosf(theta);
		mat[3][3] = 1.0f;

		return mat;
	}

	const auto rotationZ(const float& theta) noexcept
	{
		cla::float4x4 mat;
		
		for (auto i = 0; i < 4; ++i)
		{
			for (auto j = 0; j < 4; ++j)
			{
				mat[i][j] = 0.0f;
			}
		}

		mat[0][0] = std::cosf(theta);
		mat[0][1] = std::sinf(theta);
		mat[1][0] = -std::sinf(theta);
		mat[1][1] = std::cosf(theta);
		mat[2][2] = 1.0f;
		mat[3][3] = 1.0f;

		return mat;
	}

	template<typename T>
	constexpr auto rotationXYZ(const T& x, const T& y, const T& z) noexcept
	{
		return cla::compose(cla::rotationX(x), cla::compose(cla::rotationY(y), cla::rotationZ(y)));
	}

	template<typename T>
	constexpr auto translation(T&& x, T&& y, T&& z) noexcept
	{
		return std::array<std::array<T, 4>, 4>
		{
			(T)1.0f, (T)0.0f, (T)0.0f, (T)0.0f,
			(T)0.0f, (T)1.0f, (T)0.0f, (T)0.0f,
			(T)0.0f, (T)0.0f, (T)1.0f, (T)0.0f,
			x, y, z, (T)1.0f
		};
	}

	template<typename T>
	const auto projection(const T& fov, const T& aspectRatio, const T& near, const T& far) noexcept
	{
		cla::float4x4 mat;

		float fovRad = 1.0f / std::tanf(fov * 0.5f / 180.0f * 3.14159f);
		
		for (auto i = 0; i < 4; ++i)
		{
			for (auto j = 0; j < 4; ++j)
			{
				mat[i][j] = 0.0f;
			}
		}

		mat[0][0] = aspectRatio * fovRad;
		mat[1][1] = fovRad;
		mat[2][2] = far / (far - near);
		mat[3][2] = (-far * near) / (far - near);
		mat[2][3] = 1.0f;

		return mat;
	}

	template<typename T>
	constexpr auto pointAt(const cla::v3d_generic<T>& pos, const cla::v3d_generic<T>& target, const cla::v3d_generic<T>& up) noexcept
	{
		cla::v3d_generic<T> newForward = cla::normalize(cla::reduce<std::minus<>>(target, pos));

		auto newUp = cla::normalize(cla::reduce<std::minus<>>(up, cla::apply<std::multiplies<>>(newForward, cla::dot(up, newForward))));

		auto newRight = cla::cross(newUp, newForward);
		
		return std::array<std::array<T, 4>, 4>
		{
			newRight.x  , newRight.y  , newRight.z  , 0.0f,
			newUp.x     , newUp.y     , newUp.z     , 0.0f,
			newForward.x, newForward.y, newForward.z, 0.0f,
			pos.x       , pos.y       , pos.z       , 1.0f
		};

	}
}