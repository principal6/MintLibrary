#include <MintMath/Include/Geometry.h>
#include <MintMath/Include/Float3.h>
#include <MintContainer/Include/Algorithm.hpp>


namespace mint
{
	uint32 GrahamScan_FindStartPoint(const Vector<Float2>& points)
	{
		Float2 min = Float2(10000.0f, -10000.0f);
		const uint32 pointCount = points.Size();
		uint32 startPointIndex = 0;
		for (uint32 pointIndex = 0; pointIndex < pointCount; pointIndex++)
		{
			if (points[pointIndex]._y >= min._y)
			{
				if (points[pointIndex]._y == min._y)
				{
					if (points[pointIndex]._x < min._x)
					{
						min._x = points[pointIndex]._x;
						startPointIndex = pointIndex;
					}
				}
				else
				{
					min._y = points[pointIndex]._y;
					min._x = points[pointIndex]._x;
					startPointIndex = pointIndex;
				}
			}
		}
		return startPointIndex;
	}

	void GrahamScan_SortPoints(Vector<Float2>& inoutPoints)
	{
		const uint32 startPointIndex = GrahamScan_FindStartPoint(inoutPoints);
		const Float2& startPoint = inoutPoints[startPointIndex];
		struct AngleIndex
		{
			AngleIndex(const float theta, const uint32 index) : _theta{ theta }, _index{ index } { __noop; }
			float _theta = 0.0f;
			uint32 _index = 0;
		};
		struct AngleIndexComparator
		{
			bool operator()(const AngleIndex& lhs, const AngleIndex& rhs) const { return lhs._theta < rhs._theta; }
		};
		const uint32 pointCount = inoutPoints.Size();
		Vector<AngleIndex> angleIndices;
		for (uint32 pointIndex = 0; pointIndex < pointCount; pointIndex++)
		{
			if (pointIndex == startPointIndex)
			{
				continue;
			}

			const Float2 v = inoutPoints[pointIndex] - startPoint;
			const float theta = ::atan2f(-v._y, v._x);
			angleIndices.PushBack(AngleIndex(theta, pointIndex));
		}
		QuickSort(angleIndices, AngleIndexComparator());
		Vector<Float2> orderedPoints;
		orderedPoints.PushBack(startPoint);
		for (const AngleIndex& angleIndex : angleIndices)
		{
			orderedPoints.PushBack(inoutPoints[angleIndex._index]);
		}
		inoutPoints = orderedPoints;
	}

	void GrahamScan_Convexify(Vector<Float2>& inoutPoints)
	{
		if (inoutPoints.IsEmpty())
		{
			return;
		}

		GrahamScan_SortPoints(inoutPoints);

		Vector<uint32> convexPointIndices;
		convexPointIndices.Reserve(inoutPoints.Size());
		convexPointIndices.PushBack(0);
		convexPointIndices.PushBack(1);
		for (uint32 i = 2; i < inoutPoints.Size(); i++)
		{
			const uint32 index_c = convexPointIndices[convexPointIndices.Size() - 2];
			const uint32 index_b = convexPointIndices[convexPointIndices.Size() - 1];
			const uint32 index_a = i;
			const Float2& c = inoutPoints[index_c];
			const Float2& b = inoutPoints[index_b];
			const Float2& a = inoutPoints[index_a];
			const Float3 cb = Float3(b - c);
			const Float3 ba = Float3(a - b);
			const Float3 ba_x_cb = ba.Cross(cb);
			const bool is_counter_clockwise = ba_x_cb._z > 0.0f;
			if (is_counter_clockwise)
			{
				convexPointIndices.PushBack(index_a);
			}
			else
			{
				convexPointIndices.PopBack();
				--i;
			}
		}

		Vector<Float2> convexPoints;
		convexPoints.Reserve(convexPointIndices.Size());
		for (const uint32 convexPointIndex : convexPointIndices)
		{
			convexPoints.PushBack(inoutPoints[convexPointIndex]);
		}
		inoutPoints = convexPoints;
	}
}
