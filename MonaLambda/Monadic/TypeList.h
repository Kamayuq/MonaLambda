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

#define TL_INLINE inline

namespace Monadic
{

namespace ListInternal
{
	struct ListBase
	{
	};

	auto List = [](auto ...xs)
	{
		return[xs...](auto a) { return a(xs...); };
	};

	auto Head = [](auto list)
	{
		return list([](auto head, auto ...tail) { return head; });
	};

	auto Tail = [](auto list)
	{
		return list([](auto head, auto ...tail) { return List(tail...); });
	};
};

template<typename L1, size_t S1>
struct ListL 
	: public L1
	, public ListInternal::ListBase
	, private equal_comparable<ListL<L1, S1>>
{
public:
	ListL(const L1& l) : L1(l) 
	{
		static_assert(!std::is_convertible<L1*, ListInternal::ListBase*>::value, "L is already type of a List try not to nest as it might cause errors and long compile times");
	}

	auto Head() const 
	-> decltype(ListInternal::Head(std::declval<L1>())) //MSVSClang debug info is crashing without this 
	{
		L1 l1 = *this; 
		return ListInternal::Head(l1);
	}

	auto Tail() const 
	-> ListL<decltype(ListInternal::Tail(std::declval<L1>())), S1 - 1> //MSVSClang debug info is crashing without this 
	{
		L1 l1 = *this;
		auto list = ListInternal::Tail(l1);
		return ListL<decltype(list), S1 - 1>(list);
	}

	constexpr size_t Length() const
	{
		return S1;
	};

	template<typename L2, size_t S2>
	TL_INLINE bool equal_to(const ListL<L2, S2>& l2) const
	{
		if (S1 == S2)
		{
			auto& l1 = (*this);
			return InternalEq<L1, L2, S1>::equal_to(l1, l2);
		}
		else
		{
			return false;
		}
	}

private:
	template<typename L1, typename L2, size_t S>
	struct InternalEq
	{
		TL_INLINE static bool equal_to(const ListL<L1, S>& l1, const ListL<L2, S>& l2)
		{
			if (l1.Head() == l2.Head())
			{
				return l1.Tail() == l2.Tail();
			}
			return false;
		}
	};

	template<typename L1, typename L2>
	struct InternalEq<L1, L2, 0>
	{
		TL_INLINE static bool equal_to(const ListL<L1, 0>& l1, const ListL<L2, 0>& l2)
		{
			return true;
		}
	};
};

template<typename L1>
struct ListL<L1, 0>
	: public L1
	, public ListInternal::ListBase
	, private equal_comparable<ListL<L1, 0>>
{
public:
	ListL(const L1& l) : L1(l)
	{
		static_assert(!std::is_convertible<L1*, ListInternal::ListBase*>::value, "L is already type of a List try not to nest as it might cause errors and long compile times");
	}

	auto Head() const 
	-> Unit //MSVSClang debug info is crashing without this 
	{
		return Unit();
	}

	auto Tail() const 
	-> ListL<L1, 0> //MSVSClang debug info is crashing without this 
	{
		return *this;
	}

	constexpr size_t Length() const
	{
		return 0;
	};

	template<typename L2, size_t S2>
	TL_INLINE bool equal_to(const ListL<L2, S2>& l2) const
	{
		if (S2 == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

struct List
{
	template<typename... Args>
	TL_INLINE static auto Make(const Args&... xs)
	-> ListL<decltype(ListInternal::List(xs...)), sizeof...(xs)> //MSVSClang debug info is crashing without this 
	{
		auto list = ListInternal::List(xs...);
		return ListL<decltype(list), sizeof...(xs)>(list);
	}

	template<typename H, typename L1, size_t S1>
	TL_INLINE static auto Cons(const H& head, const ListL<L1, S1>& list)
	{
		return list([=](auto ...xs) { return List::Make(head, xs...); });
	};

	template<typename L1, size_t S1, typename L2, size_t S2>
	TL_INLINE static auto Concat(const ListL<L1, S1>& list1, const ListL<L2, S2>& list2)
	{
		auto a1 = [=](auto ...xs)
		{
			auto a2 = [=](auto ...ys)
			{
				return List::Make(xs..., ys...);
			};
			return list2(a2);
		};
		return list1(a1);
	};

	template<typename F>
	TL_INLINE static auto Fmap(const F& f)
	{
		return [f](auto list)
		{
			return list([f](auto ...xs) { return List::Make(f(xs)...); });
		};
	};

	template <typename F>
	TL_INLINE static auto Flatten(const F&)
	{
		return List::Make();
	}

	template <typename F, typename A, typename... B>
	TL_INLINE static auto Flatten(const F& f, const A& a, const B&... b)
	{
		return Concat(f(a), Flatten(f, b...));
	}

	template <typename F>
	TL_INLINE static auto Flatmap(const F& f)
	{
		return [f](auto list)
		{
			return list([f](auto... xs) { return Flatten(f, xs...);  });
		};
	};
};

};


/*void debugTypeList()
{
	using namespace TypeList;

	auto list1 = List(1, 2.0f);
	auto list2 = Cons('s', list1);
	auto v2 = Head(list2);
	(void)v2;

	auto list5 = Tail(list1);
	(void)list5;

	auto list3 = List();
	//auto v3 = Head(list3);
	auto list4 = Cons(1, list3);
	(void)list4;

	int integer = 7;
	auto list6 = List(integer);
	auto s1 = Length(list2);
	(void)s1;

	auto list7 = Concat(list2, list6);
	auto pair = [](auto i) { return List(-i, i); };
	auto print = [](auto i) { std::cout << i << " "; return i; };

	auto list8 = Flatmap(pair)(list7);
	auto list9 = Fmap(print)(list8);
	(void)list9;
}*/