#pragma once

#include <bifunctor.hpp>

#include <utility>
template<typename LeftA, typename RightA, typename LeftFunction, typename RightFunction>
auto
bimap(LeftFunction&& left, RightFunction&& right, std::pair<LeftA, RightA> bifunctor)
{
	return std::pair<invoke_return_t<LeftFunction, LeftA>, invoke_return_t<RightFunction, RightA>>
		{ left(std::get<0>(bifunctor)), right(std::get<1>(bifunctor)) };
}
