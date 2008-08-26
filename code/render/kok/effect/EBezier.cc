#include "stdafx.h"
//---------------------------------------------------------------------------
#include "EBezier.h"

stBCPOINT EBezier::m_saRP[EB_MAX_CP]; // 專門拿來運算用的.
//---------------------------------------------------------------------------
EBezier::EBezier()
  : m_wMaxCP(0)
{
}
//---------------------------------------------------------------------------
void EBezier::SetCPMax(WORD max)
{
  m_wMaxCP = max > EB_MAX_CP ? EB_MAX_CP : max;

  int i;
  m_aBCP[0].coef = 1;

  // 事先計算出所有控制點的係數.
  for (i = 1; i < max; i++)
    m_aBCP[i].coef = m_aBCP[i - 1].coef * (max - i) / i;
}
//---------------------------------------------------------------------------
void EBezier::SetCP(WORD idx, float* pt)
{
  // 確認指標的合法性
  if (pt == NULL) return;
  //
  if (idx < m_wMaxCP)
  {
    float coef = m_aBCP[idx].coef;
    m_aBCP[idx].x = coef * pt[0];
    m_aBCP[idx].y = coef * pt[1];
    m_aBCP[idx].z = coef * pt[2];
  } // end of if (idx < m_wNumCP)
}
//---------------------------------------------------------------------------
void EBezier::SetCP(WORD idx, float x, float y, float z)
{
  //
  if (idx < m_wMaxCP)
  {
    float coef = m_aBCP[idx].coef;
    m_aBCP[idx].x = coef * x;
    m_aBCP[idx].y = coef * y;
    m_aBCP[idx].z = coef * z;
  } // end of if (idx < m_wNumCP)
}
//---------------------------------------------------------------------------
void EBezier::GetPoint(float* pt, float t)
{
  int i, cpidx; // control point index
  float tt, val;

  cpidx = m_wMaxCP - 1;
  val = t;
  // 起始點?
  m_saRP[0].x = m_aBCP[0].x;
  m_saRP[0].y = m_aBCP[0].y;
  m_saRP[0].z = m_aBCP[0].z;
  //
  for (i = 1; i <= cpidx; i++) 
  {
    m_saRP[i].x = m_aBCP[i].x * val;
    m_saRP[i].y = m_aBCP[i].y * val;
    m_saRP[i].z = m_aBCP[i].z * val;
    val *= t;
  }

  tt = 1.0f - t;
  // 
  pt[0] = m_saRP[cpidx].x;
  pt[1] = m_saRP[cpidx].y;
  pt[2] = m_saRP[cpidx].z;

  val = tt;

  for (i = cpidx - 1; i >=0; i--)
  {
    pt[0] += m_saRP[i].x * tt;
    pt[1] += m_saRP[i].y * tt;
    pt[2] += m_saRP[i].z * tt;
    tt *= val;
  }
}