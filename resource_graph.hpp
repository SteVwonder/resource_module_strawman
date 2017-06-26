//
// Resource Graph API
//
// TODO:do not use bundled properties!!
//!
//! Resource Graph API Strawman
//!

#ifndef RESOURCE_GRAPH_HPP
#define RESOURCE_GRAPH_HPP

#include <utility>

#include <boost/graph/adjacency_list.hpp>

#include "resource_data.hpp"

namespace flux_resource_model {

struct gname_t {
  std::string graph_name;
};

using resource_graph_t = boost::adjacency_list<boost::vecS,
                                               boost::vecS,
                                               boost::directedS,
                                               resource_pool_t,
                                               resource_relation_t,
                                               gname_t>;

using edg_subsystems_map_t =
    boost::property_map<resource_graph_t,
                        multi_subsystems_t resource_relation_t::*>::type;

using vtx_subsystems_map_t =
    boost::property_map<resource_graph_t,
                        multi_subsystems_t resource_pool_t::*>::type;

using vtx_t        = boost::graph_traits<resource_graph_t>::vertex_descriptor;
using edg_t        = boost::graph_traits<resource_graph_t>::edge_descriptor;
using vtx_iterator = boost::graph_traits<resource_graph_t>::vertex_iterator;
using edg_iterator = boost::graph_traits<resource_graph_t>::edge_iterator;

struct resource_graph_db_t {
  resource_graph_t resource_graph;
  std::map<std::string, vtx_t> roots;
  std::map<std::string, std::vector<vtx_t>> by_type;
  std::map<std::string, std::vector<vtx_t>> by_name;
  std::map<std::string, std::vector<vtx_t>> by_path;
};

template <typename EV, typename SubsystemMap>
class subsystem_selector_t {
public:
  subsystem_selector_t()                            = default;
  subsystem_selector_t(const subsystem_selector_t&) = default;
  subsystem_selector_t(subsystem_selector_t&&)      = default;
  ~subsystem_selector_t()                           = default;

  subsystem_selector_t&
  operator=(const subsystem_selector_t&) = default;

  subsystem_selector_t(SubsystemMap s, multi_subsystemsS sel)
      : m_sel(std::move(std::move(sel))), m_s(s) {}

  bool
  operator()(const EV& ev) const {
    multi_subsystems_t subs = get(m_s, ev);
    multi_subsystems_t::const_iterator si;
    multi_subsystemsS::const_iterator i;
    for (si = subs.begin(); si != subs.end(); si++) {
      i = m_sel.find(si->first);
      if (i != m_sel.end()) {
        if (si->second == "*") {
          return true;
        }
        if (i->second.find(si->second) != i->second.end() ||
            i->second.find("*") != i->second.end()) {
          // std::cout << si->second << std::endl;
          return true;
        }
      }
    }
    return false;
  }

private:
  // Subsystems selector
  multi_subsystemsS m_sel;
  // TODO: Can't use a pointer because property map is from get()
  SubsystemMap m_s;
};

using f_resource_graph_t =
    boost::filtered_graph<resource_graph_t,
                          subsystem_selector_t<edg_t, edg_subsystems_map_t>,
                          subsystem_selector_t<vtx_t, vtx_subsystems_map_t>>;

using resource_color_map_t =
    boost::property_map<f_resource_graph_t,
                        boost::default_color_type resource_pool_t::*>::type;

using f_vtx_iterator = boost::graph_traits<f_resource_graph_t>::vertex_iterator;

using f_edg_iterator = boost::graph_traits<f_resource_graph_t>::edge_iterator;

// TODO self-sorting accumulator
struct scored_vtx_t {
  vtx_t v;
  int score;
};
using ordered_accumulator_t = std::set<scored_vtx_t>;
using out_iterator          = ordered_accumulator_t::iterator;

//
// Graph Output API
//
enum resource_graph_format_t { GRAPHVIZ_DOT, GRAPH_ML, NEO4J_CYPHER };

class edge_label_writer_t {
public:
  edge_label_writer_t(edg_subsystems_map_t& _props) : props(_props) {}
  void
  operator()(std::ostream& out, const edg_t& e) const {
    std::string subs1 = props[e].begin()->first;
    out << R"([label=")" << props[e][subs1] << R"("])";
  }

private:
  edg_subsystems_map_t props;
};
}

#endif // RESOURCE_GRAPH_HPP

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
