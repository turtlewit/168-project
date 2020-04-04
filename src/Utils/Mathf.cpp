// Mathf.cpp
#include "Mathf.hpp"

std::random_device Mathf::device{};
std::default_random_engine Mathf::random_engine{Mathf::device()};

constexpr int Mathf::wrap(int n, int min, int max) noexcept
{
	int range = max - min + 1;

	if (n < min)
		n += range * ((min - n) / range + 1);

	return min + (n - min) % range;
}


int Mathf::rand_range(int min, int max)
{
	std::uniform_int_distribution<> distr{min, max};
	return distr(random_engine);
}


float Mathf::rand_range(float min, float max)
{
	std::uniform_real_distribution<float> distr{min, max};
	return distr(random_engine);
}
