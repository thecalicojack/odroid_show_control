//
//  CMovingTxt.cpp
//  src
//
//  Created by michi on 01/05/2016.
//
//

#include "CMovingTxt.h"

CMovingTxt::CMovingTxt(std::string sText) : m_sText(sText), m_iStartIdx(0), m_bIncr(true) {}

CMovingTxt::~CMovingTxt() {}

std::string CMovingTxt::getText()
{
  std::string sRval = "";
  if (m_sText.size() <= MAX_LINE_SIZE)
  {
    sRval = m_sText;
  }
  else
  {
    sRval = m_sText.substr(m_iStartIdx, MAX_LINE_SIZE);
    if (m_bIncr)
    {
      if (m_iStartIdx + MAX_LINE_SIZE < m_sText.size())
      {
        m_iStartIdx++;
      }
      else
      {
        m_bIncr = false;
      }
    }
    else
    {
      if (m_iStartIdx > 0)
      {
        m_iStartIdx--;
      }
      else
      {
        m_bIncr = true;
      }
    }
  }
  return sRval;
}

void CMovingTxt::setText(std::string sText) {
  // only update text and idx if text has changed
  if (m_sText.compare(sText) != 0)
  {
    m_sText = sText;
    m_iStartIdx = 0;
    m_bIncr = true;
  }
}
