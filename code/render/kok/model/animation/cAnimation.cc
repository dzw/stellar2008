//----------------------------------------------------------------------------
//  terrainentity.cc
//  (C) 2008 cTuo
//----------------------------------------------------------------------------

#include "stdneb.h"
#include "cAnimation.h"

namespace KOK
{

unsigned int cAnimation::m_uiAnimationCount = 0; 
//-----------------------------------------------------------------------------
// Name: cAnimation()
// Desc: cAnimation ���Ľ���Ԫ
//-----------------------------------------------------------------------------
cAnimation::cAnimation()
{
	// interplate animation keyframe data between two keyframe.
	m_bInterpolateKeyFrame = true;
	m_pParent = 0;
	m_pChild = 0;
	m_pAnimationMap = NULL;
	//
	m_uiAnimationCount++;
}

//-----------------------------------------------------------------------------
// Name: ~cAnimation()
// Desc: cAnimation ���Ľ⹹Ԫ
//-----------------------------------------------------------------------------
cAnimation::~cAnimation()
{
	m_uiAnimationCount--;
	removeAllAnimationKeys();
	if( m_pParent == 0 )
	{
		n_delete( m_pAnimationMap );
	}
	m_pParent = 0;
	removeAnimationChild();
}
//-----------------------------------------------------------------------------
// Name: getAnimationMatrix()
// Desc:  compute cAnimation matrix for computing matrix of skeleton hierarchy
//-----------------------------------------------------------------------------
void cAnimation::getAnimationMatrix(D3DXMATRIXA16* pMatrix,


	cAnimationActionInfoGroup *pAnimationActionInfoGroup, 

	const 
	sComputeAnimParam& param)
{
	//PROFILE_SCOPE(cAnimation_GetAnimationMatrix);
	if (pMatrix == NULL || pAnimationActionInfoGroup == 0) return;

	// ����һ���Ϸ�
	if (param.m_iActionIndex1 < 0) return;

	// ��Ҫ����������... 
	if (param.m_iActionIndex2 >= 0)
	{
		getQuaternionToMatrixLerp( pMatrix,
			pAnimationActionInfoGroup,
			param.m_iActionIndex1,
			param.m_iActionIndex2,
			param.m_fAnimTime1,
			param.m_fAnimTime2,
			param.m_fLerpValue,
			!true );
		return;
	}

	// ���ﲻ�������ڲ�
	getQuaternionToMatrix( pMatrix, pAnimationActionInfoGroup, 
		param.m_iActionIndex1, param.m_fAnimTime1 );
}


//-----------------------------------------------------------------------------
// Name: FindScaleKey
// Desc:
//-----------------------------------------------------------------------------
bool cAnimation::FindScaleKey(cAnimationKeys* pAnimationKeys, DWORD dwTime, 
	DWORD* pdwKeyBefore, DWORD* pdwKeyAfter)
{
	if (pAnimationKeys->getNumScaleKey() <= 0) return false;

	int iM, // �����м�ֵ
		iL, // ������߽�
		iR; // �����ұ߽�

	DWORD dwKey1, dwKey2;

	iL = 0;                                    // �ҳ�������߽�
	iR = pAnimationKeys->getNumScaleKey() - 1; // �ҳ������ұ߽�
	iM = iL + ((iR - iL) >> 1);                // ��������м�ֵ

	dwKey1 = 0;
	dwKey2 = 0;

	for (;;)
	{
		if ( dwTime >= pAnimationKeys->getScaleKey( iM )->dwTime )
		{
			iL = iM;
		} // end of if ( pAnimationKeys->getScaleKey ( iL )
		else 
		{
			iR = iM;
		}

		if ((iR - iL) <= 1)
		{
			dwKey2 = iR;  
			break;
		} // end of if ((iR - iL) <= 1)
		iM = iL + ((iR - iL) >> 1);
	} // end of for (;;)

	if (dwKey2 > 0)
	{
		dwKey1 = dwKey2 - 1;
	} // end of (*pdwKeyAfter)

	*pdwKeyBefore = dwKey1;
	*pdwKeyAfter  = dwKey2;

	return true;
}

//-----------------------------------------------------------------------------
// Name: FindRotateKey
// Desc:
//-----------------------------------------------------------------------------
bool cAnimation::FindRotateKey(cAnimationKeys* pAnimationKeys, DWORD dwTime, 
	DWORD* pdwKeyBefore, DWORD* pdwKeyAfter)
{
	if (pAnimationKeys->getNumRotateKey() <= 0) return false;

	int iM, // �����м�ֵ
		iL, // ������߽�
		iR; // �����ұ߽�

	DWORD dwKey1, dwKey2;

	iL = 0;                                    // �ҳ�������߽�
	iR = pAnimationKeys->getNumRotateKey() - 1; // �ҳ������ұ߽�
	iM = iL + ((iR - iL) >> 1);                // ��������м�ֵ

	dwKey1 = 0;
	dwKey2 = 0;

	for (;;)
	{
		if ( dwTime >= pAnimationKeys->getRotateKey( iM )->dwTime )
		{
			iL = iM;
		} // end of if ( pAnimationKeys->getRotateKey ( iL )
		else 
		{
			iR = iM;
		}

		if ((iR - iL) <= 1)
		{
			dwKey2 = iR;  
			break;
		} // end of if ((iR - iL) <= 1)
		iM = iL + ((iR - iL) >> 1);
	} // end of for (;;)

	if (dwKey2 > 0)
	{
		dwKey1 = dwKey2 - 1;
	} // end of (*pdwKeyAfter)

	*pdwKeyBefore = dwKey1;
	*pdwKeyAfter  = dwKey2;

	return true;
}

//-----------------------------------------------------------------------------
// Name: FindPositionKey
// Desc:
//-----------------------------------------------------------------------------
bool cAnimation::FindPositionKey(cAnimationKeys* pAnimationKeys, DWORD dwTime, 
	DWORD* pdwKeyBefore, DWORD* pdwKeyAfter)
{
	if (pAnimationKeys->getNumPositionKey() <= 0) return false;

	int iM, // �����м�ֵ
		iL, // ������߽�
		iR; // �����ұ߽�

	DWORD dwKey1, dwKey2;

	iL = 0;                                    // �ҳ�������߽�
	iR = pAnimationKeys->getNumPositionKey() - 1; // �ҳ������ұ߽�
	iM = iL + ((iR - iL) >> 1);                // ��������м�ֵ

	dwKey1 = 0;
	dwKey2 = 0;

	for (;;)
	{
		if ( dwTime >= pAnimationKeys->getPositionKey( iM )->dwTime )
		{
			iL = iM;
		} // end of if ( pAnimationKeys->getPositionKey ( iL )
		else 
		{
			iR = iM;
		}

		if ((iR - iL) <= 1)
		{
			dwKey2 = iR;  
			break;
		} // end of if ((iR - iL) <= 1)
		iM = iL + ((iR - iL) >> 1);
	} // end of for (;;)

	if (dwKey2 > 0)
	{
		dwKey1 = dwKey2 - 1;
	} // end of (*pdwKeyAfter)

	*pdwKeyBefore = dwKey1;
	*pdwKeyAfter  = dwKey2;

	return true;
}

//-----------------------------------------------------------------------------
// Name: quaternionScale()
// Desc:  get scale of quaternion 
//-----------------------------------------------------------------------------
void cAnimation::quaternionScale(D3DXVECTOR3* pVec, cAnimationActionInfoGroup *pAnimationActionInfoGroup, DWORD dwBaseIndex, DWORD dwTime)
{
	if (pVec == 0 || pAnimationActionInfoGroup == 0) return;

	UINT iKey     = 0 ;
	////////    UINT dwp2     = 0 ;
	////////    UINT dwp3     = 0 ;
	UINT StartKey = 0 ;
	UINT EndKey   = 0 ;

	////////  DWORD t_dwStartTime  = 0 ;
	////////  DWORD t_dwEndTime    = 0 ;
	////////  DWORD t_dwStartFrame = 0 ;
	////////  DWORD t_dwEndFrame   = 0 ;

	cAnimationKeys *pAnimData = NULL;

	////////  t_dwStartTime   = pAniTab[dwBaseIndex].dwStartTime ;
	////////  t_dwEndTime     = pAniTab[dwBaseIndex].dwEndTime ;
	////////  t_dwStartFrame  = pAniTab[dwBaseIndex].dwStartFrame ;
	////////  t_dwEndFrame    = pAniTab[dwBaseIndex].dwEndFrame ;
	//jingjie test 2006.07.08
	//int iActionIndex = pActionTable->GetActTable(dwBaseIndex)->m_iActionIndex;
	//jingjie test 2006.07.09
	//unsigned int iActionIndex = pActionTable->GetActTable(dwBaseIndex)->getAnimationActionIndex();
	unsigned int iACTIndex = pAnimationActionInfoGroup->getAnimationActionInfo( dwBaseIndex )->getAnimationACTIndex();
	//jingjie test 2006.07.06
	//pAnimData = m_aAnimDataTable[iActionIndex].m_pAnimData ;
	pAnimData = m_AnimationKeysMap[iACTIndex] ;

	if (pAnimData == NULL) return;

	/*if (m_bForceQScale)
	{
	// ǿ���趨ʱ
	m_bForceQScale = false; 
	*pVec = m_ForceQScale;
	return;
	}*/
	//jingjie modified 2006.07.06
	if (pAnimData->getNumScaleKey())
	{
	//----------------------------------------------------------
	DWORD dwKeyBefore, dwKeyAfter;
	dwKeyBefore = 0;
	dwKeyAfter  = 0;

	//ѭ����Ѱ...		//jingjie modified 2006.07.06
	//ѭ����Ѱ...    for (iKey = 0 ;iKey < pAnimData->getNumScaleKey() ; iKey++)
	//ѭ����Ѱ...    {
	//ѭ����Ѱ...			//jingjie modified 2006.07.06
	//ѭ����Ѱ...      //if (dwTime < pAnimData->m_aScaleKey[iKey].dwTime)
	//ѭ����Ѱ...			if ( pAnimData->getScaleKey( iKey ) &&  dwTime < pAnimData->getScaleKey( iKey )->dwTime)
		//ѭ����Ѱ...      {
		//ѭ����Ѱ...        dwKeyAfter = iKey;
		//ѭ����Ѱ...
		//ѭ����Ѱ...        // �Ѿ�������һ��Key��.
		//ѭ����Ѱ...        if (iKey > 0)
		//ѭ����Ѱ...        {
		//ѭ����Ѱ...          dwKeyBefore= iKey - 1;
		//ѭ����Ѱ...        }
		//ѭ����Ѱ...        break;
		//ѭ����Ѱ...      } // end of if (dwTime < pAnimData->m_aScaleKey	[iKey].dwTime)
		//ѭ����Ѱ...    } // end of for (iKey = 0; i Key < ...

		n_assert(FindScaleKey(pAnimData, dwTime, &dwKeyBefore, &dwKeyAfter));

	//----------------------------------------------------------
	if (m_bInterpolateKeyFrame)
	{
		// ��Ҫ���ڲ�
		DWORD dwTime1;
		DWORD dwTime2;
		//jingjie modified 2006.07.06
		/*dwTime1 = pAnimData->m_aScaleKey[dwKeyBefore].dwTime 
		;
		dwTime2 = pAnimData->m_aScaleKey[dwKeyAfter].dwTime;
		*/
		if( pAnimData->getScaleKey( dwKeyBefore ) && pAnimData
			->getScaleKey( dwKeyAfter ) )
		{
			dwTime1 = pAnimData->getScaleKey( dwKeyBefore 
				)->dwTime ;
			dwTime2 = pAnimData->getScaleKey( dwKeyAfter )
				->dwTime;
			if ((dwTime2 - dwTime1) ==0)
			{
				// ����Ҫ���ڲ� 
				*pVec = pAnimData->getScaleKey( 
					dwKeyBefore )->vScale;
			}
			else 
			{
				D3DXVec3Lerp(pVec,

					&pAnimData->getScaleKey( dwKeyBefore )->vScale,

					& pAnimData->getScaleKey( dwKeyAfter )->vScale,

					(float)(dwTime - dwTime1)  / (dwTime2 - dwTime1));
			}
		}
	} 
	else 
	{
		// ����Ҫ���ڲ� 
		//*pVec = pAnimData->m_aScaleKey[dwKeyBefore].vScale;
		*pVec =pAnimData->getScaleKey( dwKeyBefore )->vScale;
	}
	// D3DXMatrixScaling(&matTemp, vScale.x, vScale.y, vScale.z);
	}


	////////////if (pAnimData->m_dwNumScaleKey)
	////////////{
	////////////  //----------------------------------------------------------
	////////////  dwp2 = dwp3 = 0;//  t_dwStartFrame ;

	////////////  for (iKey = 0 ;iKey < pAnimData->m_dwNumScaleKey ; iKey++)
	////////////  {
	////////////    // DWORD &l_dKeyTime = m_pKeyFrame[iKey].dwTime;
	////////////    DWORD dwKeyTime = pAnimData->m_aScaleKey[iKey].dwTime;

	////////////    if (dwKeyTime >= t_dwStartTime) StartKey = iKey;
	////////////    if (dwKeyTime >= t_dwEndTime)   EndKey = iKey;

	////////////    if (dwKeyTime >= dwTime)
	////////////    {
	////////////      dwp3 = iKey;

	////////////      if (dwKeyTime > t_dwStartTime)
	////////////      {
	////////////        dwp2= iKey - 1;
	////////////      }
	////////////      else  // when iKey == 0, then dwp2 == 0
	////////////      {
	////////////        dwp2 = iKey;
	////////////      }

	////////////      break;
	////////////    }
	////////////  }

	////////////  if (dwp3 < StartKey || dwp3 > EndKey)
	////////////  {
	////////////    dwp3 = EndKey;
	////////////    dwp2 = StartKey;
	////////////  }

	////////////  //----------------------------------------------------------
	////////////  
	////////////  if (m_bLocalLerp)
	////////////  {
	////////////    // ��Ҫ���ڲ�
	////////////    DWORD dwTime1;
	////////////    DWORD dwTime2;
	////////////    float fLerpValue;
	////////////
	////////////    dwTime1 = pAnimData->m_aScaleKey[dwp2].dwTime ;
	////////////    dwTime2 = pAnimData->m_aScaleKey[dwp3].dwTime;

	////////////    if ((dwTime2 - dwTime1) ==0) fLerpValue = 0;
	////////////    else fLerpValue =  (float)((dwTime1 - dwTime)  / (dwTime2 - dwTime1));  
	////////////   
	////////////    D3DXVec3Lerp(pVec,
	////////////                 &pAnimData->m_aScaleKey[dwp2].vScale,
	////////////                 &pAnimData->m_aScaleKey[dwp3].vScale,
	////////////                 fLerpValue);

	////////////  } else {
	////////////    // ����Ҫ���ڲ� 
	////////////    *pVec = pAnimData->m_aScaleKey[dwp2].vScale;
	////////////  }
	//////////// // D3DXMatrixScaling(&matTemp, vScale.x, vScale.y, vScale.z);
	////////////}

	return;
}

//-----------------------------------------------------------------------------
// Name: quaternionRotate()
// Desc:  get rotation of quaternion 
//-----------------------------------------------------------------------------
void cAnimation::quaternionRotate(D3DXQUATERNION* pQuat,cAnimationActionInfoGroup *pAnimationActionInfoGroup,DWORD 
	dwBaseIndex,DWORD dwTime)
{
	if (pQuat == NULL || pAnimationActionInfoGroup == 0) return;

	UINT iKey     = 0 ;
	cAnimationKeys *pAnimData = NULL;
	//jingjie test 2006.07.08
	//int iActionIndex = pActTable->GetActTable(dwBaseIndex)->m_iActionIndex;
	//jingjie test 2006.07.09
	//unsigned int iActionIndex = pActTable->GetActTable(dwBaseIndex)->getAnimationActionIndex();
	unsigned int iACTIndex = pAnimationActionInfoGroup->getAnimationActionInfo(dwBaseIndex)->getAnimationACTIndex();
	//jingjie test 2006.07.06
	//pAnimData = m_aAnimDataTable[iActionIndex].m_pAnimData ;
	pAnimData = m_AnimationKeysMap[iACTIndex];
	if (pAnimData == NULL) return;

	/*if (m_bForceQRotate)
	{
	// ǿ���趨ʱ
	m_bForceQRotate   = false; 
	*pQuat = m_ForceQRotate;
	return;
	} // end of if (m_bForceQRotate)
	*/
	// SAKU�����޸�
	// dwKeyBefore, dwKeyAfter, ������¼ dwTime ��ǰһ��Key�ͺ�һ��Key...
	//jingjie modified 2006.07.06
	if (pAnimData->getNumRotateKey())
	{
		//----------------------------------------------------------
		DWORD dwKeyBefore, dwKeyAfter;
		dwKeyBefore = 0;
		dwKeyAfter  = 0;

		//ѭ����Ѱ...    for ( iKey = 0 ; iKey < pAnimData->getNumRotateKey() ; iKey++ 	)
			//ѭ����Ѱ...    {
			//ѭ����Ѱ...      // �ҵ�����Ҫ��KeyFrame, �뿪�����Ȧ��...
			//ѭ����Ѱ...			//jingjie modified 2006.07.06
			//ѭ����Ѱ...			//if (dwTime < pAnimData->m_aRotateKey		[iKey].dwTime)
			//ѭ����Ѱ...			if ( pAnimData->getRotateKey(iKey) && dwTime < 		pAnimData->getRotateKey(iKey)->dwTime )
			//ѭ����Ѱ...      {
			//ѭ����Ѱ...        dwKeyAfter = iKey;
			//ѭ����Ѱ...        // (dwKeyTime > t_dwStartTime) ��ʾ��������Ѿ����ǵ�һ��		Frame��,
			//ѭ����Ѱ...        // iKey ���԰��ĵļ� 1. 
			//ѭ����Ѱ...        if (iKey > 0)
			//ѭ����Ѱ...        {
			//ѭ����Ѱ...          dwKeyBefore = iKey - 1;
			//ѭ����Ѱ...        }
			//ѭ����Ѱ...        // �ҵ�����Ҫ��ʱ���, �뿪�����Ȧ��...
			//ѭ����Ѱ...        break;
			//ѭ����Ѱ...      } // end of if (dwKeyTime >= dwTime)
			//ѭ����Ѱ...    } // end of for (iKey = 0; iKey < l_pAnimData->m_dwRotateKeys; 		iKey++)

			n_assert(FindRotateKey(pAnimData, dwTime, &dwKeyBefore, &dwKeyAfter));

		//----------------------------------------------------------
		if (m_bInterpolateKeyFrame)
		{
			// ��Ҫ���ڲ�
			DWORD dwTime1, dwTime2;
			if( pAnimData->getRotateKey( dwKeyBefore )  &&  
				pAnimData->getRotateKey( dwKeyAfter ) )
			{
				dwTime1 = pAnimData->getRotateKey( dwKeyBefore 
					)->dwTime ;
				dwTime2 = pAnimData->getRotateKey( dwKeyAfter 
					)->dwTime ;
				/*dwTime1 = pAnimData->m_aRotateKey
				[dwKeyBefore].dwTime ;
				dwTime2 = pAnimData->m_aRotateKey
				[dwKeyAfter].dwTime ;
				*/
				// �պ���KeyFrame��, ��������ڲ�
				if ((dwTime2 - dwTime1) == 0)
				{
					// ����Ҫ���ڲ� 
					*pQuat = pAnimData->getRotateKey( dwKeyBefore )->quatRotate;  
				}
				else 
				{ // �����ڲ����
					D3DXQuaternionSlerp(pQuat, &pAnimData->getRotateKey( dwKeyBefore )->quatRotate,
						&pAnimData->getRotateKey( dwKeyAfter )->quatRotate, 
						float (dwTime - dwTime1)  / (dwTime2 - dwTime1));

				}
			}

		} 
		else 
		{
			// ����Ҫ���ڲ� 
			//*pQuat = pAnimData->m_aRotateKey[dwKeyBefore].quatRotate;  
			*pQuat = pAnimData->getRotateKey( dwKeyBefore )->quatRotate;
		}

		pQuat->w = -pQuat->w;
	} // end of if (l_pAnimData->m_dwNumRotateKey)
	// end of SAKU�����޸�
	return;
}

//-----------------------------------------------------------------------------
// Name: quaternionPosition()
// Desc:  get position of quaternion 
//-----------------------------------------------------------------------------
void cAnimation::quaternionPosition(D3DXVECTOR3* pVec,cAnimationActionInfoGroup *pAnimationActionInfoGroup,DWORD dwBaseIndex, DWORD dwTime)
{
	if (pVec == NULL || pAnimationActionInfoGroup == 0) return;

	UINT iKey     = 0 ;

	cAnimationKeys *pAnimData = NULL;
	//jingjie test 2006.07.08
	//int iActionIndex = pActionTable->GetActTable(dwBaseIndex)->m_iActionIndex;
	//jingjie test 2006.07.09
	//unsigned int iActionIndex = pActionTable->GetActTable(dwBaseIndex)->getAnimationActionIndex();
	unsigned int iACTIndex = pAnimationActionInfoGroup->getAnimationActionInfo(dwBaseIndex)->getAnimationACTIndex();

	//jingjie test 2006.07.06
	//pAnimData = m_aAnimDataTable[iActionIndex].m_pAnimData ;
	pAnimData = m_AnimationKeysMap[iACTIndex];

	if (pAnimData == NULL) return;

	/*if (m_bForceQPosition)
	{
	// ǿ���趨ʱ
	m_bForceQPosition = false; 
	*pVec = m_ForceQPosition;
	return;
	}*/

	if (pAnimData->getNumPositionKey())
	{
	//----------------------------------------------------------
	DWORD dwKeyBefore, dwKeyAfter;
	dwKeyBefore = 0;
	dwKeyAfter  = 0;

	//ѭ����Ѱ...    for (iKey = 0 ;iKey < pAnimData->getNumPositionKey(); iKey++)
	//ѭ����Ѱ...    {
	//ѭ����Ѱ...			//jingjie modified 2006.07.06
	//ѭ����Ѱ...      //if (dwTime < pAnimData->m_aPositionKey[iKey].dwTime)
	//ѭ����Ѱ...			if (pAnimData->getPositionKey(iKey) && dwTime < pAnimData->getPositionKey(iKey)->dwTime)
		//ѭ����Ѱ...      {
		//ѭ����Ѱ...        dwKeyAfter = iKey;
		//ѭ����Ѱ...
		//ѭ����Ѱ...        if (iKey > 0)
		//ѭ����Ѱ...        {
		//ѭ����Ѱ...          dwKeyBefore = iKey - 1;
		//ѭ����Ѱ...        }
		//ѭ����Ѱ...        break;
		//ѭ����Ѱ...      } // end of if (dwKeyTime >= dwTime)
		//ѭ����Ѱ...    } // end of for (iKey = 0; iKey < l_pAnimData->m_dwNumPositionKey; iKey++)
	n_assert(FindPositionKey(pAnimData, dwTime, &dwKeyBefore, &dwKeyAfter));

	//----------------------------------------------------------

	if (m_bInterpolateKeyFrame)
	{
		// ��Ҫ���ڲ�
		DWORD dwTime1, dwTime2;
		//jingjie modified 2006.07.06
		/*dwTime1 = pAnimData->m_aPositionKey[dwKeyBefore].dwTime ;
		dwTime2 = pAnimData->m_aPositionKey[dwKeyAfter].dwTime;
		*/
		if( pAnimData->getPositionKey( dwKeyBefore ) && 
			pAnimData->getPositionKey( dwKeyAfter ) )
		{

			dwTime1 = pAnimData->getPositionKey
				(dwKeyBefore)->dwTime ;
			dwTime2 = pAnimData->getPositionKey
				(dwKeyAfter)->dwTime;
			if ((dwTime2 - dwTime1) ==0) 
			{
				// ����Ҫ���ڲ� 
				*pVec = pAnimData->getPositionKey
					(dwKeyBefore)->vPos;
			}
			else 
			{
				D3DXVec3Lerp(pVec,

					&pAnimData->getPositionKey(dwKeyBefore)->vPos,

					&pAnimData->getPositionKey(dwKeyAfter)->vPos,

					(float)(dwTime - dwTime1) / (dwTime2 - dwTime1));
			}
		}

	} 
	else 
	{
		// ����Ҫ���ڲ� 
		if( pAnimData->getPositionKey(dwKeyBefore) )
			*pVec = pAnimData->getPositionKey(dwKeyBefore)->vPos;
	} 
	} // end of if (l_pAnimData->m_dwNumPositionKey)
	return;
}

//-----------------------------------------------------------------------------
// Name: getQuaternionToMatrix()
// Desc:  get animation matrix by quaternion on current time by action
//-----------------------------------------------------------------------------
void cAnimation::getQuaternionToMatrix(D3DXMATRIXA16* pMatrix,
	cAnimationActionInfoGroup 
	*pAnimationActionInfoGroup,
	DWORD dwActionIndex,
	float curtime )
{
	if ( pMatrix == 0 ||pAnimationActionInfoGroup == 0 )     return;
	DWORD dwTime;
	float fTime;
	D3DXMATRIXA16 matTemp, matTemp1;

	//cAnimationKeys  *pAnimData   = NULL;
	D3DXMATRIXA16 *pCacheMatrix = NULL;

	BOOL bAnimate = false;


	//jingjie test 2006.07.06
	//if (!m_aAnimDataTable) return;

	// ȡ����������Ŀ�ʼʱ��
	//jingjie test 2006.07.08
	//DWORD t_dwStartTime  = pActionTable->GetActTable(dwActionIndex)->dwStartTime ;
	//jingjie test 2006.07.09
	//DWORD t_dwStartTime  = pActionTable->GetActTable(dwActionIndex)->getAnimationActionStartTime() ;

	DWORD t_dwStartTime  = pAnimationActionInfoGroup->getAnimationActionInfo( dwActionIndex )->getAnimationActionStartTime() ;
	// ȡ����������Ľ���ʱ��
	//jingjie test 2006.07.08
	//DWORD t_dwEndTime    = pActionTable->GetActTable(dwActionIndex)->dwEndTime 
	;
	//jingjie test 2006.07.09
	//DWORD t_dwEndTime    = pActionTable->GetActTable(dwActionIndex)->getAnimationActionEndTime() ;
	DWORD t_dwEndTime    = pAnimationActionInfoGroup->getAnimationActionInfo( dwActionIndex )->getAnimationActionEndTime() ;
	//jingjie test 2006.07.08
	//int iActionIndex = pActionTable->GetActTable(dwActionIndex)->m_iActionIndex;
	//jingjie test 2006.07.09
	//unsigned int iActionIndex = pActionTable->GetActTable(dwActionIndex)->getAnimationActionIndex();
	unsigned int iACTIndex = pAnimationActionInfoGroup->getAnimationActionInfo( dwActionIndex )->getAnimationACTIndex();
	//jingjie test 2006.07.06
	//pAnimData = m_aAnimDataTable[iActionIndex].m_pAnimData ;
	//pAnimData = m_AnimationKeysMap[iACTIndex];

	DWORD t_dwCnt = t_dwEndTime - t_dwStartTime ;

	// // ���FrameTime������� curtime ���� t_dwEndTime �Ļ�.
	  // ���� fmod ��ʱ�����ڷ�Χ��. ���� curtime �պõ��� t_dwEndTime ʱ, 
	  // ԭ��Ҫ�������һ�� Frame �� curtime ���� fmod ֮��ͻ��� 0,
	  // �����ɶ������Ŵ���, Ϊ�˱����������, ֻ��д�����µ�ģʽ, 
	// Ȼ���� (curtime / float(t_dwCnt)) ��Զ����Ҫ���� 1.0f. 
	// 070621 cyhsieh ���䶯��
	if( t_dwCnt == 0 )
	{
	// ��������������Σ�ֱ�Ӱ�ʱ����Ϊ0
	fTime = 0.0f;
	}
	else

	if ( (curtime / float(t_dwCnt)) > 1.0f )
	{
	float fMOD;
	fMOD = fmod(curtime, (float)t_dwEndTime);
	if (fMOD == 0.0f) 
		fTime = (float)t_dwEndTime;
	else              
		fTime = ((float)t_dwStartTime) + fMOD;
	} // end of if ((curtime / float(t_dwCnt) > 1.0f)
	else 
	{
	fTime = curtime;
	}

	dwTime = (DWORD)fTime + t_dwStartTime;

	// Ϊ��Ԫ��
	D3DXQUATERNION Quat;
	D3DXVECTOR3    vScale, vPos;

	quaternionScale(&vScale, pAnimationActionInfoGroup, dwActionIndex, dwTime);
	D3DXMatrixScaling(pMatrix, vScale.x, vScale.y, vScale.z);

	quaternionRotate(&Quat, pAnimationActionInfoGroup, dwActionIndex, dwTime);
	// ����Ԫ��ȡ����ת����
	D3DXMatrixRotationQuaternion(&matTemp, &Quat);
	D3DXMatrixMultiply(&matTemp1, pMatrix, &matTemp);

	quaternionPosition(&vPos, pAnimationActionInfoGroup, dwActionIndex, dwTime);
	D3DXMatrixTranslation(&matTemp, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(pMatrix, &matTemp1, &matTemp);
}


//-----------------------------------------------------------------------------
// Name: GetQuaternionToMatrix()
// Desc:  get animation lerp matrix by quaternion on current time between action1 and action2
//-----------------------------------------------------------------------------
void cAnimation::getQuaternionToMatrixLerp(D3DXMATRIXA16* pMatrix,
	cAnimationActionInfoGroup *pAnimationActionInfoGroup, //������
	int iActionIndex1,    // ����һ
	int iActionIndex2, // ������
	float fCurTime,     // ����һ�ѽ��е�ʱ��
	float fBakCurTime,  // �������ѽ��е�ʱ��
	float fLerpValue,     // ��������Ӱ��̶�
	bool bFixed)
{
	if ( pMatrix == 0 || pAnimationActionInfoGroup == 0 ) return;

	DWORD dwTime1,dwTime2;
	D3DXMATRIXA16 matTemp, matTemp1;

	cAnimationKeys  *pAnimData1  = NULL;
	cAnimationKeys  *pAnimData2  = NULL;


	if (pAnimationActionInfoGroup     == NULL) return;
	//jingjie test 2006.07.06
	//if (m_aAnimDataTable == NULL) return;

	cAnimationActionInfo *pAnimationActionInfo1 = 0, *pAnimationActionInfo2 = 0;
	//jingjie test 2006.07.09
	/*pAnimationActionKeyframesRange1 = pActionTable->GetActTable(iActionIndex1);
	pAnimationActionKeyframesRange2 = pActionTable->GetActTable(iActionIndex2);
	*/
	pAnimationActionInfo1 = pAnimationActionInfoGroup->getAnimationActionInfo(iActionIndex1);
	pAnimationActionInfo2 = pAnimationActionInfoGroup->getAnimationActionInfo(iActionIndex2);

	if (pAnimationActionInfo1 == NULL) return;
	if (pAnimationActionInfo2 == NULL) return;

	//jingjie test 2006.07.08
	/* DWORD dwStartTime1 = pActionTable->GetActTable(iActionIndex1)->dwStartTime 
	;
	DWORD dwEndTime1   = pActionTable->GetActTable(iActionIndex1)->dwEndTime ;
	DWORD dwStartTime2 = pActionTable->GetActTable(iActionIndex2)->dwStartTime ;
	DWORD dwEndTime2   = pActionTable->GetActTable(iActionIndex2)->dwEndTime ;
	*/
	/*DWORD dwStartTime1 = pActionTable->GetActTable(iActionIndex1)->getAnimationActionStartTime();
	DWORD dwEndTime1   = pActionTable->GetActTable(iActionIndex1)->getAnimationActionEndTime();
	DWORD dwStartTime2 = pActionTable->GetActTable(iActionIndex2)->getAnimationActionStartTime();
	DWORD dwEndTime2   = pActionTable->GetActTable(iActionIndex2)->getAnimationActionEndTime();
	*/
	//jingjie test 2006.07.09
	DWORD dwStartTime1 = pAnimationActionInfoGroup->getAnimationActionInfo(iActionIndex1)->getAnimationActionStartTime();
	DWORD dwEndTime1   = pAnimationActionInfoGroup->getAnimationActionInfo(iActionIndex1)->getAnimationActionEndTime();
	DWORD dwStartTime2 = pAnimationActionInfoGroup->getAnimationActionInfo(iActionIndex2)->getAnimationActionStartTime();
	DWORD dwEndTime2   = pAnimationActionInfoGroup->getAnimationActionInfo(iActionIndex2)->getAnimationActionEndTime();

	unsigned int iACTIndex1 = pAnimationActionInfoGroup->getAnimationActionInfo(iActionIndex1)->getAnimationACTIndex();
	unsigned int iACTIndex2 = pAnimationActionInfoGroup->getAnimationActionInfo(iActionIndex2)->getAnimationACTIndex();

	pAnimData1 = m_AnimationKeysMap[iACTIndex1];
	pAnimData2 = m_AnimationKeysMap[iACTIndex2];

	DWORD dwCnt1 = pAnimationActionInfo1->getAnimationActionTimeLength();
	DWORD dwCnt2 = pAnimationActionInfo2->getAnimationActionTimeLength();

	if (bFixed)
	{
		//----------------------------------------------------
		dwTime1 = dwStartTime1;
		//jingjie test 2006.07.08
		//dwTime2 = pActTable2->GetTimeLength();
		dwTime2 = pAnimationActionInfo2->getAnimationActionTimeLength();
		//----------------------------------------------------
	} 
	else 
	{
		// Ϊ�˱������һ��Frame��
		if ((fCurTime / (float)dwCnt1) > 1.0f)
		{
			float fMOD;
			fMOD = fmod(fCurTime, (float)dwCnt1);
			if (fMOD == 0.0f) 
				dwTime1 = dwEndTime1;
			else              
				dwTime1 = dwStartTime1 + (DWORD)fMOD;
		}
		else
		{
			dwTime1 = DWORD(fCurTime);
		} // end of if ( <= 1.0f )

		dwTime1 += dwStartTime1;

		// 
		if ((fBakCurTime / (float)dwCnt2) > 1.0f)
		{
			float fMOD;
			fMOD = fmod(fBakCurTime, (float)dwCnt2);
			if (fMOD == 0.0f)
				dwTime1 = dwEndTime2;
			else
				dwTime2 = dwStartTime2 + (DWORD)fMOD;
		}
		else
		{
			dwTime2 = DWORD(fBakCurTime);
		} // end of if ( <= 1.0f )

		dwTime2 += dwStartTime2;
	}

	// Ϊ��Ԫ��
	D3DXQUATERNION Quat0  ,Quat1  ,Quat2;
	D3DXVECTOR3    vScale0,vScale1,vScale2;
	D3DXVECTOR3    vPos0  ,vPos1  ,vPos2;

	//----------------------------------------------------
	// ����ScaleKey
	quaternionScale(&vScale1, pAnimationActionInfoGroup, iActionIndex1, dwTime1);
	quaternionScale(&vScale2, pAnimationActionInfoGroup, iActionIndex2, dwTime2);
	D3DXVec3Lerp(&vScale0, &vScale1,&vScale2,fLerpValue);

	D3DXMatrixScaling(pMatrix, vScale0.x, vScale0.y, vScale0.z);
	//----------------------------------------------------
	// ����RotateKey
	quaternionRotate(&Quat1, pAnimationActionInfoGroup, iActionIndex1, dwTime1);
	quaternionRotate(&Quat2, pAnimationActionInfoGroup, iActionIndex2, dwTime2);
	D3DXQuaternionSlerp(&Quat0, &Quat1, &Quat2, fLerpValue);

	D3DXMatrixRotationQuaternion(&matTemp, &Quat0);
	D3DXMatrixMultiply(&matTemp1, pMatrix, &matTemp);
	//----------------------------------------------------
	// ����PositionKey
	quaternionPosition(&vPos1, pAnimationActionInfoGroup, iActionIndex1, 
		dwTime1);
	quaternionPosition(&vPos2, pAnimationActionInfoGroup, iActionIndex2, 
		dwTime2);
	D3DXVec3Lerp(&vPos0, &vPos1, &vPos2,fLerpValue);

	D3DXMatrixTranslation(&matTemp, vPos0.x, vPos0.y, vPos0.z);
	D3DXMatrixMultiply(pMatrix, &matTemp1, &matTemp);
	//----------------------------------------------------
}
//-----------------------------------------------------------------------------
// Name: addAnimationKeys
// Desc:  cAnimationKeys map for adding a cAnimationKeys.
//-----------------------------------------------------------------------------
void cAnimation::addAnimationKeys( unsigned int uiIndex , cAnimationKeys* 
	pAnimationKeys )
{
	if( pAnimationKeys )
	{
		tAnimationKeysMap::iterator i = m_AnimationKeysMap.find( uiIndex );
		if( i == m_AnimationKeysMap.end() )
		{
			cAnimationKeys* pNewAnimationKeys = n_new(cAnimationKeys);
			pNewAnimationKeys->cloneAnimationKeys(pAnimationKeys);
			m_AnimationKeysMap.insert
				(tAnimationKeysMap::value_type( uiIndex, pNewAnimationKeys) );
		}
	}
}
//-----------------------------------------------------------------------------
// Name: removeAnimationKeys
// Desc:  cAnimationKeys map for remove a cAnimationKeys.
//-----------------------------------------------------------------------------
void cAnimation::removeAnimationKeys( unsigned int uiIndex )
{
	tAnimationKeysMap::iterator i = m_AnimationKeysMap.find( uiIndex );

	if( i != m_AnimationKeysMap.end() )
	{
		n_delete( i->second );
		m_AnimationKeysMap.erase(i);
	}
}
//-----------------------------------------------------------------------------
// Name: removeAnimationKeysFromAnimationMap
// Desc:  remove all cAnimationKeys of action id on AnimationMap.
//-----------------------------------------------------------------------------
void cAnimation::removeAnimationKeysFromAnimationMap( unsigned int uiIndex )
{
	if ( m_pAnimationMap )
	{
		tAnimationMap::iterator i, iend;
		i = m_pAnimationMap->begin();
		iend = m_pAnimationMap->end();
		for (; i != iend; ++i)
		{
			cAnimation* pAnimation = i->second;
			pAnimation->removeAnimationKeys( uiIndex );
		}
	}
}
//-----------------------------------------------------------------------------
// Name: removeAllAnimationKeys
// Desc:  cAnimationKeys map for remove all cAnimationKeys.
//-----------------------------------------------------------------------------
void cAnimation::removeAllAnimationKeys( )
{
	tAnimationKeysMap::iterator i, iend;
	i = m_AnimationKeysMap.begin();
	iend = m_AnimationKeysMap.end();
	for (; i != iend; ++i)
	{
		n_delete( i->second );
	}
	m_AnimationKeysMap.clear();
}

//-----------------------------------------------------------------------------
// Name: getAnimationKeys
// Desc:  get  cAnimationKeys
//-----------------------------------------------------------------------------
cAnimationKeys* cAnimation::getAnimationKeys( unsigned int uiIndex  )
{
	tAnimationKeysMap::iterator i = m_AnimationKeysMap.find( uiIndex );

	if( i != m_AnimationKeysMap.end() )
	{
		return i->second;
	}
	else
	{
		return 0;
	}
}
//-----------------------------------------------------------------------------
// Name: setSkeletonHierarchyName
// Desc:  set skeleton name for this aniamtion.
//-----------------------------------------------------------------------------
void cAnimation::setSkeletonHierarchyName(char* name, int len)
{
	if (name == NULL) return;

	m_szSkeletonHierarchyName = name;
	//if (len < 0) // �� '\0' ��β���ִ�.
	//	m_szSkeletonHierarchyName = name;
	//else 
	//	m_szSkeletonHierarchyName.SetString(name, len);
}
//-----------------------------------------------------------------------------
// Name: setSkeletonHierarchyName
// Desc:  add cAnimation's child and set skeleton name for this aniamtion.
//-----------------------------------------------------------------------------
cAnimation* cAnimation::addAnimationChild( char* szSkeletonHierarchyName )
{
	if ( (m_pParent == 0) && (m_pAnimationMap == NULL) )
	{
		m_pAnimationMap = n_new(tAnimationMap);
	}

	if ( m_pAnimationMap )
	{
		tAnimationMap::iterator i = m_pAnimationMap->find( szSkeletonHierarchyName );

		if( i == m_pAnimationMap->end() )
		{
			cAnimation* pAnimation = n_new(cAnimation);
			if( m_pAnimationMap->size() > 0 )
			{
				pAnimation->setParent( this );

			}
			setChild( pAnimation );

			m_pAnimationMap->insert(tAnimationMap::value_type( szSkeletonHierarchyName, pAnimation) );
			return pAnimation;
		}
	}
	return 0;
}
//-----------------------------------------------------------------------------
// Name: setSkeletonHierarchyName
// Desc:  remove cAnimation's child
//-----------------------------------------------------------------------------
void cAnimation::removeAnimationChild()
{
	if( m_pChild )
		n_delete( m_pChild );
}
//-----------------------------------------------------------------------------
// Name: setSkeletonHierarchyName
// Desc:  find cAnimation by SkeletonHierarchyName
//-----------------------------------------------------------------------------
cAnimation* cAnimation::findAniamtion( char* szSkeletonHierarchyName )
{
	cAnimation *pAnimation = 0;
	if (szSkeletonHierarchyName == 0) return 0;
	if (*szSkeletonHierarchyName == '\0') return 0;
	if( m_pAnimationMap )
	{
		tAnimationMap::iterator i = m_pAnimationMap->find( 
			szSkeletonHierarchyName );

		if( i != m_pAnimationMap->end() )
		{
			return i->second;
		}else return 0;
	}
	return 0;
}
//-----------------------------------------------------------------------------
// Name: setChild
// Desc:  set cAnimation's child
//-----------------------------------------------------------------------------
void cAnimation::setChild( cAnimation* pChild )
{
	if  (m_pChild) m_pChild->setChild(pChild);
	else m_pChild = pChild;
}
//-----------------------------------------------------------------------------
// Name: getChild
// Desc:  get cAnimation's child
//-----------------------------------------------------------------------------
cAnimation* cAnimation::getChild()
{
	return m_pChild;
}
//-----------------------------------------------------------------------------
// Name: setChild
// Desc:  set cAnimation's parent
//-----------------------------------------------------------------------------
void cAnimation::setParent( cAnimation* pParent )
{
	m_pParent = pParent;
}
//-----------------------------------------------------------------------------
// Name: getChild
// Desc:  get cAnimation's child
//-----------------------------------------------------------------------------
cAnimation* cAnimation::getParent()
{
	return m_pParent;
}
}
