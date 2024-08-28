/*=============================================================================
	UnMathLg.h: Large address math routines, to be used for cases where
	extra precision is needed like with map rebuilding...
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Marco
=============================================================================*/

/*-----------------------------------------------------------------------------
	Defintions.
-----------------------------------------------------------------------------*/
#pragma once

struct FPlaneLG
{
	DOUBLE X, Y, Z, W;

	FPlaneLG()
	{}
	FPlaneLG(INT)
		: X(0.0), Y(0.0), Z(0.0), W(0.0)
	{}
	FPlaneLG(DOUBLE InX, DOUBLE InY, DOUBLE InZ)
		: X(InX), Y(InY), Z(InZ), W(0.0)
	{}
	FPlaneLG(DOUBLE InX, DOUBLE InY, DOUBLE InZ, DOUBLE InW)
		: X(InX), Y(InY), Z(InZ), W(InW)
	{}
	FPlaneLG(const FVector& V)
		: X(V.X), Y(V.Y), Z(V.Z), W(0.0)
	{}
	FPlaneLG(const FPlane& V)
		: X(V.X), Y(V.Y), Z(V.Z), W(V.W)
	{}
	FPlaneLG(const FVector& InNormal, DOUBLE InW)
		: X(InNormal.X), Y(InNormal.Y), Z(InNormal.Z), W(InW)
	{}
	FPlaneLG(const FVector& InNormal, const FVector& InPoint)
		: X(InNormal.X), Y(InNormal.Y), Z(InNormal.Z), W(InNormal | InPoint)
	{}

	// Binary math operators.
	inline FPlaneLG operator^(const FPlaneLG& V) const
	{
		return FPlaneLG
		(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X
		);
	}
	inline DOUBLE operator|(const FPlaneLG& V) const
	{
		return X * V.X + Y * V.Y + Z * V.Z;
	}
	friend FPlaneLG operator*(DOUBLE Scale, const FPlaneLG& V)
	{
		return FPlaneLG(V.X * Scale, V.Y * Scale, V.Z * Scale, V.W * Scale);
	}
	inline FPlaneLG operator+(const FPlaneLG& V) const
	{
		return FPlaneLG(X + V.X, Y + V.Y, Z + V.Z, W + V.W);
	}
	inline FPlaneLG operator-(const FPlaneLG& V) const
	{
		return FPlaneLG(X - V.X, Y - V.Y, Z - V.Z, W - V.W);
	}
	inline FPlaneLG operator*(DOUBLE Scale) const
	{
		return FPlaneLG(X * Scale, Y * Scale, Z * Scale, W * Scale);
	}
	FPlaneLG operator/(DOUBLE Scale) const
	{
		const DOUBLE RScale = 1.0 / Scale;
		return FPlaneLG(X * RScale, Y * RScale, Z * RScale, W * RScale);
	}
	inline FPlaneLG operator*(const FPlaneLG& V) const
	{
		return FPlaneLG(X * V.X, Y * V.Y, Z * V.Z, W * V.W);
	}

	// Unary operators.
	inline FPlaneLG operator-() const
	{
		return FPlaneLG(-X, -Y, -Z, -W);
	}

	// Assignment operators.
	inline FPlaneLG operator+=(const FPlaneLG& V)
	{
		X += V.X; Y += V.Y; Z += V.Z; W += V.W;
		return *this;
	}
	inline FPlaneLG operator-=(const FPlaneLG& V)
	{
		X -= V.X; Y -= V.Y; Z -= V.Z; W -= V.W;
		return *this;
	}
	inline FPlaneLG operator*=(DOUBLE Scale)
	{
		X *= Scale; Y *= Scale; Z *= Scale; W *= Scale;
		return *this;
	}
	FPlaneLG operator/=(DOUBLE V)
	{
		const DOUBLE RV = 1.0 / V;
		X *= RV; Y *= RV; Z *= RV; W *= RV;
		return *this;
	}
	FPlaneLG operator*=(const FPlaneLG& V)
	{
		X *= V.X; Y *= V.Y; Z *= V.Z; W *= V.W;
		return *this;
	}
	FPlaneLG operator/=(const FPlaneLG& V)
	{
		X /= V.X; Y /= V.Y; Z /= V.Z; W /= V.W;
		return *this;
	}

	inline FVector Vector() const
	{
		return FVector(X, Y, Z);
	}
	inline FPlane Plane() const
	{
		return FPlane(X, Y, Z, W);
	}

	inline DOUBLE Size3D() const
	{
		return appSqrt(X * X + Y * Y + Z * Z);
	}
	inline DOUBLE Size4D() const
	{
		return appSqrt(X * X + Y * Y + Z * Z + W * W);
	}
	inline DOUBLE Size3DSq() const
	{
		return (X * X + Y * Y + Z * Z);
	}
	inline DOUBLE Size4DSq() const
	{
		return (X * X + Y * Y + Z * Z + W * W);
	}

	inline DOUBLE Distance3D(const FPlaneLG& B) const
	{
		return appSqrt(Square(X - B.X) + Square(Y - B.Y) + Square(Z - B.Z));
	}
	inline DOUBLE Distance3DSq(const FPlaneLG& B) const
	{
		return (Square(X - B.X) + Square(Y - B.Y) + Square(Z - B.Z));
	}

	inline UBOOL IsParallelNormal(const FPlaneLG& V) const
	{
		return (Abs((X * V.X + Y * V.Y + Z * V.Z) - 1.0) < ((DOUBLE)THRESH_NORMALS_ARE_SAME));
	}
	inline UBOOL IsParallel(const FPlaneLG& V) const
	{
		return (Abs(W - V.W) < ((DOUBLE)THRESH_POINT_ON_PLANE)) && (Abs((X * V.X + Y * V.Y + Z * V.Z)-1.0) < ((DOUBLE)THRESH_NORMALS_ARE_SAME));
	}

	inline UBOOL Normalize()
	{
		DOUBLE s = Size3D();
		if (s < SMALL_NUMBER)
			return FALSE;
		s = 1.0 / s;
		X *= s;
		Y *= s;
		W *= s;
		return TRUE;
	}
	inline FPlaneLG GetNormal() const
	{
		DOUBLE s = Size3D();
		if (s < SMALL_NUMBER)
			return FPlaneLG(0);
		s = 1.0 / s;
		return FPlaneLG(X * s, Y * s, Z * s, W);
	}
};
