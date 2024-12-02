//==============================================================================
// TEK4010.cpp
//==============================================================================
#include "jxglib/TEK4010.h"

namespace jxglib {

#if 0
void Tek_Graph(int vector_no)
{
spi_putc(0x1D); //GS
spi_putc(0x1B); //ESC
spi_putc(vector_no|0x60); //線種を選ぶ
}

void Tek_Line(int xs,int ys,int xg,int yg,int line_no)
{
unsigned char x0,y0,x1,y1;
Tek_Graph(line_no); //グラフィックモードに移行(線種を指定)

x0=(unsigned char)(xs&0x1F); //下位5byteを取り出す
x0=x0|0x40; //Tag-Bitをセット
y0=(unsigned char)(ys&0x1F); //下位5byteを取り出す
y0=y0|0x60; //Tag-Bitをセット

x1=(unsigned char)((xs>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
x1=x1|0x20; //Tag-Bitをセット
y1=(unsigned char)((ys>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
y1=y1|0x20; //Tag-Bitをセット

spi_putc(y1); //座標送信
spi_putc(y0); //座標送信
spi_putc(x1); //座標送信
spi_putc(x0); //座標送信


x0=(unsigned char)(xg&0x1F); //下位5byteを取り出す
x0=x0|0x40; //Tag-Bitをセット
y0=(unsigned char)(yg&0x1F); //下位5byteを取り出す
y0=y0|0x60; //Tag-Bitをセット

x1=(unsigned char)((xg>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
x1=x1|0x20; //Tag-Bitをセット
y1=(unsigned char)((yg>>5)&0x1F); //上位byteを5個右シフトしたあと下位5byteを取り出す
y1=y1|0x20; //Tag-Bitをセット

spi_putc(y1); //座標送信
spi_putc(y0); //座標送信
spi_putc(x1); //座標送信
spi_putc(x0); //座標送信

Tek_Alpha(); //キャラクタモードに戻す
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

}

}
