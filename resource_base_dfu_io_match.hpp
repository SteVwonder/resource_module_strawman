//!
//! Resource Base DFU Matcher API Strawman
//!

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include "resource_graph.hpp"
#include "resource_base_match.hpp"
#include "resource_base_dfu_match.hpp"

#ifndef RESOURCE_BASE_DFU_IO_MATCH_HPP
#define RESOURCE_BASE_DFU_IO_MATCH_HPP

namespace flux_resource_model {

    class resource_base_dfu_io_dom_matcher_t : public resource_base_dfu_matcher_t
    {
    public:
        resource_base_dfu_io_dom_matcher_t() = default;
        resource_base_dfu_io_dom_matcher_t (const std::string &name)
            : resource_base_dfu_matcher_t (name) { }

        virtual int dom_discover_vtx (vtx_t u, f_resource_graph_t &g)
        {
            incr ();
            if (g[u].type == "node") {
                std::cout << level_prefix ()
                          << "dom_discover_vtx: "
                          << g[u].name
                          << " (check that the node is free and has adaquate BW to the PFS)" << std::endl;
            } else if (g[u].type == "pfs1bw") {
                std::cout << level_prefix ()
                          << "dom_discover_vtx: "
                          << g[u].name
                          << " (use the planner to check PFS BW >= num_req_nodes * req_BW_per_node && num_free_nodes >= num_req_nodes)" << std::endl;
            } else {
                std::cout << level_prefix ()
                          << "dom_discover_vtx: "
                          << g[u].name
                          << " (use the planner to check for at least 1 free node && BW >= req_BW_per_node)" << std::endl;
            }
            return MATCHER_SCORE_BASELINE;
        }
        virtual int dom_finish_vtx (vtx_t u, f_resource_graph_t &g)
        {
            incr ();
            if (g[u].type == "node") {
                std::cout << level_prefix ()
                          << "dom_discover_vtx: "
                          << g[u].name
                          << " (allocate the node assuming it is free and has enough PFS BW)" << std::endl;
            } else {
                std::cout << level_prefix ()
                          << "dom_discover_vtx: "
                          << g[u].name
                          << " (use the planner to check for adaquate PFS BW & at least 1 free node)" << std::endl;
            }
            return MATCHER_SCORE_BASELINE;
        }
    };

    class resource_base_dfu_io_aux_matcher_t : public resource_base_dfu_matcher_t
    {
    public:
        resource_base_dfu_io_aux_matcher_t() = default;
        resource_base_dfu_io_aux_matcher_t (const std::string &name)
            : resource_base_dfu_matcher_t (name) { }

        virtual int aux_discover_vtx (vtx_t u, f_resource_graph_t &g)
        {
            incr ();
            std::cout << level_prefix ()
                      << "aux_discover_vtx: "
                      << g[u].name
                      << " - check to see if this resource's available BW is >= req_BW_per_node" <<std::endl;
            auto req_BW_per_node = 100; // this would normally come from the job resource request
            auto enough_bw = (g[u].alloc + req_BW_per_node <= g[u].size);
            if (enough_bw) {
                return MATCHER_SCORE_BASELINE;
            } else {
                decr();
                return MATCHER_WALK_PRUNED;
            }
        }

        virtual int aux_finish_vtx (vtx_t u, std::map<single_subsystem_t,
                std::vector<int> > &score_map, f_resource_graph_t &g)
        {
            std::cout << level_prefix ()
                      << "aux_finish_vtx: "
                      << g[u].name
                      << " - allocate the bandwidth" <<std::endl;
            // TODO: allocation should only happen on "selection" pass, not "find" pass
            decr ();
            return MATCHER_SCORE_BASELINE;
        }

    };

}

#endif // RESOURCE_BASE_DFU_IO_MATCH_HPP

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
