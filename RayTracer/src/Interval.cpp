#include "Interval.h"

#include <limits>

namespace RayTracer
{
	Interval::Interval(double minimum, double maximum)
		: m_Minimum(minimum), m_Maximum(maximum) {}

	bool Interval::SetMinimum(double minimum)
	{
		if (minimum > m_Maximum)
			return false;

		m_Minimum = minimum;
		return true;
	}

	bool Interval::SetMaximum(double maximum)
	{
		if (maximum < m_Minimum)
			return false;

		m_Maximum = maximum;
		return true;
	}

	double Interval::GetSize() const
	{
		return m_Maximum - m_Minimum;
	}

	bool Interval::DoesContain(double x) const
	{
		return x >= m_Minimum && x <= m_Maximum;
	}
	bool Interval::DoesSurround(double x) const
	{
		return x > m_Minimum && x < m_Maximum;
	}

	const Interval Interval::s_Empty;
	const Interval s_RealNumbers(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
	const Interval s_PositiveRealNumbers(0, std::numeric_limits<double>::infinity());
}