/*******************************************************
* Copyright (c) 2015-2019, ArrayFire
* All rights reserved.
*
* This file is distributed under 3-clause BSD license.
* The complete license agreement can be obtained at:
* http://arrayfire.com/licenses/BSD-3-Clause
********************************************************/

#include <common.hpp>
#include <glfw/window.hpp>

#include <iostream>

#define GLFW_THROW_ERROR(msg, err) \
    throw fg::Error("Window constructor", __LINE__, msg, err);

#include <glm/gtc/matrix_transform.hpp>

using glm::rotate;
using glm::translate;
using glm::scale;

namespace wtk
{

Widget::Widget()
    : mWindow(NULL)
{
}

Widget::Widget(int pWidth, int pHeight, const char* pTitle, const Widget* pWindow, const bool invisible)
      : mLastXpos(0), mLastYpos(0), mButton(-1), mMVP(glm::mat4(1.0f))
{
    if (!glfwInit()) {
        std::cerr << "ERROR: GLFW wasn't able to initalize\n";
        GLFW_THROW_ERROR("glfw initilization failed", fg::FG_ERR_GL_ERROR)
    }

    auto wndErrCallback = [](int errCode, const char* pDescription)
    {
        fputs(pDescription, stderr);
    };
    glfwSetErrorCallback(wndErrCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (invisible)
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    else
        glfwWindowHint(GLFW_VISIBLE, GL_TRUE);

    glfwWindowHint(GLFW_SAMPLES, 4);
    mWindow = glfwCreateWindow(pWidth, pHeight, pTitle, nullptr,
                               (pWindow!=nullptr ? pWindow->getNativeHandle(): nullptr));

    if (!mWindow) {
        std::cerr<<"Error: Could not Create GLFW Window!\n";
        GLFW_THROW_ERROR("glfw window creation failed", fg::FG_ERR_GL_ERROR)
    }

    glfwSetWindowUserPointer(mWindow, this);

    auto keyboardCallback = [](GLFWwindow* w, int a, int b, int c, int d)
    {
        static_cast<Widget*>(glfwGetWindowUserPointer(w))->keyboardHandler(a, b, c, d);
    };

    auto cursorCallback = [](GLFWwindow* w, double xpos, double ypos)
    {
        static_cast<Widget*>(glfwGetWindowUserPointer(w))->cursorHandler(xpos, ypos);
    };

    auto buttonCallback = [](GLFWwindow* w, int button, int action, int mods)
    {
        static_cast<Widget*>(glfwGetWindowUserPointer(w))->buttonHandler(button, action, mods);
    };

    glfwSetKeyCallback(mWindow, keyboardCallback);
    glfwSetCursorPosCallback(mWindow, cursorCallback);
    glfwSetMouseButtonCallback(mWindow, buttonCallback);
}

Widget::~Widget()
{
    if (mWindow)
        glfwDestroyWindow(mWindow);
}

GLFWwindow* Widget::getNativeHandle() const
{
    return mWindow;
}

void Widget::makeContextCurrent() const
{
    glfwMakeContextCurrent(mWindow);
}

long long Widget::getGLContextHandle()
{
#ifdef OS_WIN
    return reinterpret_cast<long long>(glfwGetWGLContext(mWindow));
#endif
#ifdef OS_LNX
    return reinterpret_cast<long long>(glfwGetGLXContext(mWindow));
#endif
}

long long Widget::getDisplayHandle()
{
#ifdef OS_WIN
    return reinterpret_cast<long long>(GetDC(glfwGetWin32Window(mWindow)));
#endif
#ifdef OS_LNX
    return reinterpret_cast<long long>(glfwGetX11Display());
#endif
}

void Widget::getFrameBufferSize(int* pW, int* pH)
{
    glfwGetFramebufferSize(mWindow, pW, pH);
}

void Widget::setTitle(const char* pTitle)
{
    glfwSetWindowTitle(mWindow, pTitle);
}

void Widget::setPos(int pX, int pY)
{
    glfwSetWindowPos(mWindow, pX, pY);
}

void Widget::swapBuffers()
{
    glfwSwapBuffers(mWindow);
}

void Widget::hide()
{
    glfwHideWindow(mWindow);
}

void Widget::show()
{
    glfwShowWindow(mWindow);
}

bool Widget::close()
{
    return glfwWindowShouldClose(mWindow) != 0;
}

void Widget::keyboardHandler(int pKey, int pScancode, int pAction, int pMods)
{
    if (pKey == GLFW_KEY_ESCAPE && pAction == GLFW_PRESS) {
        glfwSetWindowShouldClose(mWindow, GL_TRUE);
    }
}

void printMat4(glm::mat4 m){
    std::cout << m[0][0] << " " << m[1][0] << " " << m[2][0] << " " << m[3][0] << std::endl;
    std::cout << m[0][1] << " " << m[1][1] << " " << m[2][1] << " " << m[3][1] << std::endl;
    std::cout << m[0][2] << " " << m[1][2] << " " << m[2][2] << " " << m[3][2] << std::endl;
    std::cout << m[0][3] << " " << m[1][3] << " " << m[2][3] << " " << m[3][3] << std::endl;
}

void Widget::cursorHandler(double pXpos, double pYpos)
{
    double deltaX = mLastXpos - pXpos;
    double deltaY = mLastYpos - pYpos;
    bool majorMoveDir = abs(deltaX) > abs(deltaY);  // True for Left-Right, False for Up-Down

    /**
     * RIGHT + MajorMoveDir = true  && deltaX > 0 => Rotate CW  about Y Axis
     * RIGHT + MajorMoveDir = true  && deltaX < 0 => Rotate CCW about Y Axis
     * RIGHT + MajorMoveDir = false && deltaY > 0 => Rotate CW  about X Axis
     * RIGHT + MajorMoveDir = false && deltaY > 0 => Rotate CCW about X Axis
     *
     * LEFT + MajorMoveDir = true                => Translate by deltaX along X
     * LEFT + MajorMoveDir = false               => Translate by deltaY along Y
     *
     * (CTRL/ALT) + LEFT + MajorMoveDir = true && deltaY > 0 => Zoom In
     * (CTRL/ALT) + LEFT + MajorMoveDir = true && deltaY > 0 => Zoom Out
     */

    if (mButton == GLFW_MOUSE_BUTTON_LEFT) {
        // Translate
        float speed = 0.01;
        mMVP = translate(mMVP, glm::vec3(-deltaX, deltaY, 0.0f) * speed);

    } else if (mButton == GLFW_MOUSE_BUTTON_LEFT + 10 * GLFW_MOD_ALT ||
               mButton == GLFW_MOUSE_BUTTON_LEFT + 10 * GLFW_MOD_CONTROL) {
        // Zoom
        if(deltaY != 0) {
            float speed = 0.01;  // Cushion
            if(deltaY < 0) {
                deltaY = 1.0 / (-deltaY);
            }
            mMVP = scale(mMVP, glm::vec3(pow(deltaY, speed)));
        }
    } else if (mButton == GLFW_MOUSE_BUTTON_RIGHT) {
        double speed = 0.01;
        if (majorMoveDir) {
            // Rotate about Y axis (left <-> right)
            // mMVP = rotate(mMVP, (float)(speed * deltaX), glm::vec3(0.0, 1.0, 0.0));
        } else {
            // Rotate about X axis (up <-> down)glm::
            // mMVP = rotate(mMVP, (float)(speed * deltaY), glm::vec3(1.0, 0.0, 0.0));
        }
    }

    mLastXpos = pXpos;
    mLastYpos = pYpos;
}

void Widget::buttonHandler(int pButton, int pAction, int pMods)
{
    mButton = -1;
    if (pButton == GLFW_MOUSE_BUTTON_LEFT && pAction == GLFW_PRESS) {
        mButton = GLFW_MOUSE_BUTTON_LEFT;
    } else if (pButton == GLFW_MOUSE_BUTTON_RIGHT && pAction == GLFW_PRESS) {
        mButton = GLFW_MOUSE_BUTTON_RIGHT;
    } else if (pButton == GLFW_MOUSE_BUTTON_MIDDLE && pAction == GLFW_PRESS) {
        mButton = GLFW_MOUSE_BUTTON_MIDDLE;
    }

    if(pMods == GLFW_MOD_ALT || pMods == GLFW_MOD_CONTROL) {
        mButton += 10 * pMods;
    }

    if(pButton == GLFW_MOUSE_BUTTON_MIDDLE && pMods == GLFW_MOD_CONTROL && pAction == GLFW_PRESS) {
        mMVP = glm::mat4(1.0f);
    }
}

void Widget::pollEvents()
{
    glfwPollEvents();
}

}
