//==============================================================================
// Video.cpp
// Windows support YUY2 and NV12
// https://docs.microsoft.com/en-us/windows-hardware/drivers/stream/usb-video-class-driver-overview
//==============================================================================
#include "jxglib/USBDevice/Video.h"

#if CFG_TUD_VIDEO > 0

//-----------------------------------------------------------------------------
// USBDevice::Video
//-----------------------------------------------------------------------------
namespace jxglib::USBDevice {

Video::Video(Controller& deviceController, const char* strControl, const char* strStreaming, uint8_t endp, int width, int height, int frameRate) : Interface(deviceController, 2)
{
	uint32_t clockFrequency = 27000000; // Time stamp base clock. It is a deprecated parameter.
	uint8_t UVC_ENTITY_CAP_INPUT_TERMINAL = 0x01;
	uint8_t UVC_ENTITY_CAP_OUTPUT_TERMINAL = 0x02;
	uint8_t interfaceNum_VIDEO_CONTROL = interfaceNum_;
	uint8_t interfaceNum_VIDEO_STREAMING = interfaceNum_ + 1;
	const ConfigDesc configDesc = {
		.iad = {
			.bLength = sizeof(tusb_desc_interface_assoc_t),
			.bDescriptorType = TUSB_DESC_INTERFACE_ASSOCIATION,

			.bFirstInterface = interfaceNum_VIDEO_CONTROL,
			.bInterfaceCount = 2,
			.bFunctionClass = TUSB_CLASS_VIDEO,
			.bFunctionSubClass = VIDEO_SUBCLASS_INTERFACE_COLLECTION,
			.bFunctionProtocol = VIDEO_ITF_PROTOCOL_UNDEFINED,
			.iFunction = 0
		},
		.video_control = {
			.itf = {
				.bLength = sizeof(tusb_desc_interface_t),
				.bDescriptorType = TUSB_DESC_INTERFACE,

				.bInterfaceNumber = interfaceNum_VIDEO_CONTROL,
				.bAlternateSetting = 0,
				.bNumEndpoints = 0,
				.bInterfaceClass = TUSB_CLASS_VIDEO,
				.bInterfaceSubClass = VIDEO_SUBCLASS_CONTROL,
				.bInterfaceProtocol = VIDEO_ITF_PROTOCOL_15,
				.iInterface = deviceController.RegisterStringDesc(strControl)
			},
			.header = {
				.bLength = sizeof(tusb_desc_video_control_header_1itf_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VC_HEADER,

				.bcdUVC = VIDEO_BCD_1_50,
				.wTotalLength = sizeof(uvc_control_desc_t) - sizeof(tusb_desc_interface_t), // CS VC descriptors only
				.dwClockFrequency = clockFrequency,
				.bInCollection = 1,
				.baInterfaceNr = { interfaceNum_VIDEO_STREAMING }
			},
			.camera_terminal = {
				.bLength = sizeof(tusb_desc_video_control_camera_terminal_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VC_INPUT_TERMINAL,

				.bTerminalID = UVC_ENTITY_CAP_INPUT_TERMINAL,
				.wTerminalType = VIDEO_ITT_CAMERA,
				.bAssocTerminal = 0,
				.iTerminal = 0,
				.wObjectiveFocalLengthMin = 0,
				.wObjectiveFocalLengthMax = 0,
				.wOcularFocalLength = 0,
				.bControlSize = 3,
				.bmControls = { 0, 0, 0 }
			},
			.output_terminal = {
				.bLength = sizeof(tusb_desc_video_control_output_terminal_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VC_OUTPUT_TERMINAL,

				.bTerminalID = UVC_ENTITY_CAP_OUTPUT_TERMINAL,
				.wTerminalType = VIDEO_TT_STREAMING,
				.bAssocTerminal = 0,
				.bSourceID = UVC_ENTITY_CAP_INPUT_TERMINAL,
				.iTerminal = 0
			}
		},
		.video_streaming = {
			.itf = {
				.bLength = sizeof(tusb_desc_interface_t),
				.bDescriptorType = TUSB_DESC_INTERFACE,

				.bInterfaceNumber = interfaceNum_VIDEO_STREAMING,
				.bAlternateSetting = 0,
				.bNumEndpoints = CFG_TUD_VIDEO_STREAMING_BULK, // bulk 1, iso 0
				.bInterfaceClass = TUSB_CLASS_VIDEO,
				.bInterfaceSubClass = VIDEO_SUBCLASS_STREAMING,
				.bInterfaceProtocol = VIDEO_ITF_PROTOCOL_15,
				.iInterface = deviceController.RegisterStringDesc(strStreaming)
			},
			.header = {
				.bLength = sizeof(tusb_desc_video_streaming_input_header_1byte_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VS_INPUT_HEADER,

				.bNumFormats = 1,
				.wTotalLength = sizeof(uvc_streaming_desc_t) - sizeof(tusb_desc_interface_t)
					- sizeof(tusb_desc_endpoint_t) - (CFG_TUD_VIDEO_STREAMING_BULK? 0 : sizeof(tusb_desc_interface_t)), // CS VS descriptors only
				.bEndpointAddress = endp,
				.bmInfo = 0,
				.bTerminalLink = UVC_ENTITY_CAP_OUTPUT_TERMINAL,
				.bStillCaptureMethod = 0,
				.bTriggerSupport = 0,
				.bTriggerUsage = 0,
				.bControlSize = 1,
				.bmaControls = { 0 }
			},
			.format = {
				.bLength = sizeof(tusb_desc_video_format_uncompressed_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VS_FORMAT_UNCOMPRESSED,
				.bFormatIndex = 1, // 1-based index
				.bNumFrameDescriptors = 1,
				.guidFormat = { TUD_VIDEO_GUID_YUY2 },
				.bBitsPerPixel = 16,
				.bDefaultFrameIndex = 1,
				.bAspectRatioX = 0,
				.bAspectRatioY = 0,
				.bmInterlaceFlags = 0,
				.bCopyProtect = 0
			},
			.frame = {
				.bLength = sizeof(tusb_desc_video_frame_uncompressed_continuous_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VS_FRAME_UNCOMPRESSED,
				.bFrameIndex = 1, // 1-based index
				.bmCapabilities = 0,
				.wWidth = static_cast<uint16_t>(width),
				.wHeight = static_cast<uint16_t>(height),
				.dwMinBitRate = static_cast<uint32_t>(width * height * 16 * 1),
				.dwMaxBitRate = static_cast<uint32_t>(width * height * 16 * frameRate),
				.dwMaxVideoFrameBufferSize = static_cast<uint32_t>(width * height * 16 / 8),
				.dwDefaultFrameInterval = static_cast<uint32_t>(10000000 / frameRate),
				.bFrameIntervalType = 0, // continuous
				.dwFrameInterval = {
					static_cast<uint32_t>(10000000 / frameRate), // min
					static_cast<uint32_t>(10000000), // max
					static_cast<uint32_t>(10000000 / frameRate) // step
				}
			},
			.color = {
				.bLength = sizeof(tusb_desc_video_streaming_color_matching_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VS_COLORFORMAT,

				.bColorPrimaries = VIDEO_COLOR_PRIMARIES_BT709,
				.bTransferCharacteristics = VIDEO_COLOR_XFER_CH_BT709,
				.bMatrixCoefficients = VIDEO_COLOR_COEF_SMPTE170M
			},
#if !CFG_TUD_VIDEO_STREAMING_BULK
			.itf_alt = {
				.bLength = sizeof(tusb_desc_interface_t),
				.bDescriptorType = TUSB_DESC_INTERFACE,

				.bInterfaceNumber = interfaceNum_VIDEO_STREAMING,
				.bAlternateSetting = 1,
				.bNumEndpoints = 1,
				.bInterfaceClass = TUSB_CLASS_VIDEO,
				.bInterfaceSubClass = VIDEO_SUBCLASS_STREAMING,
				.bInterfaceProtocol = VIDEO_ITF_PROTOCOL_15,
				.iInterface = deviceController.RegisterStringDesc(strStreaming)
			},
#endif
			.ep = {
				.bLength = sizeof(tusb_desc_endpoint_t),
				.bDescriptorType = TUSB_DESC_ENDPOINT,

				.bEndpointAddress = endp,
				.bmAttributes = {
					.xfer = CFG_TUD_VIDEO_STREAMING_BULK? TUSB_XFER_BULK : TUSB_XFER_ISOCHRONOUS,
					.sync = CFG_TUD_VIDEO_STREAMING_BULK? 0 : 1 // asynchronous
				},
				.wMaxPacketSize = CFG_TUD_VIDEO_STREAMING_BULK? 64 : CFG_TUD_VIDEO_STREAMING_EP_BUFSIZE,
				.bInterval = 1
			}
		}
	};
	RegisterConfigDesc(&configDesc, sizeof(configDesc));
}

}

int board_millis()
{
	return to_ms_since_boot(get_absolute_time());
}

#define FRAME_WIDTH   128
#define FRAME_HEIGHT  96
#define FRAME_RATE    10

static unsigned frame_num = 0;
static unsigned tx_busy = 0;
static unsigned interval_ms = 1000 / FRAME_RATE;

// YUY2 frame buffer
static uint8_t frame_buffer[FRAME_WIDTH * FRAME_HEIGHT * 16 / 8];

static void fill_color_bar(uint8_t* buffer, unsigned start_position)
{
	/* EBU color bars: https://stackoverflow.com/questions/6939422 */
	static uint8_t const bar_color[8][4] = {
		/*  Y,   U,   Y,   V */
		{ 235, 128, 235, 128}, /* 100% White */
		{ 219,  16, 219, 138}, /* Yellow */
		{ 188, 154, 188,  16}, /* Cyan */
		{ 173,  42, 173,  26}, /* Green */
		{  78, 214,  78, 230}, /* Magenta */
		{  63, 102,  63, 240}, /* Red */
		{  32, 240,  32, 118}, /* Blue */
		{  16, 128,  16, 128}, /* Black */
	};
	uint8_t* p;
	uint8_t* end = &buffer[FRAME_WIDTH * 2];
	unsigned idx = (FRAME_WIDTH / 2 - 1) - (start_position % (FRAME_WIDTH / 2));
	p = &buffer[idx * 4];
	for (unsigned i = 0; i < 8; ++i) {
		for (int j = 0; j < FRAME_WIDTH / (2 * 8); ++j) {
			memcpy(p, &bar_color[i], 4);
			p += 4;
			if (end <= p) {
				p = buffer;
			}
		}
	}
	p = &buffer[FRAME_WIDTH * 2];
	for (unsigned i = 1; i < FRAME_HEIGHT; ++i) {
		memcpy(p, buffer, FRAME_WIDTH * 2);
		p += FRAME_WIDTH * 2;
	}
}

void video_send_frame(void)
{
	uint8_t ctl_idx = 0;
	uint8_t stm_idx = 0;
	static unsigned start_ms = 0;
	static unsigned already_sent = 0;

	if (!::tud_video_n_streaming(ctl_idx, stm_idx)) {
		already_sent = 0;
		frame_num = 0;
		return;
	}
	if (!already_sent) {
		already_sent = 1;
		tx_busy = 1;
		start_ms = board_millis();
		fill_color_bar(frame_buffer, frame_num);
		::tud_video_n_frame_xfer(ctl_idx, stm_idx, frame_buffer, FRAME_WIDTH * FRAME_HEIGHT * 16 / 8);
	}
	unsigned cur = board_millis();
	if (cur - start_ms < interval_ms) return; // not enough time
	if (tx_busy) return;
	start_ms += interval_ms;
	tx_busy = 1;

	fill_color_bar(frame_buffer, frame_num);
	::tud_video_n_frame_xfer(ctl_idx, stm_idx, frame_buffer, FRAME_WIDTH * FRAME_HEIGHT * 16 / 8);
}

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------
void tud_video_frame_xfer_complete_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx)
{
	tx_busy = 0;
	++frame_num;
}

int tud_video_commit_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx, const video_probe_and_commit_control_t* parameters)
{
	/* convert unit to ms from 100 ns */
	interval_ms = parameters->dwFrameInterval / 10000;
	return VIDEO_ERROR_NONE;
}

#endif
