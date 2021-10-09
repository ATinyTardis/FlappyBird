// FlappyBird.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <easyx.h>
#include <graphics.h>
#include<Windows.h>

using namespace std;

#define WIDTH 288
#define HEIGHT 512
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME)&0x8000)?1:0)

IMAGE bgImg[2],title,tutorial,text_ready,button_play,button_pause,button_resume,button_menu,button_ok,num[10],number_context[10],number_score[10],medals[4],text_game_over,score_panel,newImg;
int bgType, numPos[3][2] = { {20,20},{40,20},{60,20} } ,num_scorePos[3][2] = { {197,158},{217,158},{237,158} }, num_contextPos[3][2] = { {197,200},{217,200},{237,200} },maxScore=0;
unsigned long t,t1;

struct Land {
    int x, y;
    int vx;
    IMAGE img;
    Land(int x1, int y1, int vx1) {
        x = x1; y = y1; vx = vx1;
    }
};
struct Bird {
    float x, y;
    float vy;
    float g;
    int frame;
    int type;
    int size = 38;
    IMAGE img[3][3];
    Bird(int x1, int y1, int vy1, int g1,int frame1) {
        x1 = x; y = y1; vy = vy1; g = g1; frame = frame1;
    }
};
struct Pipe {
    int x1, y1;
    int x2, y2;
    int space;
    int vx;
    IMAGE img[2][2];
    Pipe(int x11, int y11, int x21, int y21, int vx1,int space1) {
        x1 = x11; x2 = x21; y1 = y11; y2 = y21; vx = vx1; space = space1;
    }
};



Land land(0, 420, -2);
Bird bird(20, 200, 0, 1, 0);
Pipe pipe(WIDTH,0 , WIDTH *3/2+26, 0,-2,150);

void drawAlpha( int  picture_x, int picture_y, IMAGE* picture) //x为载入图片的X坐标，y为Y坐标
{

    // 变量初始化
    DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
    DWORD* draw = GetImageBuffer();
    DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
    int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
    int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
    int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
    int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
    int dstX = 0;    //在显存里像素的角标

    // 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
    for (int iy = 0; iy < picture_height; iy++)
    {
        for (int ix = 0; ix < picture_width; ix++)
        {
            int srcX = ix + iy * picture_width; //在显存里像素的角标
            int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
            int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
            int sg = ((src[srcX] & 0xff00) >> 8);   //G
            int sb = src[srcX] & 0xff;              //B
            if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
            {
                dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
                int dr = ((dst[dstX] & 0xff0000) >> 16);
                int dg = ((dst[dstX] & 0xff00) >> 8);
                int db = dst[dstX] & 0xff;
                draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
                    | ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
                    | (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
            }
        }
    }
}

void GameInit() {

    initgraph(WIDTH, HEIGHT);
    loadimage(&bgImg[0], L"flappybird/bg_day.png");
    loadimage(&bgImg[1], L"flappybird/bg_night.png");

    loadimage(&land.img, L"flappybird/land.png");
    
    loadimage(&bird.img[0][0], L"flappybird/bird0_0.png");
    loadimage(&bird.img[0][1], L"flappybird/bird0_1.png");
    loadimage(&bird.img[0][2], L"flappybird/bird0_2.png");
    loadimage(&bird.img[1][0], L"flappybird/bird1_0.png");
    loadimage(&bird.img[1][1], L"flappybird/bird1_1.png");
    loadimage(&bird.img[1][2], L"flappybird/bird1_2.png");
    loadimage(&bird.img[2][0], L"flappybird/bird2_0.png");
    loadimage(&bird.img[2][1], L"flappybird/bird2_1.png");
    loadimage(&bird.img[2][2], L"flappybird/bird2_2.png");

    loadimage(&pipe.img[0][1], L"flappybird/pipe_up.png");
    loadimage(&pipe.img[0][0], L"flappybird/pipe_down.png");
    loadimage(&pipe.img[1][1], L"flappybird/pipe2_up.png");
    loadimage(&pipe.img[1][0], L"flappybird/pipe2_down.png");

    loadimage(&title, L"flappybird/title.png");
    loadimage(&tutorial, L"flappybird/tutorial.png");
    loadimage(&text_ready, L"flappybird/text_ready.png");
    loadimage(&button_play, L"flappybird/button_play.png");

    loadimage(&button_pause, L"flappybird/button_pause.png");
    loadimage(&button_resume, L"flappybird/button_resume.png");
    loadimage(&button_menu, L"flappybird/button_menu.png");
    loadimage(&button_ok, L"flappybird/button_ok.png");

    loadimage(&num[0], L"flappybird/font_048.png");
    loadimage(&num[1], L"flappybird/font_049.png");
    loadimage(&num[2], L"flappybird/font_050.png");
    loadimage(&num[3], L"flappybird/font_051.png");
    loadimage(&num[4], L"flappybird/font_052.png");
    loadimage(&num[5], L"flappybird/font_053.png");
    loadimage(&num[6], L"flappybird/font_054.png");
    loadimage(&num[7], L"flappybird/font_055.png");
    loadimage(&num[8], L"flappybird/font_056.png");
    loadimage(&num[9], L"flappybird/font_057.png");

    loadimage(&medals[0], L"flappybird/medals_0.png");
    loadimage(&medals[1], L"flappybird/medals_1.png");
    loadimage(&medals[2], L"flappybird/medals_2.png");
    loadimage(&medals[3], L"flappybird/medals_3.png");

    loadimage(&text_game_over, L"flappybird/text_game_over.png");
    loadimage(&score_panel, L"flappybird/score_panel.png");

    loadimage(&number_context[0], L"flappybird/number_context_00.png");
    loadimage(&number_context[1], L"flappybird/number_context_01.png");
    loadimage(&number_context[2], L"flappybird/number_context_02.png");
    loadimage(&number_context[3], L"flappybird/number_context_03.png");
    loadimage(&number_context[4], L"flappybird/number_context_04.png");
    loadimage(&number_context[5], L"flappybird/number_context_05.png");
    loadimage(&number_context[6], L"flappybird/number_context_06.png");
    loadimage(&number_context[7], L"flappybird/number_context_07.png");
    loadimage(&number_context[8], L"flappybird/number_context_08.png");
    loadimage(&number_context[9], L"flappybird/number_context_09.png");

    loadimage(&number_score[0], L"flappybird/number_score_00.png");
    loadimage(&number_score[1], L"flappybird/number_score_01.png");
    loadimage(&number_score[2], L"flappybird/number_score_02.png");
    loadimage(&number_score[3], L"flappybird/number_score_03.png");
    loadimage(&number_score[4], L"flappybird/number_score_04.png");
    loadimage(&number_score[5], L"flappybird/number_score_05.png");
    loadimage(&number_score[6], L"flappybird/number_score_06.png");
    loadimage(&number_score[7], L"flappybird/number_score_07.png");
    loadimage(&number_score[8], L"flappybird/number_score_08.png");
    loadimage(&number_score[9], L"flappybird/number_score_09.png");

    loadimage(&newImg, L"flappybird/new.png");

    pipe.y1 = min(-320, -pipe.space - 128) + rand() % (min(0, 192 - land.y - pipe.space) - max(-320, -pipe.space - 128));
    pipe.y2 = min(-320, -pipe.space - 128) + rand() % (min(0, 192 - land.y - pipe.space) - max(-320, -pipe.space - 128));

    t1 = GetTickCount();
    
    bird.type = rand() % 3;
    
}

void GameDraw(int score) {
    BeginBatchDraw();
    putimage(0, 0, &bgImg[bgType]);
    putimage(land.x, land.y, &land.img);
    
    drawAlpha(bird.x, bird.y, &bird.img[bird.type][bird.frame]);
    putimage(pipe.x1, pipe.y1, &pipe.img[0][0]);
    putimage(pipe.x1, pipe.y1+320+pipe.space, &pipe.img[0][1]);
    putimage(pipe.x2, pipe.y2, &pipe.img[1][0]);
    putimage(pipe.x2, pipe.y2 + 320 + pipe.space, &pipe.img[1][1]);

    drawAlpha(250, 20, &button_pause);

    if (score >= 100) {
        drawAlpha(numPos[0][0], numPos[0][1], &num[score / 100]);
        drawAlpha(numPos[1][0], numPos[1][1], &num[score / 10 % 10]);
        drawAlpha(numPos[2][0], numPos[2][1], &num[score % 100]);
    }
    else if (score >= 10) {
        drawAlpha(numPos[0][0], numPos[0][1], &num[score / 10]);
        drawAlpha(numPos[1][0], numPos[1][1], &num[score % 10]);
    }
    else {
        drawAlpha(numPos[1][0], numPos[1][1], &num[score]);
    }
    
    EndBatchDraw();
}

bool GameUpdate(int &score,int &pointer) {
    //ExMessage msg;
    HWND hwnd = GetForegroundWindow();
    POINT p; LPPOINT mp;

    if (KEY_DOWN(MOUSE_MOVED))
    {
        GetCursorPos(&p);
        ScreenToClient(hwnd, &p);
        mp = &p;
        if (mp->x >= 250 && mp->x <= 250 + 26 && mp->y >= 20 && mp->y <= 20 + 28) {
            drawAlpha(250, 20, &button_resume);
            while (1) {
                if (KEY_DOWN(MOUSE_MOVED)) {
                    GetCursorPos(&p);
                    ScreenToClient(hwnd, &p);
                    mp = &p;
                    if (mp->x >= 250 && mp->x <= 250 + 26 && mp->y >= 20 && mp->y <= 20 + 28) {
                        drawAlpha(250, 20, &button_pause);
                        break;
                    }
                }
            }

        }
        else bird.vy = -15;
    }
    

    t = GetTickCount();
    if (t - t1 >30) {
        int pipeX;
        if (pointer == 0)pipeX = pipe.x1 + 26;
        else pipeX= pipe.x2 + 26;

        if (bird.x > pipeX) {
            pointer = (pointer + 1) % 2;
            score++;
            if (score >= 100) {
                pipe.vx = -4;
                land.vx = -4;
                bird.g = 4;
                pipe.space = 130;
            }
            else if (score >= 10) {
                pipe.vx = -3;
                pipe.space = 140;
                land.vx = -3;
                bird.g = 2;
            }
            else {
                bird.g = 1;
            }
        }



        land.x += land.vx;
        if (land.x < -20)land.x = 0;

        pipe.x1 += pipe.vx;
        if (pipe.x1 < -52) {
            pipe.x1 = WIDTH;
            pipe.y1 = rand() % (land.y - 192) - 128-pipe.space;
            //pipe.y1 = -250 + rand() % 250;
        }

        pipe.x2 += pipe.vx;
        if (pipe.x2 < -52) {
            pipe.x2 = WIDTH;
            pipe.y2 = rand() % (land.y - 192) - 128 - pipe.space;

        }
        
        bird.vy += bird.g;
        if(bird.y+bird.size<=land.y&&!(bird.y<=0&&bird.vy<0))bird.y += 0.25 *  bird.vy;
        bird.frame = (bird.frame + 1) % 3;

        t1 = t;
    }

    if(((bird.x+bird.size>=pipe.x1&& bird.x + bird.size <= pipe.x1+52)||(bird.x +48- bird.size >= pipe.x1 && bird.x +48- bird.size <= pipe.x1 + 52))&& (bird.y +48-bird.size <= pipe.y1+320|| bird.y + bird.size >= pipe.y1+320+pipe.space)){
        return false;
        //MessageBox(GetForegroundWindow(), L"end1", L"",1);
    }
    else if(( (bird.x + bird.size >= pipe.x2 && bird.x + bird.size <= pipe.x2 + 52)||(bird.x +48- bird.size >= pipe.x2 && bird.x +48- bird.size <= pipe.x2 + 52)) && (bird.y + 48 - bird.size <= pipe.y2 + 320 || bird.y + bird.size>= pipe.y2 + 320 + pipe.space)) {
        return false;
        //MessageBox(GetForegroundWindow(), L"end2", L"", 1);
    }
    else if(bird.y + bird.size > land.y)return false;
    return true;
}

bool GameOn(int &score,int &pointer) {
    while (1) {
        GameDraw(score);
        if (!GameUpdate(score,pointer))return false;
    }
}

void GameStart() {
    bgType = rand() % 2;
    bird.type = rand() % 3;

    BeginBatchDraw();
    putimage(0, 0, &bgImg[bgType]);
    drawAlpha(55, 30, &title);//178*48
    drawAlpha(87, 108, &tutorial);//114*98 
    drawAlpha(46, 236, &text_ready);//196*62 78+98
    drawAlpha(86, 328, &button_play);//116*70

    EndBatchDraw();
    
    HWND hwnd = GetForegroundWindow();

    bool result = true; 
    while (result) {
        

        if (KEY_DOWN(MOUSE_MOVED)) {
            POINT point;
            //LPPOINT mpoint = nullptr;
            GetCursorPos(&point);
            ScreenToClient(hwnd, &point);
            LPPOINT mpoint = &point;
            if ((mpoint->x >= 86 && mpoint->x <= 86 + 116) && (mpoint->y >= 238 + 90 && mpoint->y <= 238 + 160)) {
                int score = 0,pointer=0;
                
                while(result){
                    if (!GameOn(score,pointer)) {
                        maxScore = max(maxScore, score);
                        BeginBatchDraw();
                        //medal 44*44, gameover 204*54, menu 80*28, ok 80*28, score_panel 238*126
                        drawAlpha(42, 50, &text_game_over);
                        drawAlpha(25, 124, &score_panel);
                        if (score <= 5)drawAlpha(57, 168, &medals[3]);
                        else if (score <= 10)drawAlpha(57, 168, &medals[2]);
                        else if (score <= 20)drawAlpha(57, 168, &medals[1]);
                        else drawAlpha(57, 168, &medals[0]);
                        drawAlpha(104, 280, &button_menu);
                        drawAlpha(104, 328, &button_ok);
                        if (score >= 100) {
                            drawAlpha(num_scorePos[0][0], num_scorePos[0][1], &number_score[score / 100]);
                            drawAlpha(num_scorePos[1][0], num_scorePos[1][1], &number_score[score / 10 % 10]);
                            drawAlpha(num_scorePos[2][0], num_scorePos[2][1], &number_score[score % 100]);
                        }
                        else if (score >= 10) {
                            drawAlpha(num_scorePos[0][0], num_scorePos[0][1], &number_score[score / 10]);
                            drawAlpha(num_scorePos[1][0], num_scorePos[1][1], &number_score[score % 10]);
                        }
                        else {
                            drawAlpha(num_scorePos[0][0], num_scorePos[0][1], &number_score[score]);
                        }
                        if (score >= 100) {
                            drawAlpha(num_contextPos[0][0], num_contextPos[0][1], &number_context[maxScore / 100]);
                            drawAlpha(num_contextPos[1][0], num_contextPos[1][1], &number_context[maxScore / 10 % 10]);
                            drawAlpha(num_contextPos[2][0], num_contextPos[2][1], &number_context[maxScore % 100]);
                        }
                        else if (score >= 10) {
                            drawAlpha(num_contextPos[0][0], num_contextPos[0][1], &number_context[maxScore / 10]);
                            drawAlpha(num_contextPos[1][0], num_contextPos[1][1], &number_context[maxScore % 10]);
                        }
                        else {
                            drawAlpha(num_contextPos[0][0], num_contextPos[0][1], &number_context[maxScore]);
                        }
                        if(maxScore==score)drawAlpha(num_contextPos[0][0]-30, num_contextPos[0][1], &newImg);
                        EndBatchDraw();
                        
                        HWND hwnd1 = GetForegroundWindow();
                        POINT point1;
                        while (1) {
                            if (KEY_DOWN(MOUSE_MOVED)) {
                                //LPPOINT mpoint = nullptr;
                                
                                hwnd1 = GetForegroundWindow();
                                GetCursorPos(&point1);
                                ScreenToClient(hwnd1, &point1);
                                LPPOINT mpoint = &point1;
                                if ((mpoint->x >= 104 && mpoint->x <= 104 + 80) && (mpoint->y >= 280 && mpoint->y <= 280 + 28)) {
                                    score = 0; pointer = 0;
                                    bird.x = 20; bird.y = 200; bird.vy = 0;
                                    pipe.x1 = WIDTH;pipe.y1 = rand() % (land.y - 192) - 128 - pipe.space; pipe.x2 = WIDTH * 3 / 2 + 26, pipe.y2 = rand() % (land.y - 192) - 128 - pipe.space;
                                    result = false; break;
                                }
                                else if ((mpoint->x >= 104 && mpoint->x <= 104 + 80) && (mpoint->y >= 328 && mpoint->y <= 328 + 28)) {
                                    score = 0; pointer = 0;
                                    bird.x = 20; bird.y = 200; bird.vy = 0;
                                    pipe.x1 = WIDTH; pipe.y1 = rand() % (land.y - 192) - 128 - pipe.space; pipe.x2 = WIDTH * 3 / 2 + 26, pipe.y2 = rand() % (land.y - 192) - 128 - pipe.space;
                                    bgType = rand() % 2;
                                    bird.type = rand() % 3;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }





}

int main()
{
    GameInit();
    while(1){GameStart();}
    
}