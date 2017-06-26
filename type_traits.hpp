#ifndef STRONG_CHECKING_HPP_
#define STRONG_CHECKING_HPP_

// implementation of N4502
#include <type_traits>
#include <tuple>
#include <utility>

// void_t
namespace detail {
template <class...> struct voider { using type = void; };
}
template <class... T> using void_t = typename detail::voider<T...>::type;

template <class Default, class, template <class...> class Op, class... Args>
struct detector {
  using value_t = std::false_type;
  using type = Default;
};

template <class Default, template <class...> class Op, class... Args>
struct detector<Default, void_t<Op<Args...>>, Op, Args...> {
  using value_t = std::true_type;
  using type = Op<Args...>;
};

struct nonesuch {
  nonesuch() = delete;
  ~nonesuch() = delete;
  nonesuch(nonesuch const&) = delete;
  void operator=(nonesuch const&) = delete;
};

template <template<class...> class Op, class... Args>
using is_detected = typename detector<nonesuch, void, Op, Args...>::value_t;

template <template<class...> class Op, class... Args>
using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

template <class Default, template<class...> class Op, class... Args>
using detected_or = detector<Default, void, Op, Args...>;

template <class Default, template<class...> class Op, class... Args>
using detected_or_t = typename detected_or<Default, Op, Args...>::type;

template <class Expected, template<class...> class Op, class... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

template <class To, template <class...> class Op, class... Args>
using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

namespace check {
struct ignore {
  ignore() = delete;
  ~ignore() = delete;
  ignore(ignore const &) = delete;
  ignore(ignore &&) = delete;
  ignore& operator= (ignore const &) = delete;
};

template <typename ClassType, typename ReturnType, template<class...> class Checker>
struct ReturnTypeChecker : public std::conditional_t<
  std::is_same<ReturnType, ignore>::value,
    is_detected<Checker, ClassType>,
    is_detected_exact<ReturnType, Checker, ClassType>> {};
}

#define DEFINE_CHECK(TAG, MODIFIER, RETURN, NAME, ...) \
template <typename, typename, typename> struct __ ## TAG ## _CheckCallImpl; \
template <typename T_, typename Tuple, int... Idx> \
struct __ ## TAG ## _CheckCallImpl <T_, Tuple, std::index_sequence<Idx...>> { \
  using type = decltype(std::declval<T_ MODIFIER>(). NAME(std::declval<std::tuple_element_t<Idx, Tuple>>()...)); \
}; \
\
struct __ ## TAG ## __ { \
  using Args = std::tuple< __VA_ARGS__ >; \
  template <typename T_> \
  using CheckCall = typename __ ## TAG ## _CheckCallImpl<T_, Args, std::make_index_sequence<std::tuple_size<Args>::value>>::type; \
  template <typename T_> \
  using has = check::ReturnTypeChecker<T_, RETURN, CheckCall>; \
}; \
\
template <typename T> constexpr bool Requires ## TAG () { \
   static_assert(__ ## TAG ## __::has<T>::value, "Missing Concept (name=" #TAG "): {" #MODIFIER "} " #NAME "(" #__VA_ARGS__ ") -> " #RETURN ); \
   return __ ## TAG ## __::has<T>::value; \
}

#endif
