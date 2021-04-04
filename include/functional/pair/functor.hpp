#pragma once

#include <functor.hpp>

#include <utility>
template<typename A, typename Function, typename B>
auto
fmap(Function&& fun, std::pair<A, B> functor)
{
	return std::pair<invoke_return_t<Function, A>, B>
		{ fun(std::get<0>(functor)), std::get<1>(functor) };
}
