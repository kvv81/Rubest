#include <cstring>
#include <iomanip>

#include "CRubanState.h"
#include "RubestCommon.h"

// global consts
SPackedRubanState		inital_packed_ruban_state;
SUnpackedRubanState	inital_unpacked_ruban_state;

/// Default ctor inits state by complete one
SUnpackedRubanState::SUnpackedRubanState()
{
	// F.R.U.B.L.D order
	memset(state.sides.Fasade,	RED,		sizeof(state.sides.Fasade));
	memset(state.sides.Right,		GREEN,	sizeof(state.sides.Right));
	memset(state.sides.Up,			YELLOW,	sizeof(state.sides.Up));

	memset(state.sides.Back,		ORANGE,	sizeof(state.sides.Back));
	memset(state.sides.Left,		WHITE,	sizeof(state.sides.Left));
	memset(state.sides.Down,		BLUE,		sizeof(state.sides.Down));
}

void SUnpackedRubanState::ExecuteOp(SRubanOp op)
{
	switch(op.op)
	{
		case OP_F:	F(true );	break;
		case OP_F_:	F(false);	break;
		case OP_R:	R(true );	break;
		case OP_R_:	R(false);	break;
		case OP_U:	U(true );	break;
		case OP_U_:	U(false);	break;
		case OP_B:	B(true );	break;
		case OP_B_:	B(false);	break;
		case OP_L:	L(true );	break;
		case OP_L_:	L(false);	break;
		case OP_D:	D(true );	break;
		case OP_D_:	D(false);	break;
		default:
			assert(AM_UNDEFINED_OP_VALUE);
	}
}

/// Common part of all rotation funcs
void SUnpackedRubanState::RotateSide(bool isCW, unsigned char *side)
{
	Rot4(isCW, side[0], side[2], side[7], side[5]);
	Rot4(isCW, side[1], side[4], side[6], side[3]);
}

void SUnpackedRubanState::F(bool isCW)
{
	RotateSide(isCW, state.sides.Fasade);

	Rot4(isCW, state.sides.Up[5], state.sides.Right[0], state.sides.Down[2], state.sides.Left[7]);
	Rot4(isCW, state.sides.Up[6], state.sides.Right[3], state.sides.Down[1], state.sides.Left[4]);
	Rot4(isCW, state.sides.Up[7], state.sides.Right[5], state.sides.Down[0], state.sides.Left[2]);
}

void SUnpackedRubanState::R(bool isCW)
{
	RotateSide(isCW, state.sides.Right);

	Rot4(isCW, state.sides.Fasade[2], state.sides.Up[2], state.sides.Back[5], state.sides.Down[2]);
	Rot4(isCW, state.sides.Fasade[4], state.sides.Up[4], state.sides.Back[3], state.sides.Down[4]);
	Rot4(isCW, state.sides.Fasade[7], state.sides.Up[7], state.sides.Back[0], state.sides.Down[7]);
}

void SUnpackedRubanState::U(bool isCW)
{
	RotateSide(isCW, state.sides.Up);

	Rot4(isCW, state.sides.Fasade[0], state.sides.Left[0], state.sides.Back[0], state.sides.Right[0]);
	Rot4(isCW, state.sides.Fasade[1], state.sides.Left[1], state.sides.Back[1], state.sides.Right[1]);
	Rot4(isCW, state.sides.Fasade[2], state.sides.Left[2], state.sides.Back[2], state.sides.Right[2]);
}

void SUnpackedRubanState::B(bool isCW)
{
	RotateSide(isCW, state.sides.Back);

	Rot4(isCW, state.sides.Up[0], state.sides.Left[5], state.sides.Down[7], state.sides.Right[2]);
	Rot4(isCW, state.sides.Up[1], state.sides.Left[3], state.sides.Down[6], state.sides.Right[4]);
	Rot4(isCW, state.sides.Up[2], state.sides.Left[0], state.sides.Down[5], state.sides.Right[7]);
}

void SUnpackedRubanState::L(bool isCW)
{
	RotateSide(isCW, state.sides.Left);

	Rot4(isCW, state.sides.Fasade[0], state.sides.Down[0], state.sides.Back[7], state.sides.Up[0]);
	Rot4(isCW, state.sides.Fasade[3], state.sides.Down[3], state.sides.Back[4], state.sides.Up[3]);
	Rot4(isCW, state.sides.Fasade[5], state.sides.Down[5], state.sides.Back[2], state.sides.Up[5]);
}

void SUnpackedRubanState::D(bool isCW)
{
	RotateSide(isCW, state.sides.Down);

	Rot4(isCW, state.sides.Fasade[5], state.sides.Right[5], state.sides.Back[5], state.sides.Left[5]);
	Rot4(isCW, state.sides.Fasade[6], state.sides.Right[6], state.sides.Back[6], state.sides.Left[6]);
	Rot4(isCW, state.sides.Fasade[7], state.sides.Right[7], state.sides.Back[7], state.sides.Left[7]);
}


//-----------------------------------------------------------------------------------------------

void SUnpackedRubanState::GetPackedRubanState(SPackedRubanState &dst)
{
	for(int i=16,j=47; i--; )
	{
		// boost multiply - do that by shifts!
		dst.state[i] = state.block[j] + fast_mul6(state.block[j-1]) + fast_mul36(state.block[j-2]);
		j-=3;
	}
}

void SPackedRubanState::GetUnpackedRubanState(SUnpackedRubanState &dst)
{
	for(int i=16,j=47; i--; )
	{
		// !! NOTE - unpacking is a SLOW operation!
		dst.state.block[j]		= state[i] % 6;
		dst.state.block[j-1]	= (state[i]/6) % 6;
		dst.state.block[j-2]	= state[i] / 36;

		j-=3;
	}
}

//------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const SPackedRubanState &state)
{
	stream << std::hex;
	for(int i=0;i<16;i++)
		stream << std::setw(2) << std::setfill('0') << (int)state.state[i];

	stream << std::dec;

	return stream;
}
