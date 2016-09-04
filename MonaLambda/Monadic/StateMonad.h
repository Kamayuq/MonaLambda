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

namespace Detail
{

template<typename S, typename A>
struct Pair : private equal_comparable<Pair<S, A>>
{
	typedef A ValueType;
	typedef S StateType;

	Pair(const S& state, const A& value) : state(state), value(value) {}
	S state;
	A value;

	ML_INLINE bool equal_to(const Pair<S, A>& rhs) const
	{
		return state == rhs.state
			&& value == rhs.value;
	}
};

template<typename S, typename A>
auto make_Pair(const S& state, const A& value)
{
	return Pair<S, A>(state, value);
}

} //namespace Detail

class State
{
	class StateBase
	{
	};

public:
	typedef StateBase MonadBase;
	typedef State	  MonadType;

	template<typename A>
	static ML_INLINE auto Return(const A& a)
	{
		auto ret = [a](auto s)
		{
			return Detail::make_Pair(s, a);
		};

		return Detail::make_Monad<MonadType, A>(ret);
	}

	template<typename M, typename B = typename M::MonadValueType, typename K>
	static ML_INLINE auto Bind(const M& m, const K& k)
	{
		Detail::checkMonadConstraints<M>();
		Detail::checkValueConstraints<B>();

		auto bind = [m, k](auto s) // std::pair<B, S>
		{
			auto p = m(s); // std::pair<A, S>

			static_assert(function_traits::is_callable<decltype(k)(decltype(p.value))>::value, "the state parameter has mismatched type");
			auto b = k(p.value);
			return b(p.state);
		};

		return Detail::make_Monad<MonadType, B>(bind);
	}

	template<typename M, typename S>
	static auto Eval(const M& m, const S& s)
	{
		return m(s);
	};

	template<typename S>
	ML_INLINE static auto Get()
	{
		auto get = [](auto s) { return Detail::make_Pair(s, s); };
		return Detail::make_Monad<MonadType, S>(get);
	}

	template<typename S>
	ML_INLINE static auto Put(const S& s)
	{
		auto put = [s](auto) { return Detail::make_Pair(s, Unit()); };
		return Detail::make_Monad<MonadType, Unit>(put);
	}
	
	template<typename F>
	ML_INLINE static auto Modify(const F& f)
	{
		auto modify = [f](auto s) { return Detail::make_Pair(f(s), Unit()); };
		return Detail::make_Monad<MonadType, Unit>(modify);
	}

	template
	<
		typename A, 
		typename B
	>
	ML_INLINE static auto Seq(const A& a, const B& b)
	{
		return a.Bind([b](auto) { return b; });
	}
};

template<typename OtherMonadType>
class StateT
{
	class StateTBase
	{
	};

public:
	typedef StateTBase							MonadBase;
	typedef StateT<OtherMonadType>				MonadType;
	typedef typename OtherMonadType::MonadType	InnerMonadType;

	template<typename A>
	static ML_INLINE auto Return(const A& a)
	{
		auto ret = [a](auto s)
		{
			auto inner =  InnerMonadType::Return(Detail::make_Pair(s, a));
			return Detail::toMonadBase(inner);
		};
		return Detail::make_Monad<MonadType, A>(ret);
	}

	template<typename M, typename B = typename M::MonadValueType, typename K>
	static ML_INLINE auto Bind(const M& m, const K& k)
	{
		Detail::checkMonadConstraints<M>();
		Detail::checkValueConstraints<B>();

		auto bind = [m, k](auto s)
		{
			auto mp = m(s);
			auto inner = InnerMonadType::Bind(Detail::toMonadBase(mp), [k](auto p)
			{
				auto v2 = p.value;
				auto s2 = p.state;
				auto b = k(v2);
				return b(s2);
			});
			return Detail::toMonadBase(inner);
		};
		return Detail::make_Monad<MonadType, B>(bind);
	}

	template<typename M, typename S, typename... Args>
	static auto Eval(const M& m, const S& s, const Args&... args)
	{
		return InnerMonadType::Eval(m(s), args...);
	}

	template<typename MA>
	ML_INLINE static auto ReturnM(const MA& ma)
	{
		auto retM = [ma](auto s)
		{
			auto inner = InnerMonadType::Bind(toMonadBase(ma), [s, ma](auto a)
			{
				return MonadType::Return(ma)(s);
			});
			return Detail::toMonadBase(inner);
		};
		return Detail::make_Monad<MonadType, MA>(retM);
	}

	template<typename S>
	ML_INLINE static auto Get()
	{
		auto get = [](auto s)
		{
			auto inner = InnerMonadType::Return(Detail::make_Pair(s, s));
			return Detail::toMonadBase(inner);
		};
		return Detail::make_Monad<MonadType, S>(get);
	}

	template<typename S>
	ML_INLINE static auto Put(const S& s)
	{
		auto put = [s](auto)
		{
			auto inner = InnerMonadType::Return(Detail::make_Pair(s, Unit()));
			return Detail::toMonadBase(inner);
		};
		return Detail::make_Monad<MonadType, Unit>(put);
	}
	
	template<typename F>
	ML_INLINE static auto Modify(const F& f)
	{
		auto modify = [f](auto s)
		{
			auto inner = InnerMonadType::Return(Detail::make_Pair(f(s), Unit()));
			return Detail::toMonadBase(inner);
		};
		return Detail::make_Monad<MonadType, Unit>(modify);
	}

	template
	<
		typename A, 
		typename B
	>
	ML_INLINE static auto Seq(const A& a, const B& b)
	{
		return a.Bind([b](auto) { return b; });
	}
};

}; //namespace Monadic