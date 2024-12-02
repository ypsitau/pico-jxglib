//==============================================================================
// TEK4010.cpp
//==============================================================================
#include "jxglib/TEK4010.h"

namespace jxglib {

void Tek_Graph(Printable& printable, int vector_no)
{
	printable.PutChar(0x1D); //GS
	printable.PutChar(0x1B); //ESC
	printable.PutChar(vector_no|0x60); //線種を選ぶ
}

#if 1
void Tek_Line(Printable& printable, int xs,int ys,int xg,int yg,int line_no)
{
	unsigned char x0,y0,x1,y1;
	Tek_Graph(printable, line_no); //グラフィックモードに移行(線種を指定)

	x0=(unsigned char)(xs&0x1F); //下位5byteを取り出す
	x0=x0|0x40; //Tag-Bitをセット
	y0=(unsigned char)(ys&0x1F); //下位5byteを取り出す
	y0=y0|0x60; //Tag-Bitをセット

	x1=(unsigned char)((xs>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
	x1=x1|0x20; //Tag-Bitをセット
	y1=(unsigned char)((ys>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
	y1=y1|0x20; //Tag-Bitをセット

	printable.PutChar(y1); //座標送信
	printable.PutChar(y0); //座標送信
	printable.PutChar(x1); //座標送信
	printable.PutChar(x0); //座標送信


	x0=(unsigned char)(xg&0x1F); //下位5byteを取り出す
	x0=x0|0x40; //Tag-Bitをセット
	y0=(unsigned char)(yg&0x1F); //下位5byteを取り出す
	y0=y0|0x60; //Tag-Bitをセット

	x1=(unsigned char)((xg>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
	x1=x1|0x20; //Tag-Bitをセット
	y1=(unsigned char)((yg>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
	y1=y1|0x20; //Tag-Bitをセット

	printable.PutChar(y1); //座標送信
	printable.PutChar(y0); //座標送信
	printable.PutChar(x1); //座標送信
	printable.PutChar(x0); //座標送信

	//Tek_Alpha(); //キャラクタモードに戻す
}
#endif

//------------------------------------------------------------------------------
// TEK4010
//------------------------------------------------------------------------------
void TEK4010::Test()
{
	printable_.Printf("hello\n");
	printable_.PrintfRaw("\x1b[?38h");
	printable_.PrintfRaw("\x1d\x1b\x60");
	printable_.PrintfRaw("\x1b\x0c\r\n");
#if 1
	for (int i = 0; i < 8; i++) {
		printable_.Printf("\x1b[%dm", 30 + i);
		printable_.Printf("hello\n");
	}
#endif
	for (int i = 0; i < 480; i += 5) {
		Tek_Line(printable_, 0, 240, 600, i, i);
	}

}

}
