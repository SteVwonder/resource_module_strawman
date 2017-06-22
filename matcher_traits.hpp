#include <type_traits>

#include "resource_data.hpp"
#include "resource_graph.hpp"
#include "type_traits.hpp"

namespace traits {

namespace matcher {

namespace detail {

using namespace flux_resource_model;


#define FIRST(a, ...) a
#define SECOND(a, b, ...) b

#define EMPTY()

#define EVAL(...) EVAL1024(__VA_ARGS__)
#define EVAL1024(...) EVAL512(EVAL512(__VA_ARGS__))
#define EVAL512(...) EVAL256(EVAL256(__VA_ARGS__))
#define EVAL256(...) EVAL128(EVAL128(__VA_ARGS__))
#define EVAL128(...) EVAL64(EVAL64(__VA_ARGS__))
#define EVAL64(...) EVAL32(EVAL32(__VA_ARGS__))
#define EVAL32(...) EVAL16(EVAL16(__VA_ARGS__))
#define EVAL16(...) EVAL8(EVAL8(__VA_ARGS__))
#define EVAL8(...) EVAL4(EVAL4(__VA_ARGS__))
#define EVAL4(...) EVAL2(EVAL2(__VA_ARGS__))
#define EVAL2(...) EVAL1(EVAL1(__VA_ARGS__))
#define EVAL1(...) __VA_ARGS__

#define DEFER1(m) m EMPTY()
#define DEFER2(m) m EMPTY EMPTY()()
#define DEFER3(m) m EMPTY EMPTY EMPTY()()()
#define DEFER4(m) m EMPTY EMPTY EMPTY EMPTY()()()()

#define IS_PROBE(...) SECOND(__VA_ARGS__, 0)
#define PROBE() ~, 1

#define CAT(a,b) a ## b

#define NOT(x) IS_PROBE(CAT(_NOT_, x))
#define _NOT_0 PROBE()

#define BOOL(x) NOT(NOT(x))

#define IF_ELSE(condition) _IF_ELSE(BOOL(condition))
#define _IF_ELSE(condition) CAT(_IF_, condition)

#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_0(...)             _IF_0_ELSE

#define _IF_1_ELSE(...)
#define _IF_0_ELSE(...) __VA_ARGS__

#define HAS_ARGS(...) BOOL(FIRST(_END_OF_ARGUMENTS_ __VA_ARGS__)())
#define _END_OF_ARGUMENTS_() 0

#define MAP(m, first, ...)           \
  m(first)                           \
  IF_ELSE(HAS_ARGS(__VA_ARGS__))(    \
    DEFER2(_MAP)()(m, __VA_ARGS__)   \
  )(                                 \
    /* Do nothing, just terminate */ \
  )
#define _MAP() MAP

#define STR_(X) #X
#define STR(X) STR_(X)

#define WRAP_(X) std::declval<X>()

#define SIG_IMPL_NO_ARGS(NAME)                                  \
  template <typename T> using NAME = decltype(WRAP_(T&).NAME())

// TODO -- add commas between map args

#define SIG_IMPL_ARGS(NAME, ...)                                \
  template <typename T> using NAME = decltype(WRAP_(T&).NAME(EVAL(MAP(WRAP_, __VA_ARGS__)))

#define SIG_IMPL_NO_RETURN(NAME)                                \
  template <typename T> using has_ ## NAME = is_detected<NAME, T>

#define SIG_IMPL_RETURN(NAME, RET)                              \
  template <typename T>                                         \
  using has_ ## NAME = is_detected_exact<RET, NAME, T>

#define SIG_IMPL_ARGS_GENERATE(NAME,RET,...) \
  constexpr char const * const str_for_ ## NAME = "MISSING: " #NAME "( " EVAL(MAP(STR, __VA_ARGS__))" )" " -> " #RET
#define SIG_IMPL_GENERATE(NAME,RET) \
  constexpr char const * const str_for_ ## NAME = "MISSING: " #NAME "() -> " #RET

#define SIG_(NAME)                                              \
  SIG_IMPL_NO_ARGS(NAME);                                       \
  SIG_IMPL_NO_RETURN(NAME);                                     \
  SIG_IMPL_GENERATE(NAME, <ignored>)

#define SIG_ARGS_(NAME, ...)                                    \
  SIG_IMPL_ARGS(NAME, __VA_ARGS__);                             \
  SIG_IMPL_NO_RETURN(NAME);                                     \
  SIG_IMPL_ARGS_GENERATE(NAME, <ignored>, __VA_ARGS__)

#define SIG_RETURN_(RET, NAME)                                  \
  SIG_IMPL_NO_ARGS(NAME);                                       \
  SIG_IMPL_RETURN(NAME, RET);                                   \
  SIG_IMPL_GENERATE(NAME, RET)

#define SIG_RETURN_ARGS_(RET, NAME, ...)                        \
  SIG_IMPL_ARGS(NAME, __VA_ARGS__);                             \
  SIG_IMPL_RETURN(NAME, RET);                                   \
  SIG_IMPL_ARGS_GENERATE(NAME, RET, __VA_ARGS__)

  SIG_RETURN_ARGS_(int, add_subsystem, (const single_subsystem_t &), (const std::string &));
  SIG_RETURN_(const std::string &, get_matcher_name);
  SIG_ARGS_(set_matcher_name, (const std::string &));

  SIG_RETURN_(const std::vector<single_subsystem_t> &, get_subsystems);
  SIG_RETURN_(const single_subsystem_t &, get_dom_subsystem);
  SIG_RETURN_(const multi_subsystemsS &, get_subsystemsS);

  SIG_(incr);
  SIG_(decr);

  SIG_RETURN_ARGS_(int, dom_discover_vtx, (vtx_t), (f_resource_graph_t &));
  SIG_RETURN_ARGS_(int, aux_discover_vtx, (vtx_t), (f_resource_graph_t &));
  SIG_RETURN_ARGS_(int, dom_finish_vtx, (vtx_t), (std::map<single_subsystem_t,std::vector<int>> &));
  SIG_RETURN_ARGS_(int, aux_finish_vtx, (vtx_t), (std::map<single_subsystem_t,std::vector<int>> &));

  SIG_RETURN_ARGS_(int, dom_tree_edge, (edg_t), (f_resource_graph_t &));
  SIG_RETURN_ARGS_(int, dom_back_edge, (edg_t), (f_resource_graph_t &));
  SIG_RETURN_ARGS_(int, dom_forward_or_cross_edge, (edg_t), (f_resource_graph_t &));
  SIG_RETURN_ARGS_(int, aux_up_edge, (edg_t), (f_resource_graph_t &));
  SIG_RETURN_ARGS_(int, aux_up_dom_back_edge, (edg_t), (f_resource_graph_t &));
  SIG_RETURN_ARGS_(int, aux_up_dom_forward_or_cross_edge, (edg_t), (f_resource_graph_t &));

#undef SIG_
#undef SIG_ARGS_
#undef SIG_RETURN_
#undef SIG_RETURN_ARGS_

#undef SIG_IMPL_NO_ARGS
#undef SIG_IMPL_ARGS
#undef SIG_IMPL_NO_RETURN
#undef SIG_IMPL_RETURN

} // end namespace detail

template <typename T> constexpr bool check() {
  static_assert(detail::has_add_subsystem_<T>::value,
                "MISSING: add_subsystem(const single_subsystem_t&, const "
                "std::string&) -> void");
  static_assert(detail::has_get_matcher_name_<T>::value,
                "MISSING: get_matcher_name() -> const std::string&");
  static_assert(detail::has_set_matcher_name_<T>::value,
                "MISSING: set_matcher_name(const std::string&) -> void");
  static_assert(
      detail::has_get_subsystems_<T>::value,
      "MISSING: get_subsystems() -> const std::vector<single_subsystem_t>&");
  static_assert(detail::has_get_dom_subsystem_<T>::value,
                "MISSING: get_dom_subsystem() -> const single_subsystem_t&");
  static_assert(detail::has_get_subsystemsS_<T>::value,
                "MISSING: get_subsystemsS() -> const multi_subsystemsS&");
  static_assert(detail::has_incr_<T>::value, "MISSING: incr()");
  static_assert(detail::has_decr_<T>::value, "MISSING: decr()");
  static_assert(detail::has_dom_discover_vtx_<T>::value,
                "MISSING: dom_discover_vtx(vtx_t, f_resource_graph&) -> int");
  static_assert(detail::has_aux_discover_vtx_<T>::value,
                "MISSING: aux_discover_vtx(vtx_t, f_resource_graph&) -> int");
  static_assert(detail::has_dom_finish_vtx_<T>::value,
                "MISSING: dom_finish_vtx(vtx_t, std::map<single_subsystem_t, "
                "std::vector<int>>&, f_resource_graph_t&) -> int");
  static_assert(detail::has_aux_finish_vtx_<T>::value,
                "MISSING: aux_finish_vtx(vtx_t, std::map<single_subsystem_t, "
                "std::vector<int>>&, f_resource_graph_t&) -> int");
  static_assert(detail::has_dom_tree_edge_<T>::value,
                "MISSING: dom_tree_edge(edg_t, f_resource_graph&) -> int");
  static_assert(detail::has_dom_back_edge_<T>::value,
                "MISSING: dom_back_edge(edg_t, f_resource_graph&) -> int");
  static_assert(
      detail::has_dom_forward_or_cross_edge_<T>::value,
      "MISSING: dom_forward_or_cross_edge(edg_t, f_resource_graph&) -> int");
  static_assert(detail::has_aux_up__edge_<T>::value,
                "MISSING: aux_up_edge(edg_t, f_resource_graph&) -> int");
  static_assert(
      detail::has_aux_up_back_tree_edge_<T>::value,
      "MISSING: aux_up_dom_back_edge(edg_t, f_resource_graph&) -> int");
  static_assert(detail::has_aux_up_dom_forward_or_cross_edge_<T>::value,
                "MISSING: aux_up_dom_forward_or_cross_edge(edg_t, "
                "f_resource_graph&) -> int");
  return true;
};

} // end namespace matcher

} // end namespace traits
