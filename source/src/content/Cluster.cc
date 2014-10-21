  /// \file Cluster.cc
/*
 *
 * Cluster.cc source template automatically generated by a class generator
 * Creation date : mar. juin 3 2014
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


#include "arborpfa/content/Cluster.h"

// arbor
#include "arborpfa/content/Object.h"
#include "arborpfa/content/Branch.h"
#include "arborpfa/content/Tree.h"

// std
#include <algorithm>

namespace arbor
{

Cluster::Cluster() :
		m_seedPosition(0.f, 0.f, 0.f),
		m_seedPseudoLayer(std::numeric_limits<pandora::PseudoLayer>::max()),
		m_pAssociatedTrack(NULL)
{

}

//------------------------------------------------------------------------------------------------------

Cluster::~Cluster() 
{
	for(TreeList::iterator iter = m_treeList.begin() , endIter = m_treeList.end() ; endIter != iter ; ++iter)
		delete *iter;

	m_treeList.clear();
	m_pAssociatedTrack = NULL;
}

//------------------------------------------------------------------------------------------------------

const TreeList &Cluster::GetTreeList() const
{
	return m_treeList;
}

//------------------------------------------------------------------------------------------------------

const pandora::CartesianVector &Cluster::GetSeedPosition() const
{
	return m_seedPosition;
}

//------------------------------------------------------------------------------------------------------

const pandora::PseudoLayer &Cluster::GetSeedPseudoLayer() const
{
	return m_seedPseudoLayer;
}

//------------------------------------------------------------------------------------------------------

pandora::StatusCode Cluster::AddTree(Tree *pTree)
{
	if(NULL == pTree)
		return pandora::STATUS_CODE_INVALID_PARAMETER;

	if(std::find(m_treeList.begin(), m_treeList.end(), pTree) != m_treeList.end())
		return pandora::STATUS_CODE_ALREADY_PRESENT;

	if(!m_treeList.insert(pTree).second)
		return pandora::STATUS_CODE_FAILURE;

	if(pTree->GetSeedObject()->GetPseudoLayer() < m_seedPseudoLayer)
		m_seedPseudoLayer = pTree->GetSeedObject()->GetPseudoLayer();

	if(pTree->GetSeedObject()->GetPseudoLayer() == m_seedPseudoLayer)
	{
		pandora::CartesianVector newSeedPosition(0.f, 0.f, 0.f);
		for(TreeList::iterator iter = m_treeList.begin() , endIter = m_treeList.end() ; endIter != iter ; ++iter)
		{
			Tree *pTree = *iter;
			if((*iter)->GetSeedObject()->GetPseudoLayer() == m_seedPseudoLayer)
				newSeedPosition += (*iter)->GetSeedObject()->GetPosition();
		}

		m_seedPosition = newSeedPosition;
	}

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------

pandora::StatusCode Cluster::RemoveTree(Tree *pTree)
{
	if(NULL == pTree)
		return pandora::STATUS_CODE_INVALID_PARAMETER;

	if(std::find(m_treeList.begin(), m_treeList.end(), pTree) == m_treeList.end())
		return pandora::STATUS_CODE_NOT_FOUND;

	if(1 != m_treeList.erase(pTree))
		return pandora::STATUS_CODE_FAILURE;

	if(m_treeList.empty())
	{
		m_seedPosition = pandora::CartesianVector(0.f, 0.f, 0.f);
		m_seedPseudoLayer = std::numeric_limits<pandora::PseudoLayer>::max();
		return pandora::STATUS_CODE_SUCCESS;
	}

	if(m_seedPseudoLayer < pTree->GetSeedObject()->GetPseudoLayer())
		return pandora::STATUS_CODE_SUCCESS;

	// Then upate the cluster seed position and pseudolayer
	pandora::CartesianVector newSeedPosition(0.f, 0.f, 0.f);
	pandora::PseudoLayer innermostPseudoLayer(std::numeric_limits<pandora::PseudoLayer>::max());

	for(TreeList::iterator iter = m_treeList.begin() , endIter = m_treeList.end() ; endIter != iter ; ++iter)
	{
		Tree *pTree = *iter;

		if((*iter)->GetSeedObject()->GetPseudoLayer() < innermostPseudoLayer)
		{
			innermostPseudoLayer = (*iter)->GetSeedObject()->GetPseudoLayer();
			newSeedPosition = (*iter)->GetSeedObject()->GetPosition();
		}

		if((*iter)->GetSeedObject()->GetPseudoLayer() == innermostPseudoLayer)
		{
			newSeedPosition += (*iter)->GetSeedObject()->GetPosition();
		}
	}

	m_seedPseudoLayer = innermostPseudoLayer;
	m_seedPosition = newSeedPosition;

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------

ObjectList Cluster::GetObjectList() const
{
	ObjectList returnList;

	for(TreeList::const_iterator iter = m_treeList.begin() , endIter = m_treeList.end() ; endIter != iter ; ++iter)
	{
		Tree *pTree = *iter;
		returnList.insert(pTree->GetObjectList().begin(), pTree->GetObjectList().end());
	}

	return returnList;
}

//------------------------------------------------------------------------------------------------------

pandora::CaloHitList Cluster::GetCaloHitList() const
{
	pandora::CaloHitList returnList;

	for(TreeList::const_iterator iter = m_treeList.begin() , endIter = m_treeList.end() ; endIter != iter ; ++iter)
	{
		Tree *pTree = *iter;
		pandora::CaloHitList caloHitList(pTree->GetCaloHitList());
		returnList.insert(caloHitList.begin(), caloHitList.end());
	}

	return returnList;
}

//------------------------------------------------------------------------------------------------------

pandora::StatusCode Cluster::GetAssociatedTrack(const pandora::Track *&pTrack) const
{
	pTrack = m_pAssociatedTrack;
	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------

pandora::StatusCode Cluster::SetAssociatedTrack(pandora::Track *pTrack)
{
	m_pAssociatedTrack = pTrack;
	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------

unsigned int Cluster::GetNObjects() const
{
	unsigned int nObjects = 0;

	for(TreeList::const_iterator iter = m_treeList.begin() , endIter = m_treeList.end() ; endIter != iter ; ++iter)
	{
		nObjects += (*iter)->GetObjectList().size();
	}

	return nObjects;
}

//------------------------------------------------------------------------------------------------------

unsigned int Cluster::GetNCaloHits() const
{
	unsigned int nCaloHits = 0;

	for(TreeList::const_iterator iter = m_treeList.begin() , endIter = m_treeList.end() ; endIter != iter ; ++iter)
	{
		nCaloHits += (*iter)->GetNCaloHits();
	}

	return nCaloHits;
}

} 

