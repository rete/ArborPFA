  /// \file ArborParentAlgorithm.cc
/*
 *
 * ArborParentAlgorithm.cc source template automatically generated by a class generator
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
 * @author Ete Remi
 * @copyright CNRS , IPNL
 */


#include "arborpfa/algorithm/ArborParentAlgorithm.h"

// pandora
#include "Api/PandoraContentApi.h"

// arborpfa
#include "arborpfa/api/ArborContentApi.h"
#include "arborpfa/arbor/AlgorithmHeaders.h"


using namespace pandora;

namespace arbor
{

StatusCode ArborParentAlgorithm::RunArborAlgorithm()
{
 // If specified, change the current calo hit list, i.e. the input to the clustering algorithm
 std::string originalCaloHitListName;

 if (!m_inputCaloHitListName.empty())
 {
     PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentCaloHitListName(*this, originalCaloHitListName));
     PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentCaloHitList(*this, m_inputCaloHitListName));
 }

 if(!m_objectCreationAlgorithmName.empty())
 {
 	// Run the object formation algorithm
 	const ObjectList *pObjectList = NULL;
 	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborContentApi::RunObjectCreationAlgorithm(*this, m_objectCreationAlgorithmName, pObjectList));

 	// Save the new arbor object list
		if (!pObjectList->empty())
		{
			PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborContentApi::SaveObjectList(*this, m_objectListName));

			if (m_replaceCurrentObjectList)
			{
				PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborContentApi::ReplaceCurrentObjectList(*this, m_objectListName));
			}
		}
 }

 if(!m_clusterCreationAlgorithmName.empty())
 {
 	std::string originalObjectListName;

 	if(m_objectCreationAlgorithmName.empty())
 	{
 		if(!m_objectListName.empty())
 		{
 			PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborContentApi::GetCurrentObjectListName(*this, originalObjectListName));
 			PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborContentApi::ReplaceCurrentObjectList(*this, m_objectListName));
 		}
 	}

 	// Run the cluster formation algorithm
 	const arbor::ClusterList *pClusterList = NULL;
 	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborContentApi::RunClusterCreationAlgorithm(*this, m_clusterCreationAlgorithmName, pClusterList));

 	// Save the new object list
		if (!pClusterList->empty())
		{
			PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborContentApi::SaveClusterList(*this, m_clusterListName));

			if (m_replaceCurrentClusterList)
			{
				PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborContentApi::ReplaceCurrentClusterList(*this, m_clusterListName));
			}
		}

		for(pandora::StringVector::iterator algIter = m_associationAlgorithmList.begin() , algEndIter = m_associationAlgorithmList.end() ;
				algEndIter != algIter ; ++algIter)
		{
			PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, *algIter));
		}


	 if (m_restoreOriginalObjectList && !m_objectListName.empty() && m_objectCreationAlgorithmName.empty())
	 {
	  PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborContentApi::ReplaceCurrentObjectList(*this, originalObjectListName));
	 }
 }

 // Unless specified, return current calo hit list to that when algorithm started
 if (m_restoreOriginalCaloHitList && !m_inputCaloHitListName.empty())
 {
  PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentCaloHitList(*this, originalCaloHitListName));
 }

 return STATUS_CODE_SUCCESS;
}


StatusCode ArborParentAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
	m_objectCreationAlgorithmName = "";
 PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
  "ObjectFormation", m_objectCreationAlgorithmName));

 m_clusterCreationAlgorithmName = "";
 PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
  "ClusterFormation", m_clusterCreationAlgorithmName));

 // at least run one algorithm
 if(m_objectCreationAlgorithmName.empty() && m_clusterCreationAlgorithmName.empty())
 	return STATUS_CODE_INVALID_PARAMETER;

 // Input parameters: name of input calo hit list and whether it should persist as the current list after algorithm has finished
 PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
  "InputCaloHitListName", m_inputCaloHitListName));

 m_restoreOriginalCaloHitList = !m_inputCaloHitListName.empty();
 PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
  "RestoreOriginalCaloHitList", m_restoreOriginalCaloHitList));

 m_restoreOriginalObjectList = true;
 PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
   "RestoreOriginalObjectList", m_restoreOriginalObjectList));

 // Output parameters: name of output object list and whether it should subsequently be used as the current list
 m_objectListName = "";
 PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
  "ObjectListName", m_objectListName));

 // if the arbor object creation algorithm is run, we need a name under which to save the created objects
 if(m_objectListName.empty() && !m_objectCreationAlgorithmName.empty())
 	return STATUS_CODE_INVALID_PARAMETER;

 m_clusterListName = "";
 PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
   "ClusterListName", m_clusterListName));

 // if the arbor cluster creation algorithm is run, we need a name under which to save the created clusters
 if(m_clusterListName.empty() && !m_clusterCreationAlgorithmName.empty())
 	return STATUS_CODE_INVALID_PARAMETER;

 m_replaceCurrentObjectList = true;
 PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
  "ReplaceCurrentObjectList", m_replaceCurrentObjectList));

 m_replaceCurrentClusterList = true;
 PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
  "ReplaceCurrentClusterList", m_replaceCurrentClusterList));

 PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle, "associationAlgorithms",
 		m_associationAlgorithmList));

 return STATUS_CODE_SUCCESS;
}


} 
