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
//һЩ�����
Huff* HT[MAXNUM];//���������飬�൱����
Huff* H;//���ڵ�
unordered_map<char, Huff>cnt;//�ַ�������map�����ڲ���
//count �൱���ַ�������Ŀ,map�൱��ɢ�б�cnt.size()������256�����ݶ����ַ���ASCII�뽫����뵽map��
//countΪ�ַ������ַ�������
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
    for (int i = count + 1; i < 2 * count; i++)// Huffman����Ӧ����2*count-1���ڵ�
    {
        int m1 = i - 1;//��¼��С�Ľڵ��λ��
        int m2 = i - 1;//��¼�ڶ�С�ڵ��λ��
        int fr1 = 1e9;//��¼��С��Ƶ��
        int fr2 = 1e9;//��¼�ڶ�С��Ƶ��
        for (int j = 1; j < i; j++)
        {
            if (HT[j]->parent == NULL && HT[j]->frq < fr1)//����С�Ľڵ�
            {
                fr2 = fr1;
                fr1 = HT[j]->frq;
                m2 = m1;
                m1 = j;
            }
            else if (HT[j]->parent == NULL && HT[j]->frq < fr2)//�ҵڶ�С�Ľڵ�
            {
                fr2 = HT[j]->frq;
                m2 = j;
            }
        }
        HT[i] = new Huff;
        HT[i]->frq = HT[m1]->frq + HT[m2]->frq;
        HT[m1]->parent = HT[i];
        HT[m2]->parent = HT[i];
        HT[i]->left = HT[m1];// ����ָ����С�Ľڵ�
        HT[i]->right = HT[m2];//�ҽڵ�ָ��ڶ�С�Ľڵ�
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
    // �����Ҷ�ӽڵ㣬�򱣴浱ǰ����
    if (node->left == nullptr && node->right == nullptr)
    {
        node->hcode = str;
    }
    else {
        // �ݹ����������
        if (node->left) {
            CreateHuffmanCodes(node->left, str + "0");
        }
        // �ݹ����������
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
    //��ʼ��������������ȡԴ�ļ�����Ϣ������Ϣ�����������
    Initial(cnt, HT, count, infile);
    //������������
    BuildTree(HT, count, H);
    cout << "�������������ɹ�" << endl;
    string s;
    CreateHuffmanCodes(H, s);
    Buildmap(cnt, HT, count);
    Compress(cnt, infilename, outfilename, HT, count);
    for (int i = 1; i <= 2 * count - 1; i++)
    {
        delete HT[i];
    }
    end = clock();
    cout << "ѹ��ʱ�䣺" << end - start << "����" << endl;
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
    cout << "�ع����������ɹ�" << endl;
    string s;
    CreateHuffmanCodes(H, s);
    Buildmap(cnt, HT, count);
    string tmp = "";
    char ch;
    // ��ѹ������ת��Ϊ01�ַ�������������ַ������в���
    while (fin.get(ch))
    {
        tmp += bitset<8>(ch).to_string(); // ÿ�ζ�ȡһ���ֽڲ�ת��Ϊ������
    }
    Huff* Op = H; // �Ӹ��ڵ㿪ʼ
    for (size_t i = 0; i < tmp.length(); ++i)
    {
        if (Op->left == nullptr && Op->right == nullptr) // �ҵ�Ҷ�ӽڵ�
        {
            newfile.put(Op->c); // ����ַ�
            Op = H; // �ص����ڵ�
        }
        // ����λѡ�����������
        if (tmp[i] == '0' && Op->left != nullptr)
        {
            Op = Op->left;
        }
        else if (tmp[i] == '1' && Op->right != nullptr)
        {
            Op = Op->right;
        }
    }
    // ��������Ҷ�ӽڵ㣨���ѭ������ʱ��Ҷ�ӽڵ��ϣ�
    if (Op->left == nullptr && Op->right == nullptr)
    {
        newfile.put(Op->c);
    }
    long long a = getFileSize(compressname);
    long long b = getFileSize(Newname);
    double ori = static_cast<double>(a);
    double aft = static_cast<double>(b);
    //cout << orginal << " " << after << endl;
    cout << "ѹ����Ϊ��" << (ori / aft) * 100 << "%\n";
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
    cout << "��ѹ��ʱ�䣺" << end - start << "����\n";
}
int main(int argc, char* argv[])
{
    if (argc != 4) { // ��Ҫ�������������������
        cout << "�������ĸ���Ч���ַ��������е�һ��Ϊ����.exe�ļ�������\n";
        cout << "�ڶ����ַ���Ϊ�������ļ����������ַ���Ϊ����ļ�\n";
        cout << "���ĸ��ַ���Ϊ�����ַ�����zipΪѹ�����unzipΪ��ѹ������\n";
        return 0;
    }
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int count = 0;


    // �ٴ����ѣ��������ǳ�������֮��ȴ��û�����ģ�������cmd����ִ��exeʱ�����
// ѹ�������£������src.xxxѹ�������������dst.xxx
//   [��ĳ���·��] src.xxx dst.xxx zip 
// ��ѹ�������£������src.xxx��ѹ�������������dst.xxx
//   [��ĳ���·��] src.xxx dst.xxx unzip 
    string file1 = argv[1];
    string file2 = argv[2];
    string opr = argv[3];


    if (opr == "zip") {
        // ����ѹ������
        COMPRESS(file1, file2);//, count);
    }
    else if (opr == "unzip") {
        // ���ý�ѹ����
        DECOMPRESS(file1, file2);//, count);
    }
    else {
        cout << "��Ч�Ĳ������ͣ���ʹ�� 'cmp' ����ѹ���� 'dcmp' ���н�ѹ����" << endl;
        return 0;
    }
    return 0;
}