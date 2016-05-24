//
//  Pendulum.h
//  SystemDevelopment
//
//  Created by KentaYamagishi on 2016/01/24.
//  Copyright © 2016年 KentaYamagishi. All rights reserved.
//

//mainで本ヘッダーより先にGLUTが定義されない場合以下のコメントを解除
#include "GLUT/glut.h"
#define PI 3.141592653589793238 /*円周率*/

class Pendulum{
    
private:
    double _dt; //微分する時間
    double _m;  //振り子の質量
    double _l;  //振り子の長さ
    double _c;  //
    double _g;  //重力
    double x_k0[2], x_k1[2];
    bool _cl;
    int _dep;
    
public:
    Pendulum(){
        x_k0[0] = 0;
        x_k0[1] = 0;
        _dt = 0.01;
        _m = 1.0;
        _l = 0.5;
        _c = 0.5;
        _g = 9.8;
        _cl = true;
        _dep = 1.0;
    }
    
    void set(double dt, double m, double l, double c, bool cl, int dep){
        _dt = dt;
        _m = m;
        _l = l;
        _c = c;
        _cl = cl;
        _dep = dep;
    }
    
    void process(void){
        x_k1[0] = x_k0[0] + x_k0[1] * _dt;
        x_k1[1] = x_k0[1] + ( -_g/_l*sin(x_k0[0]) - _c/_m*x_k0[1] ) * _dt;
        x_k0[0] = x_k1[0];
        x_k0[1] = x_k1[1];
    }
    
    void mouse_init(double a, double b){
        x_k0[0] += a;
        //x_k0[1] += 0;
    }
    
    void mouse_flg(void){
        x_k0[1] = 0;
    }
    
    double draw(int mode){
        
        switch(mode){
            case 1 :
                return x_k0[0]*180/PI;
                break;
            case 2 :
                return _l;
            case 3 :
                return -0.1-(0.3 * _dep);
            case 4 :
                if(_cl) return 1.0;
                else    return 0.0;
        }
        return 0;
    }
    
};