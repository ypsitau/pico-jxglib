//==============================================================================
// TEK4010.cpp
//==============================================================================
#include "jxglib/TEK4010.h"

namespace jxglib {

void Tek_Alpha(Printable& printable)
{
	printable.PutCharRaw(0x1F); //US　アルファモードへ戻す
}

void Tek_Graph(Printable& printable, int vector_no)
{
	printable.PutCharRaw(0x1D); //GS
	printable.PutCharRaw(0x1B); //ESC
	printable.PutCharRaw(vector_no|0x60); //線種を選ぶ
}

void PutCoordX(Printable& printable, uint16_t coord)
{
	printable.PutCharRaw((1 << 5) | static_cast<char>((coord >> 5) & 0x1f));
	printable.PutCharRaw((0 << 5) | (1 << 6) | static_cast<char>(coord & 0x1f));
}

void PutCoordY(Printable& printable, uint16_t coord)
{
	printable.PutCharRaw((1 << 5) | static_cast<char>((coord >> 5) & 0x1f));
	printable.PutCharRaw((1 << 5) | (1 << 6) | static_cast<char>(coord & 0x1f));
}

#if 1
void Tek_Line(Printable& printable, uint16_t xs,uint16_t ys,uint16_t xg,uint16_t yg,uint16_t line_no)
{
	unsigned char x0,y0,x1,y1;
	Tek_Graph(printable, line_no); //グラフィックモードに移行(線種を指定)

#if 1
	PutCoordY(printable, ys);
	PutCoordX(printable, xs);
	PutCoordY(printable, yg);
	PutCoordX(printable, xg);
#endif
#if 0
	x0=(unsigned char)(xs&0x1F); //下位5byteを取り出す
	x0=x0|0x40; //Tag-Bitをセット
	x1=(unsigned char)((xs>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
	x1=x1|0x20; //Tag-Bitをセット

	y0=(unsigned char)(ys & 0x1F); //下位5byteを取り出す
	y0=y0|0x60; //Tag-Bitをセット
	y1=(unsigned char)((ys>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
	y1=y1|0x20; //Tag-Bitをセット

	printable.PutCharRaw(y1); //座標送信
	printable.PutCharRaw(y0); //座標送信
	printable.PutCharRaw(x1); //座標送信
	printable.PutCharRaw(x0); //座標送信

	x0=(unsigned char)(xg&0x1F); //下位5byteを取り出す
	x0=x0|0x40; //Tag-Bitをセット
	y0=(unsigned char)(yg&0x1F); //下位5byteを取り出す
	y0=y0|0x60; //Tag-Bitをセット

	x1=(unsigned char)((xg>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
	x1=x1|0x20; //Tag-Bitをセット
	y1=(unsigned char)((yg>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
	y1=y1|0x20; //Tag-Bitをセット

	printable.PutCharRaw(y1); //座標送信
	printable.PutCharRaw(y0); //座標送信
	printable.PutCharRaw(x1); //座標送信
	printable.PutCharRaw(x0); //座標送信
#endif

	//Tek_Alpha(); //キャラクタモードに戻す
}
#endif

void Tek_Char_PosSet(Printable& printable, int xs,int ys)
{
	unsigned char x0,y0,x1,y1;

	printable.PutCharRaw(0x1D); //GS
	x0=(unsigned char)(xs&0x1F); //下位5byteを取り出す
	x0=x0|0x40; //Tag-Bitをセット
	y0=(unsigned char)(ys&0x1F); //下位5byteを取り出す
	y0=y0|0x60; //Tag-Bitをセット

	x1=(unsigned char)((xs>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
	x1=x1|0x20; //Tag-Bitをセット
	y1=(unsigned char)((ys>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
	y1=y1|0x20; //Tag-Bitをセット

	printable.PutCharRaw(y1); //座標送信
	printable.PutCharRaw(y0); //座標送信
	printable.PutCharRaw(x1); //座標送信
	printable.PutCharRaw(x0); //座標送信
	Tek_Alpha(printable); //キャラクタモードに戻す
}

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
	for (uint16_t i = 0; i < 480; i += 5) {
		Tek_Line(printable_, 0, 240, 600, i, i);
	}
	Tek_Char_PosSet(printable_, 420, 20);
	printable_.PrintfRaw("Hello World");
}

}
