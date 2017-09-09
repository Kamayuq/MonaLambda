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

	inline bool equal_to(const None& rhs) const
	{
		return message == rhs.message;
	}
};

template<typename Some>
class Option 
	: private equal_comparable<Option<Some>>
{
	enum Tag
	{
		Something,
		Nothing,
	};
	const Tag tag;

public:
	typedef Option<Some> ThisType;

	union 
	{
		const Some something;
		const None nothing;		
	};
		
	Option(const Some& something) : tag(Something), something(something) {}
	Option(const None& none) : tag(Nothing), nothing(none) {}

	inline bool equal_to(const Option<Some>& rhs) const
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

	inline bool isSome() const
	{
		return tag == Something;
	}

	static ThisType failWith(const None& nothing)
	{
		return { nothing };
	}

};

class Maybe : public Monad<Maybe>
{
public:
	template<typename A, typename K>
	static auto Bind(const Option<A>& m, const K& k)
	{
		if (m.isSome())
		{
			return k(m.something);
		}
		else
		{
			typedef decltype(k(m.something)) OptionB;
			return OptionB::failWith(m.nothing);
		}
	}

	template<typename A>
	static auto Return(const A& a)
	{
		return Option<A>(a);
	}

	template<typename A = const char*>
	static auto FailWith(const char* message)
	{
		return Option<A>(None(message));
	}
};

template<typename Inner>
class MaybeT : public Monad<MaybeT<Inner>>
{
public:
	template<typename M, typename K>
	static auto Bind(const M& ma, const K& k)
	{
		return Inner::Bind(ma, [k](auto m) constexpr
		{
			return Inner::Bind(k(m.something), [m](auto b) constexpr
			{
				typedef decltype(b) B;
				if (m.isSome())
				{
					return Inner::Return(b);
				}
				else
				{
					return Inner::Return(B::failWith(m.nothing));
				}
			});
		});
	}

	template<typename A>
	static auto Return(const A& a)
	{
		return Inner::Return(Option<A>(a));
	}

	template<typename MA>
	static auto ReturnM(const MA& ma)
	{
		return Inner::Bind(ma, [ma](auto a) constexpr
		{
			return Return(ma);
		});
	}

	template<typename A = const char*>
	static auto FailWith(const char* message)
	{
		return Inner::Return(Option<A>(None(message)));
	}
};

}; //namespace Monadic