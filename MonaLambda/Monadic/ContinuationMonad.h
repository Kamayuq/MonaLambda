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

class Cont : public Monad<Cont>
{
public:
	template<typename M, typename K>
	static auto Bind(const M& m, const K& k)
	{
		return [m, k](auto b /* (b -> r) -> r */) constexpr
		{
			return m([b, k](auto a) constexpr
			{
				auto f2 = k(a);
				return f2(b); /* (b -> r) -> r */
			});
		};
	}

	template<typename A>
	static auto Return(const A& a)
	{
		return [a] (auto k /* 'a->'r */) constexpr
		{
			return k(a);
		};
	}
};

template<typename Inner>
class ContT : public Monad<ContT<Inner>>
{
public:
	template<typename M, typename K>
	static auto Bind(const M& m, const K& k)
	{
		return [m, k](auto b /* (b -> r) -> r */) constexpr
		{
			auto f1 = [b, k](auto a) constexpr
			{
				auto f2 = k(a);
				return f2(b); /* (b -> r) -> r */
			};

			return Inner::Bind(m(f1), [](auto b) constexpr
			{
				return b;
			});
		};
	}

	template<typename A>
	static auto Return(const A& a)
	{
		return [a](auto k /* 'a->'r */) constexpr
		{
			return Inner::Return(k(a));
		};
	}

	template<typename MA>
	static auto ReturnM(const MA& ma)
	{
		return [ma](auto k /* 'a->'r */) constexpr
		{
			return Inner::Bind(ma, [k, ma](auto a) constexpr
			{
				return Return(ma)(k);
			});
		};
	}
};

}; //namespace Monadic