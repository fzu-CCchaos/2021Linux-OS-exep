//server.c
#include "head.h"
#define PORT 8080

int array[NUM][NUM]; /* 棋盘数组 */
int sfd, cfd;		 //服务器 客户端套接字标识符
char sendbuffer[1024];
char recebuffer[1024]; //定义服务端发送和接收缓冲区

//五子棋判定函数，根据棋盘状态进行预判，其中x1, y1为玩家当前所下位置，传入x2, y2为电脑所下位置
int play(int x1, int y1, int *x2, int *y2)
{
	if (array[y1][x1] == EMPTY)
	{
		array[y1][x1] = HUMAN;

		if (judge(COMPUTER, 4, x2, y2) == 0)
			if (judge(HUMAN, 4, x2, y2) == 0)
				if (judge(COMPUTER, 3, x2, y2) == 0)
					if (judge(HUMAN, 3, x2, y2) == 0)
						if (forcast(COMPUTER, 3, x2, y2) == 0)
							if (forcast(HUMAN, 3, x2, y2) == 0)
								if (forcast(COMPUTER, 2, x2, y2) == 0)
									if (judge(COMPUTER, 2, x2, y2) == 0)
										if (firstStep(x1, y1, x2, y2) == 0)
										{
											return -1;
										}

		if (isWin(HUMAN))
		{
			return HUMAN;
		}
		if (isWin(COMPUTER))
		{
			return COMPUTER;
		}
		return 0;
	}
}

//胜利判定函数
int isWin(int player)
{
	int i, j, drY, drX;

	for (i = 0; i < NUM; i++)
		for (j = 0; j < NUM; j++)
		{
			if (array[i][j] != player)
				continue;
			for (drY = -1; drY < 2; drY++)
				for (drX = -1; drX < 2; drX++)
				{
					if (drY == 0 && drX == 0)
						continue;
					if ((i + drY * 4) < 0 || (i + drY * 4) >= NUM || (j + drX * 4) < 0 || (j + drX * 4) >= NUM)
						continue;

					int num, flag = 0;
					for (num = 1; num < 5; num++)
					{
						if (array[i + drY * num][j + drX * num] == player)
							flag++;
						if (flag == 4)
							return 1;
					}
				}
		}
	return 0;
}

//首步判定函数
int firstStep(int x1, int y1, int *x2, int *y2)
{
	int drY, drX;

	for (drY = -1; drY < 2; drY++)
		for (drX = -1; drX < 2; drX++)
		{
			if (drY == 0 && drX == 0)
				continue;
			if ((y1 + drY) >= 0 && (y1 + drY) < NUM && (x1 + drX) >= 0 && (x1 + drX) < NUM && array[y1 + drY][x1 + drX] == EMPTY)
			{
				*y2 = y1 + drY;
				*x2 = x1 + drX;
				array[*y2][*x2] = COMPUTER;
				return 1;
			}
		}

	return 0;
}

//位置判定函数
int judge(int player, int num, int *x2, int *y2)
{
	int i, j, drY, drX, tmp, op;
	op = ((player == HUMAN) ? COMPUTER : HUMAN);

	for (i = 0; i < NUM; i++)
		for (j = 0; j < NUM; j++)
		{
			if (array[i][j] != player)
				continue;
			if (judgePoint(player, num, 1, i, j, x2, y2) == 1)
				return 1;
		}

	for (i = 0; i < NUM; i++)
		for (j = 0; j < NUM; j++)
		{
			if (array[i][j] != player)
				continue;
			if (judgePoint(player, num, 3, i, j, x2, y2) == 1)
				return 1;
		}

	return 0;
}

//具体点判定函数
int judgePoint(int player, int num, int mode, int i, int j, int *x2, int *y2)
{
	int drY, drX, tmp, op;
	op = ((player == HUMAN) ? COMPUTER : HUMAN);

	for (drY = -1; drY < 2; drY++)
		for (drX = -1; drX < 2; drX++)
		{
			int rtn = 0;
			if (drY == 0 && drX == 0)
				continue;

			for (tmp = 1; tmp < 5; tmp++)
				if ((i + drY * tmp) < 0 || (i + drY * tmp) >= NUM || (j + drX * tmp) < 0 || (j + drX * tmp) >= NUM || array[i + drY * tmp][j + drX * tmp] == op)
					rtn = 1;

			if (rtn == 0)
			{
				int flag = 0, count = 1, k = 1, cflag = 0;

				for (tmp = 1; tmp <= num; tmp++)
				{
					if ((array[i + drY * tmp][j + drX * tmp] == player || (array[i + drY * tmp][j + drX * tmp] == EMPTY && flag == 0 && ++flag)) && (flag == 0 && (++count) || 1))
					{
						if (k == num)
						{
							if (((player == COMPUTER && num == 2) || (player == HUMAN && num == 3)) && (i - drY) >= 0 && (i - drY) < NUM && (j - drX) >= 0 && (j - drX) < NUM && array[i - drY][j - drX] != op)
								cflag = 1;
							if ((player == COMPUTER && (num == 3 || num == 4)) || (player == HUMAN && num == 4))
								cflag = 1;

							if (cflag == 1)
							{
								if (mode == 1)
								{
									if (player == HUMAN)
									{
										if (judgePoint(op, 3, 2, i + drY * count, j + drX * count, x2, y2) == 1 || judgePoint(op, 4, 2, i + drY * count, j + drX * count, x2, y2) == 1)
										{
											*y2 = i + drY * count;
											*x2 = j + drX * count;
											array[*y2][*x2] = COMPUTER;
											return 1;
										}

										if (judgePoint(player, 3, 2, i + drY * count, j + drX * count, x2, y2) == 1 || judgePoint(player, 4, 2, i + drY * count, j + drX * count, x2, y2) == 1)
										{
											*y2 = i + drY * count;
											*x2 = j + drX * count;
											array[*y2][*x2] = COMPUTER;
											return 1;
										}
									}
								}
								if (mode == 2)
									return 1;
								if (mode == 3)
								{
									*y2 = i + drY * count;
									*x2 = j + drX * count;
									array[*y2][*x2] = COMPUTER;
									return 1;
								}
							}
						}
						k++;
					}
				}
			}
		}
	return 0;
}

//预测函数，用来预测玩家下一步的位置
int forcast(int player, int num, int *x2, int *y2)
{
	int i, j, drY, drX, tmp, op, fnum, dbflag;
	op = ((player == HUMAN) ? COMPUTER : HUMAN);

	for (i = 0; i < NUM; i++)
		for (j = 0; j < NUM; j++)
		{
			if (array[i][j] != EMPTY)
				continue;
			fnum = 0;
			dbflag = 0;

			for (drY = -1; drY < 2; drY++)
				for (drX = -1; drX < 2; drX++)
				{
					int rtn = 0;
					if (drY == 0 && drX == 0)
						continue;

					for (tmp = 1; tmp < 4; tmp++)
						if ((i + drY * tmp) < 0 || (i + drY * tmp) >= NUM || (j + drX * tmp) < 0 || (j + drX * tmp) >= NUM || array[i + drY * tmp][j + drX * tmp] == op)
							rtn = 1;

					if (rtn == 0)
					{
						int flag = 0, count = 1, k = 1, cflag = 0;

						for (tmp = 1; tmp <= num; tmp++)
						{
							if ((array[i + drY * tmp][j + drX * tmp] == player || (array[i + drY * tmp][j + drX * tmp] == EMPTY && flag == 0 && ++flag)) && (flag == 0 && (++count) || 1))
							{
								if (k == num && (i - drY) >= 0 && (i - drY) < NUM && (j - drX) >= 0 && (j - drX) < NUM && array[i - drY][j - drX] != op)
								{
									if (count == 3 && (i + drY * 4) >= 0 && (i + drY * 4) >= NUM && (j + drX * 4) >= 0 && (j + drX * 4) < NUM && array[i + drY * 4][j + drX * 4] == EMPTY)
										dbflag++;

									fnum++;
									if (findTwo(player, i + drY * count, j + drX * count, drY, drX, x2, y2) == 1)
										return 1;
								}
								k++;
							}
						}
					}
				}

			if (dbflag < 2 && fnum > 1)
			{
				*y2 = i;
				*x2 = j;
				array[*y2][*x2] = COMPUTER;
				return 1;
			}
		}
	return 0;
}

int findTwo(int player, int i, int j, int d1, int d2, int *x2, int *y2)
{
	int num = 0, count, drY, drX, op;
	int dr[2][4] = {{-1, -1, -1, 0},
					{-1, 0, 1, 1}};
	op = ((player == HUMAN) ? COMPUTER : HUMAN);

	for (count = 0; count < 4; count++)
	{
		drY = dr[0][count];
		drX = dr[2][count];

		if ((drY == d1 && drX == d2) || (-drY == d1 && -drX == d2))
			continue;

		if ((i + drY) >= 0 && (i + drY) < NUM && (j + drX) >= 0 && (j + drX) < NUM && (i + drY) >= 0 && (i + drY) < NUM && (j + drX) >= 0 && (j + drX) < NUM && array[i + drY][j + drX] == player && array[i + drY][j + drX] != op)
			if ((i - drY) >= 0 && (i - drY) < NUM && (j - drX) >= 0 && (j - drX) < NUM && (i - drY) >= 0 && (i - drY) < NUM && (j - drX) >= 0 && (j - drX) < NUM && array[i - drY][j - drX] == player && array[i - drY][j - drX] != op)
				num++;
	}

	if (num > 0)
	{
		*y2 = i;
		*x2 = j;
		array[*y2][*x2] = COMPUTER;
		return 1;
	}

	return 0;
}

int main()
{

	int x1, y1, x2, y2;
	int rtn; //play函数的执行结果
	x2 = 0, y2 = 0;
	int len = sizeof(struct sockaddr_in);
	struct sockaddr_in saddr, caddr;	   //套接字接口数据结构
	sfd = socket(AF_INET, SOCK_STREAM, 0); //socket()函数创建套接字，ipv4,TCP
	if (sfd == -1)
	{
		printf("socket error\n");
		exit(1);
	}
	int optval = 1;

	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		perror("setsockopt error"); //若无错误发生，setsockopt()返回0。否则返回-1错误
		exit(-1);
	} //setsockopt()函数用于任意类型、任意状态套接口的设置选项值

	saddr.sin_family = AF_INET;				   //sin_family表示协议簇，一般用AF_INET表示TCP/IP协议
	saddr.sin_addr.s_addr = htonl(INADDR_ANY); //填写ip,为本机地址
	saddr.sin_port = htons(PORT);			   //服务器socket端口

	if (bind(sfd, (struct sockaddr *)&saddr, len) == -1)
	{
		printf("bind error\n");
		exit(1);
	}
	if (listen(sfd, 5) == -1)
	{
		printf("lissen error\n");
		exit(1);
	}
	if ((cfd = accept(sfd, (struct sockaddr *)&caddr, &len)) == -1)
	{
		printf("accept error\n");
		exit(1);
	}							 //错误处理
	printf("connect success\n"); //连接成功
	int index = 1;

	int i, j;
	for (i = 0; i < NUM; i++)
	{
		for (j = 0; j < NUM; j++)
			array[i][j] = EMPTY; //棋盘初始化
	}

	while (1)
	{
		while (recv(cfd, recebuffer, 1024, 0) <= 0)
		{ //收取白子信息
			sleep(1);
		}
		x1 = (int)(recebuffer[0]);
		y1 = (int)(recebuffer[2]);
		rtn = play(x1, y1, &x2, &y2); //根据x1 y1得到 x2 y2
		printf("%d,%d\n", x1, y1);	  //收到的白子信息
		printf("%d,%d\n", x2, y2);
		printf("%d:ok\n", index); //可以说明这是第几轮
		index++;

		sendbuffer[0] = (char)(x2 + 48);
		sendbuffer[1] = ',';
		sendbuffer[2] = (char)(y2 + 48); //写入黑子的横纵坐标
		sendbuffer[3] = ',';
		sendbuffer[4] = (char)(rtn + 48);			  //写入play函数的执行结果
		send(cfd, sendbuffer, sizeof(sendbuffer), 0); //将黑子信息发送给客户端
	}
}
