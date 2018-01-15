#ifndef __GL_KUBRUB__
#define __GL_KUBRUB__

#include "CGlRubanLabel.h"
#include "CRubanState.h"
#include "SRubanOp.h"

#define LABEL_SIZE 1.0f

/// KubRub class responsible for drawing kub-rub by OpenGL
class CGlKubRub {
private:
	CGlRubanLabel _label;				//< Internal class to draw labels
	SRubanOp			_cur_op;
	float					_cur_completeness;

	/// Private func to draw body
	void DrawCube(float hx, float hy, float hz);
	void TransformForSide(unsigned char side);
	void ApplyOperation(void);
	bool IsUnderTransforming(unsigned char side, unsigned char cell);
public:
	CGlKubRub();
	void SetOperation(SRubanOp op)
	{
		assert(op.op < RUBAN_OPS_NUM);
		_cur_op = op;
	}
	void SetOperationCompleteness(float completeness)
	{
		assert(completeness >= 0.0f && completeness <= 1.0f);
		_cur_completeness = completeness;
	}
	void Draw(SUnpackedRubanState &ruban_state);
};

#endif //__GL_KUBRUB__
