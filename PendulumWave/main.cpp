//#include <windows.h>
//#include <condefs.h>

#include <GLUT/glut.h>
#include <iostream>
#include <stdlib.h>    //C言語標準ライブラリのインクルード
#include <stdio.h>
#include <math.h>
#include "Pendulum.h"

#define KEY_ESC 27	//ESCキーの登録
#define PI 3.141592653589793238 /*円周率*/
#define PI2 2.0*PI

//プロトタイプ宣言
void T_Box(double, double, double, double, double, double, int);
void T_Torus(double, double, double, double, double, double, int);
void T_Sphere(double, double, double, double, double, double, int);
void T_Cylinder(double, double, int, int);

void timer(int);
void mouse(int, int, int, int);
void keyboard(int, int, int);
void normalkeyb(unsigned char, int, int);
void motion(int, int);
void polarview(void);
void myInit(void);
void resize(int, int);
void display(void);
///////////////////////////////////////////////////////

Pendulum *pendulum;
const int NUM = 12;
const double dt=0.01, m=2.0, l=0.5, c=0.5; //振り子のパラメータ
double tim = 0.0;
int x_0;
bool flg = false, test_flg = false;   //マウスクリックのFlg
bool cl = true;
double ans = 1;

double _dt; //微分する時間
double _m;  //振り子の質量
double _l;  //振り子の長さ
double _c;  //
double _g;  //重力
double x_k0[2], x_k1[2];
bool _cl;


//グローバル変数
int xBegin, yBegin, mButton, mflg;		/*　GLUT定数 for mouse, motion function */
float xdistance,ydistance,elevation,azimuth,zoom;/*　GLUT定数 for motion, polarview function */

GLfloat red[] = { 0.8, 0.2, 0.2, 1.0 };		/*　拡散反射成分値(色指定) */
GLfloat yellow[] = { 0.8, 0.8, 0.2, 1.0 };	/*　拡散反射成分値(色指定) */
GLfloat blue[] = { 0.2, 0.2, 0.8, 1.0 };	/*　拡散反射成分値(色指定) */

float wristDiffuse[] = { 0.8, 0.9, 1.0, 0.0 };	/*　拡散反射成分値(手首部色指定) */
float joint_diffuse[] = {1.0, 1.0, 0.9, 0.0};	/*　拡散反射成分値(関節部色指定) */
float link_diffuse[] = {0.8, 0.9, 1.0, 0.0};	/*　拡散反射成分値(リンク部色指定) */

float whiteSpecular[] = { 1.0, 1.0, 1.0, 1.0 };	/*　鏡面反射成分値(色指定) */
float shininess = 128.0;			/*　鏡面反射光の鋭さ0?128 */

float whiteAmbient[] = { 0.1, 0.1, 0.1, 1.0 };		/*　環境光反射成分値(色指定) */
float background_ambient[] = {0.0, 0.0, 0.0, 1.0};	/*　環境光反射成分値(色指定) */

GLfloat material_diffuse[] = {1.0, 1.0, 1.0, 0.0};
GLfloat green[] = { 0.2, 0.8, 0.2, 1.0 };
GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat white_black[] = { 0.8, 0.8, 0.8, 1.0 };


//メイン関数
int main(int argc, char *argv[])	/* p1.c参照 */
{
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(320, 240);
    glutInit(&argc, argv);		/* p1.c参照 */
    myInit();
    glutDisplayFunc(display);
    glutReshapeFunc(resize);		/* リシェープコールバック関数の登録 */
    glutTimerFunc(dt*1000,timer,0);		/* 30msec,番号１のタイマーをセット */
    //glutIdleFunc(idle);
    glutMainLoop();			/* イベント待ちの無限ループへ入る */
    return 0;
}

//関数群

/*/////////////////////////////////////////////////////////////*/
/*
 /* 	描写関連(H氏より授業資料として提供)
 /*
 /*/////////////////////////////////////////////////////////////*/

void display(void){
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 		/* 画面クリア */
    glLoadIdentity();    						/* スタックのクリア */
    polarview();							/* ビューイング変換 */
    
    glPushMatrix(); //支柱
    glRotated(90, 1.0, 0.0, 0.0);
    glTranslated(0.0,-1.9,0.0);
    T_Cylinder(0.02, 2.0, 100, 1);
    glPopMatrix();
    
    for(int i = 0; i < NUM; i++){
        double L,CLR;
        glPushMatrix(); //振り子
        glTranslated(0.0, 0.0, pendulum[i].draw(3));
        glRotated(pendulum[i].draw(1), 0.0, 0.0, 1.0);
        L = pendulum[i].draw(2);
        glTranslated(0.0, -L, 0.0);
        T_Cylinder(0.005, L, 100, 1);
        CLR = pendulum[i].draw(4);
        if(CLR > 0) T_Sphere(0.0, -L, 0.0, 0.1, 0.1, 0.1, 2);
        else        T_Sphere(0.0, -L, 0.0, 0.1, 0.1, 0.1, 1);
        glPopMatrix();
    }
    
    glutSwapBuffers();
    
}

/*/////////////////////////////////////////////////////////////*/
/*
 /* 	OpenGL関連？(H氏より提供)
 /*
 /*/////////////////////////////////////////////////////////////*/

//--------------------３Ｄ関数----------------
void timer(int value){ 			/* タイマ用　コールバック関数 */

    glutTimerFunc(dt*1000, timer, 0);
    tim += dt;
    if(flg){
        for(int i = 0; i < NUM; i++){
            pendulum[i].process();
        }
    }
    glutPostRedisplay();
}


void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        switch (button){
            case GLUT_RIGHT_BUTTON:
                mButton = button;
                //mflg=0;
                break;
       	    case GLUT_MIDDLE_BUTTON:
                break;
            case GLUT_LEFT_BUTTON:
                mButton = button;
                //mflg=0;
                flg = false;
                x_0 = x;
                for(int i = 0; i < NUM; i++){
                    pendulum[i].mouse_flg();
                }
                break;
                
            default:
                break;
        }
        xBegin = x;     /* 初期座標 */
        yBegin = y;
    }else flg = true;
}

void motion(int x, int y)
{                                  /* x:マウスのx座標, y:マウスのy座標 */
    int xDisp,yDisp;
    
    xDisp=x-xBegin;                     /* マウスの移動距離 */
    yDisp=y-yBegin;
    
    switch (mButton) {
        case GLUT_RIGHT_BUTTON:              /* 回転角度導出 */
            azimuth -= (float) xDisp/2.0;
            elevation -= (float) yDisp/2.0;
            break;
        case GLUT_LEFT_BUTTON:             /* 移動距離導出 */
            //ydistance += (float) yDisp/40.0;
            //xdistance += (float) xDisp/40.0;
            
            int dx;
            dx = x - x_0;
            for(int i = 0; i < NUM; i++){
                pendulum[i].mouse_init((double)dx/400, dx*0.05);
            }
            x_0 = x;

            break;
    }
    xBegin = x;
    yBegin = y;
    glutPostRedisplay();
}

void keyboard(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP: /* 上矢印キー */
            zoom+=0.1;   /* 拡大 */
            break;
            
        case GLUT_KEY_DOWN: /* 下矢印キー */
            zoom-=0.1;     /* 縮小 */
            break;
            
        case GLUT_KEY_RIGHT: /* 下矢印キー */
            ydistance += (float) 4/40.0;
            xdistance += (float) 4/40.0;
            break;
            
        case GLUT_KEY_LEFT: /* 下矢印キー */
            ydistance -= (float) 4/40.0;
            xdistance -= (float) 4/40.0;
            break;
            
        case GLUT_KEY_HOME: /* Homeキー */
            glScalef(1.0,1.0,1.0);
            zoom=0;
            xdistance=0.0;     /* 全て(回転・拡大・移動)リセット */
            ydistance=0.0;
            elevation=0.0;
            azimuth=0.0;
            break;
            
        default:
            break;
    }
}

void normalkeyb(unsigned char key, int x, int y)
{
    switch(key){
        case KEY_ESC:
      	     exit( 0 );
        default:
            break;
    }
}



void polarview( void )			/* ビューイング変換 */
{
    glScalef(1.0+zoom,1.0+zoom,1.0+zoom);       /* 拡大 */
    glTranslatef(-xdistance, 0.0, -ydistance);  /* 視点移動 */
    glRotatef( -elevation, 1.0, 0.0, 0.0);		/* x軸回転 物体を見下ろす角度（仰角）*/
    glRotatef( -azimuth, 0.0, 1.0, 0.0);		/* y軸回転 鉛直軸回りの（方位角）*/
}


void myInit(void)					/* ウィンドウの初期化 */
{
    static GLfloat lightpos[] = { 3.0, 4.0, 5.0, 1.0 }; /* 光源の位置 */
    
    static GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    static GLfloat specular[] = { 0.8, 0.8, 0.8, 1.0 };
    static GLfloat ambient[] = { 0.3, 0.3, 0.3, 1.0 };
    
    glutInitWindowPosition(100,100); /* ウィンドウの左上座標 */
    glutInitWindowSize(800,600);  /* ウィンドウサイズ */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("Pendulum Wave");
    glClearColor(0.2, 0.5, 0.6, 1.0);   /* ウィンドウの背景色 */
    //glClearColor(1.0, 1.0, 1.0, 0.0);		/* ウィンドウ白色 */
    
    glutMouseFunc(mouse);       /* マウスの割り込み指定 */
    glutSpecialFunc(keyboard);  /* キーボードの特殊キー登録 */
    glutKeyboardFunc(normalkeyb);    /* キーボードの一般キー登録 */
    glutMotionFunc(motion);
    
    glEnable(GL_NORMALIZE);	//重要
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);  /* 光源の設定 */
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    
    if(test_flg == false){
        pendulum = new Pendulum[NUM];
        for(int i = 0; i < NUM; i++){
            if(i == 0) ans = l;
            else ans = (66.0/(67-i-1)) * (66.0/(67-i-1)) * l;
            cl = !cl;
            pendulum[i].set(dt, m, ans, c, cl, i);
        }
        test_flg = true;
    }
    
}


void resize(int w, int h)			/* リシェープコールバック関数　サイズ変更の再描画用 */
{
    glViewport(0, 0, w, h);  			/* ウィンドウ全体をビューポートにする */
    glMatrixMode(GL_PROJECTION);  		/* 透視変換行列の指定 */
    glLoadIdentity();  				/* 透視変換行列の初期化 */
    gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);		/* ビューボリュームの再定義 */
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);	//重要
    glMatrixMode(GL_MODELVIEW);  			/* モデルビュー変換行列の指定 */
}


/*/////////////////////////////////////////////////////////////*/
/*
 /* 	図形の描写関連(H氏より提供)
 /*
 /*/////////////////////////////////////////////////////////////*/

//----------------------------------------------------------------------------
/* 位置、寸法、色指定して球体を描く関数 */
void T_Sphere(double x1, double y1, double z1, double x, double y, double z, int colr)
{
    glPushMatrix();
    glNormal3d(0.0, 1.0, 0.0);
    if(colr == 1) glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
    else          glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
    glMaterialfv(GL_FRONT, GL_AMBIENT, background_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, 128.0);
    glTranslated(x1, y1, z1);         //位置指定
    glScalef(x, y, z);                //寸法
    //  glutWireSphere(1.0,20,20);
    glutSolidSphere(1.0,20,20);
    glPopMatrix();
}

//----------------------------------------------------------------------------
void T_Cylinder(double radius, double height, int sides, int colr)	/* 色指定して円柱を描く関数 */
{
    double step = 6.28318530717958647692 / (double)sides;
    int i = 0;
    
    /* 材質特性を設定する colr=3,5を使用*/
    if (colr==1)
        glMaterialfv(GL_FRONT, GL_DIFFUSE, white_black);		/*　未使用 */
    if (colr==2)
        glMaterialfv(GL_FRONT, GL_DIFFUSE, red);		/*　未使用 */
    
				/*　関節部シルバー系色、環境光反射有り、鏡面反射有り */
    if (colr==3){
        glMaterialfv(GL_FRONT, GL_DIFFUSE, joint_diffuse);
        glMaterialfv(GL_FRONT, GL_AMBIENT, background_ambient);
        glMaterialf(GL_FRONT, GL_SHININESS, 20.0);
    }
				/*　手首部薄灰空色系色、鏡面光反射有り、環境光反射有り、鏡面反射有り */
    if(colr==5){
        glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, wristDiffuse );
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecular );
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, whiteAmbient );
        glMaterialf( GL_FRONT, GL_SHININESS, 128.0 );
    }
    
    glNormal3d(0.0, 1.0, 0.0);  				/* 上面描画 */
    glBegin(GL_TRIANGLE_FAN);  				/* 一辺共有の扇形三角形描画 */
    while (i < sides) {
        double t = step * (double)i++;
        glVertex3d(radius * sin(t), height, radius * cos(t));	/* 分割された頂点unko*/
    }
    glEnd();
    
    glNormal3d(0.0, -1.0, 0.0);  				/* 底面描画unko*/
    glBegin(GL_TRIANGLE_FAN); 				/* 一辺共有の扇形三角形描画unko */
    while (--i >= 0) {
        double t = step * (double)i;
        glVertex3d(radius * sin(t), -height, radius * cos(t));	/* 分割された頂点unko */
    }
    glEnd();
    
    glBegin(GL_QUAD_STRIP);  			/* 側面描画 一辺共有の四角形描画 */
    while (i <= sides) {
        double t = step * (double)i++;
        double x = sin(t);
        double z = cos(t);
        
        glNormal3d(x, 0.0, z);
        glVertex3f(radius * x, height, radius * z);		/* 分割された上部頂点 */
        glVertex3f(radius * x, -height, radius * z);	/* 分割された下部頂点 */
    }
    glEnd();
}
