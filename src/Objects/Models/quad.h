
/*
 * Copyright:
 * Daniel D. Neilson (ddneilson@ieee.org)
 * University of Saskatchewan
 * All rights reserved
 *
 * Permission granted to use for use in assignments and
 * projects for CMPT 485 & CMPT 829 at the University
 * of Saskatchewan.
 */

/*
 * Geometry for a Quad
 *
 * The Quad is centered at (0,0,0), and is entirely
 * in the xz Quad. Coordinates of: x,z \in [-1,1]
 *
 * The front face of the Quad is in the +y-axis direction.
 * The normal to the Quad is the y-axis.
 */

#pragma once
#ifndef __INC_Quad_H_
#define __INC_Quad_H_

#include "../geometry.h"
#include "../mesh.h"

namespace Object
{
namespace Models
{

class Quad : public Geometry
{
protected:
	Mesh m_mesh;
public:
	Quad();
	~Quad();

	bool init();

	virtual void rasterize() const;
};

}
}

#endif
