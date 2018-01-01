/*
 * Copyright (C) 2003 Fabien Chereau
 * Copyright (C) 2017 Georg Zotti (interactive modelling)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

//! Class which computes the daylight sky color
//! Fast implementation of the algorithm from the article
//! "A Practical Analytic Model for Daylight" by A. J. Preetham, Peter Shirley and Brian Smits.

#ifndef SKYLIGHT_HPP
#define SKYLIGHT_HPP

#include "StelUtils.hpp"
#include "StelApp.hpp"

#include <cmath>
#include <QDebug>
#include <QObject>
#include <QSettings>

// set this to 1 to use values from the original paper, or 0 to use our own tweaked values.
#define PREETHAM_ORIGINAL 1

typedef struct {
	float pos[3];       //! Vector to the position (vertical = pos[2])
	float color[3];     //! 3 component color, can be RGB or CIE color system
} skylightStruct2;

// GZ We must derive from QObject now to set parameters via GUI
class Skylight: public QObject
{
	Q_OBJECT
	Q_PROPERTY(double AYt  READ getAYt WRITE setAYt NOTIFY AYtChanged)
	Q_PROPERTY(double BYt  READ getBYt WRITE setBYt NOTIFY BYtChanged)
	Q_PROPERTY(double CYt  READ getCYt WRITE setCYt NOTIFY CYtChanged)
	Q_PROPERTY(double DYt  READ getDYt WRITE setDYt NOTIFY DYtChanged)
	Q_PROPERTY(double EYt  READ getEYt WRITE setEYt NOTIFY EYtChanged)
	Q_PROPERTY(double AYc  READ getAYc WRITE setAYc NOTIFY AYcChanged)
	Q_PROPERTY(double BYc  READ getBYc WRITE setBYc NOTIFY BYcChanged)
	Q_PROPERTY(double CYc  READ getCYc WRITE setCYc NOTIFY CYcChanged)
	Q_PROPERTY(double DYc  READ getDYc WRITE setDYc NOTIFY DYcChanged)
	Q_PROPERTY(double EYc  READ getEYc WRITE setEYc NOTIFY EYcChanged)

	Q_PROPERTY(double Axt  READ getAxt WRITE setAxt NOTIFY AxtChanged)
	Q_PROPERTY(double Bxt  READ getBxt WRITE setBxt NOTIFY BxtChanged)
	Q_PROPERTY(double Cxt  READ getCxt WRITE setCxt NOTIFY CxtChanged)
	Q_PROPERTY(double Dxt  READ getDxt WRITE setDxt NOTIFY DxtChanged)
	Q_PROPERTY(double Ext  READ getExt WRITE setExt NOTIFY ExtChanged)
	Q_PROPERTY(double Axc  READ getAxc WRITE setAxc NOTIFY AxcChanged)
	Q_PROPERTY(double Bxc  READ getBxc WRITE setBxc NOTIFY BxcChanged)
	Q_PROPERTY(double Cxc  READ getCxc WRITE setCxc NOTIFY CxcChanged)
	Q_PROPERTY(double Dxc  READ getDxc WRITE setDxc NOTIFY DxcChanged)
	Q_PROPERTY(double Exc  READ getExc WRITE setExc NOTIFY ExcChanged)

	Q_PROPERTY(double Ayt  READ getAyt WRITE setAyt NOTIFY AytChanged)
	Q_PROPERTY(double Byt  READ getByt WRITE setByt NOTIFY BytChanged)
	Q_PROPERTY(double Cyt  READ getCyt WRITE setCyt NOTIFY CytChanged)
	Q_PROPERTY(double Dyt  READ getDyt WRITE setDyt NOTIFY DytChanged)
	Q_PROPERTY(double Eyt  READ getEyt WRITE setEyt NOTIFY EytChanged)
	Q_PROPERTY(double Ayc  READ getAyc WRITE setAyc NOTIFY AycChanged)
	Q_PROPERTY(double Byc  READ getByc WRITE setByc NOTIFY BycChanged)
	Q_PROPERTY(double Cyc  READ getCyc WRITE setCyc NOTIFY CycChanged)
	Q_PROPERTY(double Dyc  READ getDyc WRITE setDyc NOTIFY DycChanged)
	Q_PROPERTY(double Eyc  READ getEyc WRITE setEyc NOTIFY EycChanged)

	Q_PROPERTY(double zX11  READ getZX11 WRITE setZX11 NOTIFY zX11Changed)
	Q_PROPERTY(double zX12  READ getZX12 WRITE setZX12 NOTIFY zX12Changed)
	Q_PROPERTY(double zX13  READ getZX13 WRITE setZX13 NOTIFY zX13Changed)
	Q_PROPERTY(double zX21  READ getZX21 WRITE setZX21 NOTIFY zX21Changed)
	Q_PROPERTY(double zX22  READ getZX22 WRITE setZX22 NOTIFY zX22Changed)
	Q_PROPERTY(double zX23  READ getZX23 WRITE setZX23 NOTIFY zX23Changed)
	Q_PROPERTY(double zX24  READ getZX24 WRITE setZX24 NOTIFY zX24Changed)
	Q_PROPERTY(double zX31  READ getZX31 WRITE setZX31 NOTIFY zX31Changed)
	Q_PROPERTY(double zX32  READ getZX32 WRITE setZX32 NOTIFY zX32Changed)
	Q_PROPERTY(double zX33  READ getZX33 WRITE setZX33 NOTIFY zX33Changed)
	Q_PROPERTY(double zX34  READ getZX34 WRITE setZX34 NOTIFY zX34Changed)

	Q_PROPERTY(double zY11  READ getZY11 WRITE setZY11 NOTIFY zY11Changed)
	Q_PROPERTY(double zY12  READ getZY12 WRITE setZY12 NOTIFY zY12Changed)
	Q_PROPERTY(double zY13  READ getZY13 WRITE setZY13 NOTIFY zY13Changed)
	Q_PROPERTY(double zY21  READ getZY21 WRITE setZY21 NOTIFY zY21Changed)
	Q_PROPERTY(double zY22  READ getZY22 WRITE setZY22 NOTIFY zY22Changed)
	Q_PROPERTY(double zY23  READ getZY23 WRITE setZY23 NOTIFY zY23Changed)
	Q_PROPERTY(double zY24  READ getZY24 WRITE setZY24 NOTIFY zY24Changed)
	Q_PROPERTY(double zY31  READ getZY31 WRITE setZY31 NOTIFY zY31Changed)
	Q_PROPERTY(double zY32  READ getZY32 WRITE setZY32 NOTIFY zY32Changed)
	Q_PROPERTY(double zY33  READ getZY33 WRITE setZY33 NOTIFY zY33Changed)
	Q_PROPERTY(double zY34  READ getZY34 WRITE setZY34 NOTIFY zY34Changed)

	Q_PROPERTY(double T  READ getT WRITE setT NOTIFY turbidityChanged)
	Q_PROPERTY(bool flagSRGB  READ getFlagSRGB WRITE setFlagSRGB NOTIFY flagSRGBChanged)


friend class AtmosphereDialog;

public:
	Skylight();
	virtual ~Skylight();
	//! Set the fixed parameters and precompute what can be
	//! This function has to be called once before any call to get_*_value()
	void setParams(float sunZenithAngle, float turbidity);

	//! Same functions but in vector mode : faster because prevents extra cosine calculations
	//! The position vectors MUST be normalized, and the vertical z component is the third one
	void setParamsv(const float * sunPos, float turbidity);
	
	// Compute the sky color at the given position in the xyY color system and store it in position.color
	// void getxyYValue(skylightStruct * position);
	//! Return the current zenith color in the xyY color system
	//! @param v 3-element vector to receive x, y, Y values
	inline void getZenithColor(float * v) const;

	//! Compute the sky color at the given position in the CIE color system and store it in p.color
	//! @param p.color[0] is CIE x color component
	//! @param p.color[1] is CIE y color component
	//! @param p.color[2] is undefined (CIE Y color component (luminance) if uncommented)
	void getxyYValuev(skylightStruct2& p) const
	{
		const float cosDistSun = sunPos[0]*p.pos[0] + sunPos[1]*p.pos[1] + sunPos[2]*p.pos[2];
		const float distSun = StelUtils::fastAcos(cosDistSun);
		const float cosDistSun_q = cosDistSun*cosDistSun;

		Q_ASSERT(p.pos[2] >= 0.f);
		const float oneOverCosZenithAngle = (p.pos[2]==0.f) ? 1e38f : 1.f / p.pos[2];
		p.color[0] = term_x * (1.f + Ax * std::exp(Bx*oneOverCosZenithAngle))
				* (1.f + Cx * std::exp(Dx*distSun) + Ex * cosDistSun_q);

		p.color[1] = term_y * (1.f + Ay * std::exp(By*oneOverCosZenithAngle))
				* (1.f + Cy * std::exp(Dy*distSun) + Ey * cosDistSun_q);

// 		p.color[2] = term_Y * (1.f + AY * std::exp(BY*oneOverCosZenithAngle))
// 				* (1.f + CY * std::exp(DY*distSun) + EY * cosDistSun_q);


		if (/*p.color[2] < 0. || */p.color[0] < 0.f || p.color[1] < 0.f)
		{
			p.color[0] = 0.25;
			p.color[1] = 0.25;
			p.color[2] = 0.;
		}
	}

	void getShadersParams(Vec3f& asunPos, float& aterm_x, float& aAx, float& aBx, float& aCx, float& aDx, float& aEx,
		float& aterm_y, float& aAy, float& aBy, float& aCy, float& aDy, float& aEy, int& doSRGB) const
	{
		asunPos=sunPos;
		aterm_x=term_x;aAx=Ax;aBx=Bx;aCx=Cx;aDx=Dx;aEx=Ex;
		aterm_y=term_y;aAy=Ay;aBy=By;aCy=Cy;aDy=Dy;aEy=Ey;
		doSRGB=(int)flagSRGB;
	}
	
	// GZ new: reset all parameters...
//	void resetPreethamData()
//	{ // TODO put back the values.
//		AYt=newAYt; BYt=newBYt; CYt=newCYt; DYt=newDYt; EYt=newEYt; AYc=newAYc; BYc=newBYc; CYc=newCYc; DYc=newDYc; EYc=newEYc;
//		Axt=newAxt; Bxt=newBxt; Cxt=newCxt; Dxt=newDxt; Ext=newExt; Axc=newAxc; Bxc=newBxc; Cxc=newCxc; Dxc=newDxc; Exc=newExc;
//		Ayt=newAyt; Byt=newByt; Cyt=newCyt; Dyt=newDyt; Eyt=newEyt; Ayc=newAyc; Byc=newByc; Cyc=newCyc; Dyc=newDyc; Eyc=newEyc;
//	}

signals:
	void AYtChanged(double val);
	void BYtChanged(double val);
	void CYtChanged(double val);
	void DYtChanged(double val);
	void EYtChanged(double val);
	void AYcChanged(double val);
	void BYcChanged(double val);
	void CYcChanged(double val);
	void DYcChanged(double val);
	void EYcChanged(double val);
	void AxtChanged(double val);
	void BxtChanged(double val);
	void CxtChanged(double val);
	void DxtChanged(double val);
	void ExtChanged(double val);
	void AxcChanged(double val);
	void BxcChanged(double val);
	void CxcChanged(double val);
	void DxcChanged(double val);
	void ExcChanged(double val);
	void AytChanged(double val);
	void BytChanged(double val);
	void CytChanged(double val);
	void DytChanged(double val);
	void EytChanged(double val);
	void AycChanged(double val);
	void BycChanged(double val);
	void CycChanged(double val);
	void DycChanged(double val);
	void EycChanged(double val);
	void zX11Changed(double val);
	void zX12Changed(double val);
	void zX13Changed(double val);
	void zX21Changed(double val);
	void zX22Changed(double val);
	void zX23Changed(double val);
	void zX24Changed(double val);
	void zX31Changed(double val);
	void zX32Changed(double val);
	void zX33Changed(double val);
	void zX34Changed(double val);
	void zY11Changed(double val);
	void zY12Changed(double val);
	void zY13Changed(double val);
	void zY21Changed(double val);
	void zY22Changed(double val);
	void zY23Changed(double val);
	void zY24Changed(double val);
	void zY31Changed(double val);
	void zY32Changed(double val);
	void zY33Changed(double val);
	void zY34Changed(double val);
	void turbidityChanged(double val);
	void flagSRGBChanged(bool val);
	
public slots:
	void setAYt(const double val){AYt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/AYt", val); emit AYtChanged(val); computeLuminanceDistributionCoefs();}
	void setBYt(const double val){BYt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/BYt", val); emit BYtChanged(val); computeLuminanceDistributionCoefs();}
	void setCYt(const double val){CYt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/CYt", val); emit CYtChanged(val); computeLuminanceDistributionCoefs();}
	void setDYt(const double val){DYt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/DYt", val); emit DYtChanged(val); computeLuminanceDistributionCoefs();}
	void setEYt(const double val){EYt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/EYt", val); emit EYtChanged(val); computeLuminanceDistributionCoefs();}
	void setAYc(const double val){AYc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/AYc", val); emit AYcChanged(val); computeLuminanceDistributionCoefs();}
	void setBYc(const double val){BYc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/BYc", val); emit BYcChanged(val); computeLuminanceDistributionCoefs();}
	void setCYc(const double val){CYc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/CYc", val); emit CYcChanged(val); computeLuminanceDistributionCoefs();}
	void setDYc(const double val){DYc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/DYc", val); emit DYcChanged(val); computeLuminanceDistributionCoefs();}
	void setEYc(const double val){EYc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/EYc", val); emit EYcChanged(val); computeLuminanceDistributionCoefs();}
	void setAxt(const double val){Axt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Axt", val); emit AxtChanged(val); computeColorDistributionCoefs();}
	void setBxt(const double val){Bxt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Bxt", val); emit BxtChanged(val); computeColorDistributionCoefs();}
	void setCxt(const double val){Cxt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Cxt", val); emit CxtChanged(val); computeColorDistributionCoefs();}
	void setDxt(const double val){Dxt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Dxt", val); emit DxtChanged(val); computeColorDistributionCoefs();}
	void setExt(const double val){Ext=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Ext", val); emit ExtChanged(val); computeColorDistributionCoefs();}
	void setAxc(const double val){Axc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Axc", val); emit AxcChanged(val); computeColorDistributionCoefs();}
	void setBxc(const double val){Bxc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Bxc", val); emit BxcChanged(val); computeColorDistributionCoefs();}
	void setCxc(const double val){Cxc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Cxc", val); emit CxcChanged(val); computeColorDistributionCoefs();}
	void setDxc(const double val){Dxc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Dxc", val); emit DxcChanged(val); computeColorDistributionCoefs();}
	void setExc(const double val){Exc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Exc", val); emit ExcChanged(val); computeColorDistributionCoefs();}
	void setAyt(const double val){Ayt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Ayt", val); emit AytChanged(val); computeColorDistributionCoefs();}
	void setByt(const double val){Byt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Byt", val); emit BytChanged(val); computeColorDistributionCoefs();}
	void setCyt(const double val){Cyt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Cyt", val); emit CytChanged(val); computeColorDistributionCoefs();}
	void setDyt(const double val){Dyt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Dyt", val); emit DytChanged(val); computeColorDistributionCoefs();}
	void setEyt(const double val){Eyt=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Eyt", val); emit EytChanged(val); computeColorDistributionCoefs();}
	void setAyc(const double val){Ayc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Ayc", val); emit AycChanged(val); computeColorDistributionCoefs();}
	void setByc(const double val){Byc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Byc", val); emit BycChanged(val); computeColorDistributionCoefs();}
	void setCyc(const double val){Cyc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Cyc", val); emit CycChanged(val); computeColorDistributionCoefs();}
	void setDyc(const double val){Dyc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Dyc", val); emit DycChanged(val); computeColorDistributionCoefs();}
	void setEyc(const double val){Eyc=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/Eyc", val); emit EycChanged(val); computeColorDistributionCoefs();}
	void setZX11(const double val){zX11=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zX11", val); emit zX11Changed(val); computeZenithColor();}
	void setZX12(const double val){zX12=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zX12", val); emit zX12Changed(val); computeZenithColor();}
	void setZX13(const double val){zX13=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zX13", val); emit zX13Changed(val); computeZenithColor();}
	void setZX21(const double val){zX21=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zX21", val); emit zX21Changed(val); computeZenithColor();}
	void setZX22(const double val){zX22=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zX22", val); emit zX22Changed(val); computeZenithColor();}
	void setZX23(const double val){zX23=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zX23", val); emit zX23Changed(val); computeZenithColor();}
	void setZX24(const double val){zX24=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zX24", val); emit zX24Changed(val); computeZenithColor();}
	void setZX31(const double val){zX31=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zX31", val); emit zX31Changed(val); computeZenithColor();}
	void setZX32(const double val){zX32=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zX32", val); emit zX32Changed(val); computeZenithColor();}
	void setZX33(const double val){zX33=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zX33", val); emit zX33Changed(val); computeZenithColor();}
	void setZX34(const double val){zX34=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zX34", val); emit zX34Changed(val); computeZenithColor();}
	void setZY11(const double val){zY11=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zY11", val); emit zY11Changed(val); computeZenithColor();}
	void setZY12(const double val){zY12=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zY12", val); emit zY12Changed(val); computeZenithColor();}
	void setZY13(const double val){zY13=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zY13", val); emit zY13Changed(val); computeZenithColor();}
	void setZY21(const double val){zY21=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zY21", val); emit zY21Changed(val); computeZenithColor();}
	void setZY22(const double val){zY22=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zY22", val); emit zY22Changed(val); computeZenithColor();}
	void setZY23(const double val){zY23=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zY23", val); emit zY23Changed(val); computeZenithColor();}
	void setZY24(const double val){zY24=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zY24", val); emit zY24Changed(val); computeZenithColor();}
	void setZY31(const double val){zY31=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zY31", val); emit zY31Changed(val); computeZenithColor();}
	void setZY32(const double val){zY32=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zY32", val); emit zY32Changed(val); computeZenithColor();}
	void setZY33(const double val){zY33=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zY33", val); emit zY33Changed(val); computeZenithColor();}
	void setZY34(const double val){zY34=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/zY34", val); emit zY34Changed(val); computeZenithColor();}

	double getAYt(void) const {return AYt; }
	double getBYt(void) const {return BYt; }
	double getCYt(void) const {return CYt; }
	double getDYt(void) const {return DYt; }
	double getEYt(void) const {return EYt; }
	double getAYc(void) const {return AYc; }
	double getBYc(void) const {return BYc; }
	double getCYc(void) const {return CYc; }
	double getDYc(void) const {return DYc; }
	double getEYc(void) const {return EYc; }
	double getAxt(void) const {return Axt; }
	double getBxt(void) const {return Bxt; }
	double getCxt(void) const {return Cxt; }
	double getDxt(void) const {return Dxt; }
	double getExt(void) const {return Ext; }
	double getAxc(void) const {return Axc; }
	double getBxc(void) const {return Bxc; }
	double getCxc(void) const {return Cxc; }
	double getDxc(void) const {return Dxc; }
	double getExc(void) const {return Exc; }
	double getAyt(void) const {return Ayt; }
	double getByt(void) const {return Byt; }
	double getCyt(void) const {return Cyt; }
	double getDyt(void) const {return Dyt; }
	double getEyt(void) const {return Eyt; }
	double getAyc(void) const {return Ayc; }
	double getByc(void) const {return Byc; }
	double getCyc(void) const {return Cyc; }
	double getDyc(void) const {return Dyc; }
	double getEyc(void) const {return Eyc; }

	double getZX11(void) const {return zX11; }
	double getZX12(void) const {return zX12; }
	double getZX13(void) const {return zX13; }
	double getZX21(void) const {return zX21; }
	double getZX22(void) const {return zX22; }
	double getZX23(void) const {return zX23; }
	double getZX24(void) const {return zX24; }
	double getZX31(void) const {return zX31; }
	double getZX32(void) const {return zX32; }
	double getZX33(void) const {return zX33; }
	double getZX34(void) const {return zX34; }

	double getZY11(void) const {return zY11; }
	double getZY12(void) const {return zY12; }
	double getZY13(void) const {return zY13; }
	double getZY21(void) const {return zY21; }
	double getZY22(void) const {return zY22; }
	double getZY23(void) const {return zY23; }
	double getZY24(void) const {return zY24; }
	double getZY31(void) const {return zY31; }
	double getZY32(void) const {return zY32; }
	double getZY33(void) const {return zY33; }
	double getZY34(void) const {return zY34; }

	double getT(void) const {return T;}
	void setT(double newT){T=newT; emit turbidityChanged(newT); }

	void setFlagSRGB(bool val){flagSRGB=val; QSettings* conf = StelApp::getInstance().getSettings(); conf->setValue("Skylight/use_sRGB", val); emit flagSRGBChanged(val);}
	bool getFlagSRGB() {return flagSRGB;}

private:
	double thetas;  // angular distance between the zenith and the sun in radian
	double T;       // Turbidity : i.e. sky "clarity"
	               //  1 : pure air
	               //  2 : exceptionnally clear
	               //  4 : clear
	               //  8 : light haze
	               // 25 : haze
	               // 64 : thin fog

	// Computed variables depending on the 2 above
	double zenithLuminance;     // Y color component of the CIE color at zenith (luminance)
	double zenithColorX;        // x color component of the CIE color at zenith
	double zenithColorY;        // y color component of the CIE color at zenith

	double eyeLumConversion;    // luminance conversion for an eye adapted to screen luminance
	                           // (around 40 cd/m^2)

	double AY, BY, CY, DY, EY;  // Distribution coefficients for the luminance distribution function
	double Ax, Bx, Cx, Dx, Ex;  // Distribution coefficients for x distribution function
	double Ay, By, Cy, Dy, Ey;  // Distribution coefficients for y distribution function

	// GZ Experimental: make fully GUI configurable parameters to play! Naming: AY=AYt*T+AYc, etc. (i.e., Tfactor, Constant)
	double AYt, BYt, CYt, DYt, EYt;  // Distribution coefficients for the luminance distribution function
	double Axt, Bxt, Cxt, Dxt, Ext;  // Distribution coefficients for x distribution function
	double Ayt, Byt, Cyt, Dyt, Eyt;  // Distribution coefficients for y distribution function
	double AYc, BYc, CYc, DYc, EYc;  // Distribution coefficients for the luminance distribution function
	double Axc, Bxc, Cxc, Dxc, Exc;  // Distribution coefficients for x distribution function
	double Ayc, Byc, Cyc, Dyc, Eyc;  // Distribution coefficients for y distribution function

	double zX11, zX12, zX13;
	double zX21, zX22, zX23, zX24;
	double zX31, zX32, zX33, zX34;
	double zY11, zY12, zY13;
	double zY21, zY22, zY23, zY24;
	double zY31, zY32, zY33, zY34;

	bool flagSRGB;             // Apply sRGB color conversion. If false, applies AdobeRGB(1998) (Stellarium's original default)

	float term_x;              // Precomputed term for x calculation
	float term_y;              // Precomputed term for y calculation
	float term_Y;              // Precomputed term for luminance calculation. We will actually not use it because luminance comes from SkyBright (Schaefer's model)

	float sunPos[3];

	// Compute CIE Y (luminance) for zenith in cd/m^2
	inline void computeZenithLuminance(void);
	// Compute CIE x and y color components
	inline void computeZenithColor(void);
	// Compute the luminance distribution coefficients
	inline void computeLuminanceDistributionCoefs(void);
	// Compute the color distribution coefficients
	inline void computeColorDistributionCoefs(void);
};

// Return the current zenith color in xyY color system
inline void Skylight::getZenithColor(float * v) const
{
	v[0] = zenithColorX;
	v[1] = zenithColorY;
	v[2] = zenithLuminance;
}

// Compute CIE luminance for zenith in cd/m^2
inline void Skylight::computeZenithLuminance(void)
{
	zenithLuminance = 1000.f * ((4.0453f*T - 4.9710f) * std::tan( (0.4444f - T*(1.f/120.f)) * (M_PIf-2.f*thetas) ) -
		0.2155f*T + 2.4192f);
	zenithLuminance=qMax(zenithLuminance, 0.00000000001);
}


// Compute CIE x and y color components
// Edit: changed some coefficients to get new sky color
// GZ: 2016-01 changed back to original Preetham values.
// GZ: 2016-01b made them configurable with 2 presets: Preetham and Stellarium.
inline void Skylight::computeZenithColor(void)
{

#ifdef PREETHAM_ORIGINAL
//	zenithColorX = (( ( (( 0.00166f*thetas - 0.00375f)*thetas + 0.00209f)*thetas)             * T
//			   +((-0.02903f*thetas + 0.06377f)*thetas - 0.03202f)*thetas + 0.00394f)  * T
//			   +(( 0.11693f*thetas - 0.21196f)*thetas + 0.06052f)*thetas + 0.25886f);
//	zenithColorY = (( ( (( 0.00275f*thetas - 0.00610f)*thetas + 0.00317f)*thetas)             * T
//			   +((-0.04214f*thetas + 0.08970f)*thetas - 0.04153f)*thetas + 0.00516f)  * T
//			   +(( 0.15346f*thetas - 0.26756f)*thetas + 0.06670f)*thetas + 0.26688f);

	zenithColorX=(( ( (( zX11*thetas + zX12)*thetas + zX13)*thetas)       * T
			+ (( zX21*thetas + zX22)*thetas + zX23)*thetas + zX24)* T
		      +((    zX31*thetas + zX32)*thetas + zX33)*thetas + zX34);
	zenithColorY=(( ( (( zY11*thetas + zY12)*thetas + zY13)*thetas)       * T
			+ (( zY21*thetas + zY22)*thetas + zY23)*thetas + zY24)* T
		      +((    zY31*thetas + zY32)*thetas + zY33)*thetas + zY34);

#else
	static float thetas2;
	static float thetas3;
	static float T2;

	thetas2 = thetas * thetas;
	thetas3 = thetas2 * thetas;
	T2 = T * T;
	zenithColorX = ( 0.00216f*thetas3 - 0.00375f*thetas2 + 0.00209f*thetas) * T2 +
	               (-0.02903f*thetas3 + 0.06377f*thetas2 - 0.03202f*thetas + 0.00394f) * T +
	               ( 0.10169f*thetas3 - 0.21196f*thetas2 + 0.06052f*thetas + 0.25886f);

	zenithColorY = ( 0.00275f*thetas3 - 0.00610f*thetas2 + 0.00317f*thetas) * T2 +
	               (-0.04214f*thetas3 + 0.08970f*thetas2 - 0.04153f*thetas + 0.00516f) * T +
	               ( 0.14535f*thetas3 - 0.26756f*thetas2 + 0.06670f*thetas + 0.26688f);
#endif
}

// Compute the luminance distribution coefficients
// FC Edit 2003(?) changed some coefficients to get new sky color
// GZ: 2016-01 changed back to original Preetham values.
// GZ: 2016-01b made them configurable with 2 presets: Preetham and Stellarium.
inline void Skylight::computeLuminanceDistributionCoefs(void)
{
	AY=AYt*T+AYc;
	BY=BYt*T+BYc; BY=qMin(0.0, BY);
	CY=CYt*T+CYc;
	DY=DYt*T+DYc;
	EY=EYt*T+EYc;

//#ifdef PREETHAM_ORIGINAL
//	AY = 0.1787f*T - 1.4630f; // Original
//	CY =-0.0227f*T + 5.3251f; // Original
//#else
//	AY = 0.2787f*T - 1.0630f; // FC values
//	CY =-0.0227f*T + 6.3251f; // FC values
//#endif
//	BY =-0.3554f*T + 0.4275f;
//	DY = 0.1206f*T - 2.5771f;
//	EY =-0.0670f*T + 0.3703f;
//	// with BY>0 the formulas in getxyYValuev make no sense, from which follows T>0.4275/0.3554(=1.203)

	// GZ For the experiments this is dangerous...
	//Q_ASSERT(BY <= 0.0);
}

// Compute the color distribution coefficients
// FC Edit 2003(?) changed some coefficients to get new sky color
// GZ: TODO 2016-01 find and change back to original Preetham values.
inline void Skylight::computeColorDistributionCoefs(void)
{
	Ax=Axt*T+Axc;
	Bx=Bxt*T+Bxc; Bx=qMin(0.0, Bx);
	Cx=Cxt*T+Cxc;
	Dx=Dxt*T+Dxc;
	Ex=Ext*T+Exc;
	Ay=Ayt*T+Ayc;
	By=Byt*T+Byc; By=qMin(0.0, By);
	Cy=Cyt*T+Cyc;
	Dy=Dyt*T+Dyc;
	Ey=Eyt*T+Eyc;

//#ifdef PREETHAM_ORIGINAL
//	Ax =-0.0193f*T - 0.2592f;
//	Bx =-0.0665f*T + 0.0008f;
//	Cx =-0.0004f*T + 0.2125f;
//	Dx =-0.0641f*T - 0.8989f;
//	Ex =-0.0033f*T + 0.0452f;

//	Ay =-0.0167f*T - 0.2608f;
//	By =-0.0950f*T + 0.0092f;
//	Cy =-0.0079f*T + 0.2102f;
//	Dy =-0.0441f*T - 1.6537f;
//	Ey =-0.0109f*T + 0.0529f;
//#else
//	Ax =-0.0148f*T - 0.1703f;
//	Bx =-0.0664f*T + 0.0011f;
//	Cx =-0.0005f*T + 0.2127f;
//	Dx =-0.0641f*T - 0.8992f;
//	Ex =-0.0035f*T + 0.0453f;

//	Ay =-0.0131f*T - 0.2498f;
//	By =-0.0951f*T + 0.0092f;
//	Cy =-0.0082f*T + 0.2404f;
//	Dy =-0.0438f*T - 1.0539f;
//	Ey =-0.0109f*T + 0.0531f;
//#endif

	// with Bx,By>0 the formulas in getxyYValuev make no sense. --> T>0.0011/0.0664=0.017; T>0.0092/0.0951=0.097
	// GZ Deactivated for experimenting...
	//Q_ASSERT(Bx <= 0.0);
	//Q_ASSERT(By <= 0.0);
}

#endif // SKYLIGHT_HPP

