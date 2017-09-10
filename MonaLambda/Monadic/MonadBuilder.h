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
#include "Common.h"

namespace Monadic
{

template<typename F, typename... Args>
struct LiftMonad
{
	template<typename MonadType>
	static auto liftM(const F& f)
	{
		static_assert(function_traits::is_callable<F(Args...)>::value, "lift cannot be called with this arguments");
		return [f](Args... args)
		{ 
			return MonadType::Return(f(args...));
		};
	}
};

template<typename F, typename... Args>
struct LiftMonad<F(Args...)> : LiftMonad<F, Args...> 
{

};

template<typename LAMBDA, typename VALUE>
class AssignedLazy : public LAMBDA
{
	template<typename TYPE>
	friend class Monad;

	typedef AssignedLazy<LAMBDA, VALUE> ThisType;

	VALUE* Assignment;

public:
	AssignedLazy(LAMBDA&& Lambda, VALUE& Assignment) : LAMBDA(std::forward<LAMBDA>(Lambda)), Assignment(&Assignment)
	{}
};

template<typename LAMBDA>
class Lazy : public LAMBDA
{
	typedef Lazy<LAMBDA> ThisType;

public:
	Lazy(LAMBDA&& Lambda) : LAMBDA(std::forward<LAMBDA>(Lambda))
	{}

	template<typename VALUE>
	friend decltype(auto) operator <<= (VALUE& a, ThisType&& l)
	{
		return AssignedLazy<LAMBDA, VALUE>(std::forward<ThisType>(l), a);
	}
};

template<typename LAMBDA>
auto MakeLazy(LAMBDA&& Lambda)
{
	return Lazy<LAMBDA>(std::forward<LAMBDA>(Lambda));
};
#define LAZY(EXPR) MakeLazy([&](){ return EXPR; })

template<typename TYPE>
class Monad
{
protected:
	template<typename M, typename K, typename LAMBDA>
	struct WrappedBind : public LAMBDA
	{
		typedef M M;
		typedef K K;
		WrappedBind(const LAMBDA& Lambda) : LAMBDA(Lambda) {}
	};

	template<typename M, typename K, typename LAMBDA>
	static auto WrapBind(const LAMBDA& Lambda) { return WrappedBind<M, K, LAMBDA>(Lambda); }

public:
	template<typename X>
	static auto Do(const Lazy<X>& x)
	{
		return x();
	};

	template<typename X, typename... XS>
	static auto Do(const Lazy<X>& x, const XS&... xs)
	{
		return TYPE::Bind(x(), [=](auto) constexpr { return Do(xs...); });
	};

	template<typename X, typename V>
	static auto Do(const AssignedLazy<X, V>& x)
	{
		return TYPE::Bind(x(), [=](auto a) constexpr { *x.Assignment = a; return TYPE::Return(a); });
	};

	template<typename X, typename V, typename... XS>
	static auto Do(const AssignedLazy<X, V>& x, const XS&... xs)
	{
		return TYPE::Bind(x(), [=](auto a) constexpr { *x.Assignment = a; return Do(xs...); });
	};
};

}; //namespace Monadic
