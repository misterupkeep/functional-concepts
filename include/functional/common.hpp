#pragma once

#include <functional>
#include <type_traits>

/**
 * Shorthand for return type of invokation
 **/
template<typename Function, typename ...Args>
using invoke_return_t = std::invoke_result_t<std::decay_t<Function>,
					     std::decay_t<Args>...>;


/**
 * The identity function.
 *
 * It is used as the only value of a function for the type (a -> b) in concept
 *  Functor.
 *
 * This means that the concept will type check only if the transformation
 *  F<A> -> F<A> is valid. However, fmapping into invalid F specialisations will
 *  still fail.
 *
 * This approach lets the compiler deduce the fmap() call without having to
 *  provide starting and ending types, at the cost of deferring some checks to
 *  fmap() calls.
**/
static constexpr auto id = [](auto x){return x;};


/** Workaround to always fail a static_assert **/
template<typename...> struct always_false { static constexpr auto value = false; };
