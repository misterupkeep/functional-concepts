#include <functional/vector.hpp>
#include <functional/array.hpp>
#include <functional/pair.hpp>
#include <functional/string.hpp>

#include <monoid.hpp>
#include <extract.hpp>

template<typename T>
concept Addable = requires(T a, T b) { a + b; };

template<Functor F> requires Addable<unwrap_first_t<F>>
auto add_two(F functor)
{ return fmap([](auto i){return i + 2;}, functor); }

#include <typeinfo>
#include <cxxabi.h>
template<typename T> struct type_name {
	const char *name;
	type_name() : name(abi::__cxa_demangle(typeid(T).name(), 0, 0, nullptr)) {}
	~type_name() { std::free((void *)name); }
	operator const char*() { return name; }
};

struct MyInt { int v; MyInt(int v):v(v){}
	// MyInt operator+(const MyInt &rhs){return v + rhs.v;}
};
MyInt sappend(MyInt l, MyInt r) { return MyInt(l.v + r.v); }

template<Semigroup T> struct mempty_prime {
	static const T value;
};

template<typename T> auto mempty_prime_v = mempty_prime<T>::value;

// template<> const MyInt mempty_prime<MyInt>::value = MyInt(3);
template<> auto mempty_prime_v<MyInt> = MyInt(3);

template<typename L, typename R> requires Monoid<L> && Monoid<R>
auto mempty<std::pair<L,R>> = std::pair{mempty<L>, mempty<R>};

template<>
auto mempty<MyInt> = MyInt(3);

template<> constexpr auto mempty<int> = 0;

#include <iostream>
int main()
{
	static_assert(is_functor<std::vector<int>>::value, "vector is_fmappable");

	static_assert(
		std::is_same<
		        int,
		        unwrap_first_t<std::array<int, 23>>
		>::value,
		"asd");


	static_assert(is_monoid_v<MyInt>, "aaa");
	static_assert(is_monoid_v<std::string>, "aaa");
	static_assert(is_monoid_v<int>, "aaa");

	std::cout << "sappend<MyInt>: 2 + 3 = "
		  << sappend(MyInt(2), MyInt(3)).v
		  << std::endl;
	// mempty<MyInt>;

	std::cout << "mempty<pair<string, string>>: "
		  << std::get<0>(mempty<std::pair<std::string, std::string>>)
		  << std::endl;

	// mempty<std::vector<int>>;

	std::cout << "mempty<std::string> = \""
		  << mempty<std::string>
		  << "\"" << std::endl;

	std::pair int_pair = { 0, 2 };
	std::pair left_char_pair = first([](int i)->char{return'A'+i;}, int_pair);

	std::cout << "std::pair: ("
		  << std::get<0>(left_char_pair) << ", "
		  << std::get<1>(left_char_pair) << ")"
		  << std::endl;

	std::vector int_vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::vector char_vec = fmap([](int i)->char{return'A'+i;}, add_two(int_vec));

	std::cout << "std::vector: ";
	for(auto a: char_vec) std::cout << a << " ";
	std::cout << std::endl;

	std::cout << "unwrap_second_t<std::array<int, 23>> = "
		  << type_name<unwrap_second_t<std::array<int, 23>>>()
		  << std::endl;

	return 0;
}
