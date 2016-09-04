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
#include "TypeList.h"

namespace Monadic
{

class ListM
{
	class ListBase
	{
	};

public:
	typedef ListBase MonadBase;
	typedef ListM	 MonadType;

	template<typename A>
	ML_INLINE static auto Return(const A& a)
	{
		auto ret = List::Make(a);
		return Detail::make_Monad<MonadType, A>(ret);
	}

	template<typename M, typename B = typename M::MonadValueType, typename K>
	ML_INLINE static auto Bind(const M& m, const K& k)
	{
		Detail::checkMonadConstraints<M>();
		Detail::checkValueConstraints<B>();

		auto bind = List::Flatmap(k)(m);
		return Detail::make_Monad<MonadType, B>(bind);
	}

	template<typename M>
	static auto Eval(const M& m)
	{
		return m;
	}
};

template<typename OtherMonadType>
class ListT
{
	class ListTBase
	{
	};

public:
	typedef ListTBase							MonadBase;
	typedef ListT<OtherMonadType>				MonadType;
	typedef typename OtherMonadType::MonadType	InnerMonadType;

	template<typename A>
	ML_INLINE static auto Return(const A& a)
	{
		auto inner = InnerMonadType::Return(a);
		auto ret = ListM::Return(Detail::toMonadBase(inner));
		return Detail::make_Monad<MonadType, A>(ret);
	}

	template<typename M, typename B = typename M::MonadValueType, typename K>
	ML_INLINE static auto Bind(const M& ma, const K& k)
	{
		Detail::checkMonadConstraints<M>();
		Detail::checkValueConstraints<B>();

		auto bind = List::Fmap([k](auto m)
		{
			auto inner = InnerMonadType::Bind(Detail::toMonadBase(m), [k](auto a)
			{
				auto mb = k(a);
				return mb.Head();
			});
			return Detail::toMonadBase(inner);
		});
		auto mb = bind(ma);
		return Detail::make_Monad<MonadType, B>(mb);
	}

	template<typename M, typename... Args>
	static auto Eval(const M& ma, const Args&... args)
	{
		return List::Fmap([args...](auto m)
		{
			auto inner = InnerMonadType::Eval(m, args...);
			return Detail::toMonadBase(inner);
		})(ma);
	}

	template<typename MA>
	ML_INLINE static auto ReturnM(const MA& ma)
	{
		auto retM = ListM::Return(ma);
		typedef typename MA::MonadValueType A;
		return Detail::make_Monad<MonadType, A>(retM);
	}
};

}; //namespace Monadic