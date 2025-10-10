#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/Point.h"

using namespace jxglib;

void test_Point()
{
	printf("=== Point Parse Method Tests ===\n");
	
	// Point テスト
	printf("\n--- Point Tests ---\n");
	
	Point p;
	
	// 正常なケース
	printf("Testing valid cases:\n");
	
	// 基本的なケース
	p = Point(); // リセット
	if (p.Parse("10,20")) {
		printf("  \"10,20\" -> (%d,%d) : PASS\n", p.x, p.y);
	} else {
		printf("  \"10,20\" : FAIL\n");
	}
	
	// 空白を含むケース
	p = Point();
	if (p.Parse("5 , 15")) {
		printf("  \"5 , 15\" -> (%d,%d) : PASS\n", p.x, p.y);
	} else {
		printf("  \"5 , 15\" : FAIL\n");
	}
	
	// ゼロ値
	p = Point();
	if (p.Parse("0,0")) {
		printf("  \"0,0\" -> (%d,%d) : PASS\n", p.x, p.y);
	} else {
		printf("  \"0,0\" : FAIL\n");
	}
	
	// 大きな値
	p = Point();
	if (p.Parse("1000,2000")) {
		printf("  \"1000,2000\" -> (%d,%d) : PASS\n", p.x, p.y);
	} else {
		printf("  \"1000,2000\" : FAIL\n");
	}
	
	printf("Testing invalid cases:\n");
	
	// NULL文字列
	p = Point();
	if (!p.Parse(nullptr)) {
		printf("  nullptr : PASS (correctly rejected)\n");
	} else {
		printf("  nullptr : FAIL (should be rejected)\n");
	}
	
	// 空文字列
	p = Point();
	if (!p.Parse("")) {
		printf("  \"\" : PASS (correctly rejected)\n");
	} else {
		printf("  \"\" : FAIL (should be rejected)\n");
	}
	
	// 負の値
	p = Point();
	if (!p.Parse("-5,10")) {
		printf("  \"-5,10\" : PASS (correctly rejected)\n");
	} else {
		printf("  \"-5,10\" : FAIL (should be rejected)\n");
	}
	
	p = Point();
	if (!p.Parse("10,-5")) {
		printf("  \"10,-5\" : PASS (correctly rejected)\n");
	} else {
		printf("  \"10,-5\" : FAIL (should be rejected)\n");
	}
	
	// カンマなし
	p = Point();
	if (!p.Parse("10 20")) {
		printf("  \"10 20\" : PASS (correctly rejected)\n");
	} else {
		printf("  \"10 20\" : FAIL (should be rejected)\n");
	}
	
	// 不正な文字
	p = Point();
	if (!p.Parse("abc,def")) {
		printf("  \"abc,def\" : PASS (correctly rejected)\n");
	} else {
		printf("  \"abc,def\" : FAIL (should be rejected)\n");
	}
	
	// 余分な文字
	p = Point();
	if (!p.Parse("10,20extra")) {
		printf("  \"10,20extra\" : PASS (correctly rejected)\n");
	} else {
		printf("  \"10,20extra\" : FAIL (should be rejected)\n");
	}
	
	// PointFloat テスト
	printf("\n--- PointFloat Tests ---\n");
	
	PointFloat pf;
	
	printf("Testing valid cases:\n");
	
	// 基本的なケース
	pf = PointFloat();
	if (pf.Parse("10.5,20.3")) {
		printf("  \"10.5,20.3\" -> (%.2f,%.2f) : PASS\n", pf.x, pf.y);
	} else {
		printf("  \"10.5,20.3\" : FAIL\n");
	}
	
	// 整数値
	pf = PointFloat();
	if (pf.Parse("10,20")) {
		printf("  \"10,20\" -> (%.2f,%.2f) : PASS\n", pf.x, pf.y);
	} else {
		printf("  \"10,20\" : FAIL\n");
	}
	
	// ゼロ値
	pf = PointFloat();
	if (pf.Parse("0.0,0.0")) {
		printf("  \"0.0,0.0\" -> (%.2f,%.2f) : PASS\n", pf.x, pf.y);
	} else {
		printf("  \"0.0,0.0\" : FAIL\n");
	}
	
	// 小数点のみ
	pf = PointFloat();
	if (pf.Parse(".5,.7")) {
		printf("  \".5,.7\" -> (%.2f,%.2f) : PASS\n", pf.x, pf.y);
	} else {
		printf("  \".5,.7\" : FAIL\n");
	}
	
	printf("Testing invalid cases:\n");
	
	// 負の値
	pf = PointFloat();
	if (!pf.Parse("-1.5,2.0")) {
		printf("  \"-1.5,2.0\" : PASS (correctly rejected)\n");
	} else {
		printf("  \"-1.5,2.0\" : FAIL (should be rejected)\n");
	}
	
	// 不正な浮動小数点数
	pf = PointFloat();
	if (!pf.Parse("1.2.3,4.5")) {
		printf("  \"1.2.3,4.5\" : PASS (correctly rejected)\n");
	} else {
		printf("  \"1.2.3,4.5\" : FAIL (should be rejected)\n");
	}
	
	// PointDouble テスト
	printf("\n--- PointDouble Tests ---\n");
	
	PointDouble pd;
	
	printf("Testing valid cases:\n");
	
	// 基本的なケース
	pd = PointDouble();
	if (pd.Parse("10.123456,20.654321")) {
		printf("  \"10.123456,20.654321\" -> (%.6f,%.6f) : PASS\n", pd.x, pd.y);
	} else {
		printf("  \"10.123456,20.654321\" : FAIL\n");
	}
	
	// 科学記法
	pd = PointDouble();
	if (pd.Parse("1e2,2e3")) {
		printf("  \"1e2,2e3\" -> (%.2f,%.2f) : PASS\n", pd.x, pd.y);
	} else {
		printf("  \"1e2,2e3\" : FAIL\n");
	}
	
	// 整数値
	pd = PointDouble();
	if (pd.Parse("100,200")) {
		printf("  \"100,200\" -> (%.2f,%.2f) : PASS\n", pd.x, pd.y);
	} else {
		printf("  \"100,200\" : FAIL\n");
	}
	
	printf("Testing invalid cases:\n");
	
	// 負の値
	pd = PointDouble();
	if (!pd.Parse("-1.0,2.0")) {
		printf("  \"-1.0,2.0\" : PASS (correctly rejected)\n");
	} else {
		printf("  \"-1.0,2.0\" : FAIL (should be rejected)\n");
	}
	
	// 無効な科学記法
	pd = PointDouble();
	if (!pd.Parse("1e,2e")) {
		printf("  \"1e,2e\" : PASS (correctly rejected)\n");
	} else {
		printf("  \"1e,2e\" : FAIL (should be rejected)\n");
	}
	
	printf("\n=== All Point Parse Tests Complete ===\n");
}

int main()
{
	stdio_init_all();
	test_Point();
	for (;;) ::tight_loop_contents();
}
