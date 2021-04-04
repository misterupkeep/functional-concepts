#pragma once

#include <functor.hpp>

#include <vector>
template<typename A, typename Function>
auto
fmap(Function&& fun, std::vector<A> functor)
{
	std::vector<invoke_return_t<Function, A>> copy_vec;
	copy_vec.reserve(functor.size());

        for(auto a: functor) {
		copy_vec.push_back(fun(a));
        }

        return copy_vec;
}
