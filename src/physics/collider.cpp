/*
 * Copyright (C) 2014 Dominik Madarasz
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "collider.h"
#include "boundingSphere.h"
#include <iostream>
#include <cstdlib>

IntersectData Collider::Intersect(const Collider& other) const
{
	if(m_type == TYPE_SPHERE && other.GetType() == TYPE_SPHERE)
	{
		BoundingSphere* self = (BoundingSphere*)this;
		return self->IntersectBoundingSphere((BoundingSphere&)other);
	}

	std::cerr << "Error: Collisions not implemented between specified "
	          << "colliders." << std::endl;
	exit(1);
	
	//Control should never reach this point
	return IntersectData(false, 0);
}

