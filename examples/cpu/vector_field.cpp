/*******************************************************
 * Copyright (c) 2015-2019, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#include <forge.h>
#include <CPUCopy.hpp>
#include <complex>
#include <cmath>
#include <vector>
#include <iostream>

const unsigned DIMX = 1000;
const unsigned DIMY = 800;
const unsigned WIN_ROWS = 2;
const unsigned WIN_COLS = 2;

float FRANGE_START = -3.f;
float FRANGE_END = 3.f;

using namespace std;

float func(float x, float y){
    return sinf(x) * cosf(y) + cosf(y) * cosf(y);
}

void simple_vecfield(float range_start, float range_end, float dx, std::vector<float> &vec, float (*map) (float, float)){
    if(range_start > range_end && dx > 0) return;
    for(float i=range_start; i < range_end; i+=dx*2){
        for(float j=range_start; j < range_end; j+=dx*2){
            vec.push_back(i - dx/2);
            vec.push_back(j - dx/2);
            vec.push_back(i + dx/2 + ((*map)(i+dx/2,j) - (*map)(i-dx/2,j)));
            vec.push_back(j + dx/2 + ((*map)(i,j+dx/2) - (*map)(i,j-dx/2)));
        }
    }
}

void component_vecfield(float range_start, float range_end, float dx, std::vector<float> &vec, float (*map) (float, float)){
    if(range_start > range_end && dx > 0) return;
    for(float i=range_start; i < range_end; i+=dx){
        for(float j=range_start; j < range_end; j+=dx){
            vec.push_back(i);
            vec.push_back(j);
            vec.push_back(((*map)(i+dx/2,j) - (*map)(i-dx/2,j))/dx);
            vec.push_back(((*map)(i,j+dx/2) - (*map)(i,j-dx/2))/dx);
        }
    }
}

int main(void){
    /*
     * First Forge call should be a window creation call
     * so that necessary OpenGL context is created for any
     * other fg::* object to be created successfully
     */
    fg::Window wnd(DIMX, DIMY, "Plotting Demo");
    wnd.makeCurrent();
    /* create an font object and load necessary font
     * and later pass it on to window object so that
     * it can be used for rendering text */
    fg::Font fnt;
#ifdef OS_WIN
    fnt.loadSystemFont("Calibri", 32);
#else
    fnt.loadSystemFont("Vera", 32);
#endif
    wnd.setFont(&fnt);


    do {
        FRANGE_START-=0.01;
        FRANGE_END+=0.01;
        std::vector<float> vector_field;
        simple_vecfield(FRANGE_START, FRANGE_END, 0.2f, vector_field, &func);
        //component_vecfield(FRANGE_START, FRANGE_END, 0.1f, vector_field, &func);
        fg::VectorField plt(vector_field.size()/4, fg::f32);                              //create a default plot
        plt.setColor(fg::FG_WHITE);                                            //use a forge predefined color
        copy(plt, &vector_field[0]);
        plt.setAxesLimits(FRANGE_END, FRANGE_START, FRANGE_END, FRANGE_START);
        // draw window and poll for events last
        wnd.draw(plt);
        wnd.swapBuffers();
    } while(!wnd.close());

    return 0;
}

