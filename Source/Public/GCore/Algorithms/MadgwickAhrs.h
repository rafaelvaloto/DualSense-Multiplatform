// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once

class FMadgwickAhrs
{
public:
	explicit FMadgwickAhrs(const float Beta = 0.08f);

	// gx,gy,gz in rad/s, ax,ay,az in m/s^2, dt in seconds
	void UpdateImu(float gx, float gy, float gz, float ax, float ay, float az,
	               float dt);
	void SetBeta(const float BetaValue);
	void GetQuaternion(float& Nq0, float& Nq1, float& Nq2, float& Nq3) const;
	void Reset();
	void Reset(float ax, float ay, float az);

private:
	float Beta;
	float q0, q1, q2, q3;
};
