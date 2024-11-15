//==============================================================================
// Color.cpp
//==============================================================================
#include "jxglib/Color.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Color
//------------------------------------------------------------------------------
// These Color names come from: https://www.w3.org/TR/css-color-3/
const Color Color::zero			{   0,   0,   0 };
const Color Color::black		{   0,   0,   0 };
const Color Color::silver		{ 192, 192, 192 };
const Color Color::gray			{ 128, 128, 128 };
const Color Color::white		{ 255, 255, 255 };
const Color Color::maroon		{ 128,   0,   0 };
const Color Color::red			{ 255,   0,   0 };
const Color Color::purple		{ 128,   0, 128 };
const Color Color::fuchsia		{ 255,   0, 255 };
const Color Color::green		{   0, 128,   0 };
const Color Color::lime			{   0, 255,   0 };
const Color Color::olive		{ 128, 128,   0 };
const Color Color::yellow		{ 255, 255,   0 };
const Color Color::navy			{   0,   0, 128 };
const Color Color::blue			{   0,   0, 255 };
const Color Color::teal			{   0, 128, 128 };
const Color Color::aqua			{   0, 255, 255 };

//------------------------------------------------------------------------------
// ColorA
//------------------------------------------------------------------------------
// These Color names come from: https://www.w3.org/TR/css-color-3/
const ColorA ColorA::zero		{   0,   0,   0, 255};
const ColorA ColorA::black		{   0,   0,   0, 255};
const ColorA ColorA::silver		{ 192, 192, 192, 255};
const ColorA ColorA::gray		{ 128, 128, 128, 255};
const ColorA ColorA::white		{ 255, 255, 255, 255};
const ColorA ColorA::maroon		{ 128,   0,   0, 255};
const ColorA ColorA::red		{ 255,   0,   0, 255};
const ColorA ColorA::purple		{ 128,   0, 128, 255};
const ColorA ColorA::fuchsia	{ 255,   0, 255, 255};
const ColorA ColorA::green		{   0, 128,   0, 255};
const ColorA ColorA::lime		{   0, 255,   0, 255};
const ColorA ColorA::olive		{ 128, 128,   0, 255};
const ColorA ColorA::yellow		{ 255, 255,   0, 255};
const ColorA ColorA::navy		{   0,   0, 128, 255};
const ColorA ColorA::blue		{   0,   0, 255, 255};
const ColorA ColorA::teal		{   0, 128, 128, 255};
const ColorA ColorA::aqua		{   0, 255, 255, 255};

}
