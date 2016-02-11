/*******************************************************
* Copyright (c) 2015-2019, ArrayFire
* All rights reserved.
*
* This file is distributed under 3-clause BSD license.
* The complete license agreement can be obtained at:
* http://arrayfire.com/licenses/BSD-3-Clause
********************************************************/

#pragma once

#include <common.hpp>
#include <chart.hpp>
#include <memory>
#include <map>
#include <glm/glm.hpp>

namespace internal
{

class vecfield_impl : public Chart2D {
    protected:
        /* plot points characteristics */
        GLuint    mNumPoints;
        fg::dtype mDataType;
        GLenum    mGLType;
        float     mLineColor[4];
        fg::MarkerType mMarkerType;
        fg::PlotType   mPlotType;
        /* OpenGL Objects */
        GLuint    mMainVBO;
        size_t    mMainVBOsize;
        GLuint    mDrawVBO;
        size_t    mDrawVBOsize;
        GLuint    mMarkerProgram;
        /* shared variable index locations */
        GLuint    mPointIndex;
        GLuint    mMarkerColIndex;
        GLuint    mMarkerTypeIndex;
        GLuint    mSpriteTMatIndex;

        std::map<int, GLuint> mVAOMap;

        /* bind and unbind helper functions
         * for rendering resources */
        void bindResources(int pWindowId);
        void unbindResources() const;

    public:
        vecfield_impl(unsigned pNumPoints, fg::dtype pDataType, fg::PlotType, fg::MarkerType);
        ~vecfield_impl();

        void setColor(fg::Color col);
        void setColor(float r, float g, float b);
        GLuint vbo() const;
        size_t size() const;

        void render(int pWindowId, int pX, int pY, int pViewPortWidth, int pViewPortHeight);
};

class _VectorField {
    private:
        std::shared_ptr<vecfield_impl> vec_f;

    public:
        _VectorField(unsigned pNumPoints, fg::dtype pDataType, fg::PlotType pType, fg::MarkerType mType)
            : vec_f(std::make_shared<vecfield_impl>(pNumPoints, pDataType, pType, mType)) {}

        inline const std::shared_ptr<vecfield_impl>& impl() const {
            return vec_f;
        }

        inline void setColor(fg::Color col) {
            vec_f->setColor(col);
        }

        inline void setColor(float r, float g, float b) {
            vec_f->setColor(r, g, b);
        }

        inline void setAxesLimits(float pXmax, float pXmin, float pYmax, float pYmin) {
            vec_f->setAxesLimits(pXmax, pXmin, pYmax, pYmin);
        }

        inline void setAxesTitles(const char* pXTitle, const char* pYTitle) {
            vec_f->setAxesTitles(pXTitle, pYTitle);
        }

        inline float xmax() const {
            return vec_f->xmax();
        }

        inline float xmin() const {
            return vec_f->xmin();
        }

        inline float ymax() const {
            return vec_f->ymax();
        }

        inline float ymin() const {
            return vec_f->ymin();
        }

        inline GLuint vbo() const {
            return vec_f->vbo();
        }

        inline size_t size() const {
            return vec_f->size();
        }
};

}
