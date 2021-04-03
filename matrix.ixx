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
	template<typename binaryop = std::multiplies<void>, typename T = float, std::size_t S>
	constexpr auto apply(const cla::matrix<T, S>& mat1, const T& operand)
	{
		std::array<std::array<T, S>, S> mat;

		for (auto i = 0; i < S; ++i)
		{
			for (auto j = 0z; j < S; ++j)
			{
				mat[i][j] = binaryop{}(mat1[i][j], operand);
			}
		}

		return mat;
	}

	template<typename binaryop = std::multiplies<void>, typename T = float, std::size_t S>
	constexpr auto apply(const cla::matrix<T, S>& mat1, const cla::matrix<T, S>& mat2)
	{
		cla::matrix<T, S> mat;

		for (auto i = 0; i < S; ++i)
		{
			for (auto j = 0; j < S; ++j)
			{
				mat[i][j] = binaryop{}(mat1[i][j], mat2[i][j]);
			}
		}

		return mat;
	}

	template<typename T = float, std::size_t S = 4>
	constexpr auto compose(const cla::matrix<T, S>& mat1, const cla::matrix<T, S>& mat2) noexcept
	{
		cla::matrix<T, S> mat;

		for (auto i = 0; i < S; i++)
		{
			for (auto j = 0; j < S; j++)
			{
				T sum = (T)0.0f;

				for (auto k = 0; k < S; k++)
				{
					sum += mat1[i][k] * mat2[k][j];
				}

				mat[i][j] = sum;
			}
		}

		return mat;
	}

	template<typename T = float>
	constexpr auto rotationX(const T& theta) noexcept
	{
		cla::matrix<T, 4> mat;

		for (auto i = 0; i < 4; ++i)
		{
			for (auto j = 0; j < 4; ++j)
			{
				mat[i][j] = (T)0.0f;
			}
		}

		mat[0][0] = (T)1.0f;
		mat[1][1] = (T)std::cosf(theta);
		mat[1][2] = (T)std::sinf(theta);
		mat[2][1] = (T)-std::sinf(theta);
		mat[2][2] = (T)std::cosf(theta);
		mat[3][3] = (T)1.0f;

		return mat;
	}

	template<typename T = float>
	constexpr auto rotationY(const T& theta) noexcept
	{
		cla::matrix<T, 4> mat;

		for (auto i = 0; i < 4; ++i)
		{
			for (auto j = 0; j < 4; ++j)
			{
				mat[i][j] = (T)0.0f;
			}
		}

		mat[0][0] = (T)std::cosf(theta);
		mat[1][1] = (T)1.0f;
		mat[0][2] = (T)std::sinf(theta);
		mat[2][0] = (T)-std::sinf(theta);
		mat[2][2] = (T)std::cosf(theta);
		mat[3][3] = (T)1.0f;

		return mat;
	}

	template<typename T = float>
	constexpr auto rotationZ(const T& theta) noexcept
	{
		cla::matrix<T, 4> mat;
		
		for (auto i = 0; i < 4; ++i)
		{
			for (auto j = 0; j < 4; ++j)
			{
				mat[i][j] = (T)0.0f;
			}
		}

		mat[0][0] = (T)std::cosf(theta);
		mat[0][1] = (T)std::sinf(theta);
		mat[1][0] = (T)-std::sinf(theta);
		mat[1][1] = (T)std::cosf(theta);
		mat[2][2] = (T)1.0f;
		mat[3][3] = (T)1.0f;

		return mat;
	}

	template<typename T = float>
	constexpr auto rotationXYZ(const T& x, const T& y, const T& z) noexcept
	{
		return cla::compose(cla::rotationX(x), cla::compose(cla::rotationY(y), cla::rotationZ(y)));
	}

	template<typename T = float>
	constexpr auto translation(T&& x, T&& y, T&& z) noexcept
	{
		return cla::matrix<T, 4>
		{
			(T)1.0f, (T)0.0f, (T)0.0f, (T)0.0f,
			(T)0.0f, (T)1.0f, (T)0.0f, (T)0.0f,
			(T)0.0f, (T)0.0f, (T)1.0f, (T)0.0f,
			x, y, z, (T)1.0f
		};
	}

	template<typename T = float>
	const auto projection(const T& fov, const T& aspectRatio, const T& near, const T& far) noexcept
	{
		cla::matrix<T, 4> mat;

		T fovRad = (T)(1.0f / std::tanf(fov * 0.5f / 180.0f * 3.14159f));
		
		for (auto i = 0; i < 4; ++i)
		{
			for (auto j = 0; j < 4; ++j)
			{
				mat[i][j] = (T)0.0f;
			}
		}

		mat[0][0] = (T)aspectRatio * fovRad;
		mat[1][1] = (T)fovRad;
		mat[2][2] = (T)far / (far - near);
		mat[3][2] = (T)(-far * near) / (far - near);
		mat[2][3] = (T)1.0f;

		return mat;
	}

	template<typename T = float>
	constexpr auto pointAt(const cla::v3d_generic<T>& pos, const cla::v3d_generic<T>& target, const cla::v3d_generic<T>& up) noexcept
	{
		cla::v3d_generic<T> newForward = cla::normalize(cla::reduce<std::minus<>>(target, pos));

		auto newUp = cla::normalize(cla::reduce<std::minus<>>(up, cla::apply<std::multiplies<>>(newForward, cla::dot(up, newForward))));

		auto newRight = cla::cross(newUp, newForward);
		
		return cla::matrix<T, 4>
		{
			(T)newRight.x,   (T)newRight.y,   (T)newRight.z,   (T)0.0f,
			(T)newUp.x,      (T)newUp.y,      (T)newUp.z,      (T)0.0f,
			(T)newForward.x, (T)newForward.y, (T)newForward.z, (T)0.0f,
			(T)pos.x,        (T)pos.y,        (T)pos.z,        (T)1.0f
		};
	}

	template<typename T = float>
	cla::matrix<T, 4> inverse(const cla::matrix<T, 4>& m) noexcept
	{
		cla::matrix<T, 4> matrix;

		matrix[0][0] = m[0][0]; matrix[0][1] = m[1][0]; matrix[0][2] = m[2][0]; matrix[0][3] = (T)0.0f;
		matrix[1][0] = m[0][1]; matrix[1][1] = m[1][1]; matrix[1][2] = m[2][1]; matrix[1][3] = (T)0.0f;
		matrix[2][0] = m[0][2]; matrix[2][1] = m[1][2]; matrix[2][2] = m[2][2]; matrix[2][3] = (T)0.0f;

		matrix[3][0] = -(m[3][0] * matrix[0][0] + m[3][1] * matrix[1][0] + m[3][2] * matrix[2][0]);
		matrix[3][1] = -(m[3][0] * matrix[0][1] + m[3][1] * matrix[1][1] + m[3][2] * matrix[2][1]);
		matrix[3][2] = -(m[3][0] * matrix[0][2] + m[3][1] * matrix[1][2] + m[3][2] * matrix[2][2]);
		matrix[3][3] = (T)1.0f;

		return matrix;
	}

	template<typename T = float>
	cla::matrix<T, 4> identity() noexcept
	{
		cla::matrix<T, 4> matrix;

		matrix[0][0] = (T)1.0f;
		matrix[0][1] = (T)0.0f;
		matrix[0][2] = (T)0.0f;
		matrix[0][3] = (T)0.0f;

		matrix[1][0] = (T)0.0f;
		matrix[1][1] = (T)1.0f;
		matrix[1][2] = (T)0.0f;
		matrix[1][3] = (T)0.0f;

		matrix[2][0] = (T)0.0f;
		matrix[2][1] = (T)0.0f;
		matrix[2][2] = (T)1.0f;
		matrix[2][3] = (T)0.0f;

		matrix[3][0] = (T)0.0f;
		matrix[3][1] = (T)0.0f;
		matrix[3][2] = (T)0.0f;
		matrix[3][3] = (T)1.0f;

		return matrix;
	}
}