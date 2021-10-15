//client.c
#include "head.h"
#define PORT 8080
int array[NUM][NUM]; /* 棋盘数组 */
int x1, y1, x2, y2;
int sockf; //客户端套接字标识符
char sendbuffer[1024];
char recebuffer[1024];

//以下为GTK窗口组件
GtkWidget *window;
GtkWidget *fixed;
GtkWidget *drawing_area;
GtkWidget *label;
extern int array[NUM][NUM];
int winflag = 0;

//函数实现棋盘绘制与刷新
gint area_redraw(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	int i, j;
	GdkColor color;
	GdkDrawable *canvas;
	GdkGC *gc;
	canvas = widget->window;
	gc = widget->style->fg_gc[GTK_WIDGET_STATE(widget)];

	//背景绘制
	color.red = 2855;
	color.green = 65535;
	color.blue = 47545;
	gdk_gc_set_rgb_fg_color(gc, &color);
	for (i = 0; i < DRAWING_AREA_WIDTH; i++)
		gdk_draw_line(canvas, gc, 0, i, DRAWING_AREA_WIDTH, i);

	//棋盘网格绘制
	color.red = 25723;
	color.green = 26985;
	color.blue = 5140;
	gdk_gc_set_rgb_fg_color(gc, &color);
	for (i = 0; i <= DRAWING_AREA_WIDTH; i += SIZE)
		gdk_draw_line(canvas, gc, i, 0, i, DRAWING_AREA_HEIGHT);
	for (i = 0; i <= DRAWING_AREA_HEIGHT; i += SIZE)
		gdk_draw_line(canvas, gc, 0, i, DRAWING_AREA_WIDTH, i);

	//双方棋子绘制
	for (i = 0; i < NUM; i++)
		for (j = 0; j < NUM; j++)
		{
			//玩家方棋子绘制，白子
			if (array[i][j] == HUMAN)
			{
				color.red = 65535;
				color.green = 65535;
				color.blue = 65535;
				gdk_gc_set_rgb_fg_color(gc, &color);
				gdk_draw_arc(canvas, gc, TRUE, ROWTOPOS(j), ROWTOPOS(i), CMANSIZE, CMANSIZE, 0, 360 * 64);
				gdk_draw_arc(canvas, gc, FALSE, ROWTOPOS(j), ROWTOPOS(i), CMANSIZE, CMANSIZE, 0, 360 * 64);
			}
			//电脑方棋子绘制，黑子
			if (array[i][j] == COMPUTER)
			{
				color.red = 10000;
				color.green = 10000;
				color.blue = 10000;
				gdk_gc_set_rgb_fg_color(gc, &color);
				gdk_draw_arc(canvas, gc, TRUE, ROWTOPOS(j), ROWTOPOS(i), CMANSIZE, CMANSIZE, 0, 360 * 64);
				gdk_draw_arc(canvas, gc, FALSE, ROWTOPOS(j), ROWTOPOS(i), CMANSIZE, CMANSIZE, 0, 360 * 64);
			}
		}

	color.red = 0;
	color.green = 0;
	color.blue = 0;
	gdk_gc_set_rgb_fg_color(gc, &color);

	return TRUE;
}

//函数实现点击事件绑定，并触发胜负判定
gint area_click(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	int ran_num, rtn;
	rtn = 10;
	char words[MAXSIZE + 10];
	char msg[MAXSIZE];
	static int count = 0;

	x1 = y1 = x2 = y2 = 0;
	GdkModifierType state;
	GtkWidget *dialog;
	GdkColor color;
	GdkDrawable *canvas;
	GdkGC *gc;
	canvas = widget->window;
	gc = widget->style->fg_gc[GTK_WIDGET_STATE(widget)];

	count++;
	gdk_window_get_pointer(widget->window, &x1, &y1, &state); //获取鼠标指针的坐标
															  //客户机：  把x1 y1 发给服务器
	//服务器：  x1,y1是套接字读取的

	sendbuffer[0] = (char)(CURTOROW(x1));
	sendbuffer[1] = ',';
	sendbuffer[2] = (char)(CURTOROW(y1));
	send(sockf, sendbuffer, sizeof(sendbuffer), 0); //将消息发送出去

	//当不满足胜利条件时，将棋子信息发送到服务器端进行判定
	if (winflag == 0)
	{
		//客户机：把x2 y2 换成套接字读入的x2 y2
		//服务器：把x2 y2发给客户端

		while (recv(sockf, recebuffer, 1024, 0) <= 0)
		{
			sleep(1); //不断等待直到收到回复
		}
		x2 = (int)(recebuffer[0] - 48);
		y2 = (int)(recebuffer[2] - 48);
		rtn = (int)(recebuffer[4] - 48);
		printf("%d,%d\n", x2, y2);
	}

	if (rtn < 0)
	{
		memset(words, 0, sizeof(words));
		snprintf(words, sizeof(words) - 1, "[电脑]说：请重新开始吧！");
		gtk_label_set_text(label, words);

		/*显示对话框*/
		memset(msg, 0, sizeof(msg));
		snprintf(msg, sizeof(msg) - 1, "可能已平局？！");
		dialog = gtk_message_dialog_new(GTK_WINDOW(window), 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, msg);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	}
	//未达到停止条件，进行棋子的绘制，并加入超时提示
	if (winflag == 0 && rtn == 0)
	{
		color.red = 65535;
		color.green = 65535;
		color.blue = 65535;
		gdk_gc_set_rgb_fg_color(gc, &color);
		gdk_draw_arc(canvas, gc, TRUE, CURTOPOS(x1), CURTOPOS(y1), CMANSIZE, CMANSIZE, 0, 360 * 64);
		gdk_draw_arc(canvas, gc, FALSE, CURTOPOS(x1), CURTOPOS(y1), CMANSIZE, CMANSIZE, 0, 360 * 64);

		color.red = 10000;
		color.green = 10000;
		color.blue = 10000;
		gdk_gc_set_rgb_fg_color(gc, &color);
		gdk_draw_arc(canvas, gc, TRUE, ROWTOPOS(x2), ROWTOPOS(y2), CMANSIZE, CMANSIZE, 0, 360 * 64);
		gdk_draw_arc(canvas, gc, FALSE, ROWTOPOS(x2), ROWTOPOS(y2), CMANSIZE, CMANSIZE, 0, 360 * 64);

		srand((unsigned)time(0) + count);
		memset(words, 0, sizeof(words));
		snprintf(words, sizeof(words) - 1, "Game is in process,player holds white chess");
		gtk_label_set_text(label, words);
	}
	//玩家胜利提示
	if (winflag == 0 && rtn == HUMAN)
	{
		winflag = 1;
		srand((unsigned)time(0) + count);
		memset(words, 0, sizeof(words));
		snprintf(words, sizeof(words) - 1, "Congratulations!You win this game!");
		gtk_label_set_text(label, words);

		/*显示对话框*/
		memset(msg, 0, sizeof(msg));
		snprintf(msg, sizeof(msg) - 1, "[玩家] 获胜\n共下了 [%d] 步棋", count);
		dialog = gtk_message_dialog_new(GTK_WINDOW(window), 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, msg);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	}
	//电脑胜利提示
	if (winflag == 0 && rtn == COMPUTER)
	{
		winflag = 1;
		srand((unsigned)time(0) + count);
		memset(words, 0, sizeof(words));
		snprintf(words, sizeof(words) - 1, "再接再厉，下次一定");
		gtk_label_set_text(label, words);

		/*显示对话框*/
		memset(msg, 0, sizeof(msg));
		snprintf(msg, sizeof(msg) - 1, "[电脑] 获胜\n共 [%d] 步棋", count);
		dialog = gtk_message_dialog_new(GTK_WINDOW(window), 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, msg);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	}
	return TRUE;
}

int main(int argc, char **argv)
{
	gtk_set_locale();
	gtk_init(&argc, &argv);

	int newsockf;
	struct sockaddr_in addr; //存储远程服务器的ip与端口号地址
	int optval = 1;
	sockf = socket(AF_INET, SOCK_STREAM, 0); //socket()函数创建套接字，ipv4,TCP
	if (setsockopt(sockf, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		perror("setsockopt error"); //若无错误发生，setsockopt()返回0。否则返回-1错误
		exit(1);
	}																   //setsockopt()函数用于任意类型、任意状态套接口的设置选项值
	addr.sin_family = AF_INET;										   //sin_family表示协议簇，一般用AF_INET表示TCP/IP协议
	addr.sin_addr.s_addr = htonl(INADDR_ANY);						   //填写ip
	addr.sin_port = htons(PORT);									   //客户端的socket端口
	newsockf = connect(sockf, (struct sockaddr *)&addr, sizeof(addr)); //发送连接服务器的请求
	if (newsockf == -1)
	{
		perror("connect failed");
		return 1;
	}

	//GTK创建新窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_usize(window, WINWIDTH, WINHEIGHT);
	gtk_container_set_border_width(GTK_CONTAINER(window), WINDOW_BORDER_WIDTH);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(window), "Five-in-a-row");
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(window));

	//创建棋盘区域
	drawing_area = gtk_drawing_area_new();
	gtk_widget_set_size_request(drawing_area, DRAWING_AREA_WIDTH, DRAWING_AREA_HEIGHT);

	//实现棋盘绘制，点击棋盘的事件绑定
	g_signal_connect(G_OBJECT(drawing_area), "expose_event", G_CALLBACK(area_redraw), NULL);
	gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK); //鼠标点击
	g_signal_connect(GTK_OBJECT(drawing_area), "button_press_event", GTK_SIGNAL_FUNC(area_click), (gpointer)drawing_area);

	//创建提示信息，并装入容器
	label = gtk_label_new(" ");
	gtk_widget_set_size_request(label, LABELWIDTH, LABELHEIGHT);
	gtk_label_set_line_wrap(label, TRUE);

	fixed = gtk_fixed_new();
	gtk_widget_set_usize(fixed, 20, 20);
	gtk_fixed_put(GTK_FIXED(fixed), label, 0, 0);
	gtk_fixed_put(GTK_FIXED(fixed), drawing_area, 0, LABELHEIGHT + 5);
	gtk_container_add(GTK_CONTAINER(window), fixed);

	int i, j;
	for (i = 0; i < NUM; i++)
	{
		for (j = 0; j < NUM; j++)
			array[i][j] = EMPTY; //棋盘初始化
	}

	//GTK界面显示

	gtk_widget_show_all(window);
	gtk_main();
	close(sockf);
	return 0;
}
