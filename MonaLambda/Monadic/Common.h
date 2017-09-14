/*
//
// Copyright(c) 2016, Arne Schober
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// *Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and / or other materials provided with the distribution.
// * Neither the name of the <organization> nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL ARNE SCHOBER BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*/


#pragma once
#include <utility>
#include <functional>

template<typename T>
class equal_comparable
{
	friend bool operator==(T const &a, T const &b) { return  a.equal_to(b); }
	friend bool operator!=(T const &a, T const &b) { return !a.equal_to(b); }
};

struct Unit : private equal_comparable<Unit>
{
	constexpr bool equal_to(const Unit& rhs) const
	{
		return true;
	}

	constexpr bool equal_to(...) const
	{
		return false;
	}
};

class non_copyable
{
	non_copyable(const non_copyable&) = delete;
	non_copyable& operator= (const non_copyable&) = delete;
protected:
	non_copyable() = default;
};

class non_copy_moveable : non_copyable
{
	non_copy_moveable(non_copy_moveable&&) = delete;
	non_copy_moveable& operator= (non_copy_moveable&&) = delete;
protected:
	non_copy_moveable() = default;
};

namespace function_traits
{
	template<typename Ret, typename Arg, typename... Rest>
	Arg first_argument_helper(Ret(*) (Arg, Rest...));

	template<typename Ret, typename F, typename Arg, typename... Rest>
	Arg first_argument_helper(Ret(F::*) (Arg, Rest...));

	template<typename Ret, typename F, typename Arg, typename... Rest>
	Arg first_argument_helper(Ret(F::*) (Arg, Rest...) const);

	template <typename F>
	decltype(first_argument_helper(&F::operator())) first_argument_helper(F);

	template <typename T>
	using first_argument = decltype(first_argument_helper(std::declval<T>()));

	///////////////////////////////////////////////////////////////////////////////

	struct can_call_test
	{
		template<typename F, typename... A>
		static decltype(std::declval<F>()(std::declval<A>()...), std::true_type())
			f(int);

		template<typename F, typename... A>
		static std::false_type
			f(...);
	};

	template<typename F, typename... A>
	struct is_callable : decltype(can_call_test::f<F, A...>(0)) 
	{
		enum 
		{
			value = std::is_convertible<is_callable*, std::true_type*>::value
		};
	};

	template<typename F, typename... A>
	struct is_callable <F(A...)> : is_callable <F, A...> { };

	// For generic types that are functors, delegate to its 'operator()'
	template <typename T>
	struct function_sig
		: public function_sig<decltype(&T::operator())>
	{};

	// for pointers to member function
	template <typename ClassType, typename ReturnType, typename... Args>
	struct function_sig<ReturnType(ClassType::*)(Args...) const> {
		//enum { arity = sizeof...(Args) };
		typedef std::function<ReturnType(Args...)> signature;
	};

	// for pointers to member function
	template <typename ClassType, typename ReturnType, typename... Args>
	struct function_sig<ReturnType(ClassType::*)(Args...) > {
		typedef std::function<ReturnType(Args...)> signature;
	};

	// for function pointers
	template <typename ReturnType, typename... Args>
	struct function_sig<ReturnType(*)(Args...)> {
		typedef std::function<ReturnType(Args...)> signature;
	};

	template <typename L>
	typename function_sig<L>::signature make_function(L l) {
		return (typename function_sig<L>::signature)(l);
	}
}