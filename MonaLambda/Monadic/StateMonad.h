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
#include "MonadBuilder.h"

namespace Monadic
{

template<typename S, typename A>
struct Pair : private equal_comparable<Pair<S, A>>
{
	typedef A ValueType;
	typedef S StateType;

	constexpr Pair(const S& state, const A& value) : state(state), value(value) {}
	S state;
	A value;

	constexpr bool equal_to(const Pair<S, A>& rhs) const
	{
		return state == rhs.state
			&& value == rhs.value;
	}
};

template<typename S, typename A>
constexpr auto MakePair(const S& state, const A& value)
{
	return Pair<S, A>(state, value);
}


class State : public Monad<State>
{
	typedef Monad<State> BaseType;
	template<typename L>
	using  WrappedMonad = typename BaseType::template WrappedMonad<L>;

public:
	template<typename M, typename K>
	constexpr static auto Bind(const WrappedMonad<M>& m, const K& k)
	{
		return BaseType::WrapMonad([m, k](auto s) constexpr
		{
			auto p = m(s);

			//static_assert(function_traits::is_callable<decltype(k)(decltype(p.value))>::value, "the state parameter has mismatched type");
			auto b = k(p.value);
			return b(p.state);
		});
	}

	template<typename A>
	constexpr static auto Return(const A& a)
	{
		return BaseType::WrapMonad([a](auto s) constexpr
		{
			return MakePair(s, a);
		});
	}

	constexpr static auto Get()
	{
		return BaseType::WrapMonad([](auto s) constexpr { return MakePair(s, s); });
	}

	template<typename S>
	constexpr static auto Put(const S& s)
	{
		return BaseType::WrapMonad([s](auto) constexpr { return MakePair(s, Unit()); });
	}
	
	template<typename F>
	constexpr static auto Modify(const F& f)
	{
		return BaseType::WrapMonad([f](auto s) constexpr { return MakePair(f(s), Unit()); });
	}

	template<typename A, typename B>
	constexpr static auto Seq(const A& a, const B& b)
	{
		return BaseType::WrapMonad(State::Bind(a, [b](auto) constexpr { return b; }));
	}
};

template<typename Inner>
class StateT : public Monad<StateT<Inner>>
{
	typedef Monad<StateT<Inner>> BaseType;
	template<typename L>
	using  WrappedMonad = typename BaseType::template WrappedMonad<L>;

	typedef Monad<Inner> InnerBaseType;
	template<typename L>
	using  InnerWrappedMonad = typename InnerBaseType::template WrappedMonad<L>;

public:
	template<typename M, typename K>
	constexpr static auto Bind(const WrappedMonad<M>& m, const K& k)
	{
		return BaseType::WrapMonad([m, k](auto s) constexpr
		{
			auto ms = m(s);
			return Inner::Bind(ms, [k](auto p) constexpr
			{
				auto v2 = p.value;
				auto s2 = p.state;
				auto b = k(v2);
				return b(s2);
			});
		});
	}

	template<typename A>
	constexpr static auto Return(const A& a)
	{
		return BaseType::WrapMonad([a](auto s) constexpr
		{
			return Inner::Return(MakePair(s, a));
		});
	}

	template<typename MA>
	constexpr static auto ReturnM(const InnerWrappedMonad<MA>& ma)
	{
		return BaseType::WrapMonad([ma](auto s) constexpr
		{
			return Inner::Bind(ma, [s, ma](auto a) constexpr
			{
				return Return(ma)(s);
			});
		});
	}

	constexpr static auto Get()
	{
		return BaseType::WrapMonad([](auto s) constexpr
		{
			return Inner::Return(MakePair(s, s));
		});
	}

	template<typename S>
	constexpr static auto Put(const S& s)
	{
		return BaseType::WrapMonad([s](auto) constexpr
		{
			return Inner::Return(MakePair(s, Unit()));
		});
	}
	
	template<typename F>
	constexpr static auto Modify(const F& f)
	{
		return BaseType::WrapMonad([f](auto s) constexpr
		{
			return Inner::Return(MakePair(f(s), Unit()));
		});
	}

	template<typename A, typename B>
	constexpr static auto Seq(const A& a, const B& b)
	{
		return BaseType::WrapMonad(BaseType::Bind(a, [b](auto) constexpr { return b; }));
	}
};

}; //namespace Monadic