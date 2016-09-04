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

class Cont
{
	class ContBase
	{
	};

public:
	typedef ContBase MonadBase;
	typedef Cont	 MonadType;

	template<typename A>
	ML_INLINE static auto Return(const A& a)
	{
		auto ret = [a](auto k /* 'a->'r */)
		{
			return k(a);
		};

		return Detail::make_Monad<MonadType, A>(ret);
	}

	template<typename M, typename B = typename M::MonadValueType, typename K>
	ML_INLINE static auto Bind(const M& m, const K& k)
	{
		Detail::checkMonadConstraints<M>();
		Detail::checkValueConstraints<B>();

		auto bind = [m, k](auto b /* (b -> r) -> r */)
		{
			auto f1 = [b, k](auto a)
			{
				auto f2 = k(a);
				return f2(b); /* (b -> r) -> r */
			};

			return m(f1);
		};

		return Detail::make_Monad<MonadType, B>(bind);
	}

	template<typename M>
	static auto Eval(const M& m)
	{
		auto f = [](auto v) { return v; };
		return m(f);
	}
};

template<typename OtherMonadType>
class ContT
{
	class ContTBase
	{
	};

public:
	typedef ContTBase							MonadBase;
	typedef ContT<OtherMonadType>				MonadType;
	typedef typename OtherMonadType::MonadType	InnerMonadType;

	template<typename A>
	ML_INLINE static auto Return(const A& a)
	{
		auto ret = [a](auto k /* 'a->'r */)
		{
			auto inner = InnerMonadType::Return(k(a));
			return Detail::toMonadBase(inner);
		};

		return Detail::make_Monad<MonadType, A>(ret);
	}

	template<typename M, typename B = typename M::MonadValueType, typename K>
	ML_INLINE static auto Bind(const M& m, const K& k)
	{
		Detail::checkMonadConstraints<M>();
		Detail::checkValueConstraints<B>();

		auto bind = [m, k](auto b /* (b -> r) -> r */)
		{
			auto f1 = [b, k](auto a)
			{
				auto f2 = k(a);
				return f2(b); /* (b -> r) -> r */
			};

			auto mb = m(f1);

			auto inner = InnerMonadType::Bind(Detail::toMonadBase(mb), [](auto b)
			{
				return b;
			});
			return Detail::toMonadBase(inner);
		};

		return Detail::make_Monad<MonadType, B>(bind);
	}

	template<typename M, typename... Args>
	static auto Eval(const M& m, const Args&... args)
	{
		auto f = [](auto v) { return v; };
		auto mb = m(f);
		return InnerMonadType::Eval(mb, args...);
	}

	template<typename MA>
	ML_INLINE static auto ReturnM(const MA& ma)
	{
		auto retM = [ma](auto k /* 'a->'r */)
		{
			auto inner = InnerMonadType::Bind(Detail::toMonadBase(ma), [k, ma](auto a)
			{
				return MonadType::Return(ma)(k);
			});
			return Detail::toMonadBase(inner);
		};
		return Detail::make_Monad<MonadType, typename MA::MonadInstanceType>(retM);
	}
};

}; //namespace Monadic