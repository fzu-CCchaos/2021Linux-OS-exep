#include <gtk/gtk.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/socket.h>  // 包含套接字函数库
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>	 // 包含AF_INET相关结构
#include <arpa/inet.h>	 // 包含AF_INET相关操作的函数
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
//#include <math.h>  //加上了这个之后，makefile里要加上-lm

#define HUMAN                1                       /* 玩 家    */
#define COMPUTER             2                       /* 电 脑    */
#define EMPTY                3                       /* 空 子    */
#define NUM                  16                      /* 棋盘行数 */
#define MAXSIZE              300                     /* 字符数组 */
#define SIZE                 28                      /* 棋子空间 */
#define CMANSIZE             (SIZE-10)               /* 棋子尺寸 */
#define WINDOW_BORDER_WIDTH  10                      /* 窗口空隙 */
#define DRAWING_AREA_WIDTH   (NUM*SIZE+1)            /* 棋盘宽度 */
#define DRAWING_AREA_HEIGHT  (NUM*SIZE+1)            /* 棋盘高度 */
#define LABELWIDTH           DRAWING_AREA_WIDTH      /* 标签宽度 */
#define LABELHEIGHT          20                      /* 标签高度 */
#define WINWIDTH             (DRAWING_AREA_WIDTH+20) /* 窗口高度 */
#define WINHEIGHT            (DRAWING_AREA_HEIGHT+LABELHEIGHT+25)

#define CURTOPOS(x) (x/SIZE*SIZE+(SIZE-CMANSIZE)/2)  /* 鼠标点击转换绘制棋子坐标 */
#define CURTOROW(x) (x/SIZE)                         /* 鼠标点击转换行数         */
#define ROWTOPOS(x) (x*SIZE+(SIZE-CMANSIZE)/2)       /* 行数转换绘制棋子坐标     */

int play(int, int, int *, int *);                    /* 游戏接口     */
int isWin(int );                                     /* 判断获胜     */
int firstStep(int, int, int *, int *);               /* 首步棋       */
int judge(int, int, int *, int *);                   /* 判断单行连子 */
int judgePoint(int, int , int, int , int , int *, int *);
int forcast(int, int, int *, int *);                 /* 预测双行连子 */
int findTwo(int, int, int, int, int, int *, int *);  /* 辅助forcast  */
//int isIllegal(int , int , int );                     /* 违反游戏规则 */
