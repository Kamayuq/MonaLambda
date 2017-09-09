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

	Pair(const S& state, const A& value) : state(state), value(value) {}
	S state;
	A value;

	inline bool equal_to(const Pair<S, A>& rhs) const
	{
		return state == rhs.state
			&& value == rhs.value;
	}
};

template<typename S, typename A>
auto MakePair(const S& state, const A& value)
{
	return Pair<S, A>(state, value);
}


class State : public Monad<State>
{
public:
	template<typename M, typename K>
	static auto Bind(const M& m, const K& k)
	{
		return [m, k](auto s) constexpr
		{
			auto p = m(s);

			//static_assert(function_traits::is_callable<decltype(k)(decltype(p.value))>::value, "the state parameter has mismatched type");
			auto b = k(p.value);
			return b(p.state);
		};
	}

	template<typename A>
	static auto Return(const A& a)
	{
		return [a](auto s) constexpr
		{
			return MakePair(s, a);
		};
	}

	template<typename S>
	static auto Get()
	{
		return [](auto s) constexpr { return MakePair(s, s); };
	}

	template<typename S>
	static auto Put(const S& s)
	{
		return [s](auto) constexpr { return MakePair(s, Unit()); };
	}
	
	template<typename F>
	static auto Modify(const F& f)
	{
		return [f](auto s) constexpr { return MakePair(f(s), Unit()); };
	}

	template<typename A, typename B>
	static auto Seq(const A& a, const B& b)
	{
		return State::Bind(a, [b](auto) constexpr { return b; });
	}
};

template<typename Inner>
class StateT : public Monad<StateT<Inner>>
{
public:
	template<typename M, typename K>
	static auto Bind(const M& m, const K& k)
	{
		return [m, k](auto s) constexpr
		{
			return Inner::Bind(m(s), [k](auto p) constexpr
			{
				auto v2 = p.value;
				auto s2 = p.state;
				auto b = k(v2);
				return b(s2);
			});
		};
	}

	template<typename A>
	static auto Return(const A& a)
	{
		return [a](auto s) constexpr
		{
			return Inner::Return(MakePair(s, a));
		};
	}

	template<typename MA>
	static auto ReturnM(const MA& ma)
	{
		return [ma](auto s) constexpr
		{
			return Inner::Bind(ma, [s, ma](auto a) constexpr
			{
				return Return(ma)(s);
			});
		};
	}

	template<typename S>
	static auto Get()
	{
		return [](auto s) constexpr
		{
			return Inner::Return(MakePair(s, s));
		};
	}

	template<typename S>
	static auto Put(const S& s)
	{
		return [s](auto) constexpr
		{
			return Inner::Return(MakePair(s, Unit()));
		};
	}
	
	template<typename F>
	static auto Modify(const F& f)
	{
		return [f](auto s) constexpr
		{
			return Inner::Return(MakePair(f(s), Unit()));
		};
	}

	template<typename A, typename B>
	static auto Seq(const A& a, const B& b)
	{
		return StateT<Inner>::Bind(a, [b](auto) constexpr { return b; });
	}
};

}; //namespace Monadic