#include "BVH.h"
#include "../Pubh.h"

namespace DSM {
	BVH::BVH(std::vector<std::shared_ptr<Hittable>> hittables, std::size_t begin, std::size_t end)
	{
		for(auto objIndex = begin; objIndex < end; ++objIndex){
			m_BoundingBox = AABB::Uion(m_BoundingBox, hittables[objIndex]->BoundingBox());
		}
		std::size_t axisIndex = m_BoundingBox.LongestAxis();
		
		auto cmpFunc = [axisIndex](const auto& box0, const auto& box1){
			return BoxCompare(box0, box1, axisIndex);
		};

		std::size_t objSpan = end - begin;
		if(objSpan == 1){
			m_Left = m_Right = hittables[begin];
		}
		else if(objSpan == 2){
			m_Left = hittables[begin];
			m_Right = hittables[begin + 1];
		}
		else{
			std::sort(std::begin(hittables) + begin, std::begin(hittables) + end, cmpFunc);
			std::size_t mid = begin + objSpan * .5f;
			m_Left = std::make_shared<BVH>(hittables, begin, mid);
			m_Right = std::make_shared<BVH>(hittables, mid, end);
		}
	}

	std::optional<HitRecord> BVH::Hit(const Ray& ray, Intervalf interval) const
	{
		std::optional<HitRecord> leftRec;
		std::optional<HitRecord> rightRec;

		if (!m_BoundingBox.Hit(ray, interval)) {
			return leftRec;
		}

		leftRec = m_Left->Hit(ray, interval);
		// 若击中了左侧的包围盒需要更新最远处
		rightRec = m_Right->Hit(ray, Intervalf{interval.GetMin(), 
			leftRec.has_value() ? leftRec->m_Time : interval.GetMax()});

		return rightRec.has_value() ? rightRec : leftRec;
	}

    bool BVH::BoxCompare(
		const std::shared_ptr<Hittable> box0, 
		const std::shared_ptr<Hittable> box1, 
		std::size_t axisIndex)
    {
		Intervalf i0 = box0->BoundingBox()[axisIndex];
		Intervalf i1 = box1->BoundingBox()[axisIndex];
        return i0.GetMin() < i1.GetMin();
    }

}
