#pragma once
#ifndef KOK_PARTICLEDATA_H
#define KOK_PARTICLEDATA_H
//------------------------------------------------------------------------------
/**
	粒子数据，从文件加载

	(C) 2008 cTuo
*/
#include "kok/material/material.h"
#include "kok/material/cMaterialAnimator.h"

namespace KOK
{
	struct sPCloudInfo
	{
		DWORD m_dwFormation;
		float m_fMoveSpeed;
		float m_fMoveSpeedVariation;
		DWORD m_dwMoveType;
		DWORD m_dwBirthMethod;
		DWORD m_dwBirthRate;
		DWORD m_dwBirthTotal;
		DWORD m_dwEmitStart;
		DWORD m_dwEmitStop;
		DWORD m_dwLife;
		DWORD m_dwLifeVariation;
		float m_fSize;
		float m_fSizeVariation;
		DWORD m_dwGrowTime;
		DWORD m_dwFadeTime;
		float m_fEmitWidth;
		float m_fEmitHeight;
		float m_fEmitDepth;
		DWORD m_dwSpinTime;
		float m_fSpinVariation;
		float m_fSpinPhaseDeg;
		float m_fSpinPhaseDegVariation;

		sPCloudInfo();
	};

	// particle superspray
	struct sSuperSprayInfo
	{
		float m_fOffAxisDeg;
		float m_fOffAxisSpreadDeg;
		float m_fOffPlaneDeg;
		float m_fOffPlaneSpreadDeg;
		float m_fMoveSpeed;
		float m_fMoveSpeedVariation;
		DWORD m_dwBirthMethod;
		DWORD m_dwBirthRate;
		DWORD m_dwBirthTotal;
		DWORD m_dwEmitStart;
		DWORD m_dwEmitStop;
		DWORD m_dwLife;
		DWORD m_dwLifeVariation;
		float m_fSize;
		float m_fSizeVariation;
		DWORD m_dwGrowTime;
		DWORD m_dwFadeTime;
		float m_fEmitWidth;
		DWORD m_dwSpinTime;
		float m_fSpinVariation;
		float m_fSpinPhaseDeg;
		float m_fSpinPhaseDegVariation;

		sSuperSprayInfo();
	};

class EmitterData
{
public:
	EmitterData();
	virtual ~EmitterData();

	virtual void ImportFromMemory( const Ptr<IO::Stream>& stream ) = 0;
	void ImportMaterialFromMemory( const Ptr<IO::Stream>& stream, int iIndex );
	void ImportMaterialAnimationFromMemory( const Ptr<IO::Stream>& stream, int iIndex );

	DWORD GetDataType( void ) { return m_dwDataType; }
	const Util::String& GetLinkName( void ) { return m_pLinkName; }

	DWORD GetMaterialSize( void ) { return m_dwNumMaterial; }
	cMaterial* GetMaterial( DWORD dwIndex );
	cMaterialAnimator* GetMaterialAnimator( DWORD dwIndex );

protected:
	DWORD       m_dwDataType;
	Util::String m_pLinkName;

	DWORD       m_dwNumMaterial;
	cMaterial*  m_pMaterial;
	cMaterialAnimator*  m_pMaterialAnimator;
};


class CloudEmitterData : public EmitterData
{
public:
	CloudEmitterData();

	virtual void ImportFromMemory( const Ptr<IO::Stream>& stream );

	sPCloudInfo& GetPCloudInfo( void ) { return m_sPCloudInfo; }


protected:
	sPCloudInfo m_sPCloudInfo;
};


// particle superspray
class SuperSprayEmitterData : public EmitterData
{
public:
	SuperSprayEmitterData();

	virtual void ImportFromMemory( const Ptr<IO::Stream>& stream );

	sSuperSprayInfo& GetSuperSprayInfo( void ) { return m_sSuperSprayInfo; }

	void CreateSpecialMaterialAnimator( void );

protected:
	sSuperSprayInfo m_sSuperSprayInfo;
};

}

#endif