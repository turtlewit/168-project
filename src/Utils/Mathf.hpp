// Mathf.h
#pragma once

#include <cstdint>
#include <cmath>
#include <random>

class Mathf {
private:
	static std::random_device device;
	static std::default_random_engine random_engine;

public:
	static constexpr long double Pi = 3.141592653589793238462643383279502884L;
	static constexpr inline float deg2rad(float degrees) noexcept { return (degrees * Pi) / 180.0f; }
	static constexpr inline float rad2deg(float radians) noexcept { return (radians * 180.0f) / Pi; }

	static constexpr inline float clamp(float value, float min, float max) noexcept { return (value < min) ? min : (max < value) ? max : value; }

	static constexpr int wrap(int n, int min, int max) noexcept;

	static inline int abs(int value) noexcept
	{ 
		uint32_t temp = value >> 31;
  		value ^= temp;
  		value += temp & 1;
		return value;
	}

	static inline float abs(float value) noexcept
	{
		union {
			float f;
			uint32_t i;
		} u;

		u.f = value;
		u.i &= 2147483647u;
		return u.f;
	}

	template <typename F>
	static constexpr F lerp(F from, F to, F weight) noexcept
	{
		if (from <= 0 && to >= 0 || from >= 0 && to <= 0)
			return weight * to + (1 - weight) * from;

		if (weight == 1)
			return to;

		const F x = from + weight * (to - from);
		return weight > 1 == to > from
			? (to < x ? x : to)
			: (to > x ? x : to);
	}

	template <typename F>
	static constexpr F lerp_delta(F from, F to, F weight, F delta) noexcept
	{
		return lerp(from, to, static_cast<F>(1.0) - std::pow<F, F>(weight, delta));
	}

	static int rand_range(int min, int max);
	static float rand_range(float min, float max);
	
	template <typename T, typename... Args>
	static T choose(T first, Args... args)
	{
		T array[] = {first, args...};
		std::uniform_int_distribution<> distr{0, sizeof...(Args)};
		return array[distr(random_engine)];
	}
};
