/*
 * iec103.h 包含该tcp/ip套接字编程所需要的基本头文件
*/



#define	MAX_LINE	256
const int TCP_CLI_PORT = 6001;
const int UDP_SER_PORT = 6002;
const int BACKLOG = 10;
const int LISTENQ = 6666;
const int MAX_CONNECT = 20;




struct SUI
{
	unsigned char au;
	unsigned char bu;
	unsigned char cu;
	unsigned char ai;
	unsigned char bi;
	unsigned char ci;
};//系统电压与系统电流

struct S_Q
{
	unsigned char sz;
	unsigned char yg;
	unsigned char wg;
	unsigned char ys;
};//系统视在公率、有功功率、无功功率、功率因数

struct S_STA
{
	unsigned char z1;
	unsigned char z2;
	unsigned char z3;
	unsigned char z4;
	unsigned char z5;
	unsigned char z6;
	unsigned char z7;
	unsigned char z8;
};//系统运行状态

struct YYUI
{
	unsigned char ai;
	unsigned char bi;
	unsigned char ci;
	unsigned char yu;
};//有源模块电压与输出电流

struct IL_ID
{
	unsigned char la;
	unsigned char lb;
	unsigned char lc;
	unsigned char da;
	unsigned char db;
	unsigned char dc;
};//负载电流与装置电流

struct PQCOS
{
	unsigned char pa;
	unsigned char pb;
	unsigned char pc;
	unsigned char qa;
	unsigned char qb;
	unsigned char qc;
	unsigned char cosa;
	unsigned char cosb;
	unsigned char cosc;
};//三相有功功率、无功功率、功率因数

struct THD
{
	unsigned char ua;
	unsigned char ub;
	unsigned char uc;
	unsigned char ia;
	unsigned char ib;
	unsigned char ic;
};//三相电压畸变率与电流畸变率

struct SU_J_X
{
	unsigned char jb;
	unsigned char x3;
	unsigned char x5;
	unsigned char x7;
	unsigned char x9;
	unsigned char x11;
	unsigned char x13;
	unsigned char x15;
	unsigned char x17;
	unsigned char x19;
	unsigned char x21;
	unsigned char x23;
	unsigned char x25;
	unsigned char x27;
	unsigned char x29;
	unsigned char x31;
	unsigned char x33;
	unsigned char x35;
	unsigned char x37;
	unsigned char x39;
	unsigned char x41;
	unsigned char x43;
	unsigned char x45;
	unsigned char x47;
	unsigned char x49;
};//系统电压基波与谐波

struct SI_J_X
{
	unsigned char jb;
	unsigned char x3;
	unsigned char x5;
	unsigned char x7;
	unsigned char x9;
	unsigned char x11;
	unsigned char x13;
	unsigned char x15;
	unsigned char x17;
	unsigned char x19;
	unsigned char x21;
	unsigned char x23;
	unsigned char x25;
	unsigned char x27;
	unsigned char x29;
	unsigned char x31;
	unsigned char x33;
	unsigned char x35;
	unsigned char x37;
	unsigned char x39;
	unsigned char x41;
	unsigned char x43;
	unsigned char x45;
	unsigned char x47;
	unsigned char x49;
};//系统电流基波与谐波

struct WYI
{
	unsigned char a;
	unsigned char b;
	unsigned char c;
};//无源模块输出电流

struct YY_STA
{
	unsigned char za;
	unsigned char zb;
	unsigned char zc;
	unsigned char zd;
	unsigned char ze;
	unsigned char zf;
	unsigned char zg;
	unsigned char zh;
	unsigned char zi;
	unsigned char zj;
};//有源模块运行状态

struct KG_STA
{
	unsigned char a1;
	unsigned char b1;
	unsigned char c1;
	unsigned char a2;
	unsigned char b2;
	unsigned char c2;
	unsigned char a3;
	unsigned char b3;
	unsigned char c3;
	unsigned char a4;
	unsigned char b4;
	unsigned char c4;
};//无源电容器开关状态

struct GZ_STA
{
	unsigned char a1;
	unsigned char b1;
	unsigned char c1;
	unsigned char a2;
	unsigned char b2;
	unsigned char c2;
	unsigned char a3;
	unsigned char b3;
	unsigned char c3;
	unsigned char a4;
	unsigned char b4;
	unsigned char c4;
};//无源电容器故障状态

struct TX_STA
{
	unsigned char z1_2;
	unsigned char z3_4;
	unsigned char z5_6;
	unsigned char z7_8;
};//无源电容器通讯状态

struct Dedicated_data
{
	struct SUI	data1;//系统电压与电流
	struct S_Q	data2;//系统功率等
	unsigned char f;//电网频率
	struct S_STA	data3;//系统运行状态
};//专用数据

struct General_data
{
	struct YYUI	data1;//有源模块电压电流
	struct IL_ID	data2;//负载电流与装置电流
	struct PQCOS	data3;//有功功率与无功功率与功率因数
	struct THD	data4;//畸变率
	struct SU_J_X	data5;//电压基波与谐波
	struct SU_J_X	data6;//电流基波与谐波
	struct WYI	data7;//无源电流
	struct YY_STA	data8;//有源状态
	struct KG_STA	data9;//电容器开关状态
	struct GZ_STA	data10;//电容器故障状态
	struct TX_STA	data11;//电容器通讯状态
};//通用数据





