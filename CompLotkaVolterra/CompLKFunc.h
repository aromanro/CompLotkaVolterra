#pragma once

#include "Eigen/Eigen"

class MyVector4d : public Eigen::Vector4d
{
	public:
	MyVector4d() {}

	MyVector4d(double val)
	{
		*this = Constant(val);
	}


	template <typename Derived> MyVector4d(const Eigen::EigenBase<Derived>& x)
	{
		Eigen::Vector4d::operator=(x);
	}

};


inline double abs(const MyVector4d& vec)
{
	return sqrt(vec.transpose() * vec);
}

template<typename T1, typename T2> class CompLKFunc
{
public:
	CompLKFunc(double s = 1.)
	{
		m_growthRates << 1., 0.72, 1.53, 1.27;

		m_interactionMatrix << 1, s*1.09, s*1.52, 0,
							   0, 1, s*0.44, s*1.36,
							   s*2.33, 0, 1, s*0.47,
							   s*1.21, s*0.51, s*0.35, 1;
	}

	CompLKFunc(const T1& growthRates, const T2& interactionMatrix)
		: m_growthRates(growthRates), m_interactionMatrix(interactionMatrix)
	{
	}


	T1 operator()(const T1& x) const
	{
		const T1 sum = m_interactionMatrix * x;

		T1 res;

		for (unsigned int i = 0; i < x.size(); ++i)
		{
			res(i) = m_growthRates(i) * x(i) * (1. - sum(i));
		}
			
		
		return std::move(res);
	}

protected:
	T1 m_growthRates;
	T2 m_interactionMatrix;
};


// this is the form Runge-Kutta expects it
template<typename T1, typename T2> class FunctorForLK
{
public:
	FunctorForLK() {}
	FunctorForLK(const CompLKFunc<T1, T2>& LKfunc) : m_LKfunc(LKfunc) {}
	
	template <typename Derived> T1 operator()(double /*t*/, const Eigen::EigenBase<Derived>& pos) 
	{
		const T1 val = pos;

		return m_LKfunc(val);
	}

protected:
	CompLKFunc<T1, T2> m_LKfunc;
};





