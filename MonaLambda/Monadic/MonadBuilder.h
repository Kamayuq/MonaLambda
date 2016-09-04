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

#define ML_INLINE inline

#ifdef _MSC_VER
	#pragma warning(disable : 4503)
#endif

namespace Monadic
{

namespace Detail
{

template<typename T>
class bindable
{
	template<typename K>
	friend ML_INLINE auto operator >= (const T &m, const K& k)
	{
		return m.Bind(k);
	}

	template<typename K>
	friend ML_INLINE auto operator > (const T &m, const K& k)
	{
		return m >= [k](auto _) { return k; };
	}
};

template<typename T, typename A>
class assignable
{
	friend ML_INLINE auto operator <<= (A& a, const T &m)
	{
		return m.AssignMonadicValue(&a);
	}
};

struct ImmutableType {};
struct AssignedType {};

class MonadBase1 {};
class MonadBase2 {};

template<typename A, typename M>
class MonadClass : public M, public MonadBase1
{
public:
	MonadClass(const M& m) : M(m)
	{
		static_assert(!std::is_base_of<MonadBase1, M>::value, "M is already type of a Monad try not to nest as it might cause errors and long compile times");
	}

	typedef A MonadValueType;
	typedef M MonadBaseType;
	typedef MonadClass<A, M> MonadClassType;
};

template<typename M>
auto toMonadBaseInternal(const std::true_type&, const M& m)
-> typename M::MonadClassType //MSVSClang debug info is crashing without this 
{
	typedef typename M::MonadClassType MB;
	return *static_cast<const MB*>(&m);
}

template<typename M>
auto toMonadBaseInternal(const std::false_type&, const M& m) 
-> M //MSVSClang debug info is crashing without this 
{
	return m;
}

template<typename M>
auto toMonadBase(const M& m) 
-> decltype(toMonadBaseInternal(typename std::is_convertible<const M*, const MonadBase1*>::type(), m)) //MSVSClang debug info is crashing without this 
{
	//is_base_of tries to instantiate the derived type when clang for MS is used
	return toMonadBaseInternal(typename std::is_convertible<const M*, const MonadBase1*>::type(), m);
};

template<typename A, typename K, class>
class LambdaWrapper : public K
{
public:
	LambdaWrapper(const K& k, A* a) : K(k) {}
};

template<typename A, typename K>
class LambdaWrapper<A, K, AssignedType> : protected K
{
	A* a;

public:
	LambdaWrapper(const K& k, A* a) : K(k), a(a) {}

	ML_INLINE auto operator() (A value) const
	-> decltype(K::operator()(value)) //MSVSClang debug info is crashing without this 
	{
		*a = value;
		return K::operator()(value);
	}
};

template<typename M, bool CheckBase = true>
void checkMonadConstraints()
{
	static_assert(!CheckBase || std::is_base_of<MonadBase1, M>::value, "Type is not a Monad Type");
	static_assert(!std::is_base_of<MonadBase2, M>::value, "Deep nesting of Monadtypes bloats the MangledNames and might cause VC++ to truncate it");
}

template<typename A>
void checkValueConstraints()
{
	static_assert(!std::is_base_of<MonadBase2, A>::value, "Using Monadtypes as values bloats the MangledNames and might cause VC++ to truncate it");
}

template<class MonadType, typename A, typename M, typename C = ImmutableType>
class Monad
	: public MonadClass<A, M>
	, public MonadType::MonadBase
	, public MonadBase2
	, private bindable<Monad<MonadType, A, M, C>>
	, private assignable<Monad<MonadType, A, M, C>, A>
{
private:
	A* a = nullptr;

public:
	typedef MonadClass<A, M> MonadClassType;
	typedef Monad<MonadType, A, M, C> MonadInstanceType;

	Monad(const M& m, A* a = nullptr) : MonadClassType(m), a(a) 
	{
		checkMonadConstraints<M, false>();
		checkValueConstraints<A>();
	}

	template<typename A>
	ML_INLINE static auto Return(const A& a)
	//-> decltype(MonadType::Return(a)) //MSVSClang debug info is crashing without this 
	{
		return MonadType::Return(a);
	}

	template
	<
		typename K,
		typename LW = LambdaWrapper<A, K, C>,
		typename B = typename std::result_of<K(A)>::type::MonadValueType
	>
	ML_INLINE auto Bind(const K& k) const 
	//-> decltype(MonadType:: template Bind<MonadClassType, B>(toMonadBase(*this), std::declval<LW>())) //MSVSClang debug info is crashing without this 
	{
#ifndef NDEBUG
		static_assert(function_traits::is_callable<K(A)>::value, "the parameter of the function bound to the instance of the monad is incompatible with the type of the monad. Have you left a trailing comma in the last statement of a DO notation?");
		typedef typename std::result_of<K(A)>::type ResultType;
		static_assert(!std::is_same<void, ResultType>::value, "returning nothing is not possible use 'return Return(Unit());' instead");
		static_assert(std::is_convertible<ResultType*, typename MonadType::MonadBase*>::value, "the function result is not type of the monad are you using the right Return?");
#endif	
		LW k2(k, a);
		return MonadType:: template Bind<MonadClassType, B>(toMonadBase(*this), k2);
	}

	template<typename... Args>
	auto Eval(const Args&... args) const 
	-> decltype(MonadType::Eval(toMonadBase(*this), args...)) //MSVSClang debug info is crashing without this 
	{
		return MonadType::Eval(toMonadBase(*this), args...);
	};

	auto AssignMonadicValue(A* a) const 
	-> Monad<MonadType, A, M, AssignedType> //MSVSClang debug info is crashing without this 
	{
		static_assert(!std::is_convertible<A*, MonadBase1*>::value, "Monadic values cannot be assigned");
		return Monad<MonadType, A, M, AssignedType>(toMonadBase(*this), a);
	}
};

template<class MonadType, typename A, typename M>
auto make_Monad_Internal(const std::true_type&, const std::true_type&, const M& m) 
-> Monad<MonadType, typename A::MonadBaseType, typename M::MonadBaseType> //MSVSClang debug info is crashing without this 
{
	typedef typename A::MonadBaseType AB;
	typedef typename M::MonadBaseType MB;
	return Monad<MonadType, AB, MB>(m);
}

template<class MonadType, typename A, typename M>
auto make_Monad_Internal(const std::true_type&, const std::false_type&, const M& m) 
-> Monad<MonadType, typename A::MonadBaseType, M> //MSVSClang debug info is crashing without this 
{
	typedef typename A::MonadBaseType AB;
	return Monad<MonadType, AB, M>(m);
}

template<class MonadType, typename A, typename M>
auto make_Monad_Internal(const std::false_type&, const std::true_type&, const M& m) 
-> Monad<MonadType, A, typename M::MonadBaseType> //MSVSClang debug info is crashing without this 
{
	typedef typename M::MonadBaseType MB;
	return Monad<MonadType, A, MB>(m);
}

template<class MonadType, typename A, typename M>
auto make_Monad_Internal(const std::false_type&, const std::false_type&, const M& m) 
-> Monad<MonadType, A, M> //MSVSClang debug info is crashing without this 
{
	return Monad<MonadType, A, M>(m);
}

template<class MonadType, typename A, typename M>
auto make_Monad(const M& m) 
-> decltype(make_Monad_Internal<MonadType, A, M>(typename std::is_base_of<MonadBase1, A>::type(), typename std::is_base_of<MonadBase1, M>::type(), m)) //MSVSClang debug info is crashing without this 
{
	return make_Monad_Internal<MonadType, A, M>(typename std::is_base_of<MonadBase1, A>::type(), typename std::is_base_of<MonadBase1, M>::type(), m);
}
//#define make_Monad(e) make_Monad(e)

}; //namespace Detail

template<typename F, typename... Args>
struct LiftMonad
{
	template<typename MonadUtilType>
	static auto liftM(const F& f)
	{
		typedef typename MonadUtilType::MonadType MonadType;
		static_assert(function_traits::is_callable<F(Args...)>::value, "lift cannot be called with this arguments");
		return [f](Args... args)
		{ 
			typedef typename std::result_of<F(Args...)>::type B;
			auto lift = MonadType::Return(f(args...));
			return Detail::make_Monad<MonadType, B>(lift);
		};
	}
};

template<typename F, typename... Args>
struct LiftMonad<F(Args...)> : LiftMonad<F, Args...> 
{

};

}; //namespace Monadic