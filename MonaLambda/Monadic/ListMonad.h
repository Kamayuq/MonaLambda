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

class ListM : public Monad<ListM>
{
	typedef Monad<ListM> BaseType;
	template<typename L>
	using  WrappedMonad = typename BaseType::template WrappedMonad<L>;

public:
	template<typename M, typename K>
	constexpr static auto Bind(const WrappedMonad<M>& m, const K& k)
	{
		return  BaseType::WrapMonad(List::Flatmap(k)(m));
	}

	template<typename A>
	constexpr static auto Return(const A& a)
	{
		return BaseType::WrapMonad(List::Make(a));
	}
};

template<typename Inner>
class ListT : public Monad<ListT<Inner>>
{
	typedef Monad<ListT<Inner>> BaseType;
	template<typename L>
	using  WrappedMonad = typename BaseType::template WrappedMonad<L>;

	typedef Monad<Inner> InnerBaseType;
	template<typename L>
	using  InnerWrappedMonad = typename InnerBaseType::template WrappedMonad<L>;

public:
	template<typename M, typename K>
	constexpr static auto Bind(const WrappedMonad<M>& ma, const K& k)
	{
		return BaseType::WrapMonad(List::Fmap([k](auto m) constexpr
		{
			return Inner::Bind(m, [k](auto a) constexpr
			{
				auto mb = k(a);
				return mb.Head();
			});
		})(ma));
	}

	template<typename A>
	constexpr static auto Return(const A& a)
	{
		return BaseType::WrapMonad(ListM::Return(Inner::Return(a)));
	}

	template<typename MA>
	static auto ReturnM(const InnerWrappedMonad<MA>& ma)
	{
		return BaseType::WrapMonad(ListM::Return(ma).Unwrap());
	}
};

}; //namespace Monadic