//   Copyright © 2021, Renjie Chen @ USTC

#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#define FREEGLUT_STATIC
#include "gl_core_3_3.h"
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#define TW_STATIC
#include <AntTweakBar.h>

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#include "glprogram.h"
#include "MyImage.h"
#include "VAOImage.h"
#include "VAOMesh.h"


GLProgram MyMesh::prog;

MyMesh M;
int viewport[4] = { 0, 0, 1280, 960 };

bool showATB = true;

std::string imagefile = "boy.png";

MyImage img;
int resize_width, resize_height;

int mousePressButton;
int mouseButtonDown;
int mousePos[2];

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, viewport[2], viewport[3]);
    M.draw(viewport);

    if (showATB) TwDraw();
    glutSwapBuffers();
}

void onKeyboard(unsigned char code, int x, int y)
{
    if (!TwEventKeyboardGLUT(code, x, y)) {
        switch (code) {
        case 17:
            exit(0);
        case 'f':
            glutFullScreenToggle();
            break;
        case ' ':
            showATB = !showATB;
            break;
        }
    }

    glutPostRedisplay();
}

void onMouseButton(int button, int updown, int x, int y)
{
    if (!showATB || !TwEventMouseButtonGLUT(button, updown, x, y)) {
        mousePressButton = button;
        mouseButtonDown = updown;

        mousePos[0] = x;
        mousePos[1] = y;
    }

    glutPostRedisplay();
}


void onMouseMove(int x, int y)
{
    if (!showATB || !TwEventMouseMotionGLUT(x, y)) {
        if (mouseButtonDown == GLUT_DOWN) {
            if (mousePressButton == GLUT_MIDDLE_BUTTON) {
                M.moveInScreen(mousePos[0], mousePos[1], x, y, viewport);
            }
        }
    }

    mousePos[0] = x; mousePos[1] = y;

    glutPostRedisplay();
}


void onMouseWheel(int wheel_number, int direction, int x, int y)
{
    if (glutGetModifiers() & GLUT_ACTIVE_CTRL) {
    }
    else
        M.mMeshScale *= direction > 0 ? 1.1f : 0.9f;

    glutPostRedisplay();
}

int initGL(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowSize(960, 960);
    glutInitWindowPosition(200, 50);
    glutCreateWindow(argv[0]);

    // !Load the OpenGL functions. after the opengl context has been created
    if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
        return -1;

    glClearColor(1.f, 1.f, 1.f, 0.f);

    glutReshapeFunc([](int w, int h) { viewport[2] = w; viewport[3] = h; TwWindowSize(w, h); });
    glutDisplayFunc(display);
    glutKeyboardFunc(onKeyboard);
    glutMouseFunc(onMouseButton);
    glutMotionFunc(onMouseMove);
    glutMouseWheelFunc(onMouseWheel);
    glutCloseFunc([]() {exit(0); });
    return 0;
}

void uploadImage(const MyImage& img)
{
    int w = img.width();
    int h = img.height();
    float x[] = { 0, 0, 0, w, 0, 0, w, h, 0, 0, h, 0 };
    M.upload(x, 4, nullptr, 0, nullptr);

    M.tex.setImage(img);
    M.tex.setClamping(GL_CLAMP_TO_EDGE);
}


void readImage(const std::string& file)
{
    int w0 = img.width(), h0 = img.height();
    img = MyImage(file);
    uploadImage(img);
    resize_width = img.width();
    resize_height = img.height();

    if (w0 != img.width() || h0 != img.height()) M.updateBBox();
}

std::vector<BYTE> shrink(std::vector<BYTE>& Ms, std::vector<BYTE>& Mg, int& w, int& ws, int& hs, int& comps, int& comp)
{
    std::vector<int> M_1, M_2;
    std::vector<std::vector<int>> path;
    std::vector<int> xpath;

    //竖向删除接缝
    while (1)
    {
        if (ws == w) break;
        for (int i = 0; i < ws; i++)
        {
            M_1.push_back((int)Mg[i * comp]);
            M_2.push_back(0);
            xpath.push_back(2);
        }
        path.push_back(xpath);
        for (int i = 1; i < hs; i++)
        {
            xpath.clear();
            for (int j = 0; j < ws; j++)
            {
                if (j == 0)
                {
                    M_2[j] = (int)Mg[(i * ws + j) * comp] + std::min(M_1[j], M_1[j + 1]);
                    if (M_1[j] < M_1[j + 1])
                        xpath.push_back(0);
                    else
                        xpath.push_back(1);
                }
                else if (j == ws - 1)
                {
                    M_2[j] = (int)Mg[(i * ws + j) * comp] + std::min(M_1[j - 1], M_1[j]);
                    if (M_1[j - 1] < M_1[j])
                        xpath.push_back(-1);
                    else
                        xpath.push_back(0);
                }
                else
                {
                    M_2[j] = (int)Mg[(i * ws + j) * comp] + std::min({ M_1[j - 1],M_1[j],M_1[j + 1] });
                    if (M_1[j - 1] < M_1[j] && M_1[j - 1] < M_1[j + 1])
                        xpath.push_back(-1);
                    else if (M_1[j] < M_1[j - 1] && M_1[j] < M_1[j + 1])
                        xpath.push_back(0);
                    else
                        xpath.push_back(1);
                }
            }
            M_1 = M_2;
            path.push_back(xpath);
        }
        //寻找能量最小的接缝索引
        int index = 0;
        for (int i = 1; i < ws; i++)
        {
            if (M_1[index] > M_1[i])
            {
                index = i;
            }
        }
        //删除位于接缝上的元素
        int k = hs - 1;
        while (path[k][index] <= 1)
        {
            auto it1 = Ms.begin() + (ws * k + index) * comps;
            auto it2 = it1 + comps;
            auto it3 = Mg.begin() + (ws * k + index) * comp;
            auto it4 = it3 + comp;
            Ms.erase(it1, it2); Mg.erase(it3, it4);
            if (path[k][index] == 1) index++;
            if (path[k][index] == -1) index--;
            k--;
        }
        auto it1 = Ms.begin() + index * comps;
        auto it2 = it1 + comps;
        auto it3 = Mg.begin() + index * comp;
        auto it4 = it3 + comp;
        Ms.erase(it1, it2); Mg.erase(it3, it4);
        ws--;
        M_1.clear(); M_2.clear(); path.clear(); xpath.clear();
    }
   
    return Ms;
}

std::vector<BYTE> magnify(std::vector<BYTE>& Ms, std::vector<BYTE>& Mg, std::vector<BYTE>& Mgt, int& w, int& ws, int& hs, int& comps, int& comp)
{
    std::vector<int> M_1, M_2;
    std::vector<std::vector<int>> path;
    std::vector<int> xpath;
    std::vector<int> indexToImg;
    for (int i = 0; i < ws; i++) indexToImg.push_back(i);
    int wg = ws;
    //竖向增加接缝
    while (1)
    {
        if (ws == w || ws >= 2 * wg) break;
        for (int i = 0; i < wg; i++)
        {
            M_1.push_back((int)Mg[i * comp]);
            M_2.push_back(0);
            xpath.push_back(2);
        }
        path.push_back(xpath);
        for (int i = 1; i < hs; i++)
        {
            xpath.clear();
            for (int j = 0; j < wg; j++)
            {
                if (j == 0)
                {
                    M_2[j] = (int)Mg[(i * wg + j) * comp] + std::min(M_1[j], M_1[j + 1]);
                    if (M_1[j] < M_1[j + 1])
                        xpath.push_back(0);
                    else
                        xpath.push_back(1);
                }
                else if (j == wg - 1)
                {
                    M_2[j] = (int)Mg[(i * wg + j) * comp] + std::min(M_1[j - 1], M_1[j]);
                    if (M_1[j - 1] < M_1[j])
                        xpath.push_back(-1);
                    else
                        xpath.push_back(0);
                }
                else
                {
                    M_2[j] = (int)Mg[(i * wg + j) * comp] + std::min({ M_1[j - 1],M_1[j],M_1[j + 1] });
                    if (M_1[j - 1] < M_1[j] && M_1[j - 1] < M_1[j + 1])
                        xpath.push_back(-1);
                    else if (M_1[j] < M_1[j - 1] && M_1[j] < M_1[j + 1])
                        xpath.push_back(0);
                    else
                        xpath.push_back(1);
                }
            }
            M_1 = M_2;
            path.push_back(xpath);
        }
        //寻找能量最小的接缝索引
        int index = 0;
        for (int i = 1; i < wg; i++)
        {
            if (M_1[index] > M_1[i])
            {
                index = i;
            }
        }
        //增加位于接缝上的元素
        int k = hs - 1;
        while (path[k][index] <= 1)
        {
            auto it = Ms.begin() + (k * ws + indexToImg[index]) * comps;
            for (int j = comps - 1; j >= 0; j--) it = Ms.insert(it, Ms[(k * ws + indexToImg[index] + 1) * comps - 1]);
            auto itt = Mgt.begin() + (k * ws + indexToImg[index]) * comp;
            for (int j = comps - 1; j >= 0; j--) itt = Mgt.insert(itt, Mgt[(k * ws + indexToImg[index] + 1) * comp - 1]);
            auto it3 = Mg.begin() + (wg * k + index) * comp;
            auto it4 = it3 + comp;
            Mg.erase(it3, it4);
            if (path[k][index] == 1) index++;
            if (path[k][index] == -1) index--;
            k--;
        }
        auto it = Ms.begin() + (indexToImg[index]) * comps;
        for (int j = comps - 1; j >= 0; j--) it = Ms.insert(it, Ms[(indexToImg[index] + 1) * comps - 1]);
        auto itt = Mgt.begin() + (indexToImg[index]) * comp;
        for (int j = comp - 1; j >= 0; j--) itt = Mgt.insert(itt, Mgt[(indexToImg[index] + 1) * comp - 1]);
        auto it3 = Mg.begin() + index * comp;
        auto it4 = it3 + comp;
        Mg.erase(it3, it4);
        ws++; wg--;
        auto itx=indexToImg.erase(indexToImg.begin() + index);
        if (itx != indexToImg.end()) 
        {
            *itx = *itx + 1;
        }
        M_1.clear(); M_2.clear(); path.clear(); xpath.clear();
    }

    return Ms;
}

std::vector<BYTE> transpose(std::vector<BYTE>& M, int h, int w, int comp)
{
    std::vector<BYTE> M_T;
    for (int j = 0; j < w; j++)
    {
        for (int i = 0; i < h; i++)
        {
            for (int k = 0; k < comp; k++)
            {
                M_T.push_back(M[(i * w + j) * comp + k]);
            }
        }
    }
    return M_T;
}

MyImage seamCarving(const MyImage& img, int w, int h)
{
    // TODO
    int ws = img.width();
    int hs = img.height();
    int comps = img.dim();
    MyImage im("boy_saliency.png");
    int comp = im.dim();
    std::vector<BYTE> Mg, Ms, Mgt;
    for (int i = 0; i < ws * hs * comps; i++)
    {
        Ms.push_back(*(img.data() + i));
    }
    for (int i = 0; i < ws * hs * comp; i++)
    {
        Mg.push_back(*(im.data() + i));
        Mgt.push_back(*(im.data() + i));
    }
    //横向缩小
    if (ws > w)
    {
        Ms = shrink(Ms, Mg, w, ws, hs, comps, comp);
    }
    //横向放大
    else if (ws < w)
    {
        while (ws < w)
        {
            Ms = magnify(Ms, Mg, Mgt, w, ws, hs, comps, comp);
            Mg = Mgt;
        }
    }
    //竖向缩小
    if (hs > h)
    {
        Ms = transpose(Ms, hs, ws, comps);
        Mg = transpose(Mg, hs, ws, comp);
        Ms = shrink(Ms, Mg, h, hs, ws, comps, comp);
        Ms = transpose(Ms, ws, hs, comps);
    }
    //竖向放大
    else if (hs < h)
    {
        Ms = transpose(Ms, hs, ws, comps);
        Mg = transpose(Mg, hs, ws, comp);
        Mgt = transpose(Mgt, hs, ws, comp);
        while (hs < h)
        {
            
            Ms = magnify(Ms, Mg, Mgt, h, hs, ws, comps, comp);
            Mg = Mgt;
        }
        Ms = transpose(Ms, ws, hs, comps);
    }
    MyImage retimg(Ms, w, h, comps);
    return retimg;
}

void createTweakbar()
{
    //Create a tweak bar
    TwBar *bar = TwNewBar("Image Viewer");
    TwDefine(" 'Image Viewer' size='220 150' color='0 128 255' text=dark alpha=128 position='5 5'"); // change default tweak bar size and color

    TwAddVarRW(bar, "Scale", TW_TYPE_FLOAT, &M.mMeshScale, " min=0 step=0.1");

    TwAddVarRW(bar, "Image filename", TW_TYPE_STDSTRING, &imagefile, " ");
    TwAddButton(bar, "Read Image", [](void*) { readImage(imagefile); }, nullptr, "");

    TwAddVarRW(bar, "Resize Width", TW_TYPE_INT32, &resize_width, "group='Seam Carving' min=1 ");
    TwAddVarRW(bar, "Resize Height", TW_TYPE_INT32, &resize_height, "group='Seam Carving' min=1 ");
    TwAddButton(bar, "Run Seam Carving", [](void* img) {
        MyImage newimg = seamCarving(*(const MyImage*)img, resize_width, resize_height);
        uploadImage(newimg);
        }, 
        &img, "");
}


int main(int argc, char *argv[])
{
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), { 100, 5000 });

    if (initGL(argc, argv)) {
        fprintf(stderr, "!Failed to initialize OpenGL!Exit...");
        exit(-1);
    }

    MyMesh::buildShaders();


    float x[] = { 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0 };
    float uv[] = { 0, 0, 1, 0, 1, 1, 0, 1 };
    int t[] = { 0, 1, 2, 2, 3, 0 };

    M.upload(x, 4, t, 2, uv);

    //////////////////////////////////////////////////////////////////////////
    TwInit(TW_OPENGL_CORE, NULL);
    //Send 'glutGetModifers' function pointer to AntTweakBar;
    //required because the GLUT key event functions do not report key modifiers states.
    TwGLUTModifiersFunc(glutGetModifiers);
    glutSpecialFunc([](int key, int x, int y) { TwEventSpecialGLUT(key, x, y); glutPostRedisplay(); }); // important for special keys like UP/DOWN/LEFT/RIGHT ...
    TwCopyStdStringToClientFunc([](std::string& dst, const std::string& src) {dst = src; });


    createTweakbar();

    //////////////////////////////////////////////////////////////////////////
    atexit([] { TwDeleteAllBars();  TwTerminate(); });  // Called after glutMainLoop ends

    glutTimerFunc(1, [](int) { readImage(imagefile); },  0);


    //////////////////////////////////////////////////////////////////////////
    glutMainLoop();

    return 0;
}

