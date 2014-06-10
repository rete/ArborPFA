  /// \file NeutralTreeMergingAlgorithm.h
/*
 *
 * NeutralTreeMergingAlgorithm.h header template automatically generated by a class generator
 * Creation date : mer. juin 4 2014
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


#ifndef NEUTRALTREEMERGINGALGORITHM_H
#define NEUTRALTREEMERGINGALGORITHM_H

// arborpfa
#include "arborpfa/algorithm/ArborAlgorithm.h"
#include "arborpfa/algorithm/ArborAlgorithmFactory.h"

// pandora
#include "Pandora/PandoraInternal.h"


namespace arbor
{

/** 
 * @brief NeutralTreeMergingAlgorithm class
 */ 
class NeutralTreeMergingAlgorithm : public ArborAlgorithm 
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
	pandora::StatusCode BuildTreeMergingList(const TreeList &inputTreeList, TreeList &treeList, TreeList &alreadyUsedTreeList, Tree *pCurrentTree);

	// algorithm parameters
	bool          m_useClustersWithTrackAssociation;
	float         m_seedSeparationDistance;

};

//--------------------------------------------------------------------------------------------------------------------

inline ArborAlgorithm *NeutralTreeMergingAlgorithm::Factory::CreateArborAlgorithm() const
{
	return new NeutralTreeMergingAlgorithm();
}

} 

#endif  //  NEUTRALTREEMERGINGALGORITHM_H