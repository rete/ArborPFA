/*
 *
 * TrackObject.h header template automatically generated by a class generator
 * Creation date : mer. f�vr. 26 2014
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


#ifndef TRACKOBJECT_H
#define TRACKOBJECT_H

#include "arborpfa/content/ArborObjectImpl.h"

// pandora
#include "Objects/Track.h"

namespace arborpfa
{

/** 
 * @brief TrackObject class
 */ 
class TrackObject : public ArborObjectImpl 
{
 public:

 /**
  * @brief Ctor
  */
 TrackObject(pandora::Track *pTrack);

 /**
  * @brief Dtor 
  */
 virtual ~TrackObject();

 /**
  *
  */
 pandora::Track* GetTrack() const;

	/**
	 * @brief Merge the content of the object in the cluster.
	 *
	 * @param pCluster the cluster into which the content will be merge
	 */
	virtual pandora::StatusCode MergeContentInCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster);

	/**
	 *
	 */
	pandora::StatusCode CreateCluster(const pandora::Algorithm &algorithm, pandora::Cluster *&pCluster);

protected:

	pandora::Track        *m_pTrack;

};

typedef std::set<TrackObject*> TrackObjectList;


inline pandora::Track *TrackObject::GetTrack() const
{
	return m_pTrack;
}

} 

#endif  //  TRACKOBJECT_H