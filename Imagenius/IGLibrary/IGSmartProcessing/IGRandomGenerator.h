#pragma once

namespace IGLibrary
{
	class IGRandomGenerator
	{
	public:
		IGRandomGenerator(int nSize);
		~IGRandomGenerator();

		double smoothNoise(double x, double y);
		double turbulence(double x, double y, int initSize);
	private:
		int m_nSize;
		double *m_pNoise;
	};
}