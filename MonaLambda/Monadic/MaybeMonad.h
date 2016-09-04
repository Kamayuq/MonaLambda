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
#include <assert.h>
#include "MonadBuilder.h"

namespace Monadic
{

struct None : private equal_comparable<None>
{
	const char* message;
	None() : message("Unknown") {}
	None(const char* message) : message(message)
	{
		assert(message != nullptr);
	}

	ML_INLINE bool equal_to(const None& rhs) const
	{
		return message == rhs.message;
	}
};

namespace Detail
{

template<typename Some>
class Option 
	:  private equal_comparable<Option<Some>>
{
	enum Tag
	{
		Something,
		Nothing,
	};
	const Tag tag;

public:
	typedef Option<Some> OptionBaseType;

	union 
	{
		const Some something;
		const None nothing;		
	};
		
	//Option() : tag(Nothing), nothing("InvalidOption") {}
	Option(const Some& something) : tag(Something), something(something) {}
	Option(const None& none) : tag(Nothing), nothing(none) {}
	//Option(const Option& option) : tag(option.tag), something(option.something), nothing(option.nothing) {}

	ML_INLINE bool equal_to(const Option<Some>& rhs) const
	{
		if (tag == rhs.tag)
		{
			if (tag == Something)
				return something == rhs.something;
			else
				return nothing == rhs.nothing;
		}
		return false;
	}

	ML_INLINE bool isValid() const
	{
		return tag == Something;
	}

	static Option failWith(const None& nothing)
	{
		return { nothing };
	}
};

}; //namespace Detail

class Maybe
{
	class MaybeBase
	{
	};

public: 
	typedef MaybeBase	MonadBase;
	typedef Maybe		MonadType;

	template<typename A>
	static ML_INLINE auto Return(const A& a)
	{
		typedef typename std::decay<decltype(a)>::type A;
		return Detail::make_Monad<MonadType, A>(Detail::Option<A>(a));
	}

	template<typename M, typename B = typename M::MonadValueType, typename K>
	static ML_INLINE auto Bind(const M& m, const K& k)
	{
		Detail::checkMonadConstraints<M>();
		Detail::checkValueConstraints<B>();

		typedef decltype(k(m.something)) OptionA;
		typedef typename OptionA::OptionBaseType OptionB;

		OptionB b = m.isValid() ? k(m.something) : OptionB::failWith(m.nothing);
		return Detail::make_Monad<MonadType, B>(b);
	}

	template <typename M>
	static typename M::OptionBaseType Eval(const M& m)
	{
		return m;
	}

	template<typename A = const char*>
	ML_INLINE static auto FailWith(const char* message)
	{
		auto ma = Detail::Option<A>(None(message));
		return Detail::make_Monad<MonadType, A>(ma);
	}
};

template<typename OtherMonadType>
class MaybeT
{
	class MaybeTBase
	{
	};

public:
	typedef MaybeTBase							MonadBase;
	typedef MaybeT<OtherMonadType>				MonadType;
	typedef typename OtherMonadType::MonadType	InnerMonadType;

	template<typename A>
	static ML_INLINE auto Return(const A& a)
	{
		auto inner = InnerMonadType::Return(Detail::Option<A>(a));
		return Detail::make_Monad<MonadType, A>(Detail::toMonadBase(inner));
	}

	template<typename M, typename B = typename M::MonadValueType, typename K>
	static ML_INLINE auto Bind(const M& ma, const K& k)
	{
		Detail::checkMonadConstraints<M>();
		Detail::checkValueConstraints<B>();

		auto inner = InnerMonadType::Bind(Detail::toMonadBase(ma), [k](auto m)
		{
			auto mb = k(m.something); //TODO This is scary
			return InnerMonadType::Bind(Detail::toMonadBase(mb), [m](auto b)
			{
				typedef typename decltype(b)::OptionBaseType OptionB;
				OptionB r = m.isValid() ? b : OptionB::failWith(m.nothing);
				return InnerMonadType::Return(r);
			});
		});
		return Detail::make_Monad<MonadType, B>(Detail::toMonadBase(inner));
	}

	template<typename M, typename... Args>
	static auto Eval(const M& ma, const Args&... args)
	{
		auto mb = InnerMonadType::Eval(ma, args...);
		return mb;
	}

	template<typename MA>
	ML_INLINE static auto ReturnM(const MA& ma)
	{
		auto inner = InnerMonadType::Bind(Detail::toMonadBase(ma), [ma](auto a)
		{
			return MonadType::Return(ma);
		});
		typedef typename MA::MonadValueType A;
		return Detail::make_Monad<MonadType, A>(Detail::toMonadBase(inner));
	}

	template<typename A = const char*>
	ML_INLINE static auto FailWith(const char* message)
	{
		auto ma = InnerMonadType::Return(Detail::Option<A>(None(message)));
		return Detail::make_Monad<MonadType, A>(ma);
	}
};

}; //namespace Monadic