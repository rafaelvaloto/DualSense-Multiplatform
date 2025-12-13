// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.

#include "Core/Algorithms/MadgwickAhrs.h"
#include <cmath>

FMadgwickAhrs::FMadgwickAhrs(const float Beta)
    : Beta(Beta)
    , q0(1.0f)
    , q1(0.0f)
    , q2(0.0f)
    , q3(0.0f)
{}

void FMadgwickAhrs::UpdateImu(float gx, float gy, float gz, float ax, float ay,
                              float az, float dt)
{
	float q0_ = q0, q1_ = q1, q2_ = q2, q3_ = q3;

	float norm = std::sqrt(ax * ax + ay * ay + az * az);
	if (norm == 0.0f)
	{
		return;
	}
	ax /= norm;
	ay /= norm;
	az /= norm;

	float _2q0 = 2.0f * q0_, _2q1 = 2.0f * q1_, _2q2 = 2.0f * q2_,
	      _2q3 = 2.0f * q3_;
	float _4q0 = 4.0f * q0_, _4q1 = 4.0f * q1_, _4q2 = 4.0f * q2_;
	float _8q1 = 8.0f * q1_, _8q2 = 8.0f * q2_;
	float q0q0 = q0_ * q0_, q1q1 = q1_ * q1_, q2q2 = q2_ * q2_, q3q3 = q3_ * q3_;

	float s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
	float s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1_ - _2q0 * ay - _4q1 +
	           _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
	float s2 = 4.0f * q0q0 * q2_ + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 +
	           _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
	float s3 = 4.0f * q1q1 * q3_ - _2q1 * ax + 4.0f * q2q2 * q3_ - _2q2 * ay;

	float s_norm = std::sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);
	if (s_norm == 0.0f)
	{
		return;
	}
	s0 /= s_norm;
	s1 /= s_norm;
	s2 /= s_norm;
	s3 /= s_norm;

	float qDot0 = 0.5f * (-q1_ * gx - q2_ * gy - q3_ * gz) - Beta * s0;
	float qDot1 = 0.5f * (q0_ * gx + q2_ * gz - q3_ * gy) - Beta * s1;
	float qDot2 = 0.5f * (q0_ * gy - q1_ * gz + q3_ * gx) - Beta * s2;
	float qDot3 = 0.5f * (q0_ * gz + q1_ * gy - q2_ * gx) - Beta * s3;

	q0_ += qDot0 * dt;
	q1_ += qDot1 * dt;
	q2_ += qDot2 * dt;
	q3_ += qDot3 * dt;

	float q_norm = std::sqrt(q0_ * q0_ + q1_ * q1_ + q2_ * q2_ + q3_ * q3_);
	q0 = q0_ / q_norm;
	q1 = q1_ / q_norm;
	q2 = q2_ / q_norm;
	q3 = q3_ / q_norm;
}

void FMadgwickAhrs::GetQuaternion(float& Nq0, float& Nq1, float& Nq2,
                                  float& Nq3) const
{
	Nq0 = this->q0;
	Nq1 = this->q1;
	Nq2 = this->q2;
	Nq3 = this->q3;
}

void FMadgwickAhrs::Reset()
{
	q0 = 1.0f;
	q1 = 0.0f;
	q2 = 0.0f;
	q3 = 0.0f;
}

void FMadgwickAhrs::Reset(float ax, float ay, float az)
{
	float norm = std::sqrt(ax * ax + ay * ay + az * az);

	if (norm < 1e-8f)
	{
		Reset();
		return;
	}

	ax /= norm;
	ay /= norm;
	az /= norm;

	if (az > 0.999f)
	{
		Reset();
		return;
	}

	if (az < -0.999f)
	{
		this->q0 = 0.0f;
		this->q1 = 1.0f; // X
		this->q2 = 0.0f;
		this->q3 = 0.0f;
		return;
	}

	float cross_x = -ay;
	float cross_y = ax;
	float cross_z = 0.0f;

	float dot = az;
	float s = std::sqrt((1.0f + dot) * 2.0f);
	float invs = 1.0f / s;

	this->q0 = 0.5f * s;       // W
	this->q1 = cross_x * invs; // X
	this->q2 = cross_y * invs; // Y
	this->q3 = cross_z * invs; // Z

	float qnorm = std::sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	float invQNorm = 1.0f / qnorm;

	this->q0 *= invQNorm;
	this->q1 *= invQNorm;
	this->q2 *= invQNorm;
	this->q3 *= invQNorm;
}

void FMadgwickAhrs::SetBeta(const float BetaValue)
{
	Beta = BetaValue;
}
