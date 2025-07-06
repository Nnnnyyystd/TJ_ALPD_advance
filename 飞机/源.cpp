#include<vector>
#include <graphics.h>
#include <windows.h>
#include <cmath>
#include <iostream>
 #include <random>
#include<fstream>
#include<iomanip>
#include<string>
using namespace std;
const double PI = 3.14;
ofstream leftEngineFile;
ofstream rightEngineFile;
ofstream warningFile;
char V_buf[128], C_buf[128];

class Button
{
public:
    bool isValid;  // 按钮是否有效
    double begineTime;
    double nowTime;
    int x, y, width, height; // 按钮的位置和尺寸
    const char* text;        // 按钮显示的文本
    COLORREF button_color;   // 按钮颜色
    Button(int pos_x, int pos_y, int tmp_width, int tmp_height, const char* tmp_text, COLORREF tmp_color, bool Valid)
        : x(pos_x), y(pos_y), width(tmp_width), height(tmp_height), text(tmp_text), button_color(tmp_color), isValid(Valid) {}
    // 绘制按钮
    void drawButton()
    {
        setbkmode(TRANSPARENT); // 设置背景为透明
        // 如果按钮有效，使用指定的颜色；否则，使用黑色
        if (isValid) {
            //cout << "isValid:" << isValid << endl;
            setfillcolor(button_color); // 有效按钮的颜色
        }
        else {
           // cout << "notValid:" << isValid << endl;
            setfillcolor(BLUE); // 无效按钮的颜色（黑色）
        }

        setlinecolor(BLACK);  // 设置边框颜色为黑色
        fillrectangle(x, y, x + width, y + height);  // 填充按钮矩形

        // 设置文字样式和颜色
        settextstyle(25, 0, "Arial");
        settextcolor(WHITE);  // 文字颜色为白色

        // 计算文本居中位置
        int textX = x + (width - textwidth(text)) / 2;
        int textY = y + (height - textheight(text)) / 2;

        // 绘制文本
        outtextxy(textX, textY, text);
    }
    // 判断按钮是否被点击
    bool isClicked(ExMessage& msg) {
        return msg.x > x && msg.x < x + width && msg.y > y && msg.y < y + height && msg.message == WM_LBUTTONDOWN ;
    }
};

vector<Button> buttons;

class N_sensor
{
public:
    double N_speed;
    double N0;
    bool is_working;
};
class T_sensor
{
public:
    double T_temp;
    double T0;
    bool is_working;
};
class C_V_sensor
{
public:
    bool isFailed_LowFuel, isFailed_FuelS ,isOverFFFail ;
    double C;
    double V;
    bool is_working;
    void draw_C_and_V()
    {
        if (isFailed_FuelS)settextcolor(RED);
        else if (isOverFFFail)settextcolor(RGB(234, 147, 010));
        else settextcolor(WHITE);
        
        sprintf_s(V_buf, "Fuel Flow: %d L/s", int(V));
        settextstyle(30, 0, "Arial");

        if (isFailed_FuelS)   outtextxy(405, 100, "Fule Flow: -- L/s");
        else outtextxy(405, 100, V_buf);


        if (isFailed_FuelS)settextcolor(RED);
        else if (isFailed_LowFuel)settextcolor(RGB(234, 147, 010));
        else settextcolor(WHITE);
       
        sprintf_s(C_buf, "Fuel: %d L", int(C));


        if (isFailed_FuelS)   outtextxy(405, 80, "Fule: -- L");
        else  outtextxy(405, 80, C_buf);
       

    }
};
class Warning
{
public:
    int t_x, t_y,level;
    const char* name;
    double begine_time, now_time;
    bool islogged = false;
    int isShow = 1;
    Warning(int x, int y, const char* w_name,int w_level,double w_begin_time=0)
    {
        t_x = x;  t_y = y;  name = w_name; level = w_level;
    }
    void draw_warning()
    {
        if (level == 0)settextcolor(WHITE);
        else if (level == 1)settextcolor(RGB(234, 147, 010));
        else if(level==2)settextcolor(RED);
        settextstyle(23, 0, "Arial");
        outtextxy(t_x, t_y, name);
    }
    void record_warning(double currentTime)
    {
        // 打开文件并追加写入
        warningFile.open("warnings.log", std::ios::app);
        if (warningFile.is_open()) {
            warningFile << "Time: " << currentTime << "s, " << "Warning: " << name << std::endl;
            warningFile.close();  // 关闭文件
        }
        else {
            cerr << "Unable to open file for writing warnings!" << std::endl;
        }
    }
};
class Engine {
private:
    double t; // 全局时间
public:
    N_sensor N_sensor1, N_sensor2;
    T_sensor T_sensor1, T_sensor2;
    double N0, T0, V0;// 停止、平稳时的值
    double N, T, C, V;// 转速、温度、燃料
    double t0;
    double engineTime;
    bool isRunning, isStopping, isStable; // 平稳状态标志，其实没用上，懒得删了
    //异常情况标志
    bool isFailed_N1S1, isFailed_N1S2, isFailed_EGTS1, isFailed_EGTS2
        , isFailed_N1S, isFailed_EGTS,
        isFailed_OverSpd1, isFailed_OverSpd1_tmp,//用于改变N0的值
        isFailed_OverSpd2, isFailed_OverSpd2_tmp,//用于改变N0的值
        isFailed_OverFF,
        isFailed_OverTemp1, isFailed_OverTemp1_tmp,
        isFailed_OverTemp2, isFailed_OverTemp2_tmp,
        isFailed_OverTemp3, isFailed_OverTemp3_tmp,
        isFailed_OverTemp4, isFailed_OverTemp4_tmp; // 故障标志
    int state;
    Engine() : t(0), engineTime(0), N(0), T(20), C(20000), N0(0), T0(20), isRunning(false), isStopping(false), isStable(false) {}
    void calculateParameters
    (double t_, double t0 = 0,
     double randFactorN = 1.0, double randFactorT = 1.0, double randV = 1.0
    )
    {
        if (state == 1)
        { // 启动逻辑
            if (t_ >= 0 && t_ <= 2) 
            {
                //if()
                N_sensor1.N_speed = 10000 * t_;
                N_sensor2.N_speed = 10000 * t_;
                T_sensor1.T_temp = 20;
                T_sensor2.T_temp = 20;
                
                if (isFailed_OverTemp1_tmp)
                {
                    T_sensor1.T_temp = 900;
                    T_sensor2.T_temp = 900;
                }

                if (isFailed_OverTemp2_tmp)
                {
                    T_sensor1.T_temp = 1005;
                    T_sensor2.T_temp = 1005;
                }


                N = (N_sensor1.N_speed + N_sensor2.N_speed) / 2;
                T = (T_sensor1.T_temp + T_sensor2.T_temp) / 2;         
                V = 5 * t_;
            }
            else
            {
                N_sensor1.N_speed = 23000 * log10(t_ - 1) + 20000;
                N_sensor2.N_speed = 23000 * log10(t_ - 1) + 20000;
                T_sensor1.T_temp = 900 * log10(t_ - 1) + 20;
                T_sensor2.T_temp = 900 * log10(t_ - 1) + 20;
                if (isFailed_OverTemp1_tmp)
                {
                    T_sensor1.T_temp = 900;
                    T_sensor2.T_temp = 900;
                }
                if (isFailed_OverTemp2_tmp)
                {
                    T_sensor1.T_temp = 1005;
                    T_sensor2.T_temp = 1005;
                }
                N = (N_sensor1.N_speed + N_sensor2.N_speed) / 2;
                T = (T_sensor1.T_temp + T_sensor2.T_temp) / 2;
                V = 42 * log10(t_ - 1) + 10;
                if (N >= 38000)
                {
                    N_sensor1.N0 = N;
                    N_sensor2.N0 = N;
                    T_sensor1.T0 = T;
                    T_sensor2.T0 = T;
                    N0 = (N_sensor1.N0 + N_sensor2.N0) / 2;
                    T0 = (T_sensor1.T0 +T_sensor2.T0) / 2;
                    V0 = V;
                    cout << "V0:" << V0 << endl;
                    cout << "T0:" << T0 << endl;
                    cout << "N0:" << N0 << endl;
                    isStable = true;
                    isRunning = false;
                }
            }
        }
        else if (state == 2 )//||state==6)
        { 
            // 停止逻辑
            V = 0;
            double b = 1 - t0;
            double A = N0 / log10(7);
            double B = (T0 - 20) / log10(7);
            N_sensor1.N_speed = (int(N) > 0) ? -A * log10(t_ + b) + N0 : 0;
            N_sensor2.N_speed = (int(N) > 0) ? -A * log10(t_ + b) + N0 : 0;
            T_sensor1.T_temp = (int(T) > 20) ? -B * log10(t_ + b) + T0 : 20;
            T_sensor2.T_temp = (int(T) > 20) ? -B * log10(t_ + b) + T0 : 20;
            N = (N_sensor1.N_speed + N_sensor2.N_speed) / 2;
            T = (T_sensor1.T_temp + T_sensor2.T_temp) / 2;
        }
        else if (state == 3)
        {
            if (isFailed_OverSpd1_tmp)
            {
                N0 = 42000;
                isFailed_OverSpd1_tmp = false;
            }   
            if (isFailed_OverSpd2_tmp)
            {
                N0 = 48000;
                isFailed_OverSpd2_tmp = false;
            }
            if (isFailed_OverTemp3_tmp)
            {
                T0 = 955;
                isFailed_OverTemp3_tmp = false;
            }
            if (isFailed_OverTemp4_tmp)
            {
                T0 = 1105;
                isFailed_OverTemp4_tmp = false;
            }
            N_sensor1.N_speed = N0 * randFactorN;
            N_sensor2.N_speed = N0 * randFactorN;
            T_sensor1.T_temp = T0 * randFactorT;
            T_sensor2.T_temp = T0 * randFactorT;
            N = (N_sensor1.N_speed + N_sensor2.N_speed) / 2;
            T = (T_sensor1.T_temp + T_sensor2.T_temp) / 2;
            if (isFailed_OverFF)
            {
                V0 = 50;
                isFailed_OverFF = false;
            }  
            V = randV;
        }
        else if (state == 4)
        {
            V += 1;
            N0 = N;
            T0 = T;
            V0 = V;
        }
        else if (state == 5)
        {
            V -= 1; N0 = N;
            T0 = T; V0 = V;
        }
    }
    // 绘制单个表盘函数
    void drawN_Dial(int X, int Y)
    {
        double N_Angle = 7 * PI * N / (38000 * 6) + PI / 180;
        if (isFailed_N1S2)N_Angle = PI / 180;// 单发转速传感器故障（无效值）：转速告警，其他正常工作，琥珀色警告,
        int r = 70;   // 表盘半径
        setlinecolor(WHITE);
       
         if (isFailed_N1S||isFailed_OverSpd2)setfillcolor(RED);
        else if (isFailed_N1S2 ||isFailed_OverSpd1)setfillcolor(RGB(234, 147, 010));
        else setfillcolor(LIGHTGRAY);
        // 绘制扇形，模拟表盘指针
        fillpie(X - r, Y - r, X + r, Y + r, 0, N_Angle);
        // 绘制表盘边框
        arc(X - r - 5, Y - r - 5, X + r + 5, Y + r + 5, 0, 14 * PI / 9);
        rectangle(X + 20, Y, X + r + 20, Y + 20);
        char N_buf[128];

        if (isFailed_N1S||isFailed_OverSpd2)settextcolor(RED);
        else if (isFailed_N1S2||isFailed_OverSpd1)
        {
            settextcolor(RGB(234, 147, 010));

        }
        else settextcolor(WHITE);

        settextstyle(20, 0, "Arial");

        sprintf_s(N_buf, " %d N1\n", int(N / 400));


        if(isFailed_N1S2||isFailed_N1S) outtextxy(X + 20, Y, " --N1");
        else outtextxy(X + 20, Y, N_buf);

    }
    void drawT_Dial(int X, int Y)
    {
        //double N_Angle = 7 * PI * N / (38000 * 6) + PI / 180;
        double T_Angle = 7 * PI * T / (724 * 6);
        if (isFailed_EGTS2)T_Angle = PI / 180;// 单发转速传感器故障（无效值）：转速告警，其他正常工作，琥珀色警告,
        int r = 70;   // 表盘半径

        if (isFailed_EGTS||isFailed_OverTemp2 || isFailed_OverTemp4)setfillcolor(RED);
        else if (isFailed_EGTS2||isFailed_OverTemp1 || isFailed_OverTemp3) setfillcolor(RGB(234, 147, 010));
        else setfillcolor(LIGHTGRAY);

        fillpie(X - r, Y - r + 200, X + r, Y + r + 200, 0, T_Angle);
        arc(X - r - 5, Y - r + 200 - 5, X + r + 5, Y + r + 200 + 5, 0, 14 * PI / 9);
        rectangle(X + 20, Y + 200, X + r + 20, Y + 200 + 20);
     
        if (isFailed_EGTS||isFailed_OverTemp2||isFailed_OverTemp4)settextcolor(RED);
        else if (isFailed_EGTS2||isFailed_OverTemp1||isFailed_OverTemp3)settextcolor(RGB(234, 147, 010));
        else settextcolor(WHITE);


        char T_buf[128];
        settextstyle(20, 0, "Arial");
        sprintf_s(T_buf, " %d°C\n", int(T));

        if (isFailed_EGTS2||isFailed_EGTS) outtextxy(X + 20, Y+200, " --°C");
        else outtextxy(X + 20, Y+200, T_buf);
    }
};
class Simulation
{
public:
    double t;                 // 全局时间
    double engineTime;        // 当前启动过程时间
    Engine lengine;
    Engine rengine;
    C_V_sensor C_V_sensor0;
    bool isRunning, isStopping, isStable, push, pull; // 平稳状态标志，是否加速
    double C = 20000;
    bool isRecording;  // 控制文件记录
    bool isN1S1Fail, isN1S2Fail, isEGTS1Fail, isEGTS2Fail,
        isN1SFail, isEGTSFail,
        isLowFuelFail, isFuelSFail,isOverFFFail
        , isOverSpd1Fail, isOverSpd2Fail,
        isOverTemp1Fail, isOverTemp2Fail, isOverTemp3Fail, isOverTemp4Fail;
    vector<Warning> warnings;
    void startEngine()
    {
        if (!isRunning && !isStopping && !isStable)
        {
            engineTime = 0;
            isRunning = true;
            isRecording = true;
            lengine.N_sensor1.is_working = true; lengine.N_sensor2.is_working = true;
            rengine.N_sensor1.is_working = true; rengine.N_sensor2.is_working = true;

            lengine.T_sensor1.is_working = true; lengine.T_sensor2.is_working = true;
            rengine.T_sensor1.is_working = true; rengine.T_sensor2.is_working = true;
            // 打开文件，清空之前的记录
            leftEngineFile.open("left_engine_data.txt", std::ios::out | std::ios::trunc);
            rightEngineFile.open("right_engine_data.txt", std::ios::out | std::ios::trunc);
            warningFile.open("warnings.log", std::ios::out | std::ios::trunc);
            warningFile.close();
            // 写入文件头信息
            leftEngineFile << "T(s)    N_sensor1_N1(%)      N_sensor2_N1(%)      T_sensor1_T(°C)       T_sensor2_T(°C)    V(L/s)    C(L)\n";
            rightEngineFile <<"T(s)    N_sensor1_N1(%)      N_sensor2_N1(%)      T_sensor1_T(°C)       T_sensor2_T(°C)    V(L/s)    C(L)\n";
        }
    }
    void stopEngine()
    {
        if (isRunning || isStable)
        {
            lengine.N0 = lengine.N;    rengine.N0 = rengine.N;
            lengine.T0 = lengine.T;    rengine.T0 = rengine.T;

            lengine.t0 = engineTime;
            rengine.t0 = engineTime;

            isStopping = true;
            isRunning = false;
            isStable = false;
        }
    }
    void create_N1S1Fail();
    void create_N1S2Fail();
    void create_EGTS1Fail();//多次创建EGTS1故障有可能导致EGTS2故障发生
    void create_EGTS2Fail();
    void create_N1SFail();//N1S故障使得所有的N传感器都失灵，因此上述故障都会产生
    void create_EGTSFail();//
    void create_LowFuelFail();
    void create_FuelSFail();
    void create_OverFFFail();
    void create_OverSpd1();
    void create_OverSpd2();
    void create_OverTemp1();
    void create_OverTemp2();
    void create_OverTemp3();
    void create_OverTemp4();
    void Show_fail();
    void Check_fail();
    void start_Warning(int Case);
    void draw_buttons(vector<Button>& buttons);
    void record_info()
    {
        // 计算转速与额定转速比率、记录数据
        double lN_ratio1 = 100.0 * lengine.N_sensor1.N_speed / 40000;
        double lN_ratio2 = 100.0 * lengine.N_sensor2.N_speed / 40000;
        double rN_ratio1 = 100.0 * rengine.N_sensor1.N_speed / 40000;
        double rN_ratio2 = 100.0 * rengine.N_sensor2.N_speed / 40000;
        double lengine_T_sensor1_T = lengine.T_sensor1.T_temp;
        double lengine_T_sensor2_T = lengine.T_sensor2.T_temp;
        double rengine_T_sensor1_T = rengine.T_sensor1.T_temp;
        double rengine_T_sensor2_T = rengine.T_sensor2.T_temp;
        double V_value  = C_V_sensor0.V;
        string lN_info1 = to_string(int(lN_ratio1));
        string lN_info2 = to_string(int(lN_ratio2));
        string rN_info1 = to_string(int(rN_ratio1));
        string rN_info2 = to_string(int(rN_ratio2));
        string lT_info1 = to_string(int(lengine_T_sensor1_T));
        string lT_info2 = to_string(int(lengine_T_sensor2_T));
        string rT_info1 = to_string(int(rengine_T_sensor1_T));
        string rT_info2 = to_string(int(rengine_T_sensor2_T));
        string V_info = to_string(V_value);
        if (!lengine.N_sensor1.is_working)
        {
            lN_info1 = "--";
        }
        if (!lengine.N_sensor2.is_working)
        {
            lN_info2 = "--";
        }
        if (!rengine.N_sensor1.is_working)
        {
            rN_info1 = "--";
        }
        if (!rengine.N_sensor2.is_working)
        {
            rN_info2 = "--";
        }
        if (!lengine.T_sensor1.is_working)
        {
            lT_info1 = "--";
        }
        if (!lengine.T_sensor2.is_working)
        {
            lT_info2 = "--";
        }
        if (!rengine.T_sensor1.is_working)
        {
            rT_info1 = "--";
        }
        if (!rengine.T_sensor2.is_working)
        {
            rT_info2 = "--";
        }
        leftEngineFile 
            << fixed << setprecision(4) << engineTime << ",          "
            << lN_info1 << ",                  "
            << lN_info2 << ",                  "
            << lT_info1 <<",                   "
            << lT_info2 << ",                  "
            << fixed << setprecision(3) <<V_info<< ",          "
            <<int(C)<<"\n";
        rightEngineFile
            << fixed << setprecision(4) << engineTime << ",          "
            << rN_info1 << ",                  "
            << rN_info2 << ",                  "
            << rT_info1 << ",                   "
            << rT_info2 << ",                  "
            << fixed << setprecision(3) << V_info << ",          "
            << int(C) << "\n";

    }
    void set_State()
    {
        C = 20000;//每一次模拟燃油油箱都设为满的
        for (int i = 0; i < buttons.size(); i++)
        {
            buttons[i].isValid = true;
        }

        while (warnings.size())
        {
            warnings.pop_back();
        }
        Warning N1S1Fail_1(15, 625, "Left Engine N_sensor1 Fail", 0, engineTime); warnings.push_back(N1S1Fail_1);//0
        Warning N1S1Fail_2(15, 650, "Left Engine N_sensor2 Fail", 0, engineTime);   warnings.push_back(N1S1Fail_2);//1
        Warning N1S1Fail_3(15, 675, "Right Engine N_sensor1 Fail", 0, engineTime); warnings.push_back(N1S1Fail_3);//2
        Warning N1S1Fail_4(15, 700, "Right Engine N_sensor2 Fail", 0, engineTime); warnings.push_back(N1S1Fail_4);//3

        Warning N1S2Fail_1(15, 725, "Left Engine N_sensor Fail", 1, engineTime);  warnings.push_back(N1S2Fail_1);//4
        Warning N1S2Fail_2(15, 750, "Right Engine N_sensor Fail", 1, engineTime); warnings.push_back(N1S2Fail_2);//5

        Warning EGTS1Fail_1(15, 775, "Left Engine T_sensor1 Fail", 0, engineTime); warnings.push_back(EGTS1Fail_1);//6
        Warning EGTS1Fail_2(15, 800, "Left Engine T_sensor2 Fail", 0, engineTime);   warnings.push_back(EGTS1Fail_2);//7
        Warning EGTS1Fail_3(15, 825, "Right Engine T_sensor1 Fail", 0, engineTime);  warnings.push_back(EGTS1Fail_3);//8
        Warning EGTS1Fail_4(15, 850, "Right Engine T_sensor2 Fail", 0, engineTime); warnings.push_back(EGTS1Fail_4);//9

        Warning EGTS2Fail_1(15, 875, "Left Engine T_sensor Fail", 1, engineTime);         warnings.push_back(EGTS2Fail_1);//10
        Warning EGTS2Fail_2(15, 900, "Right Engine T_sensor Fail", 1, engineTime);    warnings.push_back(EGTS2Fail_2);//11

        Warning N1SFail(15, 925, "Both Engine N_sensor Fail", 2, engineTime);         warnings.push_back(N1SFail);//12
        Warning EGTSFail(260, 625, "Both Engine T_sensor Fail", 2, engineTime);    warnings.push_back(EGTSFail);//13

        Warning LowFuelFail(260, 650,"Low Fuel", 1, engineTime);  warnings.push_back(LowFuelFail);//14
        Warning FuelSFail(260, 675, "Fuel C_sensor Fail", 2, engineTime);  warnings.push_back(FuelSFail);//15
        Warning OverFFFail(260, 700, "Over Fuel Flow Fail", 1, engineTime);  warnings.push_back(OverFFFail);//16
        Warning OverSpd1(260, 725, "Over Speed N1>105", 1, engineTime);  warnings.push_back(OverSpd1);//17
        Warning OverSpd2(260, 750, "Over Speed N1>120", 2, engineTime);  warnings.push_back(OverSpd2);//18

        Warning OverTemp1(260, 775, "Over Temp During Start Temp>850", 1, engineTime);  warnings.push_back(OverTemp1);//19
        Warning OverTemp2(260, 800, "Over Temp During Start Temp>1000", 2, engineTime);  warnings.push_back(OverTemp2);//20
        Warning OverTemp3(260, 825, "Over Temp During Stable Temp>850", 1, engineTime);  warnings.push_back(OverTemp3);//21
        Warning OverTemp4(260, 850, "Over Temp During Stable Temp>1200", 2, engineTime);  warnings.push_back(OverTemp4);//22

        lengine.isFailed_N1S2 = false; rengine.isFailed_N1S2 = false;     lengine.isFailed_N1S1 = false; rengine.isFailed_N1S1 = false;
        lengine.isFailed_EGTS2 = false; rengine.isFailed_EGTS2 = false;     lengine.isFailed_EGTS1 = false; rengine.isFailed_EGTS1 = false;
        lengine.isFailed_N1S = false; rengine.isFailed_N1S = false;    lengine.isFailed_EGTS = false; rengine.isFailed_EGTS = false;
        lengine.isFailed_OverSpd1 = false; rengine.isFailed_OverSpd1=false; lengine.isFailed_OverSpd2 = false; rengine.isFailed_OverSpd2 = false;
        lengine.isFailed_OverFF = false; rengine.isFailed_OverFF = false; lengine.isFailed_OverTemp1 = false; rengine.isFailed_OverTemp1 = false;
        lengine.isFailed_OverTemp2 = false; rengine.isFailed_OverTemp2 = false; lengine.isFailed_OverTemp3 = false; rengine.isFailed_OverTemp3 = false;
        lengine.isFailed_OverTemp4 = false; rengine.isFailed_OverTemp4 = false;
        lengine.isFailed_OverTemp1_tmp = false;        rengine.isFailed_OverTemp1_tmp = false;
        lengine.isFailed_OverTemp2_tmp = false;        rengine.isFailed_OverTemp2_tmp = false;
        lengine.isFailed_OverTemp3_tmp = false;        rengine.isFailed_OverTemp3_tmp = false;
        lengine.isFailed_OverTemp4_tmp = false;        rengine.isFailed_OverTemp4_tmp = false;



        C_V_sensor0.isFailed_FuelS = false; C_V_sensor0.isFailed_LowFuel = false; C_V_sensor0.isOverFFFail = false; C_V_sensor0.is_working = 1;



        lengine.N_sensor1.is_working = 1; lengine.N_sensor2.is_working = 1; rengine.N_sensor1.is_working = 1; rengine.N_sensor2.is_working = 1;
        lengine.T_sensor1.is_working = 1; lengine.T_sensor2.is_working = 1; rengine.T_sensor1.is_working = 1; rengine.T_sensor2.is_working = 1;

    }
    void runSimulation()
    {
        Button startButton(400, 250, 80, 50, "Start", GREEN,1); buttons.push_back(startButton);// 0
        Button pushButton(400, 320, 80, 50, "Push", LIGHTBLUE,1);  buttons.push_back(pushButton);
        Button stopButton(500, 250, 80, 50, "Stop", RED,1); buttons.push_back(stopButton);
        Button pullButton(500, 320, 80, 50, "Pull", LIGHTBLUE,1);  buttons.push_back(pullButton);

        Button N1S1FailButton(10, 400, 120, 40, "N1S1Fail",RGB(137, 207, 240) ,1);  buttons.push_back(N1S1FailButton);//4
        Button N1S2FailButton(160, 400, 120, 40, "N1S2Fail", RGB(137, 207, 240),1);  buttons.push_back(N1S2FailButton);//5
        Button EGTS1FailButton(310, 400, 120, 40, "EGTS1Fail", RGB(137, 207, 240),1);  buttons.push_back(EGTS1FailButton);//6
        Button EGTS2FailButton(460, 400, 120, 40, "EGTS2Fail", RGB(137, 207, 240),1);  buttons.push_back(EGTS2FailButton);//7

        Button N1SFailButton(10, 460, 120, 40, "N1SFail", RGB(137, 207, 240),1);  buttons.push_back(N1SFailButton);//8
        Button EGTSFailButton(160, 460, 120, 40, "EGTSFail", RGB(137, 207, 240),1);  buttons.push_back(EGTSFailButton);//9
        Button LowFuelButton(310, 460, 120, 40, "LowFuel", RGB(137, 207, 240),1);  buttons.push_back(LowFuelButton);//10
        Button FuelSFailButton(460, 460, 120, 40, "FuelSFail", RGB(137, 207, 240),1);  buttons.push_back(FuelSFailButton);//11

        Button OverSpd1Button(10, 520, 120, 40, "OverSpd1", RGB(137, 207, 240),1);  buttons.push_back(OverSpd1Button);//12
        Button OverSpd2Button(160, 520, 120, 40, "OverSpd2", RGB(137, 207, 240),1);  buttons.push_back(OverSpd2Button);//13
        Button OverFFButton(310, 520, 120, 40, "OverFF", RGB(137, 207, 240),1);  buttons.push_back(OverFFButton);//14

        Button OverTemp1Button(10, 580, 120, 40, "OverTemp1", RGB(137, 207, 240),1);  buttons.push_back(OverTemp1Button);//15
        Button OverTemp2Button(160, 580, 120, 40, "OverTemp2", RGB(137, 207, 240),1);  buttons.push_back(OverTemp2Button);//16
        Button OverTemp3Button(310, 580, 120, 40, "OverTemp3", RGB(137, 207, 240),1);  buttons.push_back(OverTemp3Button);//17
        Button OverTemp4Button(460, 580, 120, 40, "OverTemp4", RGB(137, 207, 240),1);  buttons.push_back(OverTemp4Button);//18

       

    
        push = false;   pull = false; isRecording = false;

        //chatgpt给出的方法：
        LARGE_INTEGER frequency, lastTime, currentTime;
        QueryPerformanceFrequency(&frequency);  // 获取计时器频率
        QueryPerformanceCounter(&lastTime);     // 记录初始时间
        double accumulatedTime = 0;             // 累积的时间
        //更新状态，在开头状态必须加（否则进入while会产生异常）
        set_State();
        cout << "size: " << warnings.size() << endl;
        while (true)
        {
            ExMessage msg;
            //获取鼠标信息，各种按钮
            if (peekmessage(&msg, EM_MOUSE))
            {
                if (startButton.isClicked(msg)) startEngine();
                if (stopButton.isClicked(msg)) stopEngine();
                if (pushButton.isClicked(msg)) push = true;
                if (pullButton.isClicked(msg)) pull = true;
                if (N1S1FailButton.isClicked(msg) && (isRunning || isStable || isStopping) && (buttons[4].isValid == true))
                {
                    isN1S1Fail = true; create_N1S1Fail();
                    buttons[4].isValid = false; buttons[4].begineTime = engineTime;
                    buttons[5].isValid = false; buttons[5].begineTime = engineTime;
                }
                if (N1S2FailButton.isClicked(msg)&&(isRunning||isStable||isStopping)  &&(buttons[5].isValid == true) )
                {
                    isN1S2Fail = true; create_N1S2Fail();
                    buttons[5].isValid = false;   buttons[5].begineTime = engineTime;
                    buttons[4].isValid = false;   buttons[4].begineTime = engineTime;
                }

                if (EGTS1FailButton.isClicked(msg) && (isRunning || isStable || isStopping) && (buttons[6].isValid == true))
                {
                    isEGTS1Fail = true; create_EGTS1Fail();
                    buttons[6].isValid = false;   buttons[6].begineTime = engineTime;
                    buttons[7].isValid = false;   buttons[7].begineTime = engineTime;
                }
                if (EGTS2FailButton.isClicked(msg) && (isRunning || isStable || isStopping) && (buttons[7].isValid == true))
                {
                    isEGTS2Fail = true; create_EGTS2Fail();
                    buttons[6].isValid = false;   buttons[6].begineTime = engineTime;
                    buttons[7].isValid = false;   buttons[7].begineTime = engineTime;
                }
                if (N1SFailButton.isClicked(msg) && (isRunning || isStable || isStopping) && (buttons[8].isValid == true))
                {
                    isN1SFail = true; create_N1SFail();
                    buttons[8].isValid = false; buttons[8].begineTime = engineTime;
                    buttons[5].isValid = false;   buttons[5].begineTime = engineTime;
                    buttons[4].isValid = false;   buttons[4].begineTime = engineTime;
                }
                if (EGTSFailButton.isClicked(msg) && (isRunning || isStable || isStopping) && (buttons[9].isValid == true))
                {
                    isEGTSFail = true; create_EGTSFail();
                    buttons[9].isValid = false; buttons[9].begineTime = engineTime;
                    buttons[6].isValid = false;   buttons[6].begineTime = engineTime;
                    buttons[7].isValid = false;   buttons[7].begineTime = engineTime;
                }

                if (LowFuelButton.isClicked(msg) && (isRunning || isStable || isStopping) && (buttons[10].isValid == true))
                {
                    isLowFuelFail = true; create_LowFuelFail();
                    buttons[10].isValid = false; buttons[10].begineTime = engineTime;
                }

                if (FuelSFailButton.isClicked(msg) && (isRunning || isStable || isStopping) && (buttons[11].isValid == true))
                {
                    isFuelSFail = true; create_FuelSFail();
                    buttons[11].isValid = false; buttons[11].begineTime = engineTime;
                }

                if (OverFFButton.isClicked(msg) && (isStable) && (buttons[14].isValid == true))
                {
                    isOverFFFail = true; create_OverFFFail();
                    buttons[14].isValid = false; buttons[14].begineTime = engineTime;
                }

                if (OverSpd1Button.isClicked(msg) && (isStable) && (buttons[12].isValid == true))
                {
                    isOverSpd1Fail = true; create_OverSpd1();
                    buttons[12].isValid = false; buttons[12].begineTime = engineTime;
                }
                if (OverSpd2Button.isClicked(msg) && (isStable) && (buttons[13].isValid == true))
                {
                    isOverSpd2Fail = true; create_OverSpd2();
                    buttons[13].isValid = false; buttons[13].begineTime = engineTime;
                }

                if (OverTemp1Button.isClicked(msg) && isRunning && buttons[15].isValid == true)
                {
                    isOverTemp1Fail = true; create_OverTemp1();
                    buttons[15].isValid = false; buttons[15].begineTime = engineTime;
                }

                if (OverTemp2Button.isClicked(msg) && isRunning && buttons[16].isValid == true)
                {
                    isOverTemp2Fail = true; create_OverTemp2();
                    buttons[16].isValid = false; buttons[16].begineTime = engineTime;
                }

                if (OverTemp3Button.isClicked(msg) && isStable && buttons[17].isValid == true)
                {
                    isOverTemp3Fail = true; create_OverTemp3();
                    buttons[17].isValid = false; buttons[17].begineTime = engineTime;
                }
                if (OverTemp4Button.isClicked(msg) && isStable && buttons[18].isValid == true)
                {
                    isOverTemp4Fail = true; create_OverTemp4();
                    buttons[18].isValid = false; buttons[18].begineTime = engineTime;
                }

            }
            QueryPerformanceCounter(&currentTime);  // 获取当前时间
            double deltaTime = (currentTime.QuadPart - lastTime.QuadPart)
                * 1000.0 / frequency.QuadPart;
            //计算从上次调用 QueryPerformanceCounter 以来经过的时间（以毫秒为单位）

            accumulatedTime += deltaTime;  // 累积时间
            lastTime = currentTime;        // 更新 lastTime
            //在省略部分的最后将accumulatedTime更新为0
            if (accumulatedTime >= 5.0)
            {
                t += accumulatedTime / 1000.0;
                C -= (lengine.V * accumulatedTime / 1000.0 + rengine.V * accumulatedTime / 1000.0);
                C_V_sensor0.C = C;
                C_V_sensor0.V = (lengine.V + rengine.V) / 2;
                Check_fail();
                if (isRunning)
                {
                    engineTime += accumulatedTime / 1000.0;
                    lengine.state = 1;
                    rengine.state = 1;
                    lengine.engineTime = engineTime;
                    rengine.engineTime = engineTime;

                    lengine.calculateParameters(engineTime);
                    rengine.calculateParameters(engineTime);

                    if ((lengine.N >= 38000) || (rengine.N >= 38000))
                    {
                        isStable = true;
                        isRunning = false;
                    }
                }
                if (isStopping)
                {
                    engineTime += accumulatedTime / 1000.0;
                    lengine.state = 2; rengine.state = 2;
                    lengine.calculateParameters(engineTime,  lengine.t0);
                    rengine.calculateParameters(engineTime,  rengine.t0);

                    if ((lengine.N <= 0 && lengine.T <= 20) && (rengine.N <= 0 && rengine.T <= 20))
                    {
                        set_State();
                        isStopping = false;
                        isRecording = false;
                        leftEngineFile.close();
                        rightEngineFile.close();
                    }
                }
                if (isStable)
                {
                    if (push)
                    {
                        engineTime += accumulatedTime / 1000.0;
                        lengine.state = 4; rengine.state = 4;
                        // 使用统一的随机增量, 3%-5% 增长
                        double randFactorN = 1.03 + (rand() % 3) * 0.01;
                        double randFactorT = 1.03 + (rand() % 3) * 0.01;

                        lengine.N = lengine.N * randFactorN;
                        lengine.T = lengine.T * randFactorT;

                        rengine.N = rengine.N * randFactorN;  // 使用相同的增量
                        rengine.T = rengine.T * randFactorT;  // 使用相同的增量

                        lengine.calculateParameters(engineTime);
                        rengine.calculateParameters(engineTime);
                        push = false;
                    }
                    else if (pull)
                    {
                        engineTime += accumulatedTime / 1000.0;
                        lengine.state = 5; rengine.state = 5;
                        // 使用统一的随机减小值
                        double randFactorN = 0.95 + (rand() % 2) * 0.01;  // 3%-5% 降低
                        double randFactorT = 0.95 + (rand() % 2) * 0.01;

                        lengine.N = lengine.N * randFactorN;
                        lengine.T = lengine.T * randFactorT;

                        rengine.N = rengine.N * randFactorN;  // 使用相同的减小值
                        rengine.T = rengine.T * randFactorT;  // 使用相同的减小值
                        lengine.calculateParameters(engineTime);
                        rengine.calculateParameters(engineTime);
                        pull = false;
                    }
                    else
                    {
                        engineTime += accumulatedTime / 1000.0;
                        lengine.state = 3; rengine.state = 3;
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::uniform_real_distribution<> distN(0.97, 1.03);  // 转速随机范围
                        std::uniform_real_distribution<> distT(0.97, 1.03);  // 温度随机范围
                        std::uniform_real_distribution<> distV(lengine.V0 * 0.97, lengine.V0 * 1.03); // 燃料流量范围

                        double randFactorN = distN(gen);
                        double randFactorT = distT(gen);
                        double randV = distV(gen);

                        lengine.calculateParameters(engineTime, 3, randFactorN, randFactorT, randV);// cout<<"left:" << lengine.T << endl;
                        rengine.calculateParameters(engineTime, 3,  randFactorN, randFactorT, randV); //cout << "right:" << rengine.T << endl;
                        
                    }
                }
                if (isRecording)  record_info();
                for (int i = 0; i < warnings.size(); i++)
                {
                    if(warnings[i].islogged) warnings[i].now_time = engineTime;
                }
                for (int i = 0; i < buttons.size(); i++)
                {
                    buttons[i].nowTime = engineTime;
                }
                BeginBatchDraw();
                cleardevice();
                Show_fail();
                draw_buttons(buttons);
                lengine.drawN_Dial(100, 100); lengine.drawT_Dial(100, 100);
                rengine.drawN_Dial(300, 100); rengine.drawT_Dial(300, 100);
                roundrect(10, 625, 590, 945,30,30);
                C_V_sensor0.draw_C_and_V();
                show_state(isRunning, isStopping, isStable);
                FlushBatchDraw();
                accumulatedTime = 0;
            }
        }

    }
    void show_state(bool isRunning, bool isStopping, bool isStable)
    {
        if (isRunning)
        {
            settextcolor(GREEN);
            settextstyle(30, 0, "Arial");
            outtextxy(420, 150, "START");
            settextcolor(WHITE);
            outtextxy(420, 200, "RUN");
        }
        else if (isStable || isStopping)
        {
            settextcolor(WHITE);
            settextstyle(30, 0, "Arial");
            outtextxy(420, 150, "START");
            settextcolor(GREEN);
            outtextxy(420, 200, "RUN");
        }
        else if (lengine.N <= 95 || rengine.N <= 95)
        {
            settextcolor(WHITE);
            settextstyle(30, 0, "Arial");
            outtextxy(420, 150, "START");
            outtextxy(420, 200, "RUN");
        }
    }
};

Simulation sim;
int main() {
    initgraph(600, 950);
    setbkcolor(BLACK);
    cleardevice();
    sim.runSimulation();
    closegraph();


 
    return 0;
}

void Simulation::draw_buttons(vector<Button>& buttons)
{
    for (int i = 0; i < buttons.size(); i++)
    {
        if (buttons[i].nowTime - buttons[i].begineTime >= 5)
        {
            buttons[i].isValid = true;
        }
        buttons[i].drawButton();

    }
}

void Simulation::create_OverTemp4()
{
    if (isOverTemp4Fail)
    {
        lengine.isFailed_OverTemp4 = true; lengine.isFailed_OverTemp4_tmp = true;
        rengine.isFailed_OverTemp4 = true; rengine.isFailed_OverTemp4_tmp = true;
        isOverTemp4Fail = false;
    }
}




void Simulation::create_OverTemp3()
{
    if (isOverTemp3Fail)
    {
        lengine.isFailed_OverTemp3 = true; lengine.isFailed_OverTemp3_tmp = true;
        rengine.isFailed_OverTemp3 = true; rengine.isFailed_OverTemp3_tmp = true;
        isOverTemp3Fail = false;
    }
}



void Simulation::create_OverTemp2()
{
    if (isOverTemp2Fail)
    {
        lengine.isFailed_OverTemp2 = true; lengine.isFailed_OverTemp2_tmp = true;
        rengine.isFailed_OverTemp2 = true; rengine.isFailed_OverTemp2_tmp = true;
        isOverTemp2Fail = false;
    }
}



void Simulation::create_OverTemp1()
{
    if (isOverTemp1Fail)
    {
        lengine.isFailed_OverTemp1 = true; lengine.isFailed_OverTemp1_tmp = true;
        rengine.isFailed_OverTemp1 = true; rengine.isFailed_OverTemp1_tmp = true;
        isOverTemp1Fail = false;
    }
}



void Simulation::create_OverSpd1()
{
    if (isOverSpd1Fail)
    {
        lengine.isFailed_OverSpd1 = true; lengine.isFailed_OverSpd1_tmp = true;
        rengine.isFailed_OverSpd1 = true; rengine.isFailed_OverSpd1_tmp = true;
        isOverSpd1Fail = false;
    }
}

void Simulation::create_OverSpd2()
{
    if (isOverSpd2Fail)
    {
        lengine.isFailed_OverSpd2 = true; lengine.isFailed_OverSpd2_tmp = true;
        rengine.isFailed_OverSpd2 = true; rengine.isFailed_OverSpd2_tmp = true;
        isOverSpd2Fail = false;
    }
}


void Simulation::create_OverFFFail()
{
    if (isOverFFFail)
    {
        lengine.isFailed_OverFF = true;
        rengine.isFailed_OverFF = true;
        isOverFFFail = false;
    }
}

void Simulation::create_FuelSFail()
{
    if (isFuelSFail)
    {
        C_V_sensor0.is_working = false;
        isFuelSFail = false;
    }
}


void Simulation::create_LowFuelFail()
{
    if (isLowFuelFail)
    {
        C = 1000;//全局的C，相当于油箱？
        isLowFuelFail = false;
    }
}


void Simulation::create_EGTSFail()
{
    if (isEGTSFail)
    {
        lengine.T_sensor1.is_working = false; lengine.T_sensor2.is_working = false; rengine.T_sensor1.is_working = false; rengine.T_sensor2.is_working = false;
        isEGTSFail = false;
    }
}
void Simulation::create_N1SFail()
{
    if (isN1SFail)
    {
        lengine.N_sensor1.is_working = false; lengine.N_sensor2.is_working = false; rengine.N_sensor1.is_working = false; rengine.N_sensor2.is_working = false;
        isN1SFail = false;
    }
}

void Simulation::create_N1S1Fail()
{
    if (isN1S1Fail)
    {
        srand(time(NULL));
        int r;//= rand() % 4;
        while (1)
        {
            r = rand() % 4;
            cout << "xxxxr:" << r << endl;
            if (r == 0 && lengine.N_sensor1.is_working)
            {
                lengine.N_sensor1.is_working = false;
                break;
            }
            else if (r == 1 && lengine.N_sensor2.is_working)
            {
                lengine.N_sensor2.is_working = false;
                break;
            }
            else if (r == 2 && rengine.N_sensor1.is_working)
            {
                rengine.N_sensor1.is_working = false;
                break;
            }
            else if (r == 3 && rengine.N_sensor2.is_working)
            {
                rengine.N_sensor2.is_working = false;
                break;
            }
            else if (!lengine.N_sensor1.is_working && !lengine.N_sensor2.is_working && !rengine.N_sensor1.is_working && !rengine.N_sensor2.is_working)
            {
                break;
            }

        }
        isN1S1Fail = false;
    }
}
void Simulation::create_N1S2Fail()
{
    if (isN1S2Fail)
    {
        srand(time(NULL));
        int r;
        while (1)
        {
            r = rand() % 2;
            if (r == 0 && (lengine.N_sensor1.is_working || lengine.N_sensor2.is_working))//left
            {
                lengine.N_sensor1.is_working = false; lengine.N_sensor2.is_working = false;
                break;
            }
            else if (r == 1 && (rengine.N_sensor1.is_working || rengine.N_sensor2.is_working))
            {
                rengine.N_sensor1.is_working = false; rengine.N_sensor2.is_working = false;
                break;
            }
            else if (!lengine.N_sensor1.is_working && !lengine.N_sensor2.is_working && !rengine.N_sensor1.is_working && !rengine.N_sensor2.is_working)
            {
                break;
            }

        }
        isN1S2Fail = false;
    }
}



void Simulation::create_EGTS1Fail()//多次创建EGTS1故障有可能导致EGTS2故障发生
{
    if (isEGTS1Fail)
    {
        srand(time(NULL));
        int r;
        while (1)
        {
            r = rand() % 4; cout << "rr:" << r << endl;

            if (r == 0 && lengine.T_sensor1.is_working)
            {
                lengine.T_sensor1.is_working = false;
                break;
            }
            else if (r == 1 && lengine.T_sensor2.is_working)

            {
                lengine.T_sensor2.is_working = false;
                break;
            }
            else if (r == 2 && rengine.T_sensor1.is_working)
            {
                rengine.T_sensor1.is_working = false;
                break;
            }
            else if (r == 3 && rengine.T_sensor2.is_working)
            {
                rengine.T_sensor2.is_working = false;
                break;
            }
            else if (!lengine.T_sensor1.is_working && !lengine.T_sensor2.is_working && !rengine.T_sensor1.is_working && !rengine.T_sensor2.is_working)
            {
                break;
            }
        }
        isEGTS1Fail = false;
    }
}
void Simulation::create_EGTS2Fail()
{
    if (isEGTS2Fail)
    {
        srand(time(NULL));
        int r;

        while (1)
        {
            r = rand() % 2;  cout << "r: " << r << endl;
            if (r == 0 && lengine.T_sensor1.is_working || lengine.T_sensor2.is_working)//left
            {
                lengine.T_sensor1.is_working = false; lengine.T_sensor2.is_working = false;
                break;
            }
            else if (r == 1 && rengine.T_sensor1.is_working || rengine.T_sensor2.is_working)
            {
                rengine.T_sensor1.is_working = false; rengine.T_sensor2.is_working = false;
                break;
            }
            else if (!lengine.T_sensor1.is_working && !lengine.T_sensor2.is_working && !rengine.T_sensor1.is_working && !rengine.T_sensor2.is_working)
            {
                break;
            }
        }
        isEGTS2Fail = false;
    }
}


void Simulation::Check_fail()
{
    //N_sensor N1S2代表发动机的两个转速传感器都失效,N1S1代表1个发动机的其中1个转速传感器失效
    if (!lengine.N_sensor1.is_working && !lengine.N_sensor2.is_working && !rengine.N_sensor1.is_working && !rengine.N_sensor2.is_working)
    {
        lengine.isFailed_N1S = true; rengine.isFailed_N1S = true;
        start_Warning(12);
        stopEngine();
    }
    if ((!lengine.N_sensor1.is_working) && (!lengine.N_sensor2.is_working))
    {
        lengine.isFailed_N1S2 = true;
        start_Warning(4);
    }
    if ((!rengine.N_sensor1.is_working) && (!rengine.N_sensor2.is_working))
    {
        rengine.isFailed_N1S2 = true;
        start_Warning(5);
    }
    if (!lengine.N_sensor1.is_working)

    {
        lengine.isFailed_N1S1 = true;
        start_Warning(0);
    }
    if (!lengine.N_sensor2.is_working)
    {
        lengine.isFailed_N1S1 = true;
        start_Warning(1);
    }
    if (!rengine.N_sensor1.is_working)

    {
        rengine.isFailed_N1S1 = true;
        start_Warning(2);
    }
    if (!rengine.N_sensor2.is_working)
    {
        rengine.isFailed_N1S1 = true;
        start_Warning(3);
    }
    //T_sensor EGTS2代表发动机的两个温度传感器都失效,EGTS1代表1个发动机的其中1个温度传感器失效
    if ((!lengine.T_sensor1.is_working) && (!lengine.T_sensor2.is_working) && (!rengine.T_sensor1.is_working) && (!rengine.T_sensor2.is_working))
    {
        lengine.isFailed_EGTS = true; rengine.isFailed_EGTS = true;
        start_Warning(13);
        stopEngine();
    }

    if ((!lengine.T_sensor1.is_working) && (!lengine.T_sensor2.is_working))
    {
        lengine.isFailed_EGTS2 = true;
        start_Warning(10);
    }
    if ((!rengine.T_sensor1.is_working) && (!rengine.T_sensor2.is_working))
    {
        rengine.isFailed_EGTS2 = true;
        start_Warning(11);
    }
    if (!lengine.T_sensor1.is_working)
    {
        lengine.isFailed_EGTS1 = true;
        start_Warning(6);
    }
    if (!lengine.T_sensor2.is_working)
    {
        lengine.isFailed_EGTS1 = true;
        start_Warning(7);
    }
    if (!rengine.T_sensor1.is_working)
    {
        rengine.isFailed_EGTS1 = true;
        start_Warning(8);
    }
    if (!rengine.T_sensor2.is_working)
    {
        rengine.isFailed_EGTS1 = true;
        start_Warning(9);
    }
    if (C <= 1000)
    { 
        C_V_sensor0.isFailed_LowFuel = true;

        start_Warning(14);
    }
    if (!C_V_sensor0.is_working)
    {
        C_V_sensor0.isFailed_FuelS = true;
        start_Warning(15);
        stopEngine();
    }
    if (C_V_sensor0.V >= 50)
    {
        C_V_sensor0.isOverFFFail = true;
        start_Warning(16);
    }
    if (C_V_sensor0.V < 49)
    {
        C_V_sensor0.isOverFFFail = false;
    }

    if ((lengine.N > 42000 && lengine.N <= 48000) && (rengine.N > 42000 && rengine.N <= 48000))
    {
        lengine.isFailed_OverSpd1 = true; rengine.isFailed_OverSpd1 = true;
        start_Warning(17);
    }

    if ((lengine.N > 48000) && (rengine.N > 48000))
    {
        lengine.isFailed_OverSpd1 = false; rengine.isFailed_OverSpd2 = false;

        lengine.isFailed_OverSpd2 = true; rengine.isFailed_OverSpd2 = true;
        start_Warning(18);
        stopEngine();
    }
    if (((lengine.T > 850 && lengine.T <= 1000) || (rengine.T > 850 && rengine.T <= 1000))&&(isRunning))
    {
        //lengine.isFailed_OverTemp1 = false; rengine.isFailed_OverTemp1 = false;

        lengine.isFailed_OverTemp1 = true; rengine.isFailed_OverTemp1 = true;
        start_Warning(19);
        //stopEngine();
    }



    if (((lengine.T > 1000) || (rengine.T > 1000)) && (isRunning))
    {
        lengine.isFailed_OverTemp1 = false; rengine.isFailed_OverTemp1 = false;

        lengine.isFailed_OverTemp2 = true; rengine.isFailed_OverTemp2 = true;
        start_Warning(20);
        stopEngine();
    }

    if (((lengine.T > 950 && lengine.T <= 1100) || (rengine.T > 950 && rengine.T <= 1100)) && (isStable))
    {
        lengine.isFailed_OverTemp3 = true; rengine.isFailed_OverTemp3 = true;
        start_Warning(21);
    }
    if (((lengine.T > 1100) || (rengine.T > 1100)) && (isStable))
    {
        lengine.isFailed_OverTemp3 = false; rengine.isFailed_OverTemp3 = false;

        lengine.isFailed_OverTemp4 = true; rengine.isFailed_OverTemp4 = true;
        start_Warning(22);
        stopEngine();
    }
    if (C<=0)
    {
        
        stopEngine();
    }
}

void Simulation::start_Warning(int Case)
{
    if (warnings[Case].islogged == false)
    {

        if (warnings[Case].isShow == 1)warnings[Case].isShow = 2;
        warnings[Case].islogged = true;
        warnings[Case].begine_time = engineTime;
        warnings[Case].record_warning(warnings[Case].begine_time);

    }
    if ((warnings[Case].now_time - warnings[Case].begine_time >= 5) && (warnings[Case].isShow == 2||warnings[Case].isShow==3))
    {
        warnings[Case].islogged = false;
        warnings[Case].isShow = 3;
    }

}
void Simulation::Show_fail()//(vector<int>& fails)//
{
    for (int i = 0; i < warnings.size(); i++)
    {
        if (warnings[i].isShow==2) warnings[i].draw_warning();
    }

}
