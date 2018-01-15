#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "CGlKubRub.h"

typedef struct {
	GLfloat r, g, b;
} RGBf;

CGlKubRub::CGlKubRub()
	: _label(LABEL_SIZE), _cur_completeness(0.0f), _cur_op(DEFAULT_RUBAN_OP)
{
}

void CGlKubRub::DrawCube(float hx, float hy, float hz)
{
	// We are using indices array to draw cube
	GLfloat vert[] = {
		-hx, -hy, -hz,
		hx,  -hy, -hz,
		hx,  hy,  -hz,
		-hx, hy,  -hz,
		-hx, -hy, hz,
		hx,  -hy, hz,
		hx,  hy,  hz,
		-hx, hy,  hz
	};
	GLbyte ind[] = {
		0,3,2,1,
		2,3,7,6,
		0,4,7,3,
		1,2,6,5,
		4,5,6,7,
		0,1,5,4
	};
	glVertexPointer(3, GL_FLOAT, 0, vert);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, ind);
}

/// Apply specific glTranslatef for specified side
void CGlKubRub::TransformForSide(unsigned char side)
{
	assert(side < 6);
	switch(side)
	{
		case SIDE_F:
			glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
			glTranslatef(0.0f, 0.0f, 1.5f);
		break;
		case SIDE_R:
			glTranslatef(1.5f, 0.0f, 0.0f);
			glRotatef(90, 0.0f, 1.0f, 0.0f);
			glRotatef(-90, 0.0f, 0.0f, 1.0f);
		break;
		case SIDE_U:
			glTranslatef(0.0f, 1.5f, 0.0f);
			glRotatef(-90, 1.0f, 0.0f, 0.0f);
			glRotatef(-90, 0.0f, 0.0f, 1.0f);
		break;
		case SIDE_B:
			glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
			glTranslatef(0.0f, 0.0f, -1.5f);
			glRotatef(180, 0.0f, 1.0f, 0.0f);
		break; 
		case SIDE_L:
			glTranslatef(-1.5f, 0.0f, 0.0f);
			glRotatef(-90, 0.0f, 1.0f, 0.0f);
			glRotatef(-90, 0.0f, 0.0f, 1.0f);
		break;
		case SIDE_D:
			glTranslatef(0.0f, -1.5f, 0.0f);
			glRotatef(90, 1.0f, 0.0f, 0.0f);
			glRotatef(-90, 0.0f, 0.0f, 1.0f);
		break;
	}
}


bool CGlKubRub::IsUnderTransforming(unsigned char side, unsigned char cell)
{
	/// Special values to mark unacceptable and not affected cases
	static const signed char X = -1, N = -2;

	/// Legend: X = unaccessible, N = not affected at all.
	/// 1st index = what side is under transform, 2nd = what side is cell belong to
	/// Ordinary values = list of affected cells for that side.
	static const signed char affected_cells[6][6][3] = {
		/*SIDE_F*/ {{X, X, X}, {0, 3, 5}, {5, 6, 7}, {N, N, N}, {2, 4, 7}, {0, 1, 2}},
		/*SIDE_R*/ {{2, 4, 7}, {X, X, X}, {2, 4, 7}, {0, 3, 5}, {N, N, N}, {2, 4, 7}},
		/*SIDE_U*/ {{0, 1, 2}, {0, 1, 2}, {X, X, X}, {0, 1, 2}, {0, 1, 2}, {N, N, N}},
		/*SIDE_B*/ {{N, N, N}, {2, 4, 7}, {0, 1, 2}, {X, X, X}, {0, 3, 5}, {5, 6, 7}},
		/*SIDE_L*/ {{0, 3, 5}, {N, N, N}, {0, 3, 5}, {2, 4, 7}, {X, X, X}, {0, 3, 5}},
		/*SIDE_D*/ {{5, 6, 7}, {5, 6, 7}, {N, N, N}, {5, 6, 7}, {5, 6, 7}, {X, X, X}}
	};
	unsigned char tr_side = _cur_op.op >> 1;
	assert(tr_side < 6);

	if(tr_side == side)
		return true;

	const signed char *cells = affected_cells[tr_side][side];
	assert(cells[0] != X);

	if(cells[0] == N) return false;

	if(cells[0] == cell || cells[1] == cell || cells[2] == cell)
		return true;

	return false;
}

void CGlKubRub::ApplyOperation(void)
{
	float angle_dir = ((_cur_op%2==0) ? -1.0f : 1.0f) * ((_cur_op > OP_U_) ? -1.0f : 1.0f);
	switch(_cur_op)
	{
		case OP_F:
		case OP_F_:
		case OP_B:
		case OP_B_:
			glRotatef(_cur_completeness * 90 * angle_dir,
				0.0f, 0.0f, 1.0f);
		break;
		case OP_R:
		case OP_R_:
		case OP_L:
		case OP_L_:
			glRotatef(_cur_completeness * 90 * angle_dir,
				1.0f, 0.0f, 0.0f);
		break;
		case OP_U:
		case OP_U_:
		case OP_D:
		case OP_D_:
			glRotatef(_cur_completeness * 90 * angle_dir,
				0.0f, 1.0f, 0.0f);
		break;
	}
}

/// Draw kub-rub of specified state. Current operaton and
/// operation completeness is also used during rendering.
void CGlKubRub::Draw(SUnpackedRubanState &ruban_state)
{
	// F.R.U.B.L.D order
	const RGBf label_colors[6]={
		{0.3f, 0.0f, 0.0f}, // bordo
		{0.1f, 0.4f, 0.1f}, // green
		{0.7f, 0.7f, 0.0f}, // yellow
		{0.7f, 0.4f, 0.0f}, // orange
		{0.7f, 0.7f, 0.7f}, // white
		{0.1f, 0.1f, 0.4f}  // blue
	};

	for(int side=0; side<6; side++)
	{
		for(int x=0;x<3;x++)
		{
			for(int y=0;y<3;y++)
			{
				unsigned char cell = x*3 + y;
				if(cell > 3) cell--;

				// Cell in center is fixed, it is absent into the data...
				bool is_fixed_cell = ((x==1) && (y==1));

				if(is_fixed_cell)	cell = 0xff;
        			glPushMatrix();

				if(IsUnderTransforming(side, cell))
					ApplyOperation();
        			TransformForSide(side);

        			glTranslatef(x-1.0f, y-1.0f, 0.0f);

				unsigned char *ruban_side = &(ruban_state.state.block[side*8]);
				const RGBf &clr = label_colors[ is_fixed_cell ? side : ruban_side[cell] ];

				//if(is_fixed_cell)
				_label.Draw(clr.r, clr.g, clr.b);
        			glPopMatrix();
			}
		}
	}

	//glColor3f(0.2f, 0.2f, 0.2f);
	//DrawCube(1.5f, 1.5f, 1.5f);
}
