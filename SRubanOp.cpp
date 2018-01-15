#include "SRubanOp.h"

SRubanOp min_ruban_op(MIN_RUBAN_OP), max_ruban_op(MAX_RUBAN_OP);
SRubanOp SRubanOp::invariants[INVARIANTS_NUM][RUBAN_OPS_NUM];

void SRubanOp::GetOrt3D(SOrt3D &dst) const
{
	switch(op)
	{
		case OP_F:
			dst.Set(1,0,0);
		break;
		case OP_R:
			dst.Set(0,1,0);
		break;
		case OP_U:
			dst.Set(0,0,1);
		break;
		case OP_B:
			dst.Set(-1,0,0);
		break;
		case OP_L:
			dst.Set(0,-1,0);
		break;
		case OP_D:
			dst.Set(0,0,-1);
		break;
		default:
		{
			std::cerr << "invalid op = " << (int)op << std::endl; 
			assert(AM_WRONG_OP_FOR_ORT_EXTRACTING);
		}
	}
}

SRubanOp SOrt3D::GetOp()
{
	if(x==1		&& y==0		&& z==0)	return SRubanOp(OP_F);
	if(x==-1	&& y==0		&& z==0)	return SRubanOp(OP_B);
 	if(x==0		&& y==1		&& z==0)	return SRubanOp(OP_R);
 	if(x==0		&& y==-1	&& z==0)	return SRubanOp(OP_L);
 	if(x==0		&& y==0		&& z==1)	return SRubanOp(OP_U);
 	if(x==0		&& y==0		&& z==-1)	return SRubanOp(OP_D);

	std::cerr << *this; 
	assert(AM_WRONG_ORT_FOR_OP_EXTRACTING);
	return SRubanOp(UNDEFINED_RUBAN_OP);
}

std::ostream& operator<<(std::ostream &stream, const SOrt3D ort)
{
	  return stream << "(" << ort.x << "," << ort.y << "," << ort.z << ")";
}

std::ostream& operator<<(std::ostream &stream, const SRubanOp op)
{
	const char op_letter[] = {"FRUBLD"};
	std::cout << op_letter[op.op>>1];
	if(op.op&1) std::cout << "'";

	return stream;
}


void SRubanOp::FillInvariantAxis(int inv_index, unsigned char op_key, unsigned char op_value)
{
//	std::cout << "!";
	SRubanOp cur_op(op_value);
	invariants[inv_index][op_key]		= cur_op;
	invariants[inv_index][op_key+1]	= cur_op.GetReverseOp();

	SRubanOp comp_op = cur_op.GetAxisComplementOp();
	SRubanOp comp_key = SRubanOp(op_key).GetAxisComplementOp();
	unsigned char op_key_comp = comp_key.op;
//	std::cout << "complement = " << (int)op_key_comp << std::endl;
	invariants[inv_index][op_key_comp]		= comp_op;
	invariants[inv_index][op_key_comp+1]	= comp_op.GetReverseOp();
}

/// Build static table.
/// Note ! - the first entry (#0) if trivial transmission,
/// so we can skip it for invariants search.
void SRubanOp::InitInvariantsTable()
{
	int inv_index = 0;
	for(int i2=0; i2<RUBAN_OPS_NUM; i2+=2)
	{
		for(int j2=0; j2<RUBAN_OPS_NUM; j2+=2)
		{
			// if directions for operations are collinear
			// (on the same line, for example - F, F_, B, B_),
			// vector mul will be zero, skip that pair!!!
			if(((i2>>1)%3)==((j2>>1)%3))
				continue;

//			std::cout << "i2=" << i2 << ", j2=" << j2 << std::endl;

			FillInvariantAxis(inv_index, i2, OP_F);
			FillInvariantAxis(inv_index, j2, OP_R);

			// Orts are represented by numbers 1,2,3 in random order
			int k = 3 - ((i2>>1)%3) - ((j2>>1)%3);
			int a=-1, b=-1, c=-1;
			switch(k)
			{
				case 0:
					a=2; b=4; c=0;
				break;
				case 1:
					a=4; b=0; c=2;
				break;
				case 2:
					a=0; b=2; c=4;
				break;
			}
										
//			std::cout << a << "|" << b << "|" << c << std::endl;

			SOrt3D f, r ,u;
		 	invariants[inv_index][a].GetOrt3D(f);
//			std::cout << "f=" << f << std::endl;
			
			invariants[inv_index][b].GetOrt3D(r);
//			std::cout << "r=" << r << std::endl;
			
			u = f*r;
//			std::cout << "u=" << u << std::endl;
			FillInvariantAxis(inv_index, c, u.GetOp());

//			std::cout << "-------" << std::endl;
			inv_index++;
		}
	}
//	std::cout << "Done!" << std::endl;
/*
	for(int i=0;i<24; i++)
	{
		for(int j=0;j<12;j++)
			std::cout << invariants[i][j]; 
		std::cout << std::endl;
	}
	*/
}


