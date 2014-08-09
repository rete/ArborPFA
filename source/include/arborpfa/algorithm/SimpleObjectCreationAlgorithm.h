  /// \file SimpleArborObjectCreationAlgorithm.h
/*
 *
 * SimpleArborObjectCreationAlgorithm.h header template automatically generated by a class generator
 * Creation date : ven. mai 23 2014
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


#ifndef SIMPLEOBJECTCREATIONALGORITHM_H
#define SIMPLEOBJECTCREATIONALGORITHM_H

#include "arborpfa/algorithm/ArborAlgorithmFactory.h"
#include "arborpfa/algorithm/ArborAlgorithm.h"

// pandora
#include "Pandora/PandoraInternal.h"

namespace arbor
{

/** 
 * @brief SimpleObjectCreationlgorithm class
 */ 
class SimpleObjectCreationAlgorithm : public ArborAlgorithm
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

protected:

	/**
	 *
	 */
	pandora::StatusCode CreateReadoutLayerMap(const pandora::CaloHitList *pCaloHitList, pandora::OrderedCaloHitList &orderedCaloHitList) const;

	/**
	 *
	 */
	pandora::StatusCode RecursiveClustering(const pandora::CaloHitList *pInputCaloHitList, pandora::CaloHitList &outputCaloHitList, pandora::CaloHitList &alreadyUsedCaloHitList, pandora::CaloHit *pCaloHitToLookAround, float maxSeparationDistance) const;

	/**
	 *
	 */
	pandora::StatusCode CreateObjectsFromCaloHitList(const pandora::CaloHitList &caloHitList, bool shouldSplit) const;

	/**
	 *
	 */
	pandora::StatusCode CreateEcalObjects(const pandora::CaloHitList &ecalCaloHitList);

	/**
	 *
	 */
	pandora::StatusCode CreateHcalObjects(const pandora::CaloHitList &hcalCaloHitList);



	// algorithm members
	bool            m_shouldSplitClusterInSingleCaloHitClustersInEcal;
	bool            m_shouldUseOnlyCaloHitsForObjectsInEcal;
	bool            m_shouldUseReadoutLayerInEcal;
	unsigned int   m_maximumSizeForClusterSplittingInEcal;
	float           m_intraLayerMaxDistanceInEcal;

	bool            m_shouldSplitClusterInSingleCaloHitClustersInHcal;
	bool            m_shouldUseOnlyCaloHitsForObjectsInHcal;
	bool            m_shouldUseReadoutLayerInHcal;
	unsigned int   m_maximumSizeForClusterSplittingInHcal;
	float           m_intraLayerMaxDistanceInHcal;
};

//--------------------------------------------------------------------------------------------------------------------

inline ArborAlgorithm *SimpleObjectCreationAlgorithm::Factory::CreateArborAlgorithm() const
{
	return new SimpleObjectCreationAlgorithm();
}

} 

#endif  //  SIMPLEOBJECTCREATIONALGORITHM_H
