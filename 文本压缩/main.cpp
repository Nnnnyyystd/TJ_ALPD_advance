#include<bitset>
#include <iostream>
#include <string>
#include <map>
#include <time.h>
#include<fstream>
#include<stack>
#include<unordered_map>
#include<queue>
#include<ctime>
#include <sstream>
using namespace std;
const int MAXNUM = 600;
class Huff {
public:
    long long frq;
    char c;
    Huff* left;
    Huff* right;
    Huff* parent;
    string hcode;
    bool operator>(const Huff& other) const {
        return frq > other.frq;
    }
};
//一些宏变量
Huff* HT[MAXNUM];//哈夫曼数组，相当于树
Huff* H;//根节点
unordered_map<char, Huff>cnt;//字符和树的map表，便于查找
//count 相当于字符种类数目,map相当于散列表，cnt.size()不超过256，根据读入字符的ASCII码将其加入到map中
//count为字符串中字符的种类
void Initial(unordered_map<char, Huff>& cnt, Huff* HT[], int& count, ifstream& file)
{
    char ch;
    while (file.get(ch))
    {
        cnt[ch].frq++;
        cnt[ch].c = ch;
    }
    count = 0;
    for (int i = 0; i < cnt.size(); i++)
    {
        if (cnt[i].c != '\0')
        {
            count++;
            HT[count] = new Huff;
            HT[count]->c = cnt[i].c;
            HT[count]->frq = cnt[i].frq;
            HT[count]->left = NULL;
            HT[count]->right = NULL;
            HT[count]->parent = NULL;
        }
    }
}
void BuildTree(Huff* HT[], int& count, Huff*& H)
{
    for (int i = count + 1; i < 2 * count; i++)// Huffman树中应该有2*count-1个节点
    {
        int m1 = i - 1;//记录最小的节点的位置
        int m2 = i - 1;//记录第二小节点的位置
        int fr1 = 1e9;//记录最小的频数
        int fr2 = 1e9;//记录第二小的频数
        for (int j = 1; j < i; j++)
        {
            if (HT[j]->parent == NULL && HT[j]->frq < fr1)//找最小的节点
            {
                fr2 = fr1;
                fr1 = HT[j]->frq;
                m2 = m1;
                m1 = j;
            }
            else if (HT[j]->parent == NULL && HT[j]->frq < fr2)//找第二小的节点
            {
                fr2 = HT[j]->frq;
                m2 = j;
            }
        }
        HT[i] = new Huff;
        HT[i]->frq = HT[m1]->frq + HT[m2]->frq;
        HT[m1]->parent = HT[i];
        HT[m2]->parent = HT[i];
        HT[i]->left = HT[m1];// 左孩子指向最小的节点
        HT[i]->right = HT[m2];//右节点指向第二小的节点
        HT[i]->parent = NULL;
    }
    H = new Huff;
    H = HT[2 * count - 1];
}
void CreateHuffmanCodes(Huff* node, string str)
{
    if (node == nullptr) {
        return;
    }
    // 如果是叶子节点，则保存当前编码
    if (node->left == nullptr && node->right == nullptr)
    {
        node->hcode = str;
    }
    else {
        // 递归遍历左子树
        if (node->left) {
            CreateHuffmanCodes(node->left, str + "0");
        }
        // 递归遍历右子树
        if (node->right) {
            CreateHuffmanCodes(node->right, str + "1");
        }
    }
}
void Buildmap(unordered_map<char, Huff>& cnt, Huff* HT[], int& count)
{
    for (int i = 1; i <= count; i++)
    {
        cnt[HT[i]->c].hcode = HT[i]->hcode;
    }
}
unsigned char stringToByte(const string& bin)
{
    unsigned char c = 0;
    for (size_t i = 0; i < bin.size(); ++i)
    {
        c = (c << 1) | (bin[i] - '0');
    }
    return c;
}
void Compress(unordered_map<char, Huff>& cnt, string infilename, string outfilename, Huff* HT[], int& count)
{
    ifstream fin;
    fin.open(infilename, ios::binary);
    ofstream outfile(outfilename, ios::binary);
    if (!fin.is_open() || !outfile.is_open())
    {
        cout << "Failed to open orginal file" << endl;
    }
    outfile << count << endl;
    for (int i = 1; i <= count; i++)
    {
        outfile << int(HT[i]->c) << " " << HT[i]->frq << endl;
    }
    char ch;
    string buf = "";
    while (fin.get(ch))
    {
        string code = cnt[ch].hcode;
        buf += code;
        while (buf.length() >= 8)
        {
            unsigned char byte = stringToByte(buf.substr(0, 8));
            outfile.put(byte);
            buf.erase(0, 8);
        }
    }
    if (buf.length() > 0)
    {
        size_t paddingSize = 8 - buf.length();
        for (size_t i = 0; i < paddingSize; ++i) {
            buf += '0';
        }
        unsigned char byte = stringToByte(buf);
        outfile.put(byte);
    }
    fin.close();
    outfile.close();
}
void COMPRESS(string infilename, string outfilename)//, int& count)
{
    clock_t start, end;
    start = clock();
    ifstream infile(infilename, ios::binary);
    int count;
    //初始化哈夫曼树，读取源文件的信息，将信息加入哈夫曼树
    Initial(cnt, HT, count, infile);
    //构建哈夫曼树
    BuildTree(HT, count, H);
    cout << "构建哈夫曼树成功" << endl;
    string s;
    CreateHuffmanCodes(H, s);
    Buildmap(cnt, HT, count);
    Compress(cnt, infilename, outfilename, HT, count);
    for (int i = 1; i <= 2 * count - 1; i++)
    {
        delete HT[i];
    }
    end = clock();
    cout << "压缩时间：" << end - start << "毫秒" << endl;
}
long long getFileSize(const string& filePath)
{
    std::ifstream file(filePath, std::ifstream::ate | std::ifstream::binary);
    if (!file.is_open()) {
        std::cerr << "Unable to open file." << std::endl;
        return -1;
    }

    auto size = file.tellg();
    file.close();
    return size;
}
void Decompress(string compressname, string Newname)
{
    ifstream fin(compressname, ios::binary);
    ofstream newfile(Newname, ios::binary);
    if (!fin.is_open() || !newfile.is_open())
    {
        cout << "Failed to open files.\n";
        return;
    }
    int count = 0;
    string line;
    getline(fin, line);
    istringstream(line) >> count;
    for (int i = 1; i <= count; i++)
    {
        if (getline(fin, line))
        {
            istringstream iss(line);
            int key, value;
            if (iss >> key >> value)
            {
                HT[i] = new Huff; // Ensure HT[i] is initialized
                HT[i]->c = static_cast<char>(key);
                HT[i]->frq = value;
                HT[i]->left = nullptr;
                HT[i]->right = nullptr;
                HT[i]->parent = nullptr;
            }
        }
    }
    BuildTree(HT, count, H);
    cout << "重构哈夫曼树成功" << endl;
    string s;
    CreateHuffmanCodes(H, s);
    Buildmap(cnt, HT, count);
    string tmp = "";
    char ch;
    // 将压缩内容转化为01字符串，利用这个字符串进行操作
    while (fin.get(ch))
    {
        tmp += bitset<8>(ch).to_string(); // 每次读取一个字节并转换为二进制
    }
    Huff* Op = H; // 从根节点开始
    for (size_t i = 0; i < tmp.length(); ++i)
    {
        if (Op->left == nullptr && Op->right == nullptr) // 找到叶子节点
        {
            newfile.put(Op->c); // 输出字符
            Op = H; // 回到根节点
        }
        // 根据位选择左或右子树
        if (tmp[i] == '0' && Op->left != nullptr)
        {
            Op = Op->left;
        }
        else if (tmp[i] == '1' && Op->right != nullptr)
        {
            Op = Op->right;
        }
    }
    // 处理最后的叶子节点（如果循环结束时在叶子节点上）
    if (Op->left == nullptr && Op->right == nullptr)
    {
        newfile.put(Op->c);
    }
    long long a = getFileSize(compressname);
    long long b = getFileSize(Newname);
    double ori = static_cast<double>(a);
    double aft = static_cast<double>(b);
    //cout << orginal << " " << after << endl;
    cout << "压缩率为：" << (ori / aft) * 100 << "%\n";
    fin.close();
    newfile.close();
}
void DECOMPRESS(string compressname, string newname)//,int& count)
{
    clock_t start, end;
    start = clock();
    //cout << "dcmppp" << endl;
    Decompress(compressname, newname);
    end = clock();
    cout << "解压缩时间：" << end - start << "毫秒\n";
}
int main(int argc, char* argv[])
{
    if (argc != 4) { // 需要五个参数，包括程序名
        cout << "请输入四个有效的字符串，其中第一个为运行.exe文件的命令\n";
        cout << "第二个字符串为待操作文件，第三个字符串为输出文件\n";
        cout << "第四个字符串为操作字符串，zip为压缩命令，unzip为解压缩命令\n";
        return 0;
    }
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int count = 0;


    // 再次提醒，参数不是程序运行之后等待用户输入的，而是在cmd窗口执行exe时传入的
// 压缩例如下，其代表将src.xxx压缩，结果保存在dst.xxx
//   [你的程序路径] src.xxx dst.xxx zip 
// 解压缩例如下，其代表将src.xxx解压缩，结果保存在dst.xxx
//   [你的程序路径] src.xxx dst.xxx unzip 
    string file1 = argv[1];
    string file2 = argv[2];
    string opr = argv[3];


    if (opr == "zip") {
        // 调用压缩函数
        COMPRESS(file1, file2);//, count);
    }
    else if (opr == "unzip") {
        // 调用解压函数
        DECOMPRESS(file1, file2);//, count);
    }
    else {
        cout << "无效的操作类型，请使用 'cmp' 进行压缩或 'dcmp' 进行解压缩。" << endl;
        return 0;
    }
    return 0;
}