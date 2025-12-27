#pragma once

namespace RayTracer
{
	class Interval
	{
	public:
		Interval() = default;
		Interval(double minimum, double maximum);

		bool SetMinimum(double minimum);
		bool SetMaximum(double maximum);

		double GetSize() const;
		bool DoesContain(double x) const;
		bool DoesSurround(double x) const;

	public:
		static const Interval s_Empty;
		static const Interval s_RealNumbers;
		static const Interval s_PositiveRealNumbers;

	private:
		double m_Minimum = 0.0;
		double m_Maximum = 0.0;
	};
}