//---------------------------------------------------------------------------
#ifndef EBezierH
#define EBezierH
#include <windows.h>
//---------------------------------------------------------------------------
#define EB_MAX_CP 10

typedef struct
{
  float x, y, z, coef;
} stBCPOINT;

class EBezier
{
private:
  WORD m_wMaxCP;
          stBCPOINT m_aBCP[EB_MAX_CP]; // Bezier Control Points
  static  stBCPOINT m_saRP[EB_MAX_CP]; // Result Points
public:
  EBezier();
  void SetCPMax(WORD max);
  void SetCP(WORD idx, float* pt);
  void SetCP(WORD idx, float x, float y, float z);
  void GetPoint(float* pt, float t);
};
//---------------------------------------------------------------------------
#endif
