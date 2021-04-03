#pragma once

#include <functor.hpp>

#include <array>
template<typename A, typename Function, size_t N>
auto
fmap(Function&& fun, std::array<A, N> functor)
{
	std::array<invoke_return_t<Function, A>, N> copy_arr;

	for(size_t i = 0; i < functor.size(); i++) {
		copy_arr[i] = fun(functor[i]);
	}

	return copy_arr;
}
