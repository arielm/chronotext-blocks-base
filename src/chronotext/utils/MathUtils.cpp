/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#include "chronotext/utils/MathUtils.h"

using namespace std;
using namespace ci;

namespace chr
{
    bool MathUtils::isRectNull(const Rectf &rect)
    {
        return (rect.x1 == 0) && (rect.y1 == 0) && (rect.x2 == 0) && (rect.y2 == 0);
    }
    
    bool MathUtils::compareRects(const Rectf &r1, const Rectf &r2)
    {
        return (r1.x1 == r2.x1) && (r1.y1 == r2.y1) && (r1.x2 == r2.x2) && (r1.y2 == r2.y2);
    }
    
    void MathUtils::transformVertices(const vector<Vec2f> &source, vector<Vec2f> &target, const MatrixAffine2f &matrix)
    {
        target.clear();
        target.reserve(source.size());
        
        for (auto &vertex : source)
        {
            target.push_back(matrix.transformPoint(vertex));
        }
    }
    
    Rectf MathUtils::getBoundingBox(const vector<Vec2f> &polygon)
    {
        float minX = numeric_limits<float>::max();
        float minY = numeric_limits<float>::max();
        float maxX = numeric_limits<float>::min();
        float maxY = numeric_limits<float>::min();
        
        for (auto &point : polygon)
        {
            if (point.x < minX) minX = point.x;
            if (point.y < minY) minY = point.y;
            
            if (point.x > maxX) maxX = point.x;
            if (point.y > maxY) maxY = point.y;
        }
        
        return Rectf(minX, minY, maxX, maxY);
    }
    
    /*
     * RETURNS numeric_limits<float>::max() IF CLOSEST POINT IS FARTHER THAN threshold DISTANCE
     *
     * REFERENCE: "Minimum Distance between a Point and a Line" BY Paul Bourke
     * http://paulbourke.net/geometry/pointlineplane
     */
    float MathUtils::getShortestDistance(const Vec2f &point, const vector<Vec2f> &polygon, bool close, float threshold)
    {
        float min = threshold * threshold; // BECAUSE IT IS MORE EFFICIENT TO WORK WITH MAGNIFIED DISTANCES

        int end = polygon.size();
        bool found = false;
        
        for (int i = 0; i < end; i++)
        {
            int i0, i1;
            
            if (i == end - 1)
            {
                if (close)
                {
                    i0 = i;
                    i1 = 0;
                }
                else
                {
                    break;
                }
            }
            else
            {
                i0 = i;
                i1 = i + 1;
            }
            
            Vec2f p0 = polygon[i0];
            Vec2f p1 = polygon[i1];
            
            if (p0 != p1)
            {
                Vec2f delta = p1 - p0;
                float u = delta.dot(point - p0) / delta.lengthSquared();
                
                if (u >= 0 && u <= 1)
                {
                    Vec2f p = p0 + u * delta;
                    float mag = (p - point).lengthSquared();
                    
                    if (mag < min)
                    {
                        min = mag;
                        found = true;
                    }
                }
                else
                {
                    float mag0 = (p0 - point).lengthSquared();
                    float mag1 = (p1 - point).lengthSquared();
                    
                    if ((mag0 < min) && (mag0 < mag1))
                    {
                        min = mag0;
                        found = true;
                    }
                    else if ((mag1 < min) && (mag1 < mag0))
                    {
                        min = mag1;
                        found = true;
                    }
                }
            }
        }
        
        return found ? math<float>::sqrt(min) : numeric_limits<float>::max();
    }
    
    /*
     * THE FOLLOWING 4 FUNCTIONS ARE BASED ON CODE FROM Cinder:
     *
     * https://github.com/cinder/Cinder/blob/ae580c2cb0fc44d0a99b233dbefdf736f7093209/src/cinder/Path2d.cpp#L754-768
     * https://github.com/cinder/Cinder/blob/ae580c2cb0fc44d0a99b233dbefdf736f7093209/src/cinder/Path2d.cpp#L824-857
     * https://github.com/cinder/Cinder/blob/ae580c2cb0fc44d0a99b233dbefdf736f7093209/src/cinder/Shape2d.cpp#L135-144
     *
     * ASSERTION: THE IMPLIED WINDING IS "ODD" (AKA "EVEN-ODD FILL-RULE")
     *
     * TODO:
     * CHECK WHAT'S NECESSARY IN ORDER TO HANDLE "NON-ZERO" WINDING (WHICH SEEMS TO BE THE DEFAULT FILL-RULE IN HTML CANVAS)
     * REFERENCE: http://blogs.adobe.com/webplatform/2013/01/30/winding-rules-in-canvas/
     */
    
    static float linearYatX(const Vec2f p[2], float x)
    {
        if (p[0].x == p[1].x) return p[0].y;
        return p[0].y + (p[1].y - p[0].y) * (x - p[0].x) / (p[1].x - p[0].x);
    }
    
    static size_t linearCrossings(const Vec2f p[2], const Vec2f &pt)
    {
        if ((p[0].x < pt.x && pt.x <= p[1].x) || (p[1].x < pt.x && pt.x <= p[0].x ))
        {
            if (pt.y > linearYatX(p, pt.x)) return 1;
        }
        
        return 0;
    }
    
    bool MathUtils::isPointInside(const Vec2f &point, const vector<Vec2f> &polygon)
    {
        const size_t size = polygon.size();
        
        if (size <= 2)
        {
            return false;
        }
        else
        {
            size_t crossings = 0;
            
            for (size_t s = 0; s < size - 1; ++s)
            {
                crossings += linearCrossings(&(polygon[s]), point);
            }
            
            Vec2f temp[2];
            temp[0] = polygon[size - 1];
            temp[1] = polygon[0];
            crossings += linearCrossings(&(temp[0]), point);
            
            return (crossings & 1) == 1;
        }
    }
    
    bool MathUtils::isPointInside(const Vec2f &point, const vector<vector<Vec2f>> &polygons)
    {
        int numPathsInside = 0;
        
        for (auto &polygon : polygons)
        {
            if (isPointInside(point, polygon))
            {
                numPathsInside++;
            }
        }
        
        return (numPathsInside % 2) == 1;
    }
}
