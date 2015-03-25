  /// \file PointingClusterAssociationAlgorithm.h
/*
 *
 * PointingClusterAssociationAlgorithm.h header template automatically generated by a class generator
 * Creation date : jeu. mars 5 2015
 *
 * This file is part of ArborPFA libraries.
 * 
 * ArborPFA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * ArborPFA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ArborPFA.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#ifndef POINTINGCLUSTERASSOCIATIONALGORITHM_H
#define POINTINGCLUSTERASSOCIATIONALGORITHM_H

#include "arborpfa/algorithm/ArborAlgorithm.h"
#include "arborpfa/algorithm/ArborAlgorithmFactory.h"
#include "arborpfa/content/ArborHelper.h"

// pandora
#include "Pandora/PandoraInternal.h"

namespace arbor
{

/** 
 * @brief PointingClusterAssociationAlgorithm class
 */ 
class PointingClusterAssociationAlgorithm : public ArborAlgorithm 
{
public:

	/**
		* @brief Factory class for instantiating algorithm
		*/
	class Factory : public ArborAlgorithmFactory
	{
		public:
			ArborAlgorithm *CreateArborAlgorithm() const;
	};

	/**
		* @brief Run the algorithm
		*/
	pandora::StatusCode RunArborAlgorithm();

	/**
		* @brief Read settings from Xml handler
		*/
	pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

	/**
	 *
	 */
	bool CanMergeCluster(const arbor::Cluster *pCluster) const;

	/**
	 *
	 */
	pandora::StatusCode FindBestMergeCluster(const arbor::Cluster *pDaughterCluster,
			const arbor::ClusterVector &clusterVector, arbor::Cluster *&pBestParentCluster) const;

protected:

	unsigned int         m_minNObjects;
	unsigned int         m_maxNObjects;
	unsigned int         m_minNPseudoLayers;
	unsigned int         m_maxNPseudoLayers;
	float                 m_barycentreImpactParameterCut;
	float                 m_ellipsoidImpactParameterCut;
	float                 m_ellipsoidClosestDistanceCut;
	float                 m_chi2NSigmaFactor;
	float                 m_chi2AssociationCut;
	float                 m_maxBarycentreAngle;
	std::string            m_energyFunctionName;
};

//--------------------------------------------------------------------------------------------------------------------

inline ArborAlgorithm *PointingClusterAssociationAlgorithm::Factory::CreateArborAlgorithm() const
{
	return new PointingClusterAssociationAlgorithm();
}

} 

#endif  //  POINTINGCLUSTERASSOCIATIONALGORITHM_H