  /// \file ArborPluginManager.cc
/*
 *
 * ArborPluginManager.cc source template automatically generated by a class generator
 * Creation date : ven. mai 30 2014
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


#include "arborpfa/content/ArborPluginManager.h"

#include "arborpfa/content/SimpleTreeBuilder.h"
#include "arborpfa/content/SimpleBranchBuilder.h"

using namespace pandora;

namespace arbor
{

ArborPluginManager::ArborPluginManager()
{
	// Register basic branch and tree builders ...
	PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterTreeBuilder("SimpleTreeBuilder", new SimpleTreeBuilder()));
	PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterBranchBuilder("SimpleBranchBuilder", new SimpleBranchBuilder()));

	// ... and set them as the current ones
	PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SetCurrentTreeBuilder("SimpleTreeBuilder"));
	PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SetCurrentBranchBuilder("SimpleBranchBuilder"));
}

//-------------------------------------------------------------------------------------------------------------

ArborPluginManager::~ArborPluginManager()
{
	for(TreeBuilderMap::iterator iter = m_treeBuilderMap.begin() , endIter = m_treeBuilderMap.end() ; endIter != iter ; ++iter)
	{
		delete iter->second;
	}

	for(BranchBuilderMap::iterator iter = m_branchBuilderMap.begin() , endIter = m_branchBuilderMap.end() ; endIter != iter ; ++iter)
	{
		delete iter->second;
	}

	m_treeBuilderMap.clear();
	m_branchBuilderMap.clear();
}

//-------------------------------------------------------------------------------------------------------------

/*
 * TreeBuilder related methods
 */

pandora::StatusCode ArborPluginManager::RegisterTreeBuilder(const std::string &treeBuilderName, ITreeBuilder *pTreeBuilder)
{
	if(NULL == pTreeBuilder)
		return STATUS_CODE_INVALID_PARAMETER;

	TreeBuilderMap::iterator findIter = m_treeBuilderMap.find(treeBuilderName);

	if(m_treeBuilderMap.end() != findIter)
		return STATUS_CODE_ALREADY_PRESENT;

	m_treeBuilderMap[treeBuilderName] = pTreeBuilder;

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborPluginManager::SetCurrentTreeBuilder(const std::string &treeBuilderName)
{
	TreeBuilderMap::iterator findIter = m_treeBuilderMap.find(treeBuilderName);

	if(m_treeBuilderMap.end() == findIter)
		return STATUS_CODE_ALREADY_PRESENT;

	m_currentTreeBuilderName = findIter->first;
	m_pCurrentTreeBuilder = findIter->second;

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborPluginManager::GetCurrentTreeBuilder(ITreeBuilder *&pTreeBuilder)
{
	if(NULL == m_pCurrentTreeBuilder)
		return STATUS_CODE_NOT_INITIALIZED;

	pTreeBuilder = m_pCurrentTreeBuilder;

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborPluginManager::GetCurrentTreeBuilderName(std::string &treeBuilderName)
{
	if(NULL == m_pCurrentTreeBuilder)
		return STATUS_CODE_NOT_INITIALIZED;

	treeBuilderName = m_currentTreeBuilderName;

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------------------

/*
 * BranchBuilder related methods
 */

pandora::StatusCode ArborPluginManager::RegisterBranchBuilder(const std::string &branchBuilderName, IBranchBuilder *pBranchBuilder)
{
	if(NULL == pBranchBuilder)
		return STATUS_CODE_INVALID_PARAMETER;

	BranchBuilderMap::iterator findIter = m_branchBuilderMap.find(branchBuilderName);

	if(m_branchBuilderMap.end() != findIter)
		return STATUS_CODE_ALREADY_PRESENT;

	m_branchBuilderMap[branchBuilderName] = pBranchBuilder;

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborPluginManager::SetCurrentBranchBuilder(const std::string &branchBuilderName)
{
	BranchBuilderMap::iterator findIter = m_branchBuilderMap.find(branchBuilderName);

	if(m_branchBuilderMap.end() == findIter)
		return STATUS_CODE_ALREADY_PRESENT;

	m_currentBranchBuilderName = findIter->first;
	m_pCurrentBranchBuilder = findIter->second;

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborPluginManager::GetCurrentBranchBuilder(IBranchBuilder *&pBranchBuilder)
{
	if(NULL == m_pCurrentBranchBuilder)
		return STATUS_CODE_NOT_INITIALIZED;

	pBranchBuilder = m_pCurrentBranchBuilder;

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborPluginManager::GetCurrentBranchBuilderName(std::string &branchBuilderName)
{
	if(NULL == m_pCurrentBranchBuilder)
		return STATUS_CODE_NOT_INITIALIZED;

	branchBuilderName = m_currentBranchBuilderName;

	return STATUS_CODE_SUCCESS;
}



} 

