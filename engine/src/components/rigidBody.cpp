/*
 * Copyright (C) 2015 Subvision Studio
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

/* * @file
 * @author Subvision Studio 
 * @section LICENSE
 *
 * Copyright (C) 2015 Subvision Studio
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
#include "rigidBody.h"

RigidBody::RigidBody(): m_body(nullptr), m_shape(nullptr), m_state(nullptr), m_mass(0)
{ }

RigidBody::RigidBody(btCollisionShape* shape, float mass, bool calcInertia, Vector3f inertia): m_body(nullptr),
                                                                                               m_shape (shape), m_state(nullptr),
                                                                                               m_mass (mass)
{
	if (calcInertia)
	{
		m_shape->calculateLocalInertia (m_mass, m_inertia);
	}
}
