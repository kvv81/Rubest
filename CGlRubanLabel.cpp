#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#define _USE_MATH_DEFINES	// To inlcude M_PI_2 for MSVS
#include <cmath>
#include <cassert>

#include "CGlRubanLabel.h"

/// Ctor builds new display list for label representation
CGlRubanLabel::CGlRubanLabel(float label_size)
  : _label_size(label_size)
{
	glNewList(LABEL_DISPLAY_LIST, GL_COMPILE);


#if 1
	glBegin(GL_QUAD_STRIP);
	//glBegin(GL_LINE_STRIP);
	
	for(int arc_num=0; arc_num<5; arc_num++)
	{
		for(int arc_step=0;
			arc_step <= SECTORS_NUM_IN_CORNER; arc_step++)
		{
			float x, y;
			GetArcedQuadCoord(arc_num%4, arc_step, x, y);
			
			glVertex3f(x, y, -LABEL_DEEPNESS*label_size);
			glVertex3f(x*INNER_LABEL_SIZE, y*INNER_LABEL_SIZE,
				LABEL_HEIGHT*label_size);
			
			if(arc_num == 4)
				break;
		}
	}
	glEnd();
#endif

	glBegin(GL_POLYGON);
	for(int arc_num=0; arc_num<4; arc_num++)
	{
		for(int arc_step=0;
			arc_step <= SECTORS_NUM_IN_CORNER; arc_step++)
		{
			float x, y;
			GetArcedQuadCoord(arc_num, arc_step, x, y);
			
			glVertex3f(x*INNER_LABEL_SIZE , y*INNER_LABEL_SIZE,
				LABEL_HEIGHT*label_size);
		}
	}
	glEnd();
	
	glEndList();
}

//------------------------------------------------------------------------------------------
// Private routines

void CGlRubanLabel::GetArcCoord(int arc_num, int arc_step,
  float &x, float &y)
{
	assert(arc_num>=0 && arc_num<4);
	
	float angle = (float) ((arc_num  + ((float)arc_step / SECTORS_NUM_IN_CORNER) ) * M_PI_2);
	x = CORNER_RADIUS * sinf(angle);
	y = CORNER_RADIUS * cosf(angle);
}

void CGlRubanLabel::GetArcedQuadCoord(int arc_num, int arc_step, float &x, float &y)
{
	GetArcCoord(arc_num, arc_step, x, y);
	
	float quad = (float) ((arc_num+0.5f) * M_PI_2);
	float length = ((0.5f - CORNER_RADIUS ) * (float)M_SQRT2) * _label_size ;
	float dx = sinf(quad) * length,
		dy = cosf(quad) * length;
	
	x*=_label_size; x+=dx;
	y*=_label_size; y+=dy;
}
//------------------------------------------------------------------------------------------

/// Draw label of specified color
void CGlRubanLabel::Draw(float color_r, float color_g, float color_b)
{
	glColor3f(color_r, color_g, color_b);
	glCallList(LABEL_DISPLAY_LIST);
}
