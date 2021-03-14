#pragma once

#include <cstring>
#include <tuple>
#include <type_traits>

// quote 
template <template <typename...> class Cls>
struct quote {
    template <typename... Args>
    using apply = Cls<Args...>;
};
template <template <std::size_t...> class Cls>
struct quote_sizet {
    template <std::size_t... Args>
    using apply = Cls<Args...>;
};

// list 
template <typename... Args>
struct typelist {
    static constexpr std::size_t size = sizeof...(Args);
};

template <std::size_t... Args>
struct sizetlist {
    static constexpr std::size_t size = sizeof...(Args);
};

template<template<class...> class T, class... Args>
typelist<Args...> do_as_typelist(typelist<T<Args...>>);

template<class T>
using as_typelist = decltype(do_as_typelist(typelist<T>()));

// apply
template <typename F, typename TL>
struct apply;

template <typename F, typename... Args>
struct apply<F, typelist<Args...>> {
    using type = typename F::template apply<Args...>;
};

template <typename F, typename TL>
using apply_t = typename apply<F, TL>::type;


// index_warpper
template<class T, std::size_t Index>
struct index_warpper {
    using type = T;
    static constexpr std::size_t index = Index;
};

template<class... Ts, std::size_t... Is>
typelist<index_warpper<Ts, Is>...> do_build_indices(typelist<Ts...>, std::index_sequence<Is...>);

template<class TL>
using build_indices = decltype(do_build_indices(TL(), std::make_index_sequence<TL::size>()));

// project
template<class F>
struct project_role {
    template<class... Args>
    using apply = typename F::template apply<typename Args::Role...>;
};

template<class F>
struct project_type {
    template<class... Args>
    using apply = typename F::template apply<typename Args::type...>;
};

template<class F>
struct project_index {
    template<typename... Args>
    using apply = typename F::template apply<Args::index...>;
};


// matchs
template<typename M, typename ABS, typename TL, typename TR, typename... Rest>
struct matchs;

template<typename M, typename ABS, typename TL, typename TR, typename... Rest>
using matchs_t = typename matchs<M, ABS, TL, TR, Rest...>::type;

template<typename M, typename AB, typename... ABS, typename Tl, typename... TL, typename Tr, typename... TR, typename I0, typename... I, typename... C, typename... CI, typename... CA, typename... Rest>
struct matchs <M, typelist<AB, ABS...>, typelist<Tl, TL...>, typelist<Tr, TR...>, typelist<I0, I...>, typelist<C...>, typelist<CI...>, typelist<CA...>, Rest...> {
    using type = matchs_t<M, typelist<ABS...>, typelist<Tl, TL...>, typelist<TR...>, typelist<I...>, typelist<C..., Tr>, typelist<CI..., I0>, typelist<CA..., AB>, Rest...>;
};
template<typename M, typename AB, typename... ABS, typename T, typename... TL, typename... TR, typename I0, typename... I, typename... C, typename... CI, typename... CA, typename... Rest>
struct matchs <M, typelist<AB, ABS...>, typelist<T, TL...>, typelist<T, TR...>, typelist<I0, I...>, typelist<C...>, typelist<CI...>, typelist<CA...>, Rest...> {
    using type = matchs_t<M, typelist<ABS...>, typelist<T, TL...>, typelist<TR...>, typelist<I...>, typelist<C..., T>, typelist<CI..., I0>, typelist<CA..., AB>, Rest...>;
};

template<typename M, typename... ABS, typename Tl, typename... TL, typename Tr, typename... TR, typename I0, typename... I, typename... C, typename... CI, typename... CA, typename... Rest>
struct matchs <M, typelist<M, ABS...>, typelist<Tl, TL...>, typelist<Tr, TR...>, typelist<I0, I...>, typelist<C...>, typelist<CI...>, typelist<CA...>, Rest...> {
    using type = matchs_t<M, typelist<>, typelist<Tl, TL...>, typelist<TR...>, typelist<I...>, typelist<C..., Tr>, typelist<CI..., I0>, typelist<CA..., M, ABS...>, Rest...>;
};

template<typename M, typename... ABS, typename T, typename... TL, typename... TR, typename I0, typename... I, typename... C, typename... CI, typename... CA, typename... Rest>
struct matchs <M, typelist<M, ABS...>, typelist<T, TL...>, typelist<T, TR...>, typelist<I0, I...>, typelist<C...>, typelist<CI...>, typelist<CA...>, Rest...> {
    using type = matchs_t<M, typelist<>, typelist<T, TL...>, typelist<TR...>, typelist<I...>, typelist<C..., T>, typelist<CI..., I0>, typelist<CA..., M, ABS...>, Rest...>;
};

template<typename M, typename Tl, typename... TL, typename Tr, typename... TR, typename I0, typename... I, typename... C, typename... CI, typename... CA, typename... Rest>
struct matchs <M, typelist<>, typelist<Tl, TL...>, typelist<Tr, TR...>, typelist<I0, I...>, typelist<C...>, typelist<CI...>, typelist<CA...>, Rest...> {
    using type = matchs_t<M, typelist<>, typelist<Tl, TL...>, typelist<TR...>, typelist<I...>, typelist<C..., Tr>, typelist<CI..., I0>, typelist<CA...>, Rest...>;
};

template<typename M, typename T, typename... TL, typename... TR, typename I0, typename... I, typename... C, typename... CI, typename... CA, typename... Rest>
struct matchs <M, typelist<>, typelist<T, TL...>, typelist<T, TR...>, typelist<I0, I...>, typelist<C...>, typelist<CI...>, typelist<CA...>, Rest...> {
    using type = matchs_t<M, typelist<CA...>, typelist<TL...>, typelist<C..., T, TR...>, typelist<CI..., I0, I...>, typelist<>, typelist<>, typelist<>, Rest..., I0>;
};

template<typename M, typename... TL, typename... C, typename... CI, typename... CA, typename... Rest>
struct matchs <M, typelist<>, typelist<TL...>, typelist<>, typelist<>, typelist<C...>, typelist<CI...>, typelist<CA...>, Rest...> {
    using type = matchs_t<M, typelist<>, typelist<TL...>, typelist<C...>, typelist<CI...>, typelist<>, typelist<>, typelist<CA...>, Rest...>;
};

template<typename M, typename... ABS, typename... TR, typename... I, typename... C, typename... CI, typename... CA, typename... Rest>
struct matchs<M, typelist<ABS...>, typelist<>, typelist<TR...>, typelist<I...>, typelist<C...>, typelist<CI...>, typelist<CA...>, Rest...> {
    using type = typelist<Rest...>;
};

template <std::size_t ... Is>
constexpr auto indexSequenceReverse(std::index_sequence<Is...> const&)
-> decltype(std::index_sequence<sizeof...(Is) - 1U - Is...>{});

template <std::size_t N>
using makeIndexSequenceReverse
= decltype(indexSequenceReverse(std::make_index_sequence<N>{}));


template <typename R, typename... D>
struct Ability {
    using Role = R;
    using DependsList = typelist<D...>;
    void asInit(...) {}
    auto asInvoke(...) { return -1; }
};

template <typename... Abilities>
class AbilityContainer {
    using AbilitisTuple = std::tuple<Abilities...>;
    AbilitisTuple _abilities;

public:
    AbilityContainer() {
        using Indices = makeIndexSequenceReverse<sizeof...(Abilities)>;
        loop_init(_abilities, Indices());
    }
    template<typename R>
    auto&& find() {
        using Indices = std::make_index_sequence<sizeof...(Abilities)>;
        auto&& matched = call_impl<R>(std::forward<AbilitisTuple>(_abilities), Indices());
        return matched;
    }
    template<typename R>
    auto&& rfind() {  // Reverse find
        using Indices = makeIndexSequenceReverse<sizeof...(Abilities)>;
        auto&& matched = call_impl<R>(std::forward<AbilitisTuple>(_abilities), Indices());
        return matched;
    }
    template<typename R, typename... Params>
    auto invoke(Params&&... params) {
        using Indices = std::make_index_sequence<sizeof...(Abilities)>;
        auto&& matched = call_impl<R>(std::forward<AbilitisTuple>(_abilities), Indices());
        return do_call(matched, params...);
    }

private:
    void init_one() {}
    template<typename A, typename... AB>
    void init_one(A&& a, AB&&... abs) {
        using list_to = typename std::decay_t<A>::DependsList;
        using list = as_typelist<AbilitisTuple>;
        using list_with_indexes = build_indices<list>;
        using list2role = apply_t<project_role<quote<typelist>>, list>;
        using matchedimap = matchs_t<
            std::decay_t<A>,
            list,
            list_to,
            list2role,
            list_with_indexes,
            typelist<>,
            typelist<>,
            typelist<>>;
        using indices = apply_t<project_index<quote_sizet<std::index_sequence>>, matchedimap>;
        do_init(a, _abilities, indices());
        init_one(abs...);
    }
    template<typename Tuple, std::size_t... I>
    auto loop_init(Tuple&& t, std::index_sequence<I...>) {
        return init_one(std::get<I>(std::forward<Tuple>(t))...);
    }

    template<typename M, typename Tuple, std::size_t... I>
    auto do_init(M&& m, Tuple&& t, std::index_sequence<I...>) {
        return m.asInit(std::get<I>(std::forward<Tuple>(t))...);
    }

    template<typename M, typename... Params>
    auto do_call(M&& m, Params&&... params) {
        return m.asInvoke(params...);
    }

    template<typename R, typename Tuple, std::size_t... I>
    auto&& call_impl(Tuple&& t, std::index_sequence<I...>) {
        return call_impl<R>(std::get<I>(std::forward<Tuple>(t))...);
    }
    template <typename R, typename First, typename... Left>
    auto&& call_impl(First&& first, Left&&... left) {
        if constexpr (std::is_same<typename std::decay<First>::type::Role, R>::value) {
            return first;
        }
        else {
            return call_impl<R>(left...);
        }
    }
    template <typename R, typename Last>
    auto&& call_impl(Last&& last) {
        return last;
    }
};
