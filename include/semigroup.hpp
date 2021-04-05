// A Semigroup is anything that has a notion of addition. That is, an
//  associative binary operator.

#pragma once

#include <functional/common.hpp>
#include <concepts>

namespace __semigroup_impl {
// SFINAE-like check if (l + r) is a valid expression
template<typename  S> static auto check_plus_callable(S l, S r) -> decltype(l + r);
template<typename...> static auto check_plus_callable(...) -> std::false_type;

// A type T is check_plus_valid iff (t1 + t2) returns something convertible back to T
template<typename S> constexpr auto check_plus_valid =
 std::is_convertible_v<decltype(check_plus_callable(std::declval<S>(), std::declval<S>())), S>;
} // namespace __semigroup_impl

// Suppress volatile return warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
#pragma GCC diagnostic ignored "-Wreturn-type"

// When a user calls sappend() on a type, we want to
//  a) check if we can use an overloaded operator+
//    1) if we can, use that
//    2) if not, throw a diagnostic error with static_assert
//
// However, we also want to be able to check if there exists a user-implemented
//  overload for sappend() _without_ causing a static_assert failure.
//
// So, if there exists a operator+ overload, set the return type to `S`.
//  If there isn't one, we return `volatile std::false_type`.
//
// Similarly, inside the function, we constexpr if the two cases: fail an
//  assertion if there is no operator+ overload, or return (l + r)
//
// Checking for overloads then turns into checking if the decltype() of the
//  would-be call is volatile-qualified. And since decltype(...) is unevaluated
//  context, the static_assert won't fail (but it does when you try to call it
//  otherwise!)
//
// Users just have to write the overload for their type S, and it will get
//  picked automatically over the base general definition:
//
//         auto sappend(MyInt l, MyInt r) { return MyInt(l.v + r.v); }
//
template<typename S> auto sappend(S l, S r)
	-> std::conditional_t<__semigroup_impl::check_plus_valid<S>, S, volatile std::false_type>
{
	if constexpr (!__semigroup_impl::check_plus_valid<S>) {
		static_assert(always_false<S>::value,
			      "Used type is not a Semigroup! Minimal implementation "
			      "requires: sappend | operator+");
	} else {
		return l + r;
	}
}
#pragma GCC diagnostic pop


template<typename S>
concept Semigroup = requires(S l, S r) {
	typename std::enable_if_t<!std::is_volatile_v<decltype(sappend(l, r))>>;
};

template<typename S> struct is_semigroup
{ static constexpr auto value =
		!std::is_volatile_v<decltype(sappend(std::declval<S>(), std::declval<S>()))>; };

template<typename S> constexpr auto is_semigroup_v = is_semigroup<S>::value;
