/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#include "chronotext/path/FollowablePath.h"
#include "chronotext/Context.h"
#include "chronotext/utils/MathUtils.h"

using namespace std;
using namespace ci;

namespace chr
{
    FollowablePath::FollowablePath(int capacity)
    :
    mode(MODE_TANGENT)
    {
        if (capacity > 0)
        {
            extendCapacity(capacity);
        }
    }
    
    FollowablePath::FollowablePath(const vector<Vec2f> &points)
    :
    mode(MODE_TANGENT)
    {
        add(points);
    }
    
    FollowablePath::FollowablePath(const Path2d &path, float approximationScale)
    :
    mode(MODE_TANGENT)
    {
        add(path.subdivide(approximationScale));
        
        if (isClosed())
        {
            setMode(FollowablePath::MODE_LOOP);
        }
    }
    
    FollowablePath::FollowablePath(InputSource::Ref inputSource)
    :
    mode(MODE_TANGENT)
    {
        read(inputSource->loadDataSource());
    }
    
    void FollowablePath::read(DataSourceRef source)
    {
        auto stream = source->createStream();
        
        int newPointsSize;
        stream->readLittle(&newPointsSize);
        
        extendCapacity(newPointsSize);
        
        // ---
        
        Vec2f point;
        
        for (int i = 0; i < newPointsSize; i++)
        {
            stream->readLittle(&point.x);
            stream->readLittle(&point.y);
            add(point);
        }
    }
    
    void FollowablePath::write(DataTargetRef target)
    {
        auto stream = target->getStream();
        
        stream->writeLittle(size());
        
        for (auto &point : points)
        {
            stream->writeLittle(point.x);
            stream->writeLittle(point.y);
        }
    }
    
    void FollowablePath::add(const vector<Vec2f> &newPoints)
    {
        extendCapacity(newPoints.size());
        
        for (auto &point : newPoints)
        {
            add(point);
        }
    }
    
    void FollowablePath::add(const ci::Vec2f &point)
    {
        if (!points.empty())
        {
            Vec2f delta = point - points.back();
            
            if (delta != Vec2f::zero())
            {
                lengths.push_back(lengths.back() + delta.length());
            }
            else
            {
                return;
            }
        }
        else
        {
            lengths.push_back(0);
        }

        points.push_back(point);
    }
    
    const vector<Vec2f>& FollowablePath::getPoints() const
    {
        return points;
    }
    
    const vector<float>& FollowablePath::getLengths() const
    {
        return lengths;
    }
    
    void FollowablePath::clear()
    {
        points.clear();
        lengths.clear();
    }
    
    int FollowablePath::size() const
    {
        return points.size();
    }
    
    bool FollowablePath::empty() const
    {
        return points.empty();
    }
    
    float FollowablePath::getLength() const
    {
        if (!points.empty())
        {
            return lengths.back();
        }
        else
        {
            return 0;
        }
    }
    
    Rectf FollowablePath::getBounds() const
    {
        if (points.empty())
        {
            return Rectf(0, 0, 0, 0);
        }
        
        float minX = points.front().x, maxX = minX;
        float minY = points.front().y, maxY = minY;
        
        for (auto &point : points)
        {
            if (point.x < minX) minX = point.x;
            if (point.y < minY) minY = point.y;
            
            if (point.x > maxX) maxX = point.x;
            if (point.y > maxY) maxY = point.y;
        }
        
        return Rectf(minX, minY, maxX, maxY);
    }
    
    void FollowablePath::close()
    {
        if (size() > 2)
        {
            if (points.front() != points.back())
            {
                add(points.front());
            }
        }
    }
    
    bool FollowablePath::isClosed() const
    {
        return (size() > 2) && (points.front() == points.back());
    }
    
    void FollowablePath::setMode(Mode mode)
    {
        FollowablePath::mode = mode;
    }
    
    FollowablePath::Mode FollowablePath::getMode() const
    {
        return mode;
    }
    
    FollowablePath::Value FollowablePath::offset2Value(float offset) const
    {
        float length = getLength();
        
        if (length > 0)
        {
            if ((mode == MODE_LOOP) || (mode == MODE_MODULO))
            {
                offset = utils::math::boundf(offset, length);
            }
            else
            {
                if (offset <= 0)
                {
                    if (mode == MODE_BOUNDED)
                    {
                        offset = 0;
                    }
                }
                else if (offset >= length)
                {
                    if (mode == MODE_BOUNDED)
                    {
                        offset = length;
                    }
                }
            }
            
            auto index = utils::search(lengths, offset, 1, size());
            auto &p0 = points[index];
            auto &p1 = points[index + 1];
            
            float ratio = (offset - lengths[index]) / (lengths[index + 1] - lengths[index]);
            
            Value value;
            value.position = p0 + (p1 - p0) * ratio;
            value.angle = math<float>::atan2(p1.y - p0.y, p1.x - p0.x);
            value.offset = offset;
            value.index = index;
            
            return value;
        }
        else
        {
            return Value();
        }
    }
    
    Vec2f FollowablePath::offset2Position(float offset) const
    {
        float length = getLength();
        
        if (length > 0)
        {
            if ((mode == MODE_LOOP) || (mode == MODE_MODULO))
            {
                offset = utils::math::boundf(offset, length);
            }
            else
            {
                if (offset <= 0)
                {
                    if (mode == MODE_BOUNDED)
                    {
                        return points.front();
                    }
                }
                else if (offset >= length)
                {
                    if (mode == MODE_BOUNDED)
                    {
                        return points.back();
                    }
                }
            }
            
            auto index = utils::search(lengths, offset, 1, size());
            auto &p0 = points[index];
            auto &p1 = points[index + 1];
            
            float ratio = (offset - lengths[index]) / (lengths[index + 1] - lengths[index]);
            return p0 + (p1 - p0) * ratio;
        }
        else
        {
            return Vec2f::zero();
        }
    }
    
    float FollowablePath::offset2Angle(float offset) const
    {
        float length = getLength();
        
        if (length > 0)
        {
            if ((mode == MODE_LOOP) || (mode == MODE_MODULO))
            {
                offset = utils::math::boundf(offset, length);
            }
            else
            {
                if (offset <= 0)
                {
                    if (mode == MODE_BOUNDED)
                    {
                        offset = 0;
                    }
                }
                else if (offset >= length)
                {
                    if (mode == MODE_BOUNDED)
                    {
                        offset = length;
                    }
                }
            }
            
            auto index = utils::search(lengths, offset, 1, size());
            auto &p0 = points[index];
            auto &p1 = points[index + 1];
            
            return math<float>::atan2(p1.y - p0.y, p1.x - p0.x);
        }
        else
        {
            return 0;
        }
    }
    
    float FollowablePath::offset2SampledAngle(float offset, float sampleSize) const
    {
        Vec2f gradient = offset2Gradient(offset, sampleSize);
        
        /*
         * WE USE AN EPSILON VALUE TO AVOID
         * DEGENERATED RESULTS IN SOME EXTREME CASES
         * (E.G. CLOSE TO 180 DEGREE DIFF. BETWEEN TWO SEGMENTS)
         */
        if (gradient.lengthSquared() > 1)
        {
            return math<float>::atan2(gradient.y, gradient.x);
        }
        else
        {
            return offset2Angle(offset);
        }
    }
    
    Vec2f FollowablePath::offset2Gradient(float offset, float sampleSize) const
    {
        Vec2f pm = offset2Position(offset - sampleSize * 0.5f);
        Vec2f pp = offset2Position(offset + sampleSize * 0.5f);
        
        return (pp - pm) * 0.5f;
    }
    
    /*
     * RETURNS false IF CLOSEST-POINT IS FARTHER THAN threshold DISTANCE
     *
     * REFERENCE: "Minimum Distance between a Point and a Line" BY Paul Bourke
     * http://paulbourke.net/geometry/pointlineplane
     */
    bool FollowablePath::findClosestPoint(const Vec2f &input, float threshold, FollowablePath::ClosePoint &output) const
    {
        float min = threshold * threshold; // BECAUSE IT IS MORE EFFICIENT TO WORK WITH MAGNIFIED DISTANCES

        int end = size();
        int index = -1;
        Vec2f position;
        float offset;

        for (int i = 0; i < end; i++)
        {
            int i0, i1;
            
            if (i == end - 1)
            {
                i0 = i - 1;
                i1 = i;
            }
            else
            {
                i0 = i;
                i1 = i + 1;
            }
            
            auto &p0 = points[i0];
            auto &p1 = points[i1];
            
            Vec2f delta = p1 - p0;
            float length = lengths[i1] - lengths[i0];
            float u = delta.dot(input - p0) / (length * length);
            
            if (u >= 0 && u <= 1)
            {
                Vec2f p = p0 + u * delta;
                float mag = (p - input).lengthSquared();
                
                if (mag < min)
                {
                    min = mag;
                    index = i0;
                    
                    position = p;
                    offset = lengths[index] + u * length;
                }
            }
            else
            {
                float mag0 = (p0 - input).lengthSquared();
                float mag1 = (p1 - input).lengthSquared();
                
                if ((mag0 < min) && (mag0 < mag1))
                {
                    min = mag0;
                    index = i0;
                    
                    position = points[i0];
                    offset = lengths[index];
                }
                else if ((mag1 < min) && (mag1 < mag0))
                {
                    min = mag1;
                    index = i1;
                    
                    position = points[i1];
                    offset = lengths[index];
                }
            }
        }
        
        if (index != -1)
        {
            output.position = position;
            output.offset = offset;
            output.distance = math<float>::sqrt(min);
            
            return true;
        }
        
        return false;
    }
    
    /*
     * REFERENCE: "Minimum Distance between a Point and a Line" BY Paul Bourke
     * http://paulbourke.net/geometry/pointlineplane
     */
    FollowablePath::ClosePoint FollowablePath::closestPointFromSegment(const Vec2f &input, int segmentIndex) const
    {
        FollowablePath::ClosePoint output;
        
        if ((segmentIndex >= 0) && (segmentIndex + 1 < size()))
        {
            int i0 = segmentIndex;
            int i1 = segmentIndex + 1;
            
            auto &p0 = points[i0];
            auto &p1 = points[i1];
            
            Vec2f delta = p1 - p0;
            float length = lengths[i1] - lengths[i0];
            float u = delta.dot(input - p0) / (length * length);
            
            if (u >= 0 && u <= 1)
            {
                Vec2f p = p0 + u * delta;
                float mag = (p - input).lengthSquared();
                
                output.position = p;
                output.offset = lengths[i0] + u * length;
                output.distance = math<float>::sqrt(mag);
            }
            else
            {
                float mag0 = (p0 - input).lengthSquared();
                float mag1 = (p1 - input).lengthSquared();
                
                if (mag0 < mag1)
                {
                    output.position = p0;
                    output.offset = lengths[i0];
                    output.distance = math<float>::sqrt(mag0);
                }
                else
                {
                    output.position = p1;
                    output.offset = lengths[i1];
                    output.distance = math<float>::sqrt(mag1);
                }
            }
        }
        else
        {
            output.distance = numeric_limits<float>::max();
        }
        
        return output;
    }
    
    void FollowablePath::extendCapacity(int amount)
    {
        int newCapacity = size() + amount;
        points.reserve(newCapacity);
        lengths.reserve(newCapacity);
    }
}
