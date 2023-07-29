#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#define FREEGLUT_STATIC
#include "gl_core_3_3.h"
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#define TW_STATIC
#include <AntTweakBar.h>


#include <ctime>
#include <memory>
#include <vector>
#include <string>
#include <cstdlib>
#include <cmath> 
#include <numeric>
#include <iostream>

#include "objloader.h"
#include "glprogram.h"
#include "MyImage.h"
#include "VAOImage.h"
#include "VAOMesh.h"
#include "trackball.h"

#include "laplacian.h"


GLProgram MyMesh::prog, MyMesh::pickProg, MyMesh::pointSetProg;
GLTexture MyMesh::colormapTex;

MyMesh M;
int viewport[4] = { 0, 0, 1280, 960 };
int actPrimType = MyMesh::PE_VERTEX;

bool showATB = true;

using MatX3f = Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>;
using MatX3i = Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor>;
MatX3f meshX;
MatX3i meshF;

void deform_preprocess()
{
    auto L = Laplacian(meshX, meshF);
}

Eigen::MatrixXf union_weight(Eigen::MatrixXi adjacency_matrix, std::vector<int> P2PVtxIds, int N, int n, double labmda)
{
    using namespace Eigen;
    MatrixXf laplacian_matrix(N + n, N);
    laplacian_matrix.fill(0);
    for (int i = 0; i < N + n; i++)
    {

        if (i < N)
        {
            int num = 0;
            for (int j = 0; j < N - 1; j++)
            {
                
                if (adjacency_matrix(i, j) == 1)
                {
                    laplacian_matrix(i, j) = 1;
                    num++;
                }
                /*else
                {
                    laplacian_matrix(i, j) = 0;
                }*/
            }
            laplacian_matrix.row(i) /= num;
            laplacian_matrix(i, i) = -1;
        }
        else
        {
            for (int j = 0; j < N; j++)
            {
                if (j == P2PVtxIds[i - N]) laplacian_matrix(i, j) = labmda;
                else laplacian_matrix(i, j) = 0;
            }
        }
    }
    return laplacian_matrix;
}

Eigen::MatrixXf cot_weight(Eigen::MatrixXi adjacency_matrix, std::vector<int> P2PVtxIds, int N, int n, double labmda)
{
    using namespace Eigen;
    MatrixXf laplacian_matrix(N + n, N);
    laplacian_matrix.fill(0);
    for (int i = 0; i < N + n; i++)
    {

        if (i < N)
        {
            std::vector<int> index;
            std::vector<double> weight;
            std::vector<double> angle;
            double sigma;
            for (int j = 0; j < N; j++)
            {

                if (adjacency_matrix(i, j) == 1)
                {
                    index.push_back(j);
                }
                /*else
                {
                    laplacian_matrix(i, j) = 0;
                }*/
            }
            int k = index.size();
            for (int j = 0; j < k - 1; j++)
            {
                sigma = (meshX.row(index[j]) - meshX.row(i)).dot(meshX.row(index[j + 1]) - meshX.row(i)) / ((meshX.row(index[j]) - meshX.row(i)).norm() * (meshX.row(index[j + 1]) - meshX.row(i)).norm());
                angle.push_back(acos(sigma));
            }
            sigma = (meshX.row(index[k - 1]) - meshX.row(i)).dot(meshX.row(index[0]) - meshX.row(i)) / ((meshX.row(index[k - 1]) - meshX.row(i)).norm() * (meshX.row(index[0]) - meshX.row(i)).norm());
            angle.push_back(acos(sigma));
            double sum= std::accumulate(angle.begin(), angle.end(), 0.);
            //std::cout << sum << std::endl;
            for (int j = 0; j < k; j++)
            {
                angle[j] = 2 * M_PI * angle[j] / sum;
            }
            /*for (int j = 0; j < k; j++)
            {
                angle[j] = sin(angle[j]);
            }*/
            double cB = (meshX.row(index[0]) - meshX.row(i)).dot(meshX.row(index[0]) - meshX.row(i)) + (meshX.row(index[k - 1]) - meshX.row(i)).dot(meshX.row(index[k - 1]) - meshX.row(i)) + 2 * cos(angle[k - 1]) * (meshX.row(index[0]) - meshX.row(i)).dot(meshX.row(index[k - 1]) - meshX.row(i));
            double sinB = sin(angle[k - 1]) / sqrt(cB) * (meshX.row(index[0]) - meshX.row(i)).norm();
            double cG = (meshX.row(index[0]) - meshX.row(i)).dot(meshX.row(index[0]) - meshX.row(i)) + (meshX.row(index[1]) - meshX.row(i)).dot(meshX.row(index[1]) - meshX.row(i)) + 2 * cos(angle[0]) * (meshX.row(index[0]) - meshX.row(i)).dot(meshX.row(index[1]) - meshX.row(i));
            double sinG=sin(angle[0])/ sqrt(cG) * (meshX.row(index[0]) - meshX.row(i)).norm();
            //if (sinB > 1) sinB = 0.1;
            //if (sinG > 1) sinG = 0.1;
            double cot = sqrt(1 - pow(sinB, 2)) / sinB + sqrt(1 - pow(sinG, 2)) / sinG;
            //std::cout << sinB << ' ' << sinG << std::endl;
            weight.push_back(cot / 2);
            for (int j = 1; j < k - 1; j++)
            {
                cB = (meshX.row(index[j]) - meshX.row(i)).dot(meshX.row(index[j]) - meshX.row(i)) + (meshX.row(index[j - 1]) - meshX.row(i)).dot(meshX.row(index[j - 1]) - meshX.row(i)) + 2 * cos(angle[j - 1]) * (meshX.row(index[j]) - meshX.row(i)).dot(meshX.row(index[j - 1]) - meshX.row(i));
                sinB = sin(angle[j - 1]) / sqrt(cB) * (meshX.row(index[j]) - meshX.row(i)).norm();
                cG = (meshX.row(index[j]) - meshX.row(i)).dot(meshX.row(index[j]) - meshX.row(i)) + (meshX.row(index[j+1]) - meshX.row(i)).dot(meshX.row(index[j+1]) - meshX.row(i)) + 2 * cos(angle[j]) * (meshX.row(index[j]) - meshX.row(i)).dot(meshX.row(index[j+1]) - meshX.row(i));
                sinG = sin(angle[j]) / sqrt(cG) * (meshX.row(index[j]) - meshX.row(i)).norm();
                //if (sinB > 1) sinB = 0.1;
                //if (sinG > 1) sinG = 0.1;
                cot = sqrt(1 - pow(sinB, 2)) / sinB + sqrt(1 - pow(sinG, 2)) / sinG;
                weight.push_back(cot / 2);
            }
            cB = (meshX.row(index[k-1]) - meshX.row(i)).dot(meshX.row(index[k-1]) - meshX.row(i)) + (meshX.row(index[k-2]) - meshX.row(i)).dot(meshX.row(index[k-2]) - meshX.row(i)) + 2 * cos(angle[k - 2]) * (meshX.row(index[k-2]) - meshX.row(i)).dot(meshX.row(index[k - 1]) - meshX.row(i));
            sinB = sin(angle[k - 2]) / sqrt(cB) * (meshX.row(index[k - 1]) - meshX.row(i)).norm();
            cG = (meshX.row(index[0]) - meshX.row(i)).dot(meshX.row(index[0]) - meshX.row(i)) + (meshX.row(index[k - 1]) - meshX.row(i)).dot(meshX.row(index[k - 1]) - meshX.row(i)) + 2 * cos(angle[k - 1]) * (meshX.row(index[0]) - meshX.row(i)).dot(meshX.row(index[k - 1]) - meshX.row(i));
            sinG = sin(angle[k - 1]) / sqrt(cG) * (meshX.row(index[k - 1]) - meshX.row(i)).norm();
            //if (sinB > 1) sinB = 0.1;
            //if (sinG > 1) sinG = 0.1;
            cot = sqrt(1 - pow(sinB, 2)) / sinB + sqrt(1 - pow(sinG, 2)) / sinG;
            weight.push_back(cot / 2);
            double Sum = std::accumulate(weight.begin(), weight.end(), 0.);
            for (int j = 0; j < k; j++)
            {
                laplacian_matrix(i, index[j]) = weight[j]/Sum;
            }
            //std::cout << std::accumulate(weight.begin(), weight.end(), 0.) << std::endl;
            laplacian_matrix(i, i) = -1;
            index.clear(); weight.clear(); angle.clear();
        }
        else
        {
            for (int j = 0; j < N; j++)
            {
                if (j == P2PVtxIds[i - N]) laplacian_matrix(i, j) = labmda;
                //else laplacian_matrix(i, j) = 0;
            }
        }
    }
    return laplacian_matrix;
}

Eigen::MatrixXf Rotation(std::vector<float> R)
{
    using namespace Eigen;
    MatrixXf r(3, 3);
    r(0, 0) = 1 - 2 * pow(R[2], 2) - 2 * pow(R[3], 2); r(1, 1) = 1 - 2 * pow(R[1], 2) - 2 * pow(R[3], 2); r(2, 2) = 1 - 2 * pow(R[1], 2) - 2 * pow(R[2], 2);
    r(0, 1) = 2 * R[1] * R[2] - 2 * R[0] * R[3]; r(1, 0) = 2 * R[1] * R[2] + 2 * R[0] * R[3];
    r(0, 2) = 2 * R[1] * R[3] + 2 * R[0] * R[2]; r(2, 0) = 2 * R[1] * R[3] - 2 * R[0] * R[2];
    r(1, 2) = 2 * R[2] * R[3] - 2 * R[0] * R[1]; r(2, 1) = 2 * R[2] * R[3] + 2 * R[0] * R[1];
    return r;
}

Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> laplacian_mesh_iditing(std::vector<int> P2PVtxIds, std::vector<float> p2pDsts, std::vector<float> R)
{
    using namespace Eigen;
    double labmda = 10.;
    //建立邻接矩阵
    const int N = meshX.rows();
    MatrixXi adjacency_matrix(N,N);
    //初始化邻接矩阵
    adjacency_matrix.fill(0);
    /*for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            adjacency_matrix(i, j) = 0;
        }
    }*/
    int M = meshF.rows();
    for (int i = 0; i < M; i++)
    {
        adjacency_matrix(meshF(i, 0), meshF(i, 1))=1; adjacency_matrix(meshF(i, 1), meshF(i, 0))=1;
        adjacency_matrix(meshF(i, 1), meshF(i, 2))=1; adjacency_matrix(meshF(i, 2), meshF(i, 1))=1;
        adjacency_matrix(meshF(i, 2), meshF(i, 0))=1; adjacency_matrix(meshF(i, 0), meshF(i, 2))=1;
    }
    //构造laplace矩阵
    MatX3f P2PDsts = Map<MatX3f>(p2pDsts.data(), p2pDsts.size() / 3, 3)*labmda;//目标点坐标
    int n = P2PDsts.rows();
    //初始化laplace矩阵
    //MatrixXf laplacian_matrix=union_weight(adjacency_matrix,P2PVtxIds,N,n,labmda);//均匀权重
    MatrixXf laplacian_matrix = cot_weight(adjacency_matrix, P2PVtxIds, N, n, labmda);//cot权重
    //将四元数组转化为旋转矩阵
    MatrixXf rotation=Rotation(R);
    // std::cout << "{ " << R[0] << ' ' << R[1] << ' ' << R[2] << ' ' << R[3] << " }" << std::endl;
    //计算laplace坐标
    MatrixXf L = laplacian_matrix.topRows(N);
    MatrixXf delta = L * meshX;
    MatrixXf Delta(N + n, 3);
    for (int i = 0; i < N + n; i++)
    {
        if (i < N)
        {
            /*for (int j = 0; j < 3; j++)
            {
                Delta(i, j) = delta(i, j);
            }*/
            Delta.row(i) = delta.row(i);
        }
        else
        {
            Delta.row(i) = P2PDsts.row(i-N);
        }
    }
    // 对选定点作用旋转矩阵
    //Delta.row(P2PVtxIds[n - 1]) = Delta.row(P2PVtxIds[n - 1]) * rotation.transpose();
    // Delta.row(N+n-1) = Delta.row(N+n-1) * rotation.transpose();
    /*for (int i = 0; i < N + n; i++)
    {
        for (int j = 0; j < N; j++)
        {
            std::cout << laplacian_matrix(i, j) << ' ';
        }
        std::cout << std::endl;
    }*/

    MatrixXf y(N, 3);
    //for (int i = 0; i < 3; i++)
    //{
    //    y.col(i) = laplacian_matrix.householderQr().solve(Delta.col(i));
    //    
    //    y.col(i) = laplacian_matrix.colPivHouseholderQr().solve(Delta.col(i));
    //}
    y = laplacian_matrix.householderQr().solve(Delta);
    //y = (laplacian_matrix.transpose() * laplacian_matrix).ldlt().solve(laplacian_matrix.transpose() * Delta);
    return y;
}

float myRotation[4] = { 1, 0, 0, 0 }; //BONUS: interactively specify the rotation for the Laplacian coordinates at the handles

void meshDeform()
{
    using namespace Eigen;

    std::vector<int> P2PVtxIds = M.getConstrainVertexIds();
    std::vector<float> p2pDsts = M.getConstrainVertexCoords();
    std::vector<float> R(myRotation,myRotation+4);

    // TODO 2: compute deformation result y using Laplacian coordinates


    Matrix<float, Dynamic, Dynamic, RowMajor> y;
    y = laplacian_mesh_iditing(P2PVtxIds, p2pDsts, R);
    //y = meshX;
    if (y.cols() > 3)  y = y.leftCols(3);
    if (y.rows() == 0 || y.cols() != 3) return;

    M.upload(y.data(), y.rows(), nullptr, 0, nullptr);
}

int mousePressButton;
int mouseButtonDown;
int mousePos[2];

bool msaa = true;



void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (msaa) glEnable(GL_MULTISAMPLE);
    else glDisable(GL_MULTISAMPLE);

    glViewport(0, 0, viewport[2], viewport[3]);
    M.draw(viewport);

    if (showATB) TwDraw();
    glutSwapBuffers();

    //glFinish();
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

        if (updown == GLUT_DOWN) {
            if (button == GLUT_LEFT_BUTTON) {
                if (glutGetModifiers()&GLUT_ACTIVE_CTRL) {
                }
                else {
                    int r = M.pick(x, y, viewport, M.PE_VERTEX, M.PO_ADD);
                }
            }
            else if (button == GLUT_RIGHT_BUTTON) {
                M.pick(x, y, viewport, M.PE_VERTEX, M.PO_REMOVE);
            }
        }
        else { // updown == GLUT_UP
            if (button == GLUT_LEFT_BUTTON);
        }

        mousePos[0] = x;
        mousePos[1] = y;
    }

    glutPostRedisplay();
}


void onMouseMove(int x, int y)
{
    meshDeform();
    std::cout << "{ " << myRotation[0] << ' ' << myRotation[1] << ' ' << myRotation[2] << ' ' << myRotation[3] << " }" << std::endl;
    if (!showATB || !TwEventMouseMotionGLUT(x, y)) {
        
        if (mouseButtonDown == GLUT_DOWN) {
            if (mousePressButton == GLUT_MIDDLE_BUTTON) {
                M.moveInScreen(mousePos[0], mousePos[1], x, y, viewport);
            }
            else if (mousePressButton == GLUT_LEFT_BUTTON) {
                if (!M.moveCurrentVertex(x, y, viewport)) {
                    meshDeform();
                }
                else {
                    const float s[2] = { 2.f / viewport[2], 2.f / viewport[3] };
                    auto r = Quat<float>(M.mQRotate)*Quat<float>::trackball(x*s[0] - 1, 1 - y*s[1], s[0]*mousePos[0] - 1, 1 - s[1]*mousePos[1]);
                    std::copy_n(r.q, 4, M.mQRotate);
                }
            }
        }
    }

    mousePos[0] = x; mousePos[1] = y;

    glutPostRedisplay();
}


void onMouseWheel(int wheel_number, int direction, int x, int y)
{
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


void createTweakbar()
{
    TwBar *bar = TwGetBarByName("MeshViewer");
    if (bar)    TwDeleteBar(bar);

    //Create a tweak bar
    bar = TwNewBar("MeshViewer");
    TwDefine(" MeshViewer size='220 100' color='0 128 255' text=dark alpha=128 position='5 5'"); // change default tweak bar size and color

    TwAddVarRO(bar, "#Vertex", TW_TYPE_INT32, &M.nVertex, " group='Mesh View'");
    TwAddVarRO(bar, "#Face", TW_TYPE_INT32, &M.nFace, " group='Mesh View'");

    TwAddVarRW(bar, "Point Size", TW_TYPE_FLOAT, &M.pointSize, " group='Mesh View' ");
    TwAddVarRW(bar, "Vertex Color", TW_TYPE_COLOR4F, M.vertexColor.data(), " group='Mesh View' help='mesh vertex color' ");


    TwAddVarRW(bar, "Edge Width", TW_TYPE_FLOAT, &M.edgeWidth, " group='Mesh View' ");
    TwAddVarRW(bar, "Edge Color", TW_TYPE_COLOR4F, M.edgeColor.data(), " group='Mesh View' help='mesh edge color' ");

    TwAddVarRW(bar, "Face Color", TW_TYPE_COLOR4F, M.faceColor.data(), " group='Mesh View' help='mesh face color' ");

    TwDefine(" MeshViewer/'Mesh View' opened=false ");

    TwAddVarRW(bar, "Rotation", TW_TYPE_QUAT4F, myRotation, " group='Modeling' open help='can be used to specify the rotation for current handle' ");
}

int main(int argc, char *argv[])
{
    if (initGL(argc, argv)) {
        fprintf(stderr, "!Failed to initialize OpenGL!Exit...");
        exit(-1);
    }

    MyMesh::buildShaders();

    std::vector<float> x;
    std::vector<int> f;

    const char* meshpath = argc > 1 ? argv[1] : "balls.obj";
    readObj(meshpath, x, f);

    meshX = Eigen::Map<MatX3f>(x.data(), x.size() / 3, 3);
    meshF = Eigen::Map<MatX3i>(f.data(), f.size() / 3, 3);


    M.upload(x.data(), x.size() / 3, f.data(), f.size() / 3, nullptr);

    //////////////////////////////////////////////////////////////////////////
    TwInit(TW_OPENGL_CORE, NULL);
    //Send 'glutGetModifers' function pointer to AntTweakBar;
    //required because the GLUT key event functions do not report key modifiers states.
    TwGLUTModifiersFunc(glutGetModifiers);
    glutSpecialFunc([](int key, int x, int y) { TwEventSpecialGLUT(key, x, y); glutPostRedisplay(); }); // important for special keys like UP/DOWN/LEFT/RIGHT ...
    TwCopyStdStringToClientFunc([](std::string& dst, const std::string& src) {dst = src; });

    createTweakbar();

    //////////////////////////////////////////////////////////////////////////
    atexit([] { TwDeleteAllBars();  TwTerminate(); }); 

    glutTimerFunc(1, [](int) {
        deform_preprocess();
    }, 
        0);


    //////////////////////////////////////////////////////////////////////////
    glutMainLoop();

    return 0;
}
