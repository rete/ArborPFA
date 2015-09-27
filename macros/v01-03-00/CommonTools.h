#ifndef CommonTools_h
#define CommonTools_h

// std
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>
#include <map>
#include <exception>
#include <stdexcept>

// root
#include "TPaveText.h"
#include "TString.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH1S.h"
#include "TH1F.h"
#include "TH1I.h"

#define TITLE_FONTSIZE 26
#define LABEL_FONTSIZE 22

#define LEFT_MARGIN 0.17
#define RIGHT_MARGIN 0.03
#define TOP_MARGIN 0.05
#define GLOBAL_FONT 42

//-----------------------------------------------------------------------------------------------------

enum AlgorithmType
{
	ARBOR_PFA,
	PANDORA_PFA
};

//-----------------------------------------------------------------------------------------------------

enum DataType
{
	SIMULATION,
	TEST_BEAM
};

//-----------------------------------------------------------------------------------------------------

template<typename T>
class Distribution
{
public:

	inline ~Distribution()
	{
		distribution.clear();
	}

	std::vector<T> distribution;
	T mean;
	T rms;
	T rms90;
	T error;
};

//-----------------------------------------------------------------------------------------------------

template <typename T>
TH1 *createHistogram(const char *name, const char *title, int nBin, int binMin, int binMax);

//-----------------------------------------------------------------------------------------------------

template <>
TH1 *createHistogram<float>(const char *name, const char *title, int nBin, int binMin, int binMax)
{
	return new TH1F(name, title, nBin, binMin, binMax);
}

//-----------------------------------------------------------------------------------------------------

template <>
TH1 *createHistogram<double>(const char *name, const char *title, int nBin, int binMin, int binMax)
{
	return new TH1D(name, title, nBin, binMin, binMax);
}

//-----------------------------------------------------------------------------------------------------

template <>
TH1 *createHistogram<int>(const char *name, const char *title, int nBin, int binMin, int binMax)
{
	return new TH1I(name, title, nBin, binMin, binMax);
}

//-----------------------------------------------------------------------------------------------------

template <>
TH1 *createHistogram<short>(const char *name, const char *title, int nBin, int binMin, int binMax)
{
	return new TH1S(name, title, nBin, binMin, binMax);
}

//-----------------------------------------------------------------------------------------------------

template <typename T>
TH1 *createHistogram(const char *name, const char *title,
		int nBin, int binMin, int binMax, const std::vector<T> &data)
{
	TH1 *pHistogram = createHistogram<T>(name, title, nBin, binMin, binMax);

	for(unsigned int i=0 ; i<data.size() ; i++)
		pHistogram->Fill(data.at(i));

	return pHistogram;
}

//-----------------------------------------------------------------------------------------------------

template <typename T>
TH1 *createHistogram(const char *name, const char *title,
		int nBin, int binMin, int binMax, const Distribution<T> &distribution)
{
	return createHistogram<T>(name, title, nBin, binMin, binMax, distribution.distribution);
}

//-----------------------------------------------------------------------------------------------------

template <typename T>
unsigned int count(const Distribution<T> &distribution, T min, T max)
{
	if(min > max)
		return 0;

	unsigned int counter = 0;

	for(unsigned int i=0 ; i<distribution.distribution.size() ; i++)
	{
		if(distribution.distribution.at(i) > min && distribution.distribution.at(i) < max)
			counter++;
	}

	return counter;
}

//-----------------------------------------------------------------------------------------------------

template <typename T>
float integral(const Distribution<T> &distribution, T min, T max)
{
	unsigned int counter = count(distribution, min, max);
	return static_cast<float>(counter)/static_cast<float>(distribution.distribution.size());
}

//-----------------------------------------------------------------------------------------------------

enum DrawAttributeType
{
 MARKER_STYLE,
 MARKER_COLOR,
 MARKER_SIZE,
 LINE_COLOR,
 LINE_STYLE,
 LINE_WIDTH,
 FILL_COLOR,
 FILL_STYLE
};

typedef std::map<DrawAttributeType, int> DrawAttributeMap;

//-----------------------------------------------------------------------------------------------------

// implementation of the draw attribute mapping
class DrawAttributeMapping
{
public:

 DrawAttributeMapping(AlgorithmType algorithmType, DataType dataType)
 {
 	m_drawAttributeMap[LINE_STYLE] = 1;

 	if(algorithmType == ARBOR_PFA)
 	{
 		m_drawAttributeMap[MARKER_COLOR] = kRed;
 		m_drawAttributeMap[LINE_COLOR] = kRed;
 		m_drawAttributeMap[MARKER_STYLE] = 23;
 	}
 	else
 	{
 		m_drawAttributeMap[MARKER_COLOR] = kBlack;
 		m_drawAttributeMap[LINE_COLOR] = kBlack;
 		m_drawAttributeMap[MARKER_STYLE] = 21;
 	}

		m_drawAttributeMap[LINE_WIDTH] = 1;
		m_drawAttributeMap[MARKER_SIZE] = 1;
		m_drawAttributeMap[FILL_COLOR] = 0;
		m_drawAttributeMap[FILL_STYLE] = 0;
 }

	int getAttribute(DrawAttributeType attributeType) const
	{
		DrawAttributeMap::const_iterator findIter = m_drawAttributeMap.find(attributeType);

		if(findIter == m_drawAttributeMap.end())
			return 0;

		return findIter->second;
	}

private:

	DrawAttributeMap    m_drawAttributeMap;
};

//-----------------------------------------------------------------------------------------------------

class GraphSettings
{
public:
	static const int arborSimMarkerStyle = 23;
	static const int arborTBMarkerStyle = 23;
	static const int pandoraSimMarkerStyle = 21;
	static const int pandoraTBMarkerStyle = 21;

	static const int arborSimMarkerColor = kRed;
	static const int arborTBMarkerColor = kRed;
	static const int pandoraSimMarkerColor = kBlack;
	static const int pandoraTBMarkerColor = kBlack;

	static const int arborSimLineColor = kRed;
	static const int arborTBLineColor = kRed;
	static const int pandoraSimLineColor = kBlack;
	static const int pandoraTBLineColor = kBlack;

	static const int arborSimLineStyle = 1;
	static const int arborTBLineStyle = 7;
	static const int pandoraSimLineStyle = 1;
	static const int pandoraTBLineStyle = 7;

	static const int commonLineWidth = 2;
	static const int commonLegendLineColor = kBlack;
	static const int commonLegendFillColor = 0;

	static const int commonMarkerStyle = 23;
	static const int commonMarkerColor = kBlack;
	static const int commonLineColor = kBlack;
	static const int commonLineStyle = 1;
};

//-----------------------------------------------------------------------------------------------------

void configureGraph(TGraph *pGraph, const DrawAttributeMapping &drawAttributeMapping)
{
	if(NULL == pGraph)
		return;

	// line
	pGraph->SetLineColor(drawAttributeMapping.getAttribute(LINE_COLOR));
	pGraph->SetLineStyle(drawAttributeMapping.getAttribute(LINE_STYLE));
	pGraph->SetLineWidth(drawAttributeMapping.getAttribute(LINE_WIDTH));

	// marker
	pGraph->SetMarkerColor(drawAttributeMapping.getAttribute(MARKER_COLOR));
	pGraph->SetMarkerSize(drawAttributeMapping.getAttribute(MARKER_SIZE));
	pGraph->SetMarkerStyle(drawAttributeMapping.getAttribute(MARKER_STYLE));

	// fill
	pGraph->SetFillColor(drawAttributeMapping.getAttribute(FILL_COLOR));
	pGraph->SetFillStyle(drawAttributeMapping.getAttribute(FILL_STYLE));
}

//-----------------------------------------------------------------------------------------------------

void drawOverlayMultiGraph(TMultiGraph *pMultiGraph, const std::string &yAxisTitle)
{
	if(NULL == pMultiGraph)
		return;

	pMultiGraph->Draw("alp");
	pMultiGraph->GetXaxis()->SetTitle("Distance between showers [cm]");
	pMultiGraph->GetXaxis()->SetRangeUser(-1, 31);
	pMultiGraph->GetYaxis()->SetTitle(yAxisTitle.c_str());
	pMultiGraph->SetTitle("");
}

//TLegend *generateLegendFromCanvas(TCanvas *pCanvas)
//{
//	if(NULL == pCanvas)
//		return;
//
//	TLegend *pLegend =
//}

//-----------------------------------------------------------------------------------------------------

std::string AlgorithmName(AlgorithmType type)
{
	if(ARBOR_PFA == type)
		return "ArborPFA";
	else
		return "PandoraPFA";
}

//-----------------------------------------------------------------------------------------------------

std::string DataName(DataType type)
{
	if(SIMULATION == type)
		return "Simulation";
	else
		return "Test Beam";
}

//-----------------------------------------------------------------------------------------------------

std::string GetStringEnergy(DataType dataType, int energy)
{
	if(SIMULATION == dataType)
	{
		std::stringstream ss;
		ss << energy;
		return ss.str();
	}
	else
	{
		switch (energy)
		{
		 case 10:
		 	return "715693";
		 case 20:
		 	return "715675";
		 case 30:
		 	return "715747";
		 case 40:
		 	return "715748";
		 case 50:
		 	return "715751";
		 case 60:
		 	return "715753";
		 case 70:
		 	return "715754";
		 case 80:
		 	return "715756";
		 default:
		 	throw std::invalid_argument("Invalid energy");
		}
	}
}

//-----------------------------------------------------------------------------------------------------

std::string BuildSingleParticleFileName(int energy, AlgorithmType algorithmType, DataType dataType)
{
	std::string fileName = "SingleParticleReconstruction_";

	if(dataType == SIMULATION)
	{
		fileName += GetStringEnergy(SIMULATION, energy) + "GeV";
	}
	else
	{
		fileName += GetStringEnergy(TEST_BEAM, energy);
	}

	fileName += "_cut_" + AlgorithmName(algorithmType) + "_";

	if(dataType == SIMULATION)
	{
		fileName += "MC";
	}
	else
	{
		fileName += "TB";
	}

	fileName += "_basic.root";

	return fileName;
}

//-----------------------------------------------------------------------------------------------------

std::string BuildOverlayEventFileName(int energy1, int energy2, int separationDistance, AlgorithmType algorithmType, DataType dataType)
{
	std::string fileName = "OverlayEventReconstruction_";

	if(dataType == SIMULATION)
	{
		fileName += GetStringEnergy(SIMULATION, energy1) + "GeV";
	}
	else
	{
		fileName += GetStringEnergy(TEST_BEAM, energy1);
	}

	fileName += "_neutral_";

	if(dataType == SIMULATION)
	{
		fileName += GetStringEnergy(SIMULATION, energy2) + "GeV";
	}
	else
	{
		fileName += GetStringEnergy(TEST_BEAM, energy2);
	}

	fileName += "_charged_";
	std::stringstream ss;
	ss << separationDistance;
	fileName += ss.str() + "cm_cut_" + AlgorithmName(algorithmType) + "_";

	if(dataType == SIMULATION)
	{
		fileName += "MC";
	}
	else
	{
		fileName += "TB";
	}

	fileName += "_basic.root";

	return fileName;
}

//-----------------------------------------------------------------------------------------------------

double resolutionFunction(double *x, double *par)
{
  return std::sqrt(par[0]*par[0]/x[0] + par[1]*par[1]);
}

//-----------------------------------------------------------------------------------------------------

TPaveText *CreateText(const TString &str)
{
  TPaveText *pt = new TPaveText(0.60, 0.81, 0.93, 0.86, "tbNDC");

  pt->SetTextSize(0.04);
  pt->SetTextColor(kGray);
  pt->SetFillColor(kWhite);
  pt->SetLineWidth(0);
  pt->SetBorderSize(0);
  pt->SetTextAlign(22); //center
  pt->AddText(str);
  pt->SetTextFont(62);

  return pt;
}

//-----------------------------------------------------------------------------------------------------

void CaliceStyle()
{
		gROOT->SetStyle("Plain");
		gStyle->SetPalette(1);
		gStyle->SetPadTickX(1);
		gStyle->SetPadTickY(1);
		gStyle->SetLabelFont(GLOBAL_FONT,"xyz");
		gStyle->SetTitleFont(GLOBAL_FONT);
		gStyle->SetTitleFont(GLOBAL_FONT,"xyz");
		gStyle->SetStatFont(GLOBAL_FONT);

		gStyle->SetFrameFillColor( kWhite );
		gStyle->SetFrameLineWidth(1);
		gStyle->SetCanvasColor(kWhite);
//		gStyle->SetOptStat(0);          /*don't show statistics box*/
		gStyle->SetTitleSize(0.035, "xyz");
		gStyle->SetTitleXOffset(1.5);
		gStyle->SetTitleYOffset(1.5);
		gStyle->SetLegendBorderSize(1);

		gStyle->SetPadTopMargin(0.05);
		gStyle->SetPadBottomMargin(0.14);
		gStyle->SetPadLeftMargin(0.15);
		gStyle->SetPadRightMargin(0.03);

		gROOT->ForceStyle();
}

//-----------------------------------------------------------------------------------------------------

template <typename T>
T Square(T x, T y)
{
	return x += y*y;
}

//-----------------------------------------------------------------------------------------------------

template <typename T>
T Mean(const std::vector<T> &vec)
{
	return std::accumulate( vec.begin() , vec.end() , 0.0 ) *1.0 / (float)vec.size();
}

//-----------------------------------------------------------------------------------------------------

template <typename T>
T RMS(const std::vector<T> &vec)
{
	T mean = Mean<T>( vec );
	T meanSquare = std::accumulate( vec.begin() , vec.end() , 0.f , Square<T> );
	return std::sqrt( (meanSquare - mean*mean ) / vec.size() );
}

//-----------------------------------------------------------------------------------------------------

template <typename T>
T RMS90( const std::vector<T> &vec )
{
	std::vector<T> distanceFromMean;
	int erasePosition = int( double(vec.size()) / 10.0 );
	T mean = Mean<T>( vec );

	for( unsigned int i=0 ; i<vec.size() ; i++ )
		distanceFromMean.push_back( abs(mean - vec.at(i))  );

	std::sort( distanceFromMean.begin() , distanceFromMean.end() );
	distanceFromMean.erase( distanceFromMean.end() - erasePosition, distanceFromMean.end() );

	return RMS<T>( distanceFromMean );
}

//-----------------------------------------------------------------------------------------------------

void CanvasFormat(TCanvas *canvas)
{
	if(canvas == 0)
		return;

	canvas->Size( 875 , 800 );
	canvas->SetWindowSize( 750 , 800 );
	canvas->SetGrid(0, 0);
	canvas->SetCanvasSize(700, 650);
}

//-----------------------------------------------------------------------------------------------------

void GraphFormat(TGraph *graph) {

	if(graph == 0)
		return;

	graph->SetEditable( kFALSE );
	graph->GetXaxis()->SetNdivisions( 510 );
	graph->GetYaxis()->SetNdivisions( 505 );
	graph->SetMarkerStyle( 23 );
	graph->SetMarkerSize( 1 );
}

//-----------------------------------------------------------------------------------------------------

void LegendFormat(TLegend *pLegend)
{
	if(NULL == pLegend)
		return;

	pLegend->SetLineColor(0);
	pLegend->SetFillColor(0);
	pLegend->SetTextFont(GLOBAL_FONT);
}

//-----------------------------------------------------------------------------------------------------

class GraphInfo
{
public:

	GraphInfo(AlgorithmType algorithmType, DataType dataType)
	{
		m_lineWidth = 4;

		if(PANDORA_PFA == algorithmType)
		{
			m_markerStyle = 21;
			m_markerColor = kBlack;
			m_lineColor = kBlack;
		}
		else
		{
			m_markerStyle = 23;
			m_markerColor = kRed;
			m_lineColor = kRed;
		}
		m_lineStyle = 0;
	}

	int m_markerStyle;
	int m_markerColor;
	int m_lineColor;
	int m_lineStyle;
	int m_lineWidth;
};


#endif  //  CommonTools_h
