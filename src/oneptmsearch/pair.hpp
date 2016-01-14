#ifndef PROT_PTM_SEARCH_PAIR_HPP_
#define PROT_PTM_SEARCH_PAIR_HPP_

#include <memory>

namespace prot {

class Pair;

typedef std::shared_ptr<Pair> PairPtr;

class Pair {
 public:
  Pair(int x,int y);

  int getX(){return x_;}

  int getY(){return y_;}

  void setX(int x){x_=x;}

  void setY(int y){y_=y;}
  
  static bool cmpPosInc(const PairPtr &a, const PairPtr &b) {
    if(a->getY() != b->getY()){
      return a->getY() < b->getY();
    }
    return a->getX() < b->getX();
  }

 protected:
  int x_=0;
  int y_=0;
};


} /* namespace prot */

#endif /* PAIR_HPP_ */