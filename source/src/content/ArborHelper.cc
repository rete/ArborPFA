/*
 *
 * ArborHelper.cc source template automatically generated by a class generator
 * Creation date : lun. mars 10 2014
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


#include "arborpfa/content/ArborHelper.h"

#include "Objects/Cluster.h"
#include "Objects/CartesianVector.h"

using namespace pandora;

namespace arborpfa
{

pandora::StatusCode ArborHelper::GetCentroid(const pandora::Cluster *pCluster, pandora::CartesianVector &centroid)
{

	if(NULL == pCluster)
		return STATUS_CODE_FAILURE;

	const OrderedCaloHitList &clusterCaloHitList = pCluster->GetOrderedCaloHitList();
	centroid.SetValues(0.f, 0.f, 0.f);
	unsigned int normalizationFactor = 0;

	for(OrderedCaloHitList::const_iterator iter = clusterCaloHitList.begin() , endIter = clusterCaloHitList.end() ; endIter != iter ; ++iter)
	{
		centroid += pCluster->GetCentroid(iter->first);
		normalizationFactor++;
	}

	centroid = centroid * (1.0/normalizationFactor);

	return STATUS_CODE_SUCCESS;
}

pandora::StatusCode ArborHelper::GetCentroidDifference(const pandora::Cluster *pCluster1, const pandora::Cluster *pCluster2, float &centroidDifference)
{

	CartesianVector centroid1(0.f, 0.f, 0.f);
	CartesianVector centroid2(0.f, 0.f, 0.f);

	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborHelper::GetCentroid(pCluster1, centroid1));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborHelper::GetCentroid(pCluster2, centroid2));

	centroidDifference = (centroid1 - centroid2).GetMagnitude();

	return STATUS_CODE_SUCCESS;
}

pandora::StatusCode ArborHelper::GetClosestDistanceApproach(const pandora::Cluster *pCluster1, const pandora::Cluster *pCluster2, float &closestDistance)
{
	if(NULL == pCluster1 || NULL == pCluster2)
		return STATUS_CODE_INVALID_PARAMETER;

	if(0 == pCluster1->GetNCaloHits() || 0 == pCluster2->GetNCaloHits())
		return STATUS_CODE_FAILURE;

	CaloHitList caloHitList1;
	CaloHitList caloHitList2;
	pCluster1->GetOrderedCaloHitList().GetCaloHitList(caloHitList1);
	pCluster2->GetOrderedCaloHitList().GetCaloHitList(caloHitList2);

	closestDistance = std::numeric_limits<float>::max();

	for(CaloHitList::const_iterator iter1 = caloHitList1.begin() , endIter1 = caloHitList1.end() ; endIter1 != iter1 ; ++iter1)
	{
		CaloHit *pCaloHit1 = *iter1;

		for(CaloHitList::const_iterator iter2 = caloHitList2.begin() , endIter2 = caloHitList2.end() ; endIter2 != iter2 ; ++iter2)
		{
			CaloHit *pCaloHit2 = *iter2;

			float distance = (pCaloHit1->GetPositionVector() - pCaloHit2->GetPositionVector()).GetMagnitude();

			if(closestDistance > distance)
			{
				closestDistance = distance;
			}
		}
	}

	return STATUS_CODE_SUCCESS;
}


} 
