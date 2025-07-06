
#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include <conio.h>
#include <Windows.h>
#include <iomanip>
#include<string>
#include<easyx.h>
#include<graphics.h>
#include <strsafe.h>
#include <stdlib.h>
#include<vector>
#include<fstream>
#include<queue>
#include <sstream> 
using namespace std;
const int GRID_SIZE = 30;
char buffer[100];
IMAGE background;
IMAGE background1;
IMAGE background2;
IMAGE background3;
IMAGE background4;
IMAGE background5;
struct player
{
    string Name;
    string Version;
    int Score;

};
// ���ڼ�¼ÿ��ģʽ����߷�
struct ModeHighScore {
    string name;
    int score;
};
class Button {
public:
    Button(int pos_x, int pos_y, int tmp_width, int tmp_height, const char* tmp_text, COLORREF tmp_color)
        : x(pos_x), y(pos_y), width(tmp_width), height(tmp_height), text(tmp_text), button_color(tmp_color)
    {}
    // ���ư�ť
    void draw_button() {
        setbkmode(TRANSPARENT);
        setfillcolor(button_color);
        setlinecolor(BLACK);
        setlinestyle(PS_DASH | PS_ENDCAP_FLAT, 1);
        fillrectangle(x, y, x + width, y + height);

        char text_[50] = { 0 };
        strcpy_s(text_, text);
        settextstyle(20, 0, "����");
        int tx = x + (width - textwidth(text_)) / 2;
        int ty = y + (height - textheight(text_)) / 2;
        outtextxy(tx, ty, text_);
    }

    // �ж�����Ƿ��ڰ�ť�ڲ�
    bool In_button(ExMessage msg) {
        return msg.x > x && msg.x < x + width && msg.y > y && msg.y < y + height;
    }

    // ���°�ť��ɫ
    void changeColor(COLORREF newColor) {
        button_color = newColor;
        draw_button();
    }

    // ˢ�°�ť״̬
    void Draw_button(ExMessage msg) {
        if (In_button(msg)) {
            changeColor(RED);  // ����ڰ�ť��ʱ����Ϊ��ɫ
        }
        else {
            changeColor(BLUE);  // ��겻�ڰ�ť��ʱ���ָ�Ϊ��ɫ
        }
    }

    bool click_button(ExMessage msg) {
        return In_button(msg) && msg.message == WM_LBUTTONDOWN;
    }

    // �����ť
    void clear_button(COLORREF background_color) {
        setfillcolor(background_color);
        setlinecolor(background_color);
        fillrectangle(x, y, x + width, y + height);
    }

    Button& operator=(const Button& other)
    {
        if (this != &other) {  // ��ֹ���Ҹ�ֵ
            x = other.x;
            y = other.y;
            width = other.width;
            height = other.height;
            text = other.text;
            button_color = other.button_color;
        }
        return *this;
    }
private:
    int x, y;
    int width, height;
    const char* text;
    COLORREF button_color;
};

// ���Ʊ���
void draw_title() 
{
    settextstyle(60, 0, "����");
    settextcolor(WHITE);
    outtextxy(250, 50, "̰");
    outtextxy(355, 50, "��");
    outtextxy(460, 50, "��");
}
// ���Ʋ˵�
void Draw_menu(vector<Button> menu_button,ExMessage msg)
{
    draw_title();
    //button5.draw_button();
    for ( auto& button : menu_button) {
        button.Draw_button(msg);
    }
}
// �����µ�ͼ��

void draw_ranking()
{
    settextstyle(60, 0, "����");
    settextcolor(WHITE);
    outtextxy(600, 50, "��");
    outtextxy(705, 50, "��");
    outtextxy(820, 50, "��");
}
void show_ranking() {
    // �� record.txt �ļ�
    ifstream file("record.txt");
    if (!file.is_open()) {
        outtextxy(100, 200, "�޷��򿪼�¼�ļ���");
        return;
    }

    vector<player> records; // �洢���м�¼
    string version, name;
    int score;

    // ��ȡ record.txt �ļ����ݲ����� vector
    while (file >> version >> name >> score) {
        records.emplace_back(player{ name, version, score });
    }
    file.close();

    // �����м�¼���շ�����������
    auto sort_by_score = [](const player& a, const player& b) {
        return a.Score > b.Score;
    };
    sort(records.begin(), records.end(), sort_by_score);

    // ��ʼ�������汾����߷�
    ModeHighScore entry_high = { "", 0 };
    ModeHighScore intermediate_high = { "", 0 };
    ModeHighScore advanced_high = { "", 0 };

    // ���������ļ�¼���ҵ�ÿ���汾����߷�
    for (const auto& record : records) {
        if (record.Version == "���Ű�" && entry_high.score == 0) {
            entry_high = { record.Name, record.Score };
        }
        else if (record.Version == "���װ�" && intermediate_high.score == 0) {
            intermediate_high = { record.Name, record.Score };
        }
        else if (record.Version == "�߼���" && advanced_high.score == 0) {
            advanced_high = { record.Name, record.Score };
        }
        // �������汾����߷־����ҵ�������ǰ�˳�ѭ��
        if (entry_high.score > 0 && intermediate_high.score > 0 && advanced_high.score > 0) {
            break;
        }
    }

    // ��ʾ����
    draw_ranking();

    // ��ʾ����ģʽ����߷�
    settextstyle(20, 0, "����");
    settextcolor(WHITE);
    outtextxy(600, 130, ("���Ű���߷�: " + entry_high.name + " - " + to_string(entry_high.score)).c_str());
    outtextxy(600, 160, ("���װ���߷�: " + intermediate_high.name + " - " + to_string(intermediate_high.score)).c_str());
    outtextxy(600, 190, ("�߼�����߷�: " + advanced_high.name + " - " + to_string(advanced_high.score)).c_str());

    // ����չʾ����ͼ��
    int startX1 = 180, startY1 = 230, width1 = 400, height1 = 600; // ��һ������
    int startX2 = 580, startY2 = 230, width2 = 400, height2 = 600; // �ڶ�������
    int startX3 = 980, startY3 = 230, width3 = 400, height3 = 600; // ����������

    int offsetY1 = height1 / 10; // ��һ��ÿ�м��
    int offsetY2 = height2 / 10; // �ڶ���ÿ�м��
    int offsetY3 = height3 / 10; // ������ÿ�м��

    // �ֱ�洢���汾��ǰ10����¼
    vector<player> entry_records;
    vector<player> intermediate_records;
    vector<player> advanced_records;

    for (const auto& record : records) {
        if (record.Version == "���Ű�" && entry_records.size() < 10) {
            entry_records.push_back(record);
        }
        else if (record.Version == "���װ�" && intermediate_records.size() < 10) {
            intermediate_records.push_back(record);
        }
        else if (record.Version == "�߼���" && advanced_records.size() < 10) {
            advanced_records.push_back(record);
        }
    }

    // ��ʾ���Ű�ǰ10��
    for (size_t i = 0; i < entry_records.size(); ++i) {
        string text = to_string(i + 1) + ". " + entry_records[i].Name + " | " + to_string(entry_records[i].Score);
        outtextxy(startX1, startY1 + i * offsetY1, text.c_str());
    }

    // ��ʾ���װ�ǰ10��
    for (size_t i = 0; i < intermediate_records.size(); ++i) {
        string text = to_string(i + 1) + ". " + intermediate_records[i].Name + " | " + to_string(intermediate_records[i].Score);
        outtextxy(startX2, startY2 + i * offsetY2, text.c_str());
    }

    // ��ʾ�߼���ǰ10��
    for (size_t i = 0; i < advanced_records.size(); ++i) {
        string text = to_string(i + 1) + ". " + advanced_records[i].Name + " | " + to_string(advanced_records[i].Score);
        outtextxy(startX3, startY3 + i * offsetY3, text.c_str());
    }
}





void update_file_and_record(int score, int maxscore, player Player, const string version, int line) {
    // �ļ���
    string filename = "record.txt";

    // 1. ��ȡ�ļ�����������
    ifstream infile(filename);
    stringstream buf;
    if (infile.is_open()) {
        buf << infile.rdbuf(); // ���ļ����ݶ��뵽�ַ�������
        infile.close();
    }

    string old_content = buf.str(); // ��ȡ�ļ�ԭ����

    // 2. ƴ���������������
    string x = " ";
    string new_content = version + x + Player.Name + x + to_string(Player.Score) + '\n' + old_content;

    // 3. ��������д���ļ�
    ofstream outfile(filename, ios::out | ios::trunc); // ios::trunc ����ļ���д��
    if (!outfile.is_open()) {
        cerr << "Failed to open the file: " << filename << endl;
        return;
    }
    outfile << new_content; // д���µ�����
    outfile.close();

    // �������������߷֣����� highscores.txt
    if (score > maxscore) {
        string highscore_file = "highscores.txt";

        // 1. ��ȡָ��������
        ifstream infile(highscore_file);
        if (!infile.is_open()) {
            cerr << "Failed to open the file: " << highscore_file << endl;
            return;
        }

        string line_content;     // �洢Ŀ��������
        stringstream remaining_buffer; // �洢��������

        int current_line = 1;
        while (getline(infile, line_content)) {
            if (current_line == line) {
                // �ҵ�Ŀ���У�ֹͣ��ȡ
                break;
            }
            remaining_buffer << line_content << '\n'; // ����֮ǰ����
            current_line++;
        }

        // ���Ŀ���в����ڣ�ֱ�ӷ���
        if (current_line != line) {
            cerr << "Line " << line << " not found in the file: " << highscore_file << endl;
            infile.close();
            return;
        }

        // ��ʣ�����ݴ洢
        stringstream buffer;
        buffer << infile.rdbuf(); // ��ȡĿ���к������
        string remaining_content = buffer.str();
        infile.close();

        // 2. �µ�Ŀ��������
        string new_line = Player.Name + " " + to_string(score);

        // 3. д���ļ�
        ofstream outfile(highscore_file, ios::out | ios::trunc); // ����д��ģʽ
        if (!outfile.is_open()) {
            cerr << "Failed to open the file: " << highscore_file << endl;
            return;
        }

        // д��֮ǰ�����ݡ���Ŀ���С��Լ�ʣ������
        outfile << remaining_buffer.str();
        outfile << new_line << '\n';
        outfile << remaining_content;
        outfile.close();
    }
}

enum Direction {
    UP, DOWN, LEFT, RIGHT
};
enum FoodType { BANANA, APPLE, POISON };


class Snakebody {
public:
    int x;
    int y;
    Direction dir;

    Snakebody(int t_x, int t_y, Direction t_dir) : x(t_x), y(t_y), dir(t_dir) {}

    void draw_body() const 
    {
        setfillcolor(LIGHTGREEN);
        fillcircle(x, y, 15);
    }

    void draw_head(COLORREF color) const
    {
        setfillcolor(color);
        fillcircle(x, y, 15);
    }
};
class Snake
{
public:
    deque<Snakebody> snake;
    int s_score;
    int is_eat;
    void change_direction(ExMessage& msg)
    {
        if (msg.message == WM_KEYDOWN) {
            switch (msg.vkcode) {
            case 'w': case 'W': case VK_UP:
                if (snake.front().dir != DOWN) snake.front().dir = UP;
                break;
            case 's': case 'S': case VK_DOWN:
                if (snake.front().dir != UP) snake.front().dir = DOWN;
                break;
            case 'a': case 'A': case VK_LEFT:
                if (snake.front().dir != RIGHT) snake.front().dir = LEFT;
                break;
            case 'd': case 'D': case VK_RIGHT:
                if (snake.front().dir != LEFT) snake.front().dir = RIGHT;
                break;
            default:
                break;
            }
        }
    }

    void change_direction1(ExMessage& msg)
    {
        if (msg.message == WM_KEYDOWN) {
            switch (msg.vkcode) {
            case 'w': case 'W': 
                if (snake.front().dir != DOWN) snake.front().dir = UP;
                break;
            case 's': case 'S': 
                if (snake.front().dir != UP) snake.front().dir = DOWN;
                break;
            case 'a': case 'A': 
                if (snake.front().dir != RIGHT) snake.front().dir = LEFT;
                break;
            case 'd': case 'D': 
                if (snake.front().dir != LEFT) snake.front().dir = RIGHT;
                break;
            default:
                break;
            }
        }
    }

    void change_direction2(ExMessage& msg)
    {
        if (msg.message == WM_KEYDOWN) {
            switch (msg.vkcode) {
             case VK_UP:
                if (snake.front().dir != DOWN) snake.front().dir = UP;
                break;
             case VK_DOWN:
                if (snake.front().dir != UP) snake.front().dir = DOWN;
                break;
            case VK_LEFT:
                if (snake.front().dir != RIGHT) snake.front().dir = LEFT;
                break;
             case VK_RIGHT:
                if (snake.front().dir != LEFT) snake.front().dir = RIGHT;
                break;
            default:
                break;
            }
        }
    }
    bool in_snakes(int x, int y)
    {
        for (int i = 0; i < snake.size(); i++)
        {
            if (x == snake[i].x && y == snake[i].y)
            {
                return true;
                break;
            }
        }
        return false;
    }
    void generate_random_snake()//, const WallManager& wall_manager) 
    {
        snake.clear();
        srand(time(0));
        int x = 1 + rand() % 28;
        int y = 1 + rand() % 28;
        int startX = x * GRID_SIZE + 15;
        int startY = y * GRID_SIZE + 15;
        Snakebody snake_head(startX, startY, RIGHT);
        snake.push_back(snake_head);
    }

    void generate_random_snake(Snake& other)//, const WallManager& wall_manager) 
    {
        
        snake.clear();
        srand(time(0));
        while (1)
        {
            int x = 1 + rand() % 28;
            int y = 1 + rand() % 28;
            int startX = x * GRID_SIZE + 15;
            int startY = y * GRID_SIZE + 15;
             startX = x * GRID_SIZE + 15;
             startY = y * GRID_SIZE + 15;
            if (other.in_snakes(startX, startY))
            {
                continue;
            }
            else
            {
                Snakebody snake_head(startX, startY, RIGHT);
                snake.push_back(snake_head);
                break;
            }
        }
        
      
       
    }

    void draw_snake(COLORREF head_color)
    {
        int head_x = snake.front().x;
        int head_y = snake.front().y;
        for (int i = 0; i < snake.size(); i++)
        {
            if (snake[i].x==head_x&&snake[i].y==head_y)
            {
                snake[i].draw_head(head_color);
            }
            else
            {
                snake[i].draw_body();
            }
        }
    }
};


class Wall {
public:
    int w_x;
    int w_y;

    Wall(int t_x = 0, int t_y = 0) : w_x(t_x), w_y(t_y) {}

    void draw_wall() const {
        setfillcolor(RED);
        fillcircle(w_x, w_y, 15);
    }
};
class WallManager {
public:
    vector<Wall> walls;

    WallManager() {
        for (int i = 0; i < 30; i++) {
            walls.push_back(Wall{ 15 + i * GRID_SIZE, 15 });
            walls.push_back(Wall{ 15 + i * GRID_SIZE, 885 });
            walls.push_back(Wall{ 15, 15 + i * GRID_SIZE });
            walls.push_back(Wall{ 885, 15 + i * GRID_SIZE });
        }
    }
    void refresh_wall_manager()
    {
        for (int i = 0; i < 30; i++) {
            walls.push_back(Wall{ 15 + i * GRID_SIZE, 15 });
            walls.push_back(Wall{ 15 + i * GRID_SIZE, 885 });
            walls.push_back(Wall{ 15, 15 + i * GRID_SIZE });
            walls.push_back(Wall{ 885, 15 + i * GRID_SIZE });
        }
    }
    void draw_walls() const {
        for (const auto& wall : walls) {
            wall.draw_wall();
        }
    }

    bool check_collision_with_wall(int x, int y) const
    {
        for (const auto& wall : walls) {
            if (wall.w_x == x && wall.w_y == y) {
                return true;
            }
        }
        return false;
    }
    bool is_position_validss(int x, int y) const
    {
        // ���λ���Ƿ�����Ϸ������
        // ���ǵ�Բ�İ뾶��15��ʵ�ʷ�Χ��Ҫ����ƫ��
        if (x < 15 || x > 885 || y < 15 || y > 885) {
            return false;
        }
        // ����Ƿ���ǽ��ײ
        if (check_collision_with_wall(x, y)) {
            return false;
        }
        return true;
    }

    void add_wall_from_snake(deque<Snakebody>& snake)
    {
        if (snake.back().dir == RIGHT && check_collision_with_wall(snake.back().x - 30, snake.back().y) == false)
        {
            Snakebody t_tail(snake.back().x - 30, snake.back().y, RIGHT);
            snake.push_back(t_tail);
            snake.pop_front();
        }
        else if (snake.back().dir == LEFT && check_collision_with_wall(snake.back().x + 30, snake.back().y) == false)
        {
            Snakebody t_tail(snake.back().x + 30, snake.back().y, LEFT);
            snake.push_back(t_tail);
            snake.pop_front();
        }
        else if (snake.back().dir == UP && check_collision_with_wall(snake.back().x, snake.back().y + 30) == false)
        {
            Snakebody t_tail(snake.back().x, snake.back().y + 30, UP);
            snake.push_back(t_tail);
            snake.pop_front();
        }
        else if (snake.back().dir == DOWN && check_collision_with_wall(snake.back().x, snake.back().y - 30) == false)
        {
            Snakebody t_tail(snake.back().x, snake.back().y - 30, DOWN);
            snake.push_back(t_tail);
            snake.pop_front();
        }


        for (auto& segment : snake)
        {
            if (!check_collision_with_wall(segment.x, segment.y))
            {
                walls.emplace_back(segment.x, segment.y);
            }
        }
    }


};

class Food {
public:
    int f_x;
    int f_y;
    FoodType type;
    Food(int x = 0, int y = 0, FoodType t_foodtype = BANANA) : f_x(x), f_y(y), type(t_foodtype) {}
    void draw_food() const
    {
        switch (type)
        {
        case BANANA:
            setfillcolor(YELLOW);
            fillcircle(f_x, f_y, 15);
            break;
        case APPLE:
            setfillcolor(LIGHTRED);
            fillcircle(f_x, f_y, 15);
            break;
        case POISON:
            setfillcolor(BLUE);
            fillcircle(f_x, f_y, 15);
            break;
        }
    }
};
class FoodManager
{
public:
    vector<Food> foods;

    bool in_foods(int x, int y)
    {
        if (foods.size() == 0)
        {
            return false;
        }
        for (int i = 0; i < foods.size(); i++)
        {
            if (x == foods[i].f_x && y == foods[i].f_y)
            {
                return true;
                break;
            }
        }
        return false;
    }

    void draw_all_food()
    {
        for (const auto& food : foods)
        {
            food.draw_food();
        }
    }

    bool no_any_kind_food()
    {
        int APPLE_cnt = 0;
        int BANANA_cnt = 0;
        int POISON_cnt = 0;
        for (int i = 0; i < foods.size(); i++)
        {
            if (foods[i].type == APPLE)
            {
                APPLE_cnt++;
            }
            else if (foods[i].type == BANANA)
            {
                BANANA_cnt++;
            }
            else if (foods[i].type == POISON)
            {
                POISON_cnt++;
            }
        }
        if (APPLE_cnt == 0 || BANANA_cnt == 0 || POISON_cnt == 0)
        {
            cout << "true:" << "apple:" << APPLE_cnt << " "<<"banana:" << BANANA_cnt << " " <<"poison:" << POISON_cnt << endl;
            return true;
        }
        else
        {
            cout << "false:" << "apple:" << APPLE_cnt << " " << "banana:" << BANANA_cnt << " " << "poison:" << POISON_cnt << endl;
            return false;
        }
    }
    void add_food_from_snake(deque<Snakebody>& snake, WallManager wall_manager)
    {
        if (snake.back().dir == RIGHT && wall_manager.check_collision_with_wall(snake.back().x - 30, snake.back().y) == false)
        {
            Snakebody t_tail(snake.back().x - 30, snake.back().y, RIGHT);
            snake.push_back(t_tail);
        }
        else if (snake.back().dir == LEFT && wall_manager.check_collision_with_wall(snake.back().x + 30, snake.back().y) == false)
        {
            Snakebody t_tail(snake.back().x + 30, snake.back().y, LEFT);
            snake.push_back(t_tail);
        }
        else if (snake.back().dir == UP && wall_manager.check_collision_with_wall(snake.back().x, snake.back().y + 30) == false)
        {
            Snakebody t_tail(snake.back().x, snake.back().y + 30, UP);
            snake.push_back(t_tail);
        }
        else if (snake.back().dir == DOWN && wall_manager.check_collision_with_wall(snake.back().x, snake.back().y - 30) == false)
        {
            Snakebody t_tail(snake.back().x, snake.back().y - 30, DOWN);
            snake.push_back(t_tail);
        }


        for (auto& segment : snake)
        {
            if (!wall_manager.check_collision_with_wall(segment.x, segment.y))
            {
                foods.emplace_back(segment.x, segment.y);
            }
        }
    }
};

class Information
{
public:

    void draw_score_and_time(int& score, int& game_time, int& max_score, int life,Snake& class_snake,COLORREF color)//, const deque<Snakebody>& snake)
    {
        settextcolor(color);
        settextstyle(30, 0, "Consolas");

        char score_text[50];
        sprintf_s(score_text, "����: %d", score);
        outtextxy(950, 30, score_text);


        char length_text[50];
        int len = class_snake.snake.size();
        sprintf_s(length_text, "����: %d", len);
        outtextxy(950, 60, length_text);


        char time_text[50];
        sprintf_s(time_text, "ʱ��: %d s", game_time);
        outtextxy(950, 90, time_text);


        char max_score_text[50];
        sprintf_s(max_score_text, "��߷�: %d", max_score);
        outtextxy(950, 120, max_score_text);

        if (life)
        {
            char life_text[50];
            sprintf_s(life_text, "ʣ������: %d", life);
            outtextxy(950, 150, life_text);
        }


    }

    player get_highest_score(const string& mode) {
        ifstream file("record.txt");
        if (!file.is_open()) {
            cerr << "�޷����ļ� record.txt" << endl;
            return { "", "", 0 }; // ����һ��Ĭ�ϵ� player �ṹ��
        }

        vector<player> records;
        string version, name;
        int score;

        // ��ȡ�ļ��е����ݲ��洢
        while (file >> version >> name >> score) {
            if (version == mode) {
                records.push_back({ name, version, score });
            }
        }
        file.close();

        // ���û���ҵ���Ӧģʽ�ļ�¼������Ĭ��ֵ
        if (records.empty()) {
            return { "", "", 0 };
        }

        // �ҵ�������ߵļ�¼
        auto it = max_element(records.begin(), records.end(), [](const player& a, const player& b) {
            return a.Score < b.Score; // �ȽϷ��������ظ���ļ�¼
            });

        return *it; // ������߷ֵļ�¼
    }

    void draw_chuji_rule()
    {
        settextcolor(BLACK);
        settextstyle(50, 0, "Consolas");
        outtextxy(1000, 200, "��Ϸ����");

        settextstyle(30, 0, "Consolas");
        settextcolor(YELLOW);
        outtextxy(1000, 250, "��ɫ");
        settextcolor(BLACK);
        outtextxy(1060, 250, "�����㽶��ÿ�γԵ�+15�֣�����+1");

        settextcolor(LIGHTRED);
        outtextxy(1000, 300, "��ɫ");
        settextcolor(BLACK);
        outtextxy(1060, 300, "����ƻ����ÿ�γԵ�+10�֣�����+2");

        settextcolor(BLUE);
        outtextxy(1000, 350, "��ɫ");
        settextcolor(BLACK);
        outtextxy(1060, 350, "����ҩ��ÿ�γԵ�-10�֣�����-1");

        settextcolor(GREEN);
        outtextxy(1000, 400, "��ɫ");
        settextcolor(BLACK);
        outtextxy(1060, 400, "������ͷ����ʼ����Ϊ1");


        outtextxy(1060, 450, "�������ⷽ�����ʼ��Ϸ����ײ��");
        outtextxy(1060, 500, "�Լ�����ײ��ǽ��Ϸ����");
    }


    void draw_duoren_rule()
    {
        settextcolor(WHITE);
        settextstyle(50, 0, "Consolas");
        outtextxy(1000, 200, "��Ϸ����");

        settextstyle(30, 0, "Consolas");
      //  settextcolor(YELLOW);
        outtextxy(1000, 250, "��ɫ");
       // settextcolor(BLACK);
        outtextxy(1060, 250, "�������1��ͨ��WASD���Ʒ���");

        settextcolor(MAGENTA);
        outtextxy(1000, 300, "��ɫ");
        settextcolor(WHITE);
        outtextxy(1060, 300, "�������2��ͨ�������������Ʒ���");

        outtextxy(1060, 350, "�������ⷽ�����ʼ��Ϸ����ײ��");
        outtextxy(1060, 400, "�Է�����ײ��ǽ��Ϸ����");
    }
    void draw_jijie_rule()
    {
        settextcolor(BLACK);
        settextstyle(40, 0, "Consolas");
        outtextxy(1000, 200, "��Ϸ����"); 

        settextstyle(30, 0, "Consolas");
        outtextxy(1000, 250, "��ײǽ֮�����ǽ,ֱ��û���㹻��");
        outtextxy(1000, 300, "�ռ������߻���ʳ��Ϊֹ");
    }


    void draw_gaoji_rule()
    {
        settextcolor(WHITE);
        settextstyle(40, 0, "Consolas");
        outtextxy(1000, 200, "��Ϸ����");

        settextstyle(30, 0, "Consolas");
        outtextxy(1000, 250, "��ײǽ֮�����ʳ��,ֱ��û���㹻��");
        outtextxy(1000, 300, "�ռ�������Ϊֹ,��ֻ��5��������ģʽ��");
        outtextxy(1000, 350, "�߿���ײ���Լ��������þ���Ϸ����");
    }

    void write_record_to_file(const player& Player, const string& filename = "record.txt")
    {
        // ���ļ���׷��ģʽд���¼�¼
        ofstream outfile(filename, ios::app); // ios::app ȷ��׷��д��
        if (!outfile.is_open()) {
            cerr << "Failed to open the file: " << filename << endl;
            return;
        }

        // д���¼�¼
        outfile << Player.Version << " " << Player.Name << " " << Player.Score << '\n';

        // �ر��ļ�
        outfile.close();
        cout << "Record written to " << filename << ": "
            << Player.Version << " " << Player.Name << " " << Player.Score << endl;
    }

    void draw_chuji_end(int& score,Snake& class_snake,int& maxscore)
    {
        settextcolor(BLACK);
        settextstyle(30, 0, "Consolas");
        outtextxy(1000, 300, "��Ϸ����!");

        outtextxy(1000, 360, "���ķ�����");
        char last_score_text[50];
        sprintf_s(last_score_text, "%d", score);
        outtextxy(1170, 360, last_score_text);

        outtextxy(1000, 420, "���ĳ��ȣ�");
        char len_text[50];
        sprintf_s(len_text, "%d", class_snake.snake.size());
        outtextxy(1170, 420, len_text);

        outtextxy(1000, 480, "��߷֣�");
        char max_text[50];
        sprintf_s(max_text, "%d", maxscore);
        outtextxy(1170, 480, max_text);
        outtextxy(1000, 540, "3���ص�������");
    }


    void draw_jinjie_end(int& score, Snake& class_snake, int& maxscore,int& tmp_len,COLORREF color)
    {

        settextcolor(color);
        settextstyle(30, 0, "Consolas");
        outtextxy(1000, 300, "��Ϸ����!");

        outtextxy(1000, 360, "���ķ�����");
        char last_score_text[50];
        sprintf_s(last_score_text, "%d", score);
        outtextxy(1170, 360, last_score_text);

        outtextxy(1000, 420, "���ĳ��ȣ�");
        char len_text[50];
        sprintf_s(len_text, "%d", tmp_len);
        outtextxy(1170, 420, len_text);

        outtextxy(1000, 480, "��߷֣�");
        char max_text[50];
        sprintf_s(max_text, "%d", maxscore);
        outtextxy(1170, 480, max_text);


        outtextxy(1000, 540, "�����κη�������¿�ʼ��Ϸ");
        outtextxy(1000, 600, "���¿�ʼ��Ϸ֮��������");
        outtextxy(1000, 660, "����ESC���˳���Ϸ");
    }

    void draw_gaoji_end(int& score, Snake& class_snake, int& maxscore, int& tmp_len, COLORREF color,int life)
    {

        settextcolor(color);
        settextstyle(30, 0, "Consolas");
        outtextxy(1000, 300, "��Ϸ����!");

        outtextxy(1000, 360, "���ķ�����");
        char last_score_text[50];
        sprintf_s(last_score_text, "%d", score);
        outtextxy(1170, 360, last_score_text);

        outtextxy(1000, 420, "���ĳ��ȣ�");
        char len_text[50];
        sprintf_s(len_text, "%d", tmp_len);
        outtextxy(1170, 420, len_text);

        outtextxy(1000, 480, "��ʷ��߷֣�");
        char max_text[50];
        sprintf_s(max_text, "%d", maxscore);
        outtextxy(1170, 480, max_text);

        outtextxy(1000, 540, "ʣ����������");
        char life_text[50];
        sprintf_s(life_text, "%d", life);
        outtextxy(1170, 540, life_text);


        outtextxy(1000, 600, "�����κη�������¿�ʼ��Ϸ");
        outtextxy(1000, 660, "���¿�ʼ��Ϸ֮��������");
        outtextxy(1000, 720, "����ESC���˳���Ϸ");
    }
    void draw_duoren_end( Snake& snake1, Snake& snake2)
    {

        settextcolor(WHITE);
        settextstyle(30, 0, "Consolas");
        outtextxy(1000, 300, "��Ϸ����!");

        outtextxy(1000, 360, "���1�ķ�����");
        char score_text1[50];
        sprintf_s(score_text1, "%d", snake1.s_score);
        outtextxy(1170, 360, score_text1);

        outtextxy(1000, 420, "���1�ĳ��ȣ�");
        char len_text1[50];
        sprintf_s(len_text1, "%d", snake1.snake.size());
        outtextxy(1170, 420, len_text1);

        outtextxy(1000, 480, "���2�ķ�����");
        char score_text2[50];
        sprintf_s(score_text1, "%d", snake2.s_score);
        outtextxy(1170, 480, score_text1);

        outtextxy(1000, 540, "���2�ĳ��ȣ�");
        char len_text2[50];
        sprintf_s(len_text2, "%d", snake2.snake.size());
        outtextxy(1170, 540, len_text2);
        outtextxy(1000, 600, "5���ص�������");
    }


    void draw_score_and_time(int game_time,Snake snake1,Snake snake2,COLORREF color)
    {
        settextcolor(color);
        settextstyle(30, 0, "Consolas");

        char length_text[50];
        int len = snake1.snake.size();
        sprintf_s(length_text, "���1����: %d", len);
        outtextxy(950, 30, length_text);

        char score_text1[50];
        sprintf_s(score_text1, "���1����: %d", snake1.s_score);
        outtextxy(950, 60, score_text1);


        char length_text2[50];
        int len2 = snake2.snake.size();
        sprintf_s(length_text2, "���2����: %d", len2);
        outtextxy(950, 90, length_text2);

        char score_text2[50];
        sprintf_s(score_text2, "���2����: %d", snake2.s_score);
        outtextxy(950, 120, score_text2);

        char time_text[50];
        sprintf_s(time_text, "ʱ��: %d s", game_time);
        outtextxy(950, 150, time_text);

    }
    void find_and_print_records(const string& target_name) {
        // �� record.txt �ļ�
        ifstream file("record.txt");
        if (!file.is_open()) {
            cerr << "�޷��򿪼�¼�ļ���" << endl;
            return;
        }

        vector<player> matching_records; // ���ڴ洢ƥ��ļ�¼
        string version, name;
        int score;

        // ��ȡ record.txt �ļ�����
        while (file >> version >> name >> score) {
            if (name == target_name) {
                matching_records.push_back({ name, version, score });
            }
        }
        file.close();

        // ����Ƿ���ƥ���¼
        if (matching_records.empty()) {
            cout << "δ�ҵ������� \"" << target_name << "\" ƥ��ļ�¼��" << endl;
        }
        else {
            cout << "�ҵ����¼�¼��" << endl;
            for (const auto& record : matching_records) {
                cout << "��Ϸ�汾: " << record.Version
                    << ", ����: " << record.Name
                    << ", ����: " << record.Score << endl;
            }
        }
    }

};

class Information Info;

class Game_chuji
{
protected:
    FoodManager food_manager;
    Snake class_snake;
    WallManager wall_manager;
    ExMessage  msg1 = { 0 };
    int is_eat = 0;
    int score = 0;
    int DELAY = 90;

    player Player;
public:
    Game_chuji(ExMessage t_msg,player p)
    {
        msg1 = t_msg;
        Player = p;
    }
    void Pause_Until_Keypress()
    {
        bool start_game = false;
        while (1)
        {
            peekmessage(&msg1, EX_KEY);
            if (msg1.message == WM_KEYDOWN)
            {
                
                switch (msg1.vkcode) 
                {
                case 'w': case 'W': case VK_UP:
                    class_snake.snake.front().dir = UP; start_game = true;
                    break;
                case 's': case 'S': case VK_DOWN:
                    class_snake.snake.front().dir = DOWN; start_game = true;
                    break;
                case 'a': case 'A': case VK_LEFT:
                    class_snake.snake.front().dir = LEFT;   start_game = true;
                    break;
                case 'd': case 'D': case VK_RIGHT:
                    class_snake.snake.front().dir = RIGHT;  start_game = true;
                    break;
                default:
                    break;
                }
            }
            if (start_game)break;
        }

    }
    void update_speed()
    {
        if (score >= 0 && score <= 20)
        {
            DELAY = 90;
        }
        else if (score > 20 && score <= 40)
        {
            DELAY = 80;

        }
        else if (score > 40 && score <= 60)
        {
            DELAY = 70;

        }
        else if (score > 60 && score <= 80)
        {
            DELAY = 60;

        }
        else if (score > 80 && score <= 100)
        {
            DELAY = 50;

        }
        else if (score > 100)
        {
            DELAY = 40;

        }
    }

    int Check_snake_eat_foods_chuji()
    {
        for (auto it = food_manager.foods.begin(); it != food_manager.foods.end(); ++it)
        {
            if (it->f_x == class_snake.snake.front().x && it->f_y == class_snake.snake.front().y)
            {
                FoodType type = it->type;
                food_manager.foods.erase(it);

                switch (type) {
                case BANANA:
                    return 1;  // �㽶������+1
                case APPLE:
                    return 2;  // ƻ��������+2
                case POISON:
                    return 3; // ��ҩ������-1,ʹ��3����
                default:
                    return 0;
                }
            }
        }
        return 0;
    }
    void Generate_Foods_chuji()
    {
        food_manager.foods.clear();

        int b_food_count = 1 + rand() % 5;

        for (int i = 0; i < b_food_count; ++i)
        {

            while (true)
            {
                int x = 1 + rand() % 28;
                int y = 1 + rand() % 28;
                int fx = x * GRID_SIZE + 15;
                int fy = y * GRID_SIZE + 15;

                bool in_snake = class_snake.in_snakes(fx, fy);// false;

                bool in_food = food_manager.in_foods(fx, fy);//in_foods(food_manager.foods, fx, fy);

                if (!in_snake && !in_food && !wall_manager.check_collision_with_wall(fx, fy))
                {
                    food_manager.foods.push_back(Food(fx, fy, BANANA));
                    cout << "banaa:" << fx<<" " << fy << endl;
                    break;
                }
            }
        }

        int a_food_count = 1 + rand() % 5;
        for (int i = 0; i < a_food_count; ++i)
        {

            while (true)
            {
                int x = 1 + rand() % 28;
                int y = 1 + rand() % 28;
                int fx = x * GRID_SIZE + 15;
                int fy = y * GRID_SIZE + 15;

                bool in_snake = class_snake.in_snakes(fx, fy);// false;

                bool in_food = food_manager.in_foods(fx, fy);

                if (!in_snake && !in_food &&!wall_manager.check_collision_with_wall(fx, fy))
                {
                    food_manager.foods.push_back(Food(fx, fy, APPLE));
                    cout << "apple:" << fx<<" " << fy << endl;
                    break;
                }
            }
        }

        int p_food_count = 1 + rand() % 5;
        for (int i = 0; i < p_food_count; ++i)
        {

            while (true)
            {
                int x = 1 + rand() % 28;
                int y = 1 + rand() % 28;
                int fx = x * GRID_SIZE + 15;
                int fy = y * GRID_SIZE + 15;

                bool in_snake = class_snake.in_snakes(fx, fy);// false;

                bool in_food = food_manager.in_foods(fx, fy);

                if (!in_snake&&!in_food  && !wall_manager.check_collision_with_wall(fx, fy))
                {
                    food_manager.foods.push_back(Food(fx, fy, POISON));
                    cout << "poison:" << fx<<" " << fy << endl;
                    break;
                }
            }
        }

    }
    bool is_Overss()
    {
        // ����ߵĳ���Ϊ0����ȫ��ʧ������Ϸ����
        if (class_snake.snake.empty()) {
            return true;
        }

        const Snakebody& head = class_snake.snake.front();
        // ����Ƿ�ײǽ
        if (wall_manager.check_collision_with_wall(head.x, head.y)) {
            return true;
        }

        // ����Ƿ�ײ���Լ�
        for (auto iter = class_snake.snake.begin() + 1; iter != class_snake.snake.end(); ++iter) {
            if (head.x == iter->x && head.y == iter->y) {
                return true;
            }
        }

        return false;
    }
    void update_snake_chuji()
    {
        bool is_live = true;
        // �����µ���ͷ
        Snakebody new_head = class_snake.snake.front();
        switch (new_head.dir)
        {
        case UP:    new_head.y -= GRID_SIZE; break;
        case DOWN:  new_head.y += GRID_SIZE; break;
        case LEFT:  new_head.x -= GRID_SIZE; break;
        case RIGHT: new_head.x += GRID_SIZE; break;
        }
        class_snake.snake.push_front(new_head);

        // ���ݳԵ���ʳ�����ʹ����ߵĳ���
        if (is_eat == 0)  // û�Ե�ʳ��
        {
            class_snake.snake.pop_back();
        }
        else if (is_eat == 1)  // �Ե��㽶������+1
        {
            score += 5;
            // ����Ҫ pop_back������ԭ����β�����൱�ڳ���+1
            is_eat = 0;
        }
        else if (is_eat == 2)  // �Ե�ƻ��������+2
        {
            score += 10;
            Snakebody new_tail = class_snake.snake.back();
            int new_tail_x = new_tail.x;
            int new_tail_y = new_tail.y;

            switch (new_tail.dir)
            {
            case UP:    new_tail.y += GRID_SIZE; break;
            case DOWN:  new_tail.y -= GRID_SIZE; break;
            case LEFT:  new_tail.x += GRID_SIZE; break;
            case RIGHT: new_tail.x -= GRID_SIZE; break;
            }
            // �����β��λ����Ч���Ͳ�����½ڵ㣬�൱��ֻ����1��
            if (wall_manager.is_position_validss(new_tail.x, new_tail.y)) {
                class_snake.snake.push_back(new_tail);
            }
            is_eat = 0;
        }
        else if (is_eat == 3)  // �Ե���ҩ����-1 
        {
            score -= 10;
            class_snake.snake.pop_back();  // ���Ƴ�һ�ڣ�����push_front��

            // ����߳���Ϊ1�ҳԵ���ҩ��������ߣ���Ϸ������
            if (class_snake.snake.size() == 1)
            {
                class_snake.snake.clear();  // ����ߣ�ʹ�䳤�ȱ�Ϊ0
                is_live = false;
                //cout << "end" << endl;
            }
            else if (class_snake.snake.size() > 1)
            {
                class_snake.snake.pop_back();  // ���Ƴ�һ�ڣ���ҩЧ����
            }
            is_eat = 0;
        }
        if (is_live)
        {
            // ��������ÿһ�ڵķ���
            for (size_t i = class_snake.snake.size() - 1; i > 0; --i)
            {
                class_snake.snake[i].dir = class_snake.snake[i - 1].dir;
            }
        }

    }

    void Draw_snake_and_food_and_info(WallManager& wall_manager,Snake& c_snake,FoodManager& food_manager)
    {
        wall_manager.draw_walls();
        food_manager.draw_all_food();
        if(!c_snake.snake.empty())
        c_snake.draw_snake(GREEN);
    }


    void run_game()
    {

        flushmessage(EX_KEY);

        setlinecolor(RGB(155, 174, 230));
        cleardevice();

        putimage(0, 0, &background1);

        int maxscore;
        player max_player;
        max_player = Info.get_highest_score("���Ű�");
        maxscore = max_player.Score;

        class_snake.generate_random_snake();

        Generate_Foods_chuji();

        Draw_snake_and_food_and_info(wall_manager, class_snake, food_manager);//����

        Info.draw_chuji_rule();//���ƹ���

        Pause_Until_Keypress();//�ȴ���������

        clock_t start_time = clock();
        score = 0;
        DELAY = 90;
        while (1)
        {
            //BeginBatchDraw();
            // 1. ���ȼ��ʹ�������
            peekmessage(&msg1, EX_KEY);
            class_snake.change_direction(msg1);
            // 2. �����ߵ�λ��
            update_snake_chuji();
            // 3. �����ײ
            if (is_Overss())
            {

                BeginBatchDraw();
                cleardevice();
               

                putimage(0, 0, &background1);
                Info.draw_chuji_end(score, class_snake, maxscore);
                Draw_snake_and_food_and_info(wall_manager, class_snake, food_manager);

                flushmessage(EX_KEY);
                FlushBatchDraw();
                EndBatchDraw();
                Sleep(3000);
                break;  
            }
            // 4. ����Ƿ�Ե�ʳ��
            is_eat = Check_snake_eat_foods_chuji();
            // 5. ����Ƿ���Ҫ������ʳ��
            if (food_manager.no_any_kind_food())  Generate_Foods_chuji();
            //����Ƿ���Ҫ����DELAY
            if (is_eat) update_speed();
            // 6. ��������Ԫ��
            BeginBatchDraw();
            cleardevice();
            putimage(0, 0, &background1);
            Draw_snake_and_food_and_info(wall_manager, class_snake, food_manager);
            int game_time = (clock() - start_time) / CLOCKS_PER_SEC;

            if (score >= maxscore)
            {
                maxscore = score;
                Info.draw_score_and_time(score, game_time, maxscore,0, class_snake,BLACK);
            }
            else
            {
                Info.draw_score_and_time(score, game_time, maxscore, 0, class_snake,BLACK);
            }

            FlushBatchDraw();
            Sleep(DELAY);
        }
        EndBatchDraw();

        Player.Score = score;
        Player.Version = "���Ű�";

        food_manager.foods.clear();
        wall_manager.walls.clear();
        wall_manager.refresh_wall_manager();
        class_snake.snake.clear();
        score = 0;
        Info.write_record_to_file(Player, "record.txt");
    }
};


class Game_jinjie :public Game_chuji 
{
public:
    Game_jinjie(ExMessage msg2, player p) :Game_chuji(msg2, p) {};
    void Generate_Foods_jinjie(bool clear_foods_vector)
    {
        if(clear_foods_vector)food_manager.foods.clear();
        
        int b_food_count = 1 + rand() % 10;

        for (int i = 0; i < b_food_count; ++i)
        {
            while (true)
            {
                int x = 1 + rand() % 28;
                int y = 1 + rand() % 28;
                int fx = x * GRID_SIZE + 15;
                int fy = y * GRID_SIZE + 15;

                bool in_snake = class_snake.in_snakes(fx, fy);// false;

                bool in_food = food_manager.in_foods(fx, fy);//in_foods(food_manager.foods, fx, fy);
                if (!in_snake && !in_snake && !wall_manager.check_collision_with_wall(fx, fy))
                {
                    food_manager.foods.push_back(Food(fx, fy, BANANA));
                    break;
                }
            }
        }
    }
    void update_snakes_jinjie()
    {
        // �����µ���ͷ
        Snakebody new_head = class_snake.snake.front();
        switch (new_head.dir)
        {
        case UP:    new_head.y -= GRID_SIZE; break;
        case DOWN:  new_head.y += GRID_SIZE; break;
        case LEFT:  new_head.x -= GRID_SIZE; break;
        case RIGHT: new_head.x += GRID_SIZE; break;
        }
        class_snake.snake.push_front(new_head);

        // ���ݳԵ���ʳ�����ʹ����ߵĳ���
        if (is_eat == 0)  // û�Ե�ʳ��
        {
            class_snake.snake.pop_back();
        }
        else if (is_eat == 1)  // �Ե��㽶������+1
        {
            score += 5;
            // ����Ҫ pop_back������ԭ����β�����൱�ڳ���+1
            is_eat = 0;
        }
    }

    void pause_until_keypress_for_restart(deque<Snakebody>& snake, ExMessage& msg) 
    {
        // �ȴ��������룬ֱ����Ұ��¼���ʱ�ſ�ʼ��һ����Ϸ
        while (true)
        {
            peekmessage(&msg, EX_KEY);
            if (msg.message == WM_KEYDOWN)
            {
                switch (msg.vkcode)
                {
                case 'w': case 'W': case VK_UP:snake.front().dir = UP; return;
                case 's': case 'S': case VK_DOWN:snake.front().dir = DOWN; return;
                case 'a': case 'A': case VK_LEFT:snake.front().dir = LEFT; return;
                case 'd': case 'D': case VK_RIGHT:snake.front().dir = RIGHT; return;
                case VK_ESCAPE: return;
                default:
                    break;
                }
            }
        }
    }
    void run_game()
    {
        flushmessage(EX_KEY);
        setlinecolor(RGB(155, 174, 230));
        cleardevice();

        putimage(0, 0, &background3);
        int maxscore;

        player max_player;

        max_player = Info.get_highest_score("���װ�");

        maxscore = max_player.Score;

        class_snake.generate_random_snake();
        Game_jinjie::Generate_Foods_jinjie(true);

        Draw_snake_and_food_and_info(wall_manager,class_snake,food_manager);
        Info.draw_jijie_rule();
        Pause_Until_Keypress();

        clock_t start_time = clock();

        while (true)
        {
            //�������
            peekmessage(&msg1, EX_KEY);
            //�ı䷽��
            class_snake.change_direction(msg1);
            //����״̬
            Game_jinjie::update_snakes_jinjie();
            //����Ƿ����
            if (is_Overss())
            {
                int tmp_len = class_snake.snake.size();
                DELAY = 90;
                wall_manager.add_wall_from_snake(class_snake.snake);

                class_snake.generate_random_snake();
                Game_jinjie::Generate_Foods_jinjie(true);

                if (food_manager.foods.size() + 1 >= 900 - wall_manager.walls.size())
                {
                    cleardevice();
                    Draw_snake_and_food_and_info(wall_manager, class_snake, food_manager);

                    settextcolor(RED);
                    settextstyle(30, 0, "Consolas");
                    outtextxy(1000, 400, "Sorry! ��Ϸ��Ľ����ˣ��޷����ɸ����ʳ��");

                    EndBatchDraw();
                    Sleep(3000);
                    break;  
                }
                cleardevice();


                putimage(0, 0, &background3);
                Info.draw_jinjie_end( score,  class_snake,  maxscore,  tmp_len,BLACK);
                Draw_snake_and_food_and_info(wall_manager, class_snake, food_manager);


                FlushBatchDraw();
                EndBatchDraw();
                flushmessage(EX_KEY);
             
                pause_until_keypress_for_restart(class_snake.snake, msg1);
                if (msg1.vkcode == VK_ESCAPE) break;
                //class_snake.change_direction(msg1);
                start_time = clock();  // ���ü�ʱ��
                score = 0;
                continue;
            }
            //����Ƿ�Ե�ʳ��
            is_eat = Check_snake_eat_foods_chuji();
            //����Ƿ���Ҫ����ʳ��
            if (food_manager.foods.empty())  Game_jinjie::Generate_Foods_jinjie(true);
            //�����ٶ�
            if (is_eat) update_speed();
            //��ʼ����
            BeginBatchDraw();
            cleardevice();

            putimage(0, 0, &background3);
            Draw_snake_and_food_and_info(wall_manager, class_snake, food_manager);
            int game_time = (clock() - start_time) / CLOCKS_PER_SEC;
            //���·���
            if (score >= maxscore)
            {
                maxscore = score;
                Info.draw_score_and_time(score, game_time, maxscore, 0, class_snake,BLACK);
            }
            else
            {
                Info.draw_score_and_time(score, game_time, maxscore, 0, class_snake,BLACK);
            }
            FlushBatchDraw();
     
            Sleep(DELAY);
        }
        EndBatchDraw();
        Player.Score = score;
        Player.Version = "���װ�";
        food_manager.foods.clear();
        wall_manager.walls.clear();
        wall_manager.refresh_wall_manager();
        class_snake.snake.clear();
        Info.write_record_to_file(Player, "record.txt");
    }
};

class Game_gaoji :public Game_jinjie
{
private:
    int life;
public:
    Game_gaoji(ExMessage msg2, player p) :Game_jinjie(msg2, p) {};
    bool Is_Over_gaoji()
    {
        Snakebody& head = class_snake.snake.front();//front();
        if (wall_manager.check_collision_with_wall(head.x, head.y)) return true;
        return false;
    }


    void run_game()
    {
        int maxscore;

        player max_player;

        max_player = Info.get_highest_score("�߼���");

        maxscore = max_player.Score;


        life = 5;
        flushmessage(EX_KEY);
        setlinecolor(RGB(155, 174, 230));
        cleardevice();
        putimage(0, 0, &background4);
        class_snake.generate_random_snake();
        Game_jinjie::Generate_Foods_jinjie(false);
        Draw_snake_and_food_and_info(wall_manager, class_snake, food_manager);
        Info.draw_gaoji_rule();
        Pause_Until_Keypress();

        clock_t start_time = clock();
       
        while (true)
        {
            //�������
            peekmessage(&msg1, EX_KEY);
            //�ı䷽��
            class_snake.change_direction(msg1);
            //����״̬
            Game_jinjie::update_snakes_jinjie();
            //����Ƿ����
            if (Game_gaoji::Is_Over_gaoji())
            {
                int tmp_len = class_snake.snake.size();
                life--;
                food_manager.add_food_from_snake(class_snake.snake, wall_manager);
                class_snake.generate_random_snake();
                Game_jinjie::Generate_Foods_jinjie(false);
                if (food_manager.foods.size() + 1 >= 900 - wall_manager.walls.size() || life == 0)
                {
                    cleardevice();
                    putimage(0, 0, &background4);
                    Draw_snake_and_food_and_info(wall_manager, class_snake, food_manager);
                    settextcolor(RED);
                    settextstyle(30, 0, "Consolas");
                    outtextxy(1000, 400, "Sorry! ��Ϸ��Ľ����ˣ������Ѿ�");
                    EndBatchDraw();
                    Sleep(3000);
                    break;  // �˳���ѭ��
                }
                cleardevice();
                putimage(0, 0, &background4);
                Info.draw_gaoji_end(score, class_snake, maxscore, tmp_len,WHITE,life);
                Draw_snake_and_food_and_info(wall_manager, class_snake, food_manager);
                EndBatchDraw();
                pause_until_keypress_for_restart(class_snake.snake, msg1);
                if (msg1.vkcode == VK_ESCAPE) break;
                start_time = clock();  // ���ü�ʱ��
                score = 0;
                continue;
            }
            //����Ƿ�Ե�ʳ��
            is_eat = Check_snake_eat_foods_chuji();
            //����Ƿ���Ҫ����ʳ��
            if (food_manager.foods.empty())  Game_jinjie::Generate_Foods_jinjie(true);
            //�����ٶ�
            if (is_eat) update_speed();
            //��ʼ����
            BeginBatchDraw();
            cleardevice();
            putimage(0, 0, &background4);
            Draw_snake_and_food_and_info(wall_manager, class_snake, food_manager);
            int game_time = (clock() - start_time) / CLOCKS_PER_SEC;
            //���·���
            if (score >= maxscore)
            {
                maxscore = score;
                Info.draw_score_and_time(score, game_time, maxscore, life, class_snake,WHITE);
            }
            else
            {
                Info.draw_score_and_time(score, game_time, maxscore, life, class_snake,WHITE);
            }
            FlushBatchDraw();

            Sleep(DELAY);
        }
        EndBatchDraw();
        Player.Score = score;
        Player.Version = "�߼���";
        food_manager.foods.clear();
        wall_manager.walls.clear();
        wall_manager.refresh_wall_manager();
        class_snake.snake.clear();
        Info.write_record_to_file(Player, "record.txt");
    }
};

class Game_duoren {
private:
    Snake snake1;       // ��1����
    int score1 = 0;     // ��1����ҵ÷�
    int is_eat1 = 0;    // ��1������Ƿ�Ե�ʳ��

    Snake snake2;       // ��2����
    int score2 = 0;     // ��2����ҵ÷�
    int is_eat2 = 0;    // ��2������Ƿ�Ե�ʳ��

    FoodManager food_manager;
    WallManager wall_manager;
    ExMessage  msg1 = { 0 };

    int is_eat = 0;
    int DELAY = 90;

public:
    Game_duoren(ExMessage t_msg)//,player p1)
    {
        msg1 = t_msg;
    }
    void Pause_Until_Keypress()
    {
        bool start_game = false;
        bool start_game1 = false;
        while (1)
        {
            peekmessage(&msg1, EX_KEY);
            if (msg1.message == WM_KEYDOWN)
            {
                switch (msg1.vkcode)
                {
                case 'w': case 'W':
                    snake1.snake.front().dir = UP; start_game = true;
                    break;
                case 's': case 'S':
                    snake1.snake.front().dir = DOWN; start_game = true;
                    break;
                case 'a': case 'A':
                    snake1.snake.front().dir = LEFT;   start_game = true;
                    break;
                case 'd': case 'D':
                    snake1.snake.front().dir = RIGHT;  start_game = true;
                    break;
                case VK_UP:
                    snake2.snake.front().dir = UP; start_game = true;
                    break;
                case VK_DOWN:
                    snake2.snake.front().dir = DOWN; start_game = true;
                    break;
                case VK_LEFT:
                    snake2.snake.front().dir = LEFT;   start_game = true;
                    break;
                case VK_RIGHT:
                    snake2.snake.front().dir = RIGHT;  start_game = true;
                    break;
                default:
                    break;
                }
            }
            if (start_game)break;
        }

    }

    void update_speed(int score)
    {
        score = max(score1, score2);
        if (score >= 0 && score <= 20)
        {
            DELAY = 90;
        }
        else if (score > 20 && score <= 40)
        {
            DELAY = 80;

        }
        else if (score > 40 && score <= 60)
        {
            DELAY = 70;

        }
        else if (score > 60 && score <= 80)
        {
            DELAY = 60;

        }
        else if (score > 80 && score <= 100)
        {
            DELAY = 50;

        }
        else if (score > 100)
        {
            DELAY = 40;

        }
    }


    void Generate_Foods_duoren()
    {
        food_manager.foods.clear();

        int b_food_count = 1 + rand() % 10;

        for (int i = 0; i < b_food_count; ++i)
        {
            while (true)
            {
                int x = 1 + rand() % 28;
                int y = 1 + rand() % 28;
                int fx = x * GRID_SIZE + 15;
                int fy = y * GRID_SIZE + 15;

                bool in_snake1 = snake1.in_snakes(fx, fy);// false;
                bool in_snake2 = snake2.in_snakes(fx, fy);// false;

                bool in_food = food_manager.in_foods(fx, fy);//in_foods(food_manager.foods, fx, fy);

                if (!in_snake1 && !in_snake2 && !wall_manager.check_collision_with_wall(fx, fy))
                {
                    food_manager.foods.push_back(Food(fx, fy, BANANA));
                    break;
                }
            }
        }
    }
    int Check_snake_eat_foods_duoren(Snake& class_snake)
    {
        for (auto it = food_manager.foods.begin(); it != food_manager.foods.end(); ++it)
        {
            if (it->f_x == class_snake.snake.front().x && it->f_y == class_snake.snake.front().y)
            {
                FoodType type = it->type;
                food_manager.foods.erase(it);

                switch (type) 
                {
                case BANANA:
                    return 1;  // �㽶������+1
                case APPLE:
                    return 2;  // ƻ��������+2
                case POISON:
                    return 3; // ��ҩ������-1,ʹ��3����
                default:
                    return 0;
                }
            }
        }
        return 0;
    }
    bool is_Over_duoren()
    {
        const Snakebody& head1 = snake1.snake.front();
        const Snakebody& head2 = snake2.snake.front();

        // ײǽ
        if ( wall_manager.check_collision_with_wall(head2.x, head2.y)
            || wall_manager.check_collision_with_wall(head1.x, head1.y)
            ) 
        {
            return true;
        }

        // ������ײ
        for (const auto& body : snake1.snake) {
            if (head2.x == body.x && head2.y == body.y) return true;
        }
        for (const auto& body : snake2.snake) {
            if (head1.x == body.x && head1.y == body.y) return true;
        }

        return false;
    }
    void update_snakes_duoren(Snake& class_snake)
    {
        //bool is_live = true;
        // �����µ���ͷ
        Snakebody new_head = class_snake.snake.front();
        switch (new_head.dir)
        {
        case UP:    new_head.y -= GRID_SIZE; break;
        case DOWN:  new_head.y += GRID_SIZE; break;
        case LEFT:  new_head.x -= GRID_SIZE; break;
        case RIGHT: new_head.x += GRID_SIZE; break;
        }
        class_snake.snake.push_front(new_head);

        // ���ݳԵ���ʳ�����ʹ����ߵĳ���
        if (class_snake.is_eat == 0)  // û�Ե�ʳ��
        {
            class_snake.snake.pop_back();
        }
        else if (class_snake.is_eat == 1)  // �Ե��㽶������+1
        {
            class_snake.s_score += 5;
            // ����Ҫ pop_back������ԭ����β�����൱�ڳ���+1
            class_snake.is_eat = 0;
        }
    }

    
    void Draw_snake_and_food_and_info(WallManager& wall_manager, Snake& snake1, Snake& snake2, FoodManager& food_manager)
    {
        wall_manager.draw_walls();
        food_manager.draw_all_food();
        snake1.draw_snake(WHITE);
        snake2.draw_snake(MAGENTA);
    }

    void run_game()
    {
        msg1.message = { 0 };
        setlinecolor(RGB(155, 174, 230));
        cleardevice();

        putimage(0, 0, &background5);
        snake1.generate_random_snake();
       
        snake2.generate_random_snake(snake1);
       
        Generate_Foods_duoren();
        Game_duoren::Draw_snake_and_food_and_info(wall_manager, snake1,snake2, food_manager);
        Info.draw_duoren_rule();
        Pause_Until_Keypress();

        clock_t start_time = clock();
       int score = 0;
        DELAY = 90;
        while (1)
        {
            BeginBatchDraw();
            // 1. ���ȼ��ʹ�������
            peekmessage(&msg1, EX_KEY);


            snake1.change_direction1(msg1);
            snake2.change_direction2(msg1);
            // 2. �����ߵ�λ��
            update_snakes_duoren(snake1);
            update_snakes_duoren(snake2);
           // update_snake_chuji();
            // 3. �����ײ
            if (is_Over_duoren())
            {
                BeginBatchDraw();
                cleardevice();
                putimage(0, 0, &background5);

                Draw_snake_and_food_and_info(wall_manager, snake1, snake2, food_manager);
                Info.draw_duoren_end(snake1, snake2);

                msg1.message = { 0 };
                flushmessage(EX_KEY);
                FlushBatchDraw();
                EndBatchDraw();
                Sleep(5000);
                break;
            }
            snake1.is_eat = Check_snake_eat_foods_duoren(snake1);
            snake2.is_eat = Check_snake_eat_foods_duoren(snake2);
       
            if(food_manager.foods.empty())
            {
                Generate_Foods_duoren();
            }
            //����Ƿ���Ҫ����DELAY
            if (is_eat) update_speed(max(snake1.s_score,snake2.s_score));
            // 6. ��������Ԫ��
            BeginBatchDraw();
            cleardevice();
            putimage(0, 0, &background5);
            Draw_snake_and_food_and_info(wall_manager, snake1, snake2, food_manager);
            int game_time = (clock() - start_time) / CLOCKS_PER_SEC;
            Info.draw_score_and_time(game_time,snake1,snake2,WHITE);
          

            FlushBatchDraw();
            Sleep(DELAY);
        }
        EndBatchDraw();

        food_manager.foods.clear();
        wall_manager.walls.clear();
        wall_manager.refresh_wall_manager();
    }

};


int main()
{
    ExMessage msg = { 0 };
    initgraph(1500, 900);
    loadimage(&background, "14.jpg", 1500, 900);
    loadimage(&background1, "2.jpg", 1500, 900);
    loadimage(&background2, "1.jpg", 1500, 900);
    loadimage(&background3, "3.jpg", 1500, 900);
    loadimage(&background4, "4.jpg", 1500, 900);
    loadimage(&background5, "27.jpg", 1500, 900);
    putimage(0, 0, &background);
    InputBox(buffer, sizeof(buffer), "����¼�޷�������ϷŶ�����¼��", "�����������û�����", "���û�");
    player human;
    human.Name = buffer;
    cout << "name:" <<human.Name<< endl;

    Button button1(350, 140, 100, 50, "����ģʽ", BLUE);
    Button button2(350, 220, 100, 50, "����ģʽ", BLUE);
    Button button3(350, 300, 100, 50, "�߼�ģʽ", BLUE);
    Button button4(350, 380, 100, 50, "˫��ģʽ", BLUE);
    Button button5(350, 460, 100, 50, "���а�", BLUE);
    Button button6(350, 540, 100, 50, "�˳���Ϸ", BLUE);

    vector<Button> menu_button{ button1, button2, button3, button4, button5 ,button6};

    bool show_menu = true;    // ���ư�ť����ʾ
    bool show_ranking_flag = false; // �������а���ʾ
    bool play_chuji_game = false;
    bool play_jinjie_game = false;
    bool play_gaoji_game = false;
    bool play_duoren_game = false;
    Button return_button(50, 150, 100, 50, "����", BLUE);
    Button check_button(50, 230, 100, 50, "��ѯ�ɼ�", BLUE);
    class Game_chuji game1(msg,human);
    class Game_gaoji game3(msg,human);
    class Game_jinjie game2(msg,human);
    class Game_duoren game4(msg);
    while (1) 
    {
        if (peekmessage(&msg)) 
        {
            
           // BeginBatchDraw();
            if (show_menu) 
            {

                BeginBatchDraw();
                cleardevice();
                putimage(0, 0, &background);
                Draw_menu(menu_button, msg);
                EndBatchDraw();

                if (button5.click_button(msg)) 
                {
                    show_menu = false;
                    show_ranking_flag = true;
                    play_chuji_game = false;
                    play_jinjie_game = false;
                    play_gaoji_game = false;
                    play_duoren_game = false;
                }

                if (button1.click_button(msg))
                {
                    show_menu = false;
                    show_ranking_flag = false;
                    play_chuji_game = true;
                    play_jinjie_game = false;
                    play_gaoji_game = false;
                    play_duoren_game = false;
                }

                if (button2.click_button(msg))
                {
                    show_menu = false;
                    show_ranking_flag = false;//true;
                    play_chuji_game = false;//true;
                    play_jinjie_game = true;//false;
                    play_gaoji_game = false;
                    play_duoren_game = false;
                }
                if (button3.click_button(msg))
                {
                    show_menu = false;
                    show_ranking_flag = false;//true;
                    play_chuji_game = false;//true;
                    play_jinjie_game = false;//false;
                    play_gaoji_game = true;
                    play_duoren_game = false;
                }

                if (button4.click_button(msg))
                {
                    show_menu = false;
                    show_ranking_flag = false;//true;
                    play_chuji_game = false;//true;
                    play_jinjie_game = false;//false;
                    play_gaoji_game = false;
                    play_duoren_game = true;//false;
                }
            }

            if (show_ranking_flag)
            { 
                BeginBatchDraw();
                cleardevice();
                return_button.Draw_button(msg);
                check_button.Draw_button(msg);
                show_ranking();
               
               

                EndBatchDraw();
                if (return_button.click_button(msg)) 
                {
                    show_menu = true;
                    show_ranking_flag = false;
                    play_chuji_game = false;
                    play_jinjie_game = false;
                    play_gaoji_game = false;
                }

                if (check_button.click_button(msg))
                {
                    settextcolor(WHITE);
                    settextstyle(50, 0, "Consolas");
                    outtextxy(900, 150, "����cmd�����в鿴��ѯ���");
                    Info.find_and_print_records(human.Name);
                    show_menu = false;
                    show_ranking_flag = true;
                    play_chuji_game = false;
                    play_jinjie_game = false;
                    play_gaoji_game = false;
                }
            }

            if (play_chuji_game)
            {
                game1.run_game();
                cleardevice();
                show_menu = true;
                show_ranking_flag = false;
                play_chuji_game = false;
                play_jinjie_game = false;
                play_gaoji_game = false;
            }

            if (play_jinjie_game)
            {
                game2.run_game();
                cleardevice();
                show_menu = true;
                show_ranking_flag = false;
                play_chuji_game = false;
                play_jinjie_game = false;
                play_gaoji_game = false;
            }

            if (play_gaoji_game)
            {
                game3.run_game();
                cleardevice();
                show_menu = true;
                show_ranking_flag = false;
                play_chuji_game = false;
                play_jinjie_game = false;
                play_gaoji_game = false;
                play_duoren_game = false;
            }

            if (play_duoren_game)
            {
                game4.run_game();
                cleardevice();
                show_menu = true;
                show_ranking_flag = false;
                play_chuji_game = false;
                play_jinjie_game = false;
                play_gaoji_game = false;
                play_duoren_game = false;
            }

            if (button6.click_button(msg))
            {
                return 0;

            }
        }
    }

    closegraph();
    return 0;
}
