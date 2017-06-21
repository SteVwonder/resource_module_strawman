//!
//! Resource Graph Traverser API Strawman (Depth First Visit on DOM, Up Visit on
//! AUX)
//!

// TODO: Integrate jobspec
// TODO: Change comparisons to bitwise operators for perf optimization
// TODO: Boundary conditions -- what does it mean to have MATCHER_WALK_PRUNED
// rc?
// TODO: better than linear edge search?
// TODO: scoring scheme for reservation vs. allocation
// TODO: add constraint checks for the concepts of dfu_matcher_t.
// TODO: score has not been worked out yet

#ifndef RESOURCE_BASE_DFU_TRAVERSE_HPP
#define RESOURCE_BASE_DFU_TRAVERSE_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "resource_base_dfu_match.hpp"

namespace flux_resource_model {

//!
//! Resource graph base traverser. Perform the depth-first visit on the dominant
//! subsystem and up visit on each and all of the auxiliary subsystems selected
//! by the matcher (type passed in to the template parameter. Call back the
//! visitor methods of the macher at various graph visit events.
//!
template <typename dfu_matcher_t>
class resource_base_dfu_traverser_t {
public:
  resource_base_dfu_traverser_t()  = default;
  ~resource_base_dfu_traverser_t() = default;

  //!
  //! Begin walk. Require a matcher that complies with DF-UP traverser
  //! visits on the dominant subsystem and the up visits on each and every
  //! auxiliary subsystem (i.e., ordered list).
  //!
  //! \param g     a graph filtered according to the vtx/edg selectors
  //!              of dfu_matcher_t m.
  //! \param roots map containing roots of all subsystems
  //! \param m     dfu_matcher_t
  //! \return      0 on success; non-zero integer on an error
  //!
  int
  begin_walk(f_resource_graph_t &g,
             std::map<std::string, vtx_t>
                 roots,
             dfu_matcher_t &matcher);

private:
  int
  aux_event(f_resource_graph_t &g, edg_t e, boost::default_color_type c,
            resource_color_map_t &color, const single_subsystem_t &h,
            dfu_matcher_t &matcher);

  int
  dom_event(f_resource_graph_t &g, edg_t e, boost::default_color_type c,
            resource_color_map_t &color, dfu_matcher_t &matcher);

  int
  aux_upv(f_resource_graph_t &g, vtx_t v, resource_color_map_t &color,
          const single_subsystem_t &h, dfu_matcher_t &matcher);

  int
  dom_dfv(f_resource_graph_t &g, vtx_t v, resource_color_map_t &color,
          dfu_matcher_t &matcher);

  std::map<std::string, vtx_t> cp_roots;
};

template <typename dfu_matcher_t>
int
resource_base_dfu_traverser_t<dfu_matcher_t>::begin_walk(
    f_resource_graph_t &g,
    std::map<std::string, vtx_t>
        roots,
    dfu_matcher_t &matcher) {
  resource_color_map_t color = get(&resource_pool_t::color_map, g);
  cp_roots                   = roots;
  vtx_t root                 = cp_roots[matcher.get_dom_subsystem()];
  return dom_dfv(g, root, color, matcher);
}

template <typename dfu_matcher_t>
int
resource_base_dfu_traverser_t<dfu_matcher_t>::aux_event(
    f_resource_graph_t &g, edg_t e, boost::default_color_type c,
    resource_color_map_t &color, const single_subsystem_t &h,
    dfu_matcher_t &matcher) {
  switch (c) {
  case boost::white_color: matcher.aux_up_edge(e, g); break;
  case boost::gray_color: matcher.aux_up_dom_back_edge(e, g); break;
  default: matcher.aux_up_dom_forward_or_cross_edge(e, g); break;
  }
  return aux_upv(g, target(e, g), color, h, matcher);
}

template <typename dfu_matcher_t>
int
resource_base_dfu_traverser_t<dfu_matcher_t>::dom_event(
    f_resource_graph_t &g,
    edg_t e,
    boost::default_color_type c,
    resource_color_map_t &color,
    dfu_matcher_t &matcher) {
  int score = MATCHER_WALK_NOT_TREE_EDGE;
  switch (c) {
  case boost::white_color:
    matcher.dom_tree_edge(e, g);
    score = dom_dfv(g, target(e, g), color, matcher);
    break;
  case boost::gray_color: matcher.dom_back_edge(e, g); break;
  default: matcher.dom_forward_or_cross_edge(e, g); break;
  }
  return score;
}

template <typename dfu_matcher_t>
int
resource_base_dfu_traverser_t<dfu_matcher_t>::aux_upv(
    f_resource_graph_t &g,
    vtx_t v,
    resource_color_map_t &color,
    const single_subsystem_t &h,
    dfu_matcher_t &matcher) {
  boost::graph_traits<f_resource_graph_t>::out_edge_iterator ei;
  boost::graph_traits<f_resource_graph_t>::out_edge_iterator ei_end;
  std::map<single_subsystem_t, std::vector<int>> score_map;

  // Don't color a visit on an auxiliary subsystem
  // Preorder filter can reduce the further search
  if (matcher.aux_discover_vtx(v, g) == MATCHER_WALK_PRUNED) {
    return MATCHER_WALK_PRUNED;
  }

  if (v != cp_roots[h]) {
    tie(ei, ei_end) = out_edges(v, g);
    for (; ei != ei_end; ++ei) {
      // linear search !!  BGL doesn't have edge lookup by its property
      if (g[*ei].member_of.find(h) != g[*ei].member_of.end()) {
        // we trasparent pass even MATCHER_WALK_PRUNED or ERROR
        // to post-up callback
        boost::default_color_type c = color[target(*ei, g)];
        score_map[h].push_back(aux_event(g, *ei, c, color, h, matcher));
        break;
      }
    }
  }

  // Don't color a visit on an auxiliary subsystem
  return matcher.aux_finish_vtx(v, score_map, g);
}

template <typename dfu_matcher_t>
int
resource_base_dfu_traverser_t<dfu_matcher_t>::dom_dfv(
    f_resource_graph_t &g,
    vtx_t v,
    resource_color_map_t &color,
    dfu_matcher_t &matcher) {
  boost::graph_traits<f_resource_graph_t>::out_edge_iterator ei, ei_end;
  boost::graph_traits<f_resource_graph_t>::in_edge_iterator iei, iei_end;
  std::map<single_subsystem_t, std::vector<int>> score_map;
  boost::default_color_type c;
  color[v] = boost::gray_color;

  if (matcher.dom_discover_vtx(v, g) == MATCHER_WALK_PRUNED) {
    return MATCHER_WALK_PRUNED;
  }

  // Walk down the dominant subsystem
  std::vector<single_subsystem_t>::const_iterator iter;
  iter = matcher.get_subsystems().begin();

  tie(ei, ei_end) = out_edges(v, g);
  for (; ei != ei_end; ++ei) {
    c = color[target(*ei, g)];
    if (g[*ei].member_of.find(*iter) != g[*ei].member_of.end()) {
      int score = dom_event(g, *ei, c, color, matcher);
      if (score >= MATCHER_SCORE_BASELINE) {
        // TODO: score has not been worked out yet
        score_map[*iter].push_back(score);
      }
    }
  }

  // Walk up each auxilary subsystem for further constraint check
  for (iter++; iter != matcher.get_subsystems().end(); iter++) {
    tie(ei, ei_end) = out_edges(v, g);
    for (; ei != ei_end; ++ei) {
      c = color[target(*ei, g)];
      if (g[*ei].member_of.find(*iter) != g[*ei].member_of.end()) {
        score_map[*iter].push_back(aux_event(g, *ei, c, color, *iter, matcher));
        break; // have only one edge per aux hierarchy
      }
    }
  }

  color[v] = boost::black_color;
  return matcher.dom_finish_vtx(v, score_map, g);
}
}

#endif // RESOURCE_BASE_DFU_TRAVERSE_HPP

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
