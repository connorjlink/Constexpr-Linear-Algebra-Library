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
		cla::matrix<T, S> mat;

		for (auto i = 0; i < S; ++i)
		{
			for (auto j = 0; j < S; ++j)
			{
				mat[i][j] = binaryop{}(mat[i][j], operand);
			}
		}

		return mat;
	}

	template<typename binaryop = std::multiplies<void>, typename T = float, std::size_t S>
	constexpr auto reduce(const cla::matrix<T, S>& mat1, const cla::matrix<T, S>& mat2)
	{
		cla::matrix<T, S> mat;

		for (auto i = 0; i < S; ++i)
		{
			for (auto j = 0; j < S; ++j)
			{
				mat.data[i][j] = binaryop{}(mat1.data[i][j], mat2.data[i][j]);
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
					sum += mat1.data[i][k] * mat2.data[k][j];
				}

				mat.data[i][j] = sum;
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
				mat.data[i][j] = (T)0.0f;
			}
		}

		mat.data[0][0] = (T)1.0f;
		mat.data[1][1] = (T)std::cosf(theta);
		mat.data[1][2] = (T)std::sinf(theta);
		mat.data[2][1] = (T)-std::sinf(theta);
		mat.data[2][2] = (T)std::cosf(theta);
		mat.data[3][3] = (T)1.0f;

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
				mat.data[i][j] = (T)0.0f;
			}
		}

		mat.data[0][0] = (T)std::cosf(theta);
		mat.data[1][1] = (T)1.0f;
		mat.data[0][2] = (T)std::sinf(theta);
		mat.data[2][0] = (T)-std::sinf(theta);
		mat.data[2][2] = (T)std::cosf(theta);
		mat.data[3][3] = (T)1.0f;

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
				mat.data[i][j] = (T)0.0f;
			}
		}

		mat.data[0][0] = (T)std::cosf(theta);
		mat.data[0][1] = (T)std::sinf(theta);
		mat.data[1][0] = (T)-std::sinf(theta);
		mat.data[1][1] = (T)std::cosf(theta);
		mat.data[2][2] = (T)1.0f;
		mat.data[3][3] = (T)1.0f;

		return mat;
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
				mat.data[i][j] = (T)0.0f;
			}
		}

		mat.data[0][0] = (T)aspectRatio * fovRad;
		mat.data[1][1] = (T)fovRad;
		mat.data[2][2] = (T)far / (far - near);
		mat.data[3][2] = (T)(-far * near) / (far - near);
		mat.data[2][3] = (T)1.0f;

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
			(T)newRight.x, (T)newRight.y, (T)newRight.z, (T)0.0f,
			(T)newUp.x, (T)newUp.y, (T)newUp.z, (T)0.0f,
			(T)newForward.x, (T)newForward.y, (T)newForward.z, (T)0.0f,
			(T)pos.x, (T)pos.y, (T)pos.z, (T)1.0f
		};
	}

	template<typename T = float>
	constexpr auto inverse(const cla::matrix<T, 4>& m) noexcept
	{
		cla::matrix<T, 4> matrix;

		matrix.data[0][0] = m.data[0][0]; matrix.data[0][1] = m.data[1][0]; matrix.data[0][2] = m.data[2][0]; matrix.data[0][3] = (T)0.0f;
		matrix.data[1][0] = m.data[0][1]; matrix.data[1][1] = m.data[1][1]; matrix.data[1][2] = m.data[2][1]; matrix.data[1][3] = (T)0.0f;
		matrix.data[2][0] = m.data[0][2]; matrix.data[2][1] = m.data[1][2]; matrix.data[2][2] = m.data[2][2]; matrix.data[2][3] = (T)0.0f;

		matrix.data[3][0] = -(m.data[3][0] * matrix.data[0][0] + m.data[3][1] * matrix.data[1][0] + m.data[3][2] * matrix.data[2][0]);
		matrix.data[3][1] = -(m.data[3][0] * matrix.data[0][1] + m.data[3][1] * matrix.data[1][1] + m.data[3][2] * matrix.data[2][1]);
		matrix.data[3][2] = -(m.data[3][0] * matrix.data[0][2] + m.data[3][1] * matrix.data[1][2] + m.data[3][2] * matrix.data[2][2]);
		matrix.data[3][3] = (T)1.0f;

		return matrix;
	}

	template<typename T = float>
	constexpr auto identity() noexcept
	{
		cla::matrix<T, 4> matrix;

		matrix.data[0][0] = (T)1.0f;
		matrix.data[0][1] = (T)0.0f;
		matrix.data[0][2] = (T)0.0f;
		matrix.data[0][3] = (T)0.0f;

		matrix.data[1][0] = (T)0.0f;
		matrix.data[1][1] = (T)1.0f;
		matrix.data[1][2] = (T)0.0f;
		matrix.data[1][3] = (T)0.0f;

		matrix.data[2][0] = (T)0.0f;
		matrix.data[2][1] = (T)0.0f;
		matrix.data[2][2] = (T)1.0f;
		matrix.data[2][3] = (T)0.0f;

		matrix.data[3][0] = (T)0.0f;
		matrix.data[3][1] = (T)0.0f;
		matrix.data[3][2] = (T)0.0f;
		matrix.data[3][3] = (T)1.0f;

		return matrix;
	}

	template<typename T = float, std::size_t S>
	constexpr auto operator*(const cla::matrix<T, S>& mat1, const cla::matrix<T, S>& mat2) noexcept
	{
		return cla::compose(mat1, mat2);
	}

	template<typename T = float>
	constexpr auto operator~(const cla::matrix<T, 4>& mat) noexcept
	{
		return cla::inverse(mat);
	}
}