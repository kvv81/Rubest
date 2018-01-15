#ifndef __RUBAN_STATE__
#define __RUBAN_STATE__

#include <cstring>
#include "SRubanOp.h"

// F.R.U.B.L.D order
#define RED			0
#define GREEN		1
#define YELLOW	2
#define ORANGE	3
#define WHITE		4
#define BLUE		5

struct SUnpackedRubanState;	// Forward declaration

/// Packed ruban state
struct SPackedRubanState {
	unsigned char state[16];	///< packed state data (enough for 6^48 states)

/*
	SPackedRubanState(SPackedRubanState &b)
	{
		memspy(state, b.state, sizeof(state);
	}
*/

	/// unpacker
	void GetUnpackedRubanState(SUnpackedRubanState &dst);

	/// bitwise compare func (used in tree walking)
	int Compare(const SPackedRubanState &b) const	// compare operator to navigate in tree
	{
		return memcmp(state, b.state, sizeof(state));
	}
};

/// Define operator for output state to stream (debug purposes)
std::ostream& operator<<(std::ostream& stream, const SPackedRubanState &state);
extern SPackedRubanState inital_packed_ruban_state;


/// Unpacked ruban state data
// F.R.U.B.L.D order
struct SUnpackedRubanSides {
	unsigned char Fasade[8], Right[8], Up[8], Back[8], Left[8], Down[8];
};


/// Unpacked ruban state
struct SUnpackedRubanState {

	/// Union for convenient binary serialization
	union URubanState
	{
		SUnpackedRubanSides sides;
		unsigned char block[48];	// as binary data
	} state;

/*
	operator==(const SUnpackedRubanState &b) const	// compare operator to navigate in tree
	{
		return memcpm(state, b.state, sizeof(state));
	}
*/
	SUnpackedRubanState();

	/// Generalized func to process operation
	void ExecuteOp(const SRubanOp op);

	/// Funcs to process state linked to facets
	void F(bool isCW);
	void R(bool isCW);
	void U(bool isCW);
	void B(bool isCW);
	void L(bool isCW);
	void D(bool isCW);

	/// Packing routine
	void GetPackedRubanState(SPackedRubanState &dst);

	/// internal func for ruban operation processing
	void RotateSide(bool isCW, unsigned char *side);

	/// internal func - cyclic swap
	void Rot4(bool isCW, unsigned char &a, unsigned char &b, unsigned char &c, unsigned char &d)
	{
		if(isCW)
		{
			unsigned char tmp=d;
			d=c; c=b; b=a; a=tmp;
		}
		else
		{
			unsigned char tmp=a;
			a=b; b=c; c=d; d=tmp;
		}
	}
};

extern SUnpackedRubanState inital_unpacked_ruban_state;

#endif	// __RUBAN_STATE__
