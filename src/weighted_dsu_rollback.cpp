#include "graphlib/weighted_dsu_rollback.h"
#include <numeric>
#include <stdexcept>
namespace graphlib {
WeightedDsuRollback::WeightedDsuRollback(int n) { if(n<0)throw std::invalid_argument("DSU size must be non-negative");parent_.resize(n);rank_.assign(n,0);delta_.assign(n,0);std::iota(parent_.begin(),parent_.end(),0); }
std::pair<int,long long> WeightedDsuRollback::root_and_offset(int x) const { if(x<0||x>=static_cast<int>(parent_.size()))throw std::out_of_range("DSU vertex out of range");long long d=0;while(parent_[x]!=x){d+=delta_[x];x=parent_[x];}return{x,d};}
bool WeightedDsuRollback::unite(int x,int y,long long w){auto [rx,px]=root_and_offset(x);auto [ry,py]=root_and_offset(y);if(rx==ry){history_.push({-1,-1,0,0,false});return py-px==w;}if(rank_[rx]<rank_[ry]){std::swap(rx,ry);std::swap(px,py);w=-w;}history_.push({ry,rx,rank_[rx],delta_[ry],true});parent_[ry]=rx;delta_[ry]=px+w-py;if(rank_[rx]==rank_[ry])++rank_[rx];return true;}
int WeightedDsuRollback::snapshot() const{return static_cast<int>(history_.size());}
void WeightedDsuRollback::rollback(){if(history_.empty())return;auto s=history_.top();history_.pop();if(!s.merged)return;parent_[s.child]=s.child;delta_[s.child]=s.child_delta;rank_[s.root]=s.root_rank;}
void WeightedDsuRollback::rollback_to(int id){if(id<0||id>static_cast<int>(history_.size()))throw std::out_of_range("Invalid DSU snapshot");while(static_cast<int>(history_.size())>id)rollback();}
bool WeightedDsuRollback::connected(int x,int y)const{return root_and_offset(x).first==root_and_offset(y).first;}
long long WeightedDsuRollback::difference(int x,int y)const{auto a=root_and_offset(x),b=root_and_offset(y);if(a.first!=b.first)throw std::logic_error("Vertices are not connected");return b.second-a.second;}
}
