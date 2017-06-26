#include <type_traits>

#include "resource_data.hpp"
#include "resource_graph.hpp"
#include "type_traits.hpp"

namespace traits {
namespace matcher {

  using namespace flux_resource_model;

  DEFINE_CHECK(AddSubsystem, &, int, add_subsystem, const single_subsystem_t &, const std::string &);
  DEFINE_CHECK(GetMatcherName, &, const std::string &, get_matcher_name, );
  DEFINE_CHECK(SetMatcherName, &, check::ignore, set_matcher_name, const std::string &);

  DEFINE_CHECK(GetSubsystems, &, const std::vector<single_subsystem_t> &, get_subsystems, );
  DEFINE_CHECK(GetDomSubsystem, &, const single_subsystem_t &, get_dom_subsystem, );
  DEFINE_CHECK(GetSubsystemsS, &, const multi_subsystemsS &, get_subsystemsS, );

  DEFINE_CHECK(Incr, &, check::ignore, incr, );
  DEFINE_CHECK(Decr, &, check::ignore, decr, );

  DEFINE_CHECK(DomDiscoverVertex, &, int, dom_discover_vtx, vtx_t, f_resource_graph_t &);
  DEFINE_CHECK(AuxDiscoverVertex, &, int, aux_discover_vtx, vtx_t, f_resource_graph_t &);
  DEFINE_CHECK(DomFinishVertex, &, int, dom_finish_vtx, vtx_t, std::map<single_subsystem_t, std::vector<int>> &);
  DEFINE_CHECK(AuxFinishVertex, &, int, aux_finish_vtx, vtx_t, std::map<single_subsystem_t, std::vector<int>> &);

  DEFINE_CHECK(DomTreeEdge, &, int, dom_tree_edge, edg_t, f_resource_graph_t &);
  DEFINE_CHECK(DomBackEdge, &, int, dom_back_edge, edg_t, f_resource_graph_t &);
  DEFINE_CHECK(DomForwardOrCrossEdge, &, int, dom_forward_or_cross_edge, edg_t, f_resource_graph_t &);
  DEFINE_CHECK(AuxUpEdge, &, int, aux_up_edge, edg_t, f_resource_graph_t &);
  DEFINE_CHECK(AuxUpDomBackEdge, &, int, aux_up_dom_back_edge, edg_t, f_resource_graph_t &);
  DEFINE_CHECK(AuxUpDomForwardOrCrossEdge, &, int, aux_up_dom_forward_or_cross_edge, edg_t, f_resource_graph_t &);

template <typename T> constexpr bool check() {
  constexpr bool result =
    RequiresAddSubsystem<T>() &&
    RequiresAuxDiscoverVertex<T>() &&
    RequiresAuxFinishVertex<T>() &&
    RequiresAuxUpDomBackEdge<T>() &&
    RequiresAuxUpDomForwardOrCrossEdge<T>() &&
    RequiresAuxUpEdge<T>() &&
    RequiresDecr<T>() &&
    RequiresDomBackEdge<T>() &&
    RequiresDomDiscoverVertex<T>() &&
    RequiresDomFinishVertex<T>() &&
    RequiresDomForwardOrCrossEdge<T>() &&
    RequiresDomTreeEdge<T>() &&
    RequiresGetDomSubsystem<T>() &&
    RequiresGetMatcherName<T>() &&
    RequiresGetSubsystems<T>() &&
    RequiresGetSubsystemsS<T>() &&
    RequiresIncr<T>() &&
    RequiresSetMatcherName<T>();
  static_assert(result, "Concept of Matcher Traits not met");
  return result;
}

} // end namespace matcher

} // end namespace traits
