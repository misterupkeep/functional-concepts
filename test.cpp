#include <functional/vector.hpp>
#include <functional/array.hpp>
#include <functional/pair.hpp>

#include <bifunctor.hpp>
#include <extract.hpp>

template<typename T>
concept Addable = requires(T a, T b) {
	a + b;
};

template<Functor F> requires Addable<unwrap_first_t<F>>
auto
add_two(F functor)
{
	return fmap([](auto i){return i + 2;}, functor);
}

#include <typeinfo>
#include <cxxabi.h>
template<typename T> struct type_name {
	const char *name;
	type_name() : name(abi::__cxa_demangle(typeid(T).name(), 0, 0, nullptr)) {}
	~type_name() { std::free((void *)name); }
	operator const char*() { return name; }
};


#include <iostream>
int
main()
{
	static_assert(is_functor<std::vector<int>>::value, "vector is_fmappable");

	static_assert(
		std::is_same<
		        int,
		        unwrap_first_t<std::array<int, 23>>
		>::value,
		"asd");

	std::pair<int, int> int_pair = { 0, 2 };
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
