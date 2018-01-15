#ifndef __RUBAN_OP__
#define __RUBAN_OP__

#include <iostream>
#include <cassert>
#include "CRubanAssertMessages.h"

#define MIN_RUBAN_OP				0
#define MAX_RUBAN_OP				11
#define RUBAN_OPS_NUM				12
#define UNDEFINED_RUBAN_OP	0xFF

#define INVARIANTS_NUM  24

#define OP_F		0
#define OP_F_		1
#define OP_R		2
#define OP_R_		3
#define OP_U		4
#define OP_U_		5
#define OP_B		6
#define OP_B_		7
#define OP_L		8
#define OP_L_		9
#define OP_D		10
#define OP_D_		11

#define DEFAULT_RUBAN_OP OP_U

// OPERATION >> 1 = SIDE
#define SIDE_F 0
#define SIDE_R 1
#define SIDE_U 2
#define SIDE_B 3
#define SIDE_L 4
#define SIDE_D 5

/// \brief Class of 3D-vector (used for orts - vectors with length of 1)
/// For for generating combinations
struct SOrt3D
{
	int x, y, z;

	SOrt3D() : x(0L), y(0L), z(0L) {}
	SOrt3D(const SOrt3D &b) : x(b.x), y(b.y), z(b.z) {}

	void Set(int _x, int _y, int _z)
	{
		x=_x; y=_y; z=_z;
	}

	bool operator==(const SOrt3D &b) const
	{
		return ((x==b.x) &&  (y==b.y) && (z==b.z));
	}

	SOrt3D operator*(const SOrt3D &b) const
	{
		SOrt3D res;
		res.x = y*b.z - z*b.y;
		res.y = z*b.x - x*b.z;
		res.z = x*b.y - y*b.x;
		return res;
	}

	/// Link-function for mapping ort->operation
	struct SRubanOp GetOp();
	//----------------------------

};

/// Operator for output ort to stream (debug purposes)
std::ostream& operator<<(std::ostream &stream, const SOrt3D ort);

/// Ruban operation (one of 12 - 6 facets*2 direction(CW+CCW))
struct SRubanOp
{
	/// Global array for fast mapping operation->invarinat (for each invariant num)
	static SRubanOp invariants[INVARIANTS_NUM][RUBAN_OPS_NUM];
	static void InitInvariantsTable();	///< Initial routine

	/// internal-usage func
	static void FillInvariantAxis(int inv_index, unsigned char op_key, unsigned char op_value);

	unsigned char op;

	SRubanOp() : op(UNDEFINED_RUBAN_OP) {}

	SRubanOp(const SRubanOp &b) : op(b.op) {}

	SRubanOp(unsigned char _op) : op(_op)
	{
		assert(IsValid());
	}

	operator unsigned char()
	{
		return op;
	}

	bool IsValid() const
	{
		return (op<=MAX_RUBAN_OP);
	}

	/// Get reverse operation (OP_F <-> OP_F_, OP_R <-> OP_R_, etc).
	SRubanOp GetReverseOp()
	{
		if(!IsValid())	// if op undefined, reverse is undefined too!
		{
//			assert(0);
			return *this;
		}

		SRubanOp res(op^1);	// (op XOR 1) = revert LSB
		return res;
	}

	/// Get complement operation (Left<->Right, Up<->Down, Facade<->Back), direction will be keeped(CW or CCW)
	SRubanOp GetAxisComplementOp()
	{
		if(!IsValid())  // if op undefined, reverse is undefined too!
		{
			assert(AM_INVALID_OP_FOR_COMPLEMENT_OP_CALCULATION);
			return *this;
		}

		SRubanOp res((op+(RUBAN_OPS_NUM>>1)) % RUBAN_OPS_NUM);
		return res;
	}

	SRubanOp& operator++()
	{
		op++;
		return *this;
	}

	SRubanOp& operator=(const SRubanOp &b)
	{
		//assert(b.IsValid());
		op = b.op;
		return *this;
	}

	bool operator==(const SRubanOp &b) const
	{
		return (op == b.op);
	}

	bool operator!=(const SRubanOp &b) const
	{
		return (op != b.op);
	}

/*
	bool operator<=(const SRubanOp &b) const
	{
		return (op <= b.op);
	}
*/

	/// Get corresponding ort
	void GetOrt3D(SOrt3D &dst) const;

	bool operator==(const SOrt3D &b) const
	{
		SOrt3D this_ort;
		GetOrt3D(this_ort);
		return (this_ort == b);
	}

	/// return Nth invariant (using precalculated map)
	SRubanOp GetInvariant(int invariant_num) const
	{
		assert(invariant_num < INVARIANTS_NUM);
		return invariants[invariant_num][op];
	}

};

/// Operator for output operation to stream (debug purposes)
std::ostream& operator<<(std::ostream &stream, const SRubanOp op);

extern SRubanOp min_ruban_op, max_ruban_op;

#endif // __RUBAN_OP__
