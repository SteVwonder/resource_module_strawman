//!
//! Resource Generator API Strawman
//!

#ifndef RESOURCE_GEN_HPP
#define RESOURCE_GEN_HPP

#include <string>
#include <vector>

#include "resource_graph.hpp"
#include "resource_spec.hpp"

namespace flux_resource_model {

class resource_generator_t {
public:
  resource_generator_t();
  ~resource_generator_t();
  int
  read_sspecs(const std::vector<sspec_t *> &sspecs, resource_graph_db_t &db);
  int
  read_sspecs(const std::string &f, resource_graph_db_t &g);
  const std::string &
  get_err_message();

private:
  int
  path_prefix(const std::string &path, int uplevel, std::string &prefix);
  int
  gen_id(const resource_graph_t &g, id_meth_t im, const vtx_t &p,
         const vtx_t &v, int i);
  vtx_t
  gen_new(const vtx_t &p, const sspec_t &s, int i, resource_graph_db_t &db);
  int
  gen_children(const vtx_t &p, const std::vector<sspec_t *> &c,
               resource_graph_db_t &db);
  int
  gen_subsystem(const sspec_t &root, resource_graph_db_t &db);
  std::string err_msg;
};
}

#endif // RESOURCE_GEN_HPP

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
