/*******************************************************
 * Copyright (c) 2015-2019, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#pragma once

#include <fg/defines.h>

namespace internal
{
class _VectorField;
}

namespace fg
{

/**
   \class VectorField

   \brief Line graph to display vector fields.
 */
class VectorField {
    private:
        internal::_VectorField* value;

    public:
        /**
           Creates a VectorField object

           \param[in] pNumPoints is number of data points to display
           \param[in] pDataType takes one of the values of \ref dtype that indicates
                      the integral data type of vector field data
         */
        FGAPI VectorField(unsigned pNumPoints, dtype pDataType, fg::PlotType=fg::FG_LINE, fg::MarkerType=fg::FG_NONE);

        /**
           Copy constructor for VectorField

           \param[in] other is the VectorField of which we make a copy of.
         */
        FGAPI VectorField(const VectorField& other);

        /**
           VectorField Destructor
         */
        FGAPI ~VectorField();

        /**
           Set the color of the vectors

           \param[in] col takes values of fg::Color to define vector field color
        */
        FGAPI void setColor(fg::Color col);

        /**
           Set the color of the vector field

           \param[in] pRed is Red component in range [0, 1]
           \param[in] pGreen is Green component in range [0, 1]
           \param[in] pBlue is Blue component in range [0, 1]
         */
        FGAPI void setColor(float pRed, float pGreen, float pBlue);

        /**
           Set the chart axes limits

           \param[in] pXmax is X-Axis maximum value
           \param[in] pXmin is X-Axis minimum value
           \param[in] pYmax is Y-Axis maximum value
           \param[in] pYmin is Y-Axis minimum value
         */
        FGAPI void setAxesLimits(float pXmax, float pXmin, float pYmax, float pYmin);

        /**
           Set axes titles in histogram(bar chart)

           \param[in] pXTitle is X-Axis title
           \param[in] pYTitle is Y-Axis title
         */
        FGAPI void setAxesTitles(const char* pXTitle, const char* pYTitle);

        /**
           Get X-Axis maximum value

           \return Maximum value along X-Axis
         */
        FGAPI float xmax() const;

        /**
           Get X-Axis minimum value

           \return Minimum value along X-Axis
         */
        FGAPI float xmin() const;

        /**
           Get Y-Axis maximum value

           \return Maximum value along Y-Axis
         */
        FGAPI float ymax() const;

        /**
           Get Y-Axis minimum value

           \return Minimum value along Y-Axis
         */
        FGAPI float ymin() const;

        /**
           Get the OpenGL Vertex Buffer Object identifier

           \return OpenGL VBO resource id.
         */
        FGAPI unsigned vbo() const;

        /**
           Get the OpenGL Vertex Buffer Object resource size

           \return OpenGL VBO resource size.
         */
        FGAPI unsigned size() const;

        /**
           Get the handle to internal implementation of Histogram
         */
        FGAPI internal::_VectorField* get() const;
};

}
