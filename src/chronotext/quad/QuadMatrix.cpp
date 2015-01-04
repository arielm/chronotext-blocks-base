/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#include "chronotext/quad/QuadMatrix.h"

using namespace std;
using namespace ci;

namespace chr
{
    QuadMatrix::QuadMatrix()
    {
        setToIdentity();
    }
    
    QuadMatrix& QuadMatrix::load(const QuadMatrix &matrix)
    {
        memcpy(m, matrix.m, sizeof(m));
        return *this;
    }
    
    QuadMatrix& QuadMatrix::load(const Matrix44f &matrix)
    {
        memcpy(m, matrix.m, sizeof(m));
        return *this;
    }
    
    QuadMatrix& QuadMatrix::load(const MatrixAffine2f &matrix)
    {
        m[ 0] = matrix.m[0]; m[ 4] = matrix.m[2]; m[ 8] = 0.0f; m[12] = matrix.m[4];
        m[ 1] = matrix.m[1]; m[ 5] = matrix.m[3]; m[ 9] = 0.0f; m[13] = matrix.m[5];
        m[ 2] = 0.0f;        m[ 6] = 0.0f;        m[10] = 1.0f; m[14] = 0.0f;
        m[ 3] = 0.0f;        m[ 7] = 0.0f;        m[11] = 0.0f; m[15] = 1.0f;
        
        return *this;
    }

    void QuadMatrix::push()
    {
        stack.push_back(values);
    }
    
    void QuadMatrix::pop()
    {
        if (!stack.empty())
        {
            values = stack.back();
            stack.pop_back();
        }
    }
    
    void QuadMatrix::setToIdentity()
    {
        m00 = m11 = m22 = m33 = 1.0f;
        m01 = m02 = m03 = m10 = m12 = m13 = m20 = m21 = m23 = m30 = m31 = m32 = 0.0f;
    }
    
    void QuadMatrix::setTranslation(float x, float y, float z)
    {
        m00 = m11 = m22 = m33 = 1.0f;
        m01 = m02 = m10 = m12 = m20 = m21 = m30 = m31 = m32 = 0.0f;
        
        m03 = x;
        m13 = y;
        m23 = z;
    }
    
    void QuadMatrix::translate(float x, float y, float z)
    {
        m03 += x * m00 + y * m01 + z * m02;
        m13 += x * m10 + y * m11 + z * m12;
        m23 += x * m20 + y * m21 + z * m22;
        m33 += x * m30 + y * m31 + z * m32;
    }
    
    void QuadMatrix::scale(float x, float y, float z)
    {
        m00 *= x;
        m10 *= x;
        m20 *= x;
        m30 *= x;
        
        m01 *= y;
        m11 *= y;
        m21 *= y;
        m31 *= y;
        
        m02 *= z;
        m12 *= z;
        m22 *= z;
        m32 *= z;
    }
    
    void QuadMatrix::rotateX(float a)
    {
        float c = math<float>::cos(a);
        float s = math<float>::sin(a);
        
        float r01 = m01 * c + m02 * s;
        float r02 = m02 * c - m01 * s;
        
        float r11 = m11 * c + m12 * s;
        float r12 = m12 * c - m11 * s;
        
        float r21 = m21 * c + m22 * s;
        float r22 = m22 * c - m21 * s;
        
        float r31 = m31 * c + m32 * s;
        float r32 = m32 * c - m31 * s;
        
        m01 = r01;
        m02 = r02;
        m11 = r11;
        m12 = r12;
        m21 = r21;
        m22 = r22;
        m31 = r31;
        m32 = r32;
    }
    
    void QuadMatrix::rotateY(float a)
    {
        float c = math<float>::cos(a);
        float s = math<float>::sin(a);
        
        float r00 = m00 * c - m02 * s;
        float r02 = m00 * s + m02 * c;
        
        float r10 = m10 * c - m12 * s;
        float r12 = m10 * s + m12 * c;
        
        float r20 = m20 * c - m22 * s;
        float r22 = m20 * s + m22 * c;
        
        float r30 = m30 * c - m32 * s;
        float r32 = m30 * s + m32 * c;
        
        m00 = r00;
        m02 = r02;
        m10 = r10;
        m12 = r12;
        m20 = r20;
        m22 = r22;
        m30 = r30;
        m32 = r32;
    }
    
    void QuadMatrix::rotateZ(float a)
    {
        float c = math<float>::cos(a);
        float s = math<float>::sin(a);
        
        float r00 = m00 * c + m01 * s;
        float r01 = m01 * c - m00 * s;
        
        float r10 = m10 * c + m11 * s;
        float r11 = m11 * c - m10 * s;
        
        float r20 = m20 * c + m21 * s;
        float r21 = m21 * c - m20 * s;
        
        float r30 = m30 * c + m31 * s;
        float r31 = m31 * c - m30 * s;
        
        m00 = r00;
        m01 = r01;
        m10 = r10;
        m11 = r11;
        m20 = r20;
        m21 = r21;
        m30 = r30;
        m31 = r31;
    }
    
    void QuadMatrix::rotateXY(float sx, float sy)
    {
        float cx = math<float>::sqrt(1.0f - sx * sx);
        float cy = math<float>::sqrt(1.0f - sy * sy);
        
        float cxy = cx + cy;
        
        float r00 = m00 * cy - m02 * sy;
        float r01 = m01 * cx + m02 * sx;
        float r02 = m00 * sy + m02 * cxy - m01 * sx;
        
        float r10 = m10 * cy - m12 * sy;
        float r11 = m11 * cx + m12 * sx;
        float r12 = m10 * sy + m12 * cxy - m11 * sx;
        
        float r20 = m20 * cy - m22 * sy;
        float r21 = m21 * cx + m22 * sx;
        float r22 = m20 * sy + m22 * cxy - m21 * sx;
        
        float r30 = m30 * cy - m32 * sy;
        float r31 = m31 * cx + m32 * sx;
        float r32 = m30 * sy + m32 * cxy - m31 * sx;
        
        m00 = r00;
        m01 = r01;
        m02 = r02;
        m10 = r10;
        m11 = r11;
        m12 = r12;
        m20 = r20;
        m21 = r21;
        m22 = r22;
        m30 = r30;
        m31 = r31;
        m32 = r32;
    }
    
    Vec3f QuadMatrix::transform(float x, float y) const
    {
        float x00 = x * m00;
        float x10 = x * m10;
        float x20 = x * m20;
        
        float y01 = y * m01;
        float y11 = y * m11;
        float y21 = y * m21;
        
        return Vec3f(x00 + y01 + m03, x10 + y11 + m13, x20 + y21 + m23);
    }
    
    void QuadMatrix::addTransformedQuad(const Quad &quad, std::vector<Vertex> &vertices) const
    {
        float x100 = quad.x1 * m00;
        float x110 = quad.x1 * m10;
        float x120 = quad.x1 * m20;
        
        float y101 = quad.y1 * m01;
        float y111 = quad.y1 * m11;
        float y121 = quad.y1 * m21;
        
        float x200 = quad.x2 * m00;
        float x210 = quad.x2 * m10;
        float x220 = quad.x2 * m20;
        
        float y201 = quad.y2 * m01;
        float y211 = quad.y2 * m11;
        float y221 = quad.y2 * m21;
        
        vertices.emplace_back(x100 + y101 + m03, x110 + y111 + m13, x120 + y121 + m23, quad.u1, quad.v1); // x1, y1
        vertices.emplace_back(x100 + y201 + m03, x110 + y211 + m13, x120 + y221 + m23, quad.u1, quad.v2); // x1, y2
        vertices.emplace_back(x200 + y201 + m03, x210 + y211 + m13, x220 + y221 + m23, quad.u2, quad.v2); // x2, y2
        vertices.emplace_back(x200 + y101 + m03, x210 + y111 + m13, x220 + y121 + m23, quad.u2, quad.v1); // x2, y1
    }
}
