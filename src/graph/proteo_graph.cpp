#include "base/logger.hpp"
#include "base/fasta_reader.hpp"
#include "graph/graph_util.hpp"
#include "graph/proteo_graph.hpp"

namespace prot {

ProteoGraph::ProteoGraph(DbResSeqPtr db_res_seq_ptr,
                         MassGraphPtr graph_ptr,
                         bool is_nme,
                         double convert_ratio,
                         int max_ptm_sum_mass) {
  db_proteo_ptr_ = getDbProteoformPtr(db_res_seq_ptr);
  graph_ptr_ = graph_ptr;
  is_nme_ = is_nme;
  node_num_ = num_vertices(*graph_ptr.get());
  LOG_DEBUG("node num " << node_num_);
  pair_num_ = node_num_ * (node_num_ + 1) /2;
  compSeqMasses(convert_ratio);
  compDistances(convert_ratio, max_ptm_sum_mass);
}

int ProteoGraph::getVecIndex(int v1, int v2) {
  int index =  (node_num_ + node_num_ - v1 + 1) * v1 /2  + (v2 - v1);
  return index;
}

int ProteoGraph::getSeqMass(int v1, int v2) {
  int index = getVecIndex(v1, v2);
  return seq_masses_[index];
}

void ProteoGraph::compSeqMasses(double convert_ratio) {
  DbResSeqPtr db_res_seq_ptr = db_proteo_ptr_->getDbResSeqPtr();
  seq_masses_= std::vector<int>(pair_num_, 0);
  for (int i = 0; i < node_num_; i ++) {
    int mass = 0;
    for (int j = i+1; j < node_num_; j++) {
      int cur_mass 
          = std::round(db_res_seq_ptr->getResiduePtr(j-1)->getMass() * convert_ratio) ;
      mass += cur_mass;
      int index = getVecIndex(i, j);
      seq_masses_[index] = mass;
    }
  }
}

void ProteoGraph::compDistances(double convert_ratio, int max_ptm_sum_mass) {
  MassGraph *g_p = graph_ptr_.get();
  //get mass without ptms

  std::vector<std::set<int>> dist_vecs; 
  for (int i = 0; i < pair_num_; i++) {; 
    std::set<int> empty_set;
    dist_vecs.push_back(empty_set);
  }
  // initialize pair (i, i)
  for (int i = 0; i < node_num_; i++) {
    int index = getVecIndex(i, i);
    dist_vecs[index].insert(0);
  }
  for (int i = 0; i < node_num_ - 1; i++) {
    for (int j = i+1; j < node_num_; j++) {
      Vertex v2 = vertex(j, *g_p);
      Vertex pre_v2 = vertex(j-1, *g_p);
      int index = getVecIndex(i, j);
      int pre_index = getVecIndex(i, j-1);
      boost::graph_traits<MassGraph>::out_edge_iterator ei, ei_end;
      boost::tie(ei, ei_end) = out_edges(pre_v2, *g_p);
      for( ; ei != ei_end; ++ei) {
        if(target(*ei, *g_p) == v2 ) {
          MassGraph::edge_descriptor e = *ei;
          int d =(*g_p)[e].int_mass_;
          for (std::set<int>::iterator it=dist_vecs[pre_index].begin(); 
               it!=dist_vecs[pre_index].end(); it++) {
            int new_d = d + *it;
            if (std::abs(new_d - seq_masses_[index]) <= max_ptm_sum_mass) {
              dist_vecs[index].insert(new_d);
            }
          }
        }
      }
    }
  }
  int count = 0;
  for (int i = 0; i < node_num_ - 1; i++) {
    for (int j = i+1; j < node_num_; j++) {
      int index = getVecIndex(i, j);
      for (std::set<int>::iterator it=dist_vecs[index].begin(); 
           it!=dist_vecs[index].end(); it++) {
        DistTuplePtr tuple_ptr(new DistTuple(graph_ptr_, i, j, *it));
        tuple_vec_.push_back(tuple_ptr);
      }
      count += dist_vecs[index].size();
      //LOG_DEBUG("i " << i << " j " << j << " size " << dist_vecs[index].size());
    }
  }
  LOG_DEBUG("count " << count);
}

}
