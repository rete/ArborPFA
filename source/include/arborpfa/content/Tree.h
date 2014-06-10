  /// \file Tree.h
/*
 *
 * Tree.h header template automatically generated by a class generator
 * Creation date : jeu. mai 29 2014
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


#ifndef TREE_H
#define TREE_H

// arborpfa
#include "arborpfa/arbor/ArborTypes.h"

// pandora
#include "Pandora/StatusCodes.h"
#include "Pandora/PandoraInputTypes.h"
#include "Objects/CartesianVector.h"

namespace arbor
{

/** 
 * @brief Tree class
 */ 
class Tree 
{
 public:

 /**
  * @brief Ctor
  */
 Tree(Object *pSeedObject);//, ITreeBuilder *pTreeBuilder);

 /**
  * @brief Dtor 
  */
 virtual ~Tree();

 /**
  *
  */
 const ObjectList &GetObjectList() const;

 /**
  *
  */
 const BranchList &GetBranchList() const;

 /**
  *
  */
 Object *GetSeedObject() const;

 /**
  *
  */
 unsigned int GetNCaloHits() const;

protected:

 /**
  *
  */
// pandora::StatusCode BuildBranches(IBranchBuilder *pBranchBuilder);

 /**
  *
  */
 pandora::StatusCode BuildTree(ITreeBuilder *pTreeBuilder);

 /**
  *
  */
 pandora::StatusCode ClearBranches();

 /**
  *
  */
 pandora::StatusCode AddBranch(Branch *pBranch);

 /**
  *
  */
 pandora::StatusCode RemoveBranch(Branch *pBranch);

 Object       *m_pSeedObject;
 ObjectList    m_objectList;
 BranchList    m_branchList;

 friend class ClusterManager;
 friend class Cluster;
}; 

} 

#endif  //  TREE_H