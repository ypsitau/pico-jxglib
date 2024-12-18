//==============================================================================
// Color.cpp
// Color names come from: https://www.w3.org/TR/css-color-3/
//==============================================================================
#include "jxglib/Color.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Color
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// ColorRGB565
//------------------------------------------------------------------------------
const ColorRGB565 ColorRGB565::zero			{   0,   0,   0 };
const ColorRGB565 ColorRGB565::black		{   0,   0,   0 };
const ColorRGB565 ColorRGB565::silver		{ 192, 192, 192 };
const ColorRGB565 ColorRGB565::gray			{ 128, 128, 128 };
const ColorRGB565 ColorRGB565::white		{ 255, 255, 255 };
const ColorRGB565 ColorRGB565::maroon		{ 128,   0,   0 };
const ColorRGB565 ColorRGB565::red			{ 255,   0,   0 };
const ColorRGB565 ColorRGB565::purple		{ 128,   0, 128 };
const ColorRGB565 ColorRGB565::fuchsia		{ 255,   0, 255 };
const ColorRGB565 ColorRGB565::green		{   0, 128,   0 };
const ColorRGB565 ColorRGB565::lime			{   0, 255,   0 };
const ColorRGB565 ColorRGB565::olive		{ 128, 128,   0 };
const ColorRGB565 ColorRGB565::yellow		{ 255, 255,   0 };
const ColorRGB565 ColorRGB565::navy			{   0,   0, 128 };
const ColorRGB565 ColorRGB565::blue			{   0,   0, 255 };
const ColorRGB565 ColorRGB565::teal			{   0, 128, 128 };
const ColorRGB565 ColorRGB565::aqua			{   0, 255, 255 };

//------------------------------------------------------------------------------
// ColorBGR565
//------------------------------------------------------------------------------
const ColorBGR565 ColorBGR565::zero			{   0,   0,   0 };
const ColorBGR565 ColorBGR565::black		{   0,   0,   0 };
const ColorBGR565 ColorBGR565::silver		{ 192, 192, 192 };
const ColorBGR565 ColorBGR565::gray			{ 128, 128, 128 };
const ColorBGR565 ColorBGR565::white		{ 255, 255, 255 };
const ColorBGR565 ColorBGR565::maroon		{ 128,   0,   0 };
const ColorBGR565 ColorBGR565::red			{ 255,   0,   0 };
const ColorBGR565 ColorBGR565::purple		{ 128,   0, 128 };
const ColorBGR565 ColorBGR565::fuchsia		{ 255,   0, 255 };
const ColorBGR565 ColorBGR565::green		{   0, 128,   0 };
const ColorBGR565 ColorBGR565::lime			{   0, 255,   0 };
const ColorBGR565 ColorBGR565::olive		{ 128, 128,   0 };
const ColorBGR565 ColorBGR565::yellow		{ 255, 255,   0 };
const ColorBGR565 ColorBGR565::navy			{   0,   0, 128 };
const ColorBGR565 ColorBGR565::blue			{   0,   0, 255 };
const ColorBGR565 ColorBGR565::teal			{   0, 128, 128 };
const ColorBGR565 ColorBGR565::aqua			{   0, 255, 255 };

//------------------------------------------------------------------------------
// ColorRGB555
//------------------------------------------------------------------------------
const ColorRGB555 ColorRGB555::zero			{   0,   0,   0 };
const ColorRGB555 ColorRGB555::black		{   0,   0,   0 };
const ColorRGB555 ColorRGB555::silver		{ 192, 192, 192 };
const ColorRGB555 ColorRGB555::gray			{ 128, 128, 128 };
const ColorRGB555 ColorRGB555::white		{ 255, 255, 255 };
const ColorRGB555 ColorRGB555::maroon		{ 128,   0,   0 };
const ColorRGB555 ColorRGB555::red			{ 255,   0,   0 };
const ColorRGB555 ColorRGB555::purple		{ 128,   0, 128 };
const ColorRGB555 ColorRGB555::fuchsia		{ 255,   0, 255 };
const ColorRGB555 ColorRGB555::green		{   0, 128,   0 };
const ColorRGB555 ColorRGB555::lime			{   0, 255,   0 };
const ColorRGB555 ColorRGB555::olive		{ 128, 128,   0 };
const ColorRGB555 ColorRGB555::yellow		{ 255, 255,   0 };
const ColorRGB555 ColorRGB555::navy			{   0,   0, 128 };
const ColorRGB555 ColorRGB555::blue			{   0,   0, 255 };
const ColorRGB555 ColorRGB555::teal			{   0, 128, 128 };
const ColorRGB555 ColorRGB555::aqua			{   0, 255, 255 };

//------------------------------------------------------------------------------
// ColorBGR555
//------------------------------------------------------------------------------
const ColorBGR555 ColorBGR555::zero			{   0,   0,   0 };
const ColorBGR555 ColorBGR555::black		{   0,   0,   0 };
const ColorBGR555 ColorBGR555::silver		{ 192, 192, 192 };
const ColorBGR555 ColorBGR555::gray			{ 128, 128, 128 };
const ColorBGR555 ColorBGR555::white		{ 255, 255, 255 };
const ColorBGR555 ColorBGR555::maroon		{ 128,   0,   0 };
const ColorBGR555 ColorBGR555::red			{ 255,   0,   0 };
const ColorBGR555 ColorBGR555::purple		{ 128,   0, 128 };
const ColorBGR555 ColorBGR555::fuchsia		{ 255,   0, 255 };
const ColorBGR555 ColorBGR555::green		{   0, 128,   0 };
const ColorBGR555 ColorBGR555::lime			{   0, 255,   0 };
const ColorBGR555 ColorBGR555::olive		{ 128, 128,   0 };
const ColorBGR555 ColorBGR555::yellow		{ 255, 255,   0 };
const ColorBGR555 ColorBGR555::navy			{   0,   0, 128 };
const ColorBGR555 ColorBGR555::blue			{   0,   0, 255 };
const ColorBGR555 ColorBGR555::teal			{   0, 128, 128 };
const ColorBGR555 ColorBGR555::aqua			{   0, 255, 255 };

//------------------------------------------------------------------------------
// ColorRGB111
//------------------------------------------------------------------------------
const ColorRGB111 ColorRGB111::zero			{   0,   0,   0 };
const ColorRGB111 ColorRGB111::black		{   0,   0,   0 };
const ColorRGB111 ColorRGB111::silver		{ 192, 192, 192 };
const ColorRGB111 ColorRGB111::gray			{ 128, 128, 128 };
const ColorRGB111 ColorRGB111::white		{ 255, 255, 255 };
const ColorRGB111 ColorRGB111::maroon		{ 128,   0,   0 };
const ColorRGB111 ColorRGB111::red			{ 255,   0,   0 };
const ColorRGB111 ColorRGB111::purple		{ 128,   0, 128 };
const ColorRGB111 ColorRGB111::fuchsia		{ 255,   0, 255 };
const ColorRGB111 ColorRGB111::green		{   0, 128,   0 };
const ColorRGB111 ColorRGB111::lime			{   0, 255,   0 };
const ColorRGB111 ColorRGB111::olive		{ 128, 128,   0 };
const ColorRGB111 ColorRGB111::yellow		{ 255, 255,   0 };
const ColorRGB111 ColorRGB111::navy			{   0,   0, 128 };
const ColorRGB111 ColorRGB111::blue			{   0,   0, 255 };
const ColorRGB111 ColorRGB111::teal			{   0, 128, 128 };
const ColorRGB111 ColorRGB111::aqua			{   0, 255, 255 };

//------------------------------------------------------------------------------
// ColorBGR111
//------------------------------------------------------------------------------
const ColorBGR111 ColorBGR111::zero			{   0,   0,   0 };
const ColorBGR111 ColorBGR111::black		{   0,   0,   0 };
const ColorBGR111 ColorBGR111::silver		{ 192, 192, 192 };
const ColorBGR111 ColorBGR111::gray			{ 128, 128, 128 };
const ColorBGR111 ColorBGR111::white		{ 255, 255, 255 };
const ColorBGR111 ColorBGR111::maroon		{ 128,   0,   0 };
const ColorBGR111 ColorBGR111::red			{ 255,   0,   0 };
const ColorBGR111 ColorBGR111::purple		{ 128,   0, 128 };
const ColorBGR111 ColorBGR111::fuchsia		{ 255,   0, 255 };
const ColorBGR111 ColorBGR111::green		{   0, 128,   0 };
const ColorBGR111 ColorBGR111::lime			{   0, 255,   0 };
const ColorBGR111 ColorBGR111::olive		{ 128, 128,   0 };
const ColorBGR111 ColorBGR111::yellow		{ 255, 255,   0 };
const ColorBGR111 ColorBGR111::navy			{   0,   0, 128 };
const ColorBGR111 ColorBGR111::blue			{   0,   0, 255 };
const ColorBGR111 ColorBGR111::teal			{   0, 128, 128 };
const ColorBGR111 ColorBGR111::aqua			{   0, 255, 255 };

}
