//
//  CMovingTxt.hpp
//  src
//
//  Created by michi on 01/05/2016.
//
//

#ifndef CMovingTxt_h_
#define CMovingTxt_h_

#include <string>
#include "CApp.h"

class CMovingTxt {
private:
  std::string m_sText;
  int m_iStartIdx;
  bool m_bIncr;
public:
  CMovingTxt(std::string sText);
  ~CMovingTxt();
  std::string getText();
  void setText(std::string);
protected:

};


#endif /* CMovingTxt_hpp */
