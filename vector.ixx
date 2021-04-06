module;
#include <tuple>
#include <type_traits>
#include <functional>
//#include <concepts>
export module vector;

import stdex;
import core;

export namespace cla
{
	template<typename binaryop = std::multiplies<void>, typename T = float>
	constexpr auto apply(const cla::v3d_generic<T>& vec, const T& operand) noexcept
	{
		cla::vf3d outVec;
	
		outVec.x = binaryop{}(vec.x, operand);
		outVec.y = binaryop{}(vec.y, operand);
		outVec.z = binaryop{}(vec.z, operand);
	
		return outVec;
	}

	template<typename binaryop = std::multiplies<void>, typename T = float>
	constexpr auto reduce(const cla::v3d_generic<T>& vec1, const cla::v3d_generic<T>& vec2) noexcept
	{
		cla::vf3d outVec;

		outVec.x = binaryop{}(vec1.x, vec2.x);
		outVec.y = binaryop{}(vec1.y, vec2.y);
		outVec.z = binaryop{}(vec1.z, vec2.z);

		return outVec;
	}

	template<typename T = float>
	constexpr auto sum(const cla::v3d_generic<T>& vec) noexcept
	{
		return (vec.x + vec.y + vec.z);
	}

	template<typename T = float>
	constexpr auto length2(const cla::v3d_generic<T>& vec) noexcept
	{
		return cla::sum(cla::reduce<std::multiplies<>>(vec, vec));
	}

	template<typename T = float>
	constexpr auto length(const cla::v3d_generic<T>& vec) noexcept
	{
		return std::my_sqrt(cla::length2(vec));
	}

	template<typename T = float>
	constexpr auto normalize(const cla::v3d_generic<T>& vec) noexcept
	{
		return cla::apply<std::divides<>>(vec, (cla::length(vec)));
	}

	template<bool Normalize = false, typename T = float>
	constexpr auto dot(const cla::v3d_generic<T>& vec1, const cla::v3d_generic<T>& vec2) noexcept
	{
		if constexpr (Normalize) return cla::sum(cla::reduce<std::multiplies<>>(cla::normalize(vec1), cla::normalize(vec2)));
		else return cla::sum(cla::reduce<std::multiplies<>>(vec1, vec2));
	}

	template<typename T = float>
	constexpr auto cross(const cla::v3d_generic<T>& vec1, const cla::v3d_generic<T>& vec2) noexcept
	{
		cla::v3d_generic<T> outVec;

		outVec.x = ((vec1.y * vec2.z) - (vec1.z * vec2.y));
		outVec.y = ((vec1.z * vec2.x) - (vec1.x * vec2.z));
		outVec.z = ((vec1.x * vec2.y) - (vec1.y * vec2.x));
		outVec.w = vec1.w;

		return outVec;
	}

	template<typename T = float>
	constexpr auto mul(const cla::v3d_generic<T>& vec, const cla::matrix<T, 4>& mat) noexcept
	{
		cla::v3d_generic<T> outVec;

		outVec.x = vec.x * mat.data[0][0] + vec.y * mat.data[1][0] + vec.z * mat.data[2][0] + mat.data[3][0];
		outVec.y = vec.x * mat.data[0][1] + vec.y * mat.data[1][1] + vec.z * mat.data[2][1] + mat.data[3][1];
		outVec.z = vec.x * mat.data[0][2] + vec.y * mat.data[1][2] + vec.z * mat.data[2][2] + mat.data[3][2];
		outVec.w = vec.x * mat.data[0][3] + vec.y * mat.data[1][3] + vec.z * mat.data[2][3] + mat.data[3][3];

		return outVec;
	}

	template<typename T = float>
	constexpr auto intersect(const cla::v3d_generic<T>& plane_p, const cla::v3d_generic<T>& plane_n, const cla::v3d_generic<T>& lineStart, const cla::v3d_generic<T>& lineEnd) noexcept
	{
		auto planeN = cla::normalize(plane_n);
		T plane_d = -cla::dot(planeN, plane_p);

		T ad = cla::dot(lineStart, planeN);
		T bd = cla::dot(lineEnd, planeN);

		T t = (-plane_d - ad) / (bd - ad);

		cla::v3d_generic<T> lineStartToEnd = cla::reduce<std::minus<>>(lineEnd, lineStart);
		cla::v3d_generic<T> lineToIntersect = cla::apply<std::multiplies<>>(lineStartToEnd, t);

		return cla::reduce<std::plus<>>(lineStart, lineToIntersect);
	}

	template<typename T = float>
	constexpr auto inverse(const cla::v3d_generic<T>& vec) noexcept
	{
		cla::v3d_generic<T> outVec;

		outVec.x = -vec.x;
		outVec.y = -vec.y;
		outVec.z = -vec.z;
		outVec.w = +vec.w;
		
		return outVec;
	}

	template<typename T = float>
	constexpr auto set(cla::v3d_generic<T>& vec, const T& operand) noexcept
	{
		vec.x = operand;
		vec.y = operand;
		vec.z = operand;

		return vec;
	}

	template<typename T = float>
	constexpr auto operator+(const cla::v3d_generic<T>& vec1, const cla::v3d_generic<T>& vec2) noexcept
	{
		return cla::reduce<std::plus<>>(vec1, vec2);
	}

	template<typename T = float>
	constexpr auto operator-(const cla::v3d_generic<T>& vec1, const cla::v3d_generic<T>& vec2) noexcept
	{
		return cla::reduce<std::minus<>>(vec1, vec2);
	}

	template<typename T = float>
	constexpr auto operator/(const cla::v3d_generic<T>& vec1, const cla::v3d_generic<T>& vec2) noexcept
	{
		return cla::reduce<std::divides<>>(vec1, vec2);
	}

	template<typename T = float>
	constexpr auto operator*(const cla::v3d_generic<T>& vec1, const cla::v3d_generic<T>& vec2) noexcept
	{
		return cla::reduce<std::multiplies<>>(vec1, vec2);
	}


	template<typename T = float>
	constexpr auto operator+(cla::v3d_generic<T>& vec, const T& operand) noexcept
	{
		return cla::apply<std::plus<>>(vec, operand);
	}

	template<typename T = float>
	constexpr auto operator-(cla::v3d_generic<T>& vec, const T& operand) noexcept
	{
		return cla::apply<std::minus<>>(vec, operand);
	}

	template<typename T = float>
	constexpr auto operator/(cla::v3d_generic<T>& vec, const T& operand) noexcept
	{
		return cla::apply<std::divides<>>(vec, operand);
	}

	template<typename T = float>
	constexpr auto operator*(cla::v3d_generic<T>& vec, const T& operand) noexcept
	{
		return cla::apply<std::multiplies<>>(vec, operand);
	}

	template<typename T = float>
	constexpr auto operator^(cla::v3d_generic<T>& vec, const T& operand) noexcept
	{
		return cla::set(vec, operand);
	}

	template<typename T = float>
	constexpr auto operator!(const cla::v3d_generic<T>& vec) noexcept
	{
		return cla::inverse(vec);
	}

	template<typename T = float>
	constexpr auto operator*(const cla::v3d_generic<T>& vec, const cla::matrix<T, 4>& mat) noexcept
	{
		return cla::mul(vec, mat);
	}


	template<typename T = float>
	constexpr auto clip(cla::v3d_generic<T>&& plane_p, cla::v3d_generic<T>&& plane_n, cla::tri<T>& in_tri, cla::tri<T>& out_tri1, cla::tri<T>& out_tri2) noexcept
	{
		plane_n = cla::normalize(plane_n);

		auto dist = [&](const cla::vf3d& p)
		{
			cla::vf3d n = cla::normalize(p);
			return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - cla::dot(plane_n, plane_p));
		};

		cla::vf3d* inside_points[3];  int nInsidePointCount = 0;
		cla::vf3d* outside_points[3]; int nOutsidePointCount = 0;

		float d0 = dist(in_tri.p1);
		float d1 = dist(in_tri.p2);
		float d2 = dist(in_tri.p3);

		if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p1; }
		else { outside_points[nOutsidePointCount++] = &in_tri.p1; }
		if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p2; }
		else { outside_points[nOutsidePointCount++] = &in_tri.p2; }
		if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p3; }
		else { outside_points[nOutsidePointCount++] = &in_tri.p3; }


		if (nInsidePointCount == 3)
		{
			out_tri1 = in_tri;

			return 1;
		}

		else if (nInsidePointCount == 1 && nOutsidePointCount == 2)
		{
			out_tri1.p1 = *inside_points[0];
			out_tri1.p2 = intersect(plane_p, plane_n, *inside_points[0], *outside_points[0]);
			out_tri1.p3 = intersect(plane_p, plane_n, *inside_points[0], *outside_points[1]);

			return 1;
		}

		else if (nInsidePointCount == 2 && nOutsidePointCount == 1)
		{
			out_tri1.p1 = *inside_points[0];
			out_tri1.p2 = *inside_points[1];
			out_tri1.p3 = intersect(plane_p, plane_n, *inside_points[0], *outside_points[0]);

			out_tri2.p1 = *inside_points[1];
			out_tri2.p2 = out_tri1.p3;
			out_tri2.p3 = intersect(plane_p, plane_n, *inside_points[1], *outside_points[0]);

			return 2;
		}

		else return 0;
	}
}