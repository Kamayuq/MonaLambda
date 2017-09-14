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
#include <functional>

namespace Monadic
{

template<typename F, typename... Args>
struct LiftMonad
{
	template<typename MonadType>
	constexpr static auto liftM(const F& f)
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

template<typename LAMBDA>
class Lazy : public LAMBDA
{
	typedef Lazy<LAMBDA> ThisType;

public:
	constexpr Lazy(LAMBDA&& Lambda) : LAMBDA(std::forward<LAMBDA>(Lambda))
	{}
};

template<typename LAMBDA, typename VALUE>
class AssignedLazy : public Lazy<LAMBDA>
{
	template<typename TYPE>
	friend class Monad;

	typedef AssignedLazy<LAMBDA, VALUE> ThisType;

	VALUE* Assignment;

public:
	constexpr AssignedLazy(LAMBDA&& Lambda, VALUE& Assignment) : Lazy<LAMBDA>(std::forward<LAMBDA>(Lambda)), Assignment(&Assignment)
	{}
};

template<typename VALUE, typename LAMBDA>
constexpr decltype(auto) operator <<= (VALUE& a, Lazy<LAMBDA>&& l)
{
	return AssignedLazy<LAMBDA, VALUE>(std::forward<Lazy<LAMBDA>>(l), a);
}

template<typename LAMBDA>
constexpr auto MakeLazy(LAMBDA&& Lambda)
{
	return Lazy<LAMBDA>(std::forward<LAMBDA>(Lambda));
};
#define LAZY(EXPR) MakeLazy([&](){ return EXPR; })

template<typename C, typename L1, typename L2>
auto If(C Comparator, const Lazy<L1>& f1, const Lazy<L2>& f2)
{
	typedef typename decltype(f1())::MonadType MonadType;

	auto b1 = MonadType::Bind(f1(), [&](auto r1)
	{
		return MonadType::Return(r1);
	});
	auto b2 = MonadType::Bind(f2(), [&](auto r2)
	{
		return MonadType::Return(r2);
	});

	auto res = Comparator() ? b1 : b2;

	return LAZY(res);
}


template<typename MonadType>
class Monad
{
public:
	template<typename LAMBDA>
	struct WrappedMonad : public LAMBDA
	{
		typedef MonadType MonadType;
		constexpr WrappedMonad(const LAMBDA& Lambda) : LAMBDA(Lambda) {}
		constexpr const LAMBDA& Unwrap() const { return *this; }
	};

protected:
	template<typename LAMBDA>
	constexpr static auto WrapMonad(const LAMBDA& Lambda) { return WrappedMonad<LAMBDA>(Lambda); }

private:
	template<typename X>
	constexpr static auto Do(const Lazy<X>& x)
	{
		return MonadType::Bind(x(), [=](auto a) constexpr { return MonadType::Return(a); });
	};

	template<typename X, typename... XS>
	constexpr static auto Do(const Lazy<X>& x, const XS&... xs)
	{
		return MonadType::Bind(x(), [=](auto) constexpr { return MonadType::Do(xs...); });
	};

	template<typename X, typename V>
	constexpr static auto Do(const AssignedLazy<X, V>& x)
	{
		return MonadType::Bind(x(), [=](auto a) constexpr { *x.Assignment = a; return MonadType::Return(a); });
	};

	template<typename X, typename V, typename... XS>
	constexpr static auto Do(const AssignedLazy<X, V>& x, const XS&... xs)
	{
		return MonadType::Bind(x(), [=](auto a) constexpr { *x.Assignment = a; return MonadType::Do(xs...); });
	};

public:
	template<typename... XS>
	constexpr static auto Delay(const XS&... xs)
	{
		//typedef decltype(MonadType::Do(xs...)) R;
		//typedef std::function<R()> Erasure;
		return MonadType::Do(xs...);
	}
};

template<typename X, typename... XS>
constexpr auto Do(const X& x, const XS&... xs)
{
	typedef typename decltype(x())::MonadType MonadType;
	return MonadType::Delay(x, xs...);
}

}; //namespace Monadic
